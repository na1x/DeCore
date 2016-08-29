#include <algorithm>
#include <assert.h>

#include "engine.h"
#include "player.h"
#include "rules.h"
#include "deck.h"
#include "dataWriter.h"
#include "dataReader.h"

namespace decore {

Engine::Engine()
    : mDeck(NULL)
    , mPlayerIdCounter(0)
    , mCurrentPlayer(NULL)
    , mRoundIndex(0)
    , mDefender(NULL)
    , mQuit(false)
    , mCurrentRoundAttackerId(NULL)
    , mPassedCounter(0)
{
    pthread_mutex_init(&mLock, NULL);
}

Engine::~Engine()
{
    delete mDeck;
    for(std::vector<const PlayerId*>::iterator it = mGeneratedIds.begin(); it != mGeneratedIds.end(); ++it) {
        delete *it;
    }
    pthread_mutex_destroy(&mLock);
}

PlayerId *Engine::add(Player& player)
{
    if (mDeck) {
        return NULL;
    }

    PlayerId* id = new PlayerIdImplementation(mPlayerIdCounter++);

    // initialize player related data
    mGeneratedIds.push_back(id);
    mPlayers[id] = &player;
    mPlayersCards[id] = CardSet();

    // add this player to game observers
    addGameObserver(player);

    player.idCreated(id);

    lock();
    if (!mCurrentPlayer) {
        // if no current player - pick first one
        mCurrentPlayer = mGeneratedIds[0];
    }
    unlock();

    return id;
}

bool Engine::setDeck(const Deck &deck)
{
    if (mDeck) {
        // already set
        return false;
    }

    if (deck.empty()) {
        // nothing to do with empty card set
        return false;
    }

    if (mPlayers.size() < 2) {
        // add more players
        return false;
    }

    mDeck = new Deck(deck);

    CardSet cards;

    cards.insert(deck.begin(), deck.end());

    std::for_each(mGameObservers.begin(), mGameObservers.end(), GameStartNotification(mDeck->trumpSuit(), mGeneratedIds, cards));

    return true;
}

void Engine::lock()
{
    pthread_mutex_lock(&mLock);
}

void Engine::unlock()
{
    pthread_mutex_unlock(&mLock);
}

unsigned int Engine::playerIndex(const PlayerId* id)
{
    std::vector<const PlayerId*>::iterator it = std::find(mGeneratedIds.begin(), mGeneratedIds.end(), id);
    assert(it != mGeneratedIds.end());
    return it - mGeneratedIds.begin();
}

bool Engine::playRound()
{
    if (!mDeck) {
        // no cards set
        return false;
    }

    if (mGeneratedIds.size() < 2) {
        // too few players - at least two should be set
        return false;
    }

    if (gameEnded()) {
        return false;
    }

    lock();
    // prepare round data
    // pick current player as first attacker
    mAttackers.push_back(mCurrentPlayer);
    // pick next player as defender
    mDefender = Rules::pickNext(mGeneratedIds, mCurrentPlayer);
    // gather rest players as additional attackers
    const PlayerId* attacker = mDefender;
    while((attacker = Rules::pickNext(mGeneratedIds, attacker)) != mCurrentPlayer) {
        mAttackers.push_back(attacker);
    }

    unlock();

    std::for_each(mGameObservers.begin(), mGameObservers.end(), RoundStartNotification(mAttackers, mDefender, mRoundIndex));

    bool defended = playCurrentRound();

    std::for_each(mGameObservers.begin(), mGameObservers.end(), RoundEndNotification(mRoundIndex));

    lock();
    mRoundIndex++;

    // if attack failed "next move" goes to defender
    // or to next player after the defender otherwise
    mCurrentPlayer = defended ? mDefender : Rules::pickNext(mGeneratedIds, mDefender);

    // cleanup
    mAttackers.clear();
    mDefender = NULL;
    mTableCards.clear();

    unlock();

    return !gameEnded();
}

const PlayerId *Engine::getLoser()
{
    std::vector<const PlayerId*> playersWithCards;
    for(std::map<const PlayerId*, CardSet>::const_iterator it = mPlayersCards.begin(); it != mPlayersCards.end(); ++it) {
        if(!it->second.empty()) {
            playersWithCards.push_back(it->first);
        }
    }
    return playersWithCards.empty() ? NULL : 1 == playersWithCards.size() ? playersWithCards[0] : NULL;
}

void Engine::addGameObserver(GameObserver &observer)
{
    mGameObservers.push_back(&observer);
}

bool Engine::gameEnded() const
{
    // check no game cards left
    // check that less than one player have cards
    if (!mDeck->empty()) {
        return false;
    }

    unsigned int playersWithCards = 0;
    for(std::map<const PlayerId*, CardSet>::const_iterator it = mPlayersCards.begin(); it != mPlayersCards.end(); ++it) {
        if(!it->second.empty()) {
            playersWithCards++;
        }
    }

    return playersWithCards < 2;
}

void Engine::save(DataWriter& writer)
{
    if (!mDeck || !mCurrentPlayer) {
        // save called too early - nothing to save actually because the game has not been even started
        return;
    }

    lock();

    // save players count
    writer.write(mGeneratedIds.size());
    // save each player cards
    for (std::vector<const PlayerId*>::const_iterator it = mGeneratedIds.begin(); it != mGeneratedIds.end(); ++it) {
        const CardSet& playerCards = mPlayersCards.at(*it);
        writer.write(playerCards.begin(), playerCards.end());
    }
    // save deck
    writer.write(mDeck->begin(), mDeck->end());
    // save trump suit
    writer.write(mDeck->trumpSuit());
    // save current player index
    writer.write(playerIndex(mCurrentPlayer));
    // save current round index
    writer.write(mRoundIndex);

    // save current round related data
    // save attackers
    writer.write(mAttackers.size());
    if (!mAttackers.empty()) {
        for (std::vector<const PlayerId*>::iterator it = mAttackers.begin(); it != mAttackers.end(); ++it) {
            writer.write(playerIndex(*it));
        }
        assert(mDefender);
        writer.write(playerIndex(mDefender));
        writer.write(mPassedCounter);
        assert(mCurrentRoundAttackerId);
        writer.write(playerIndex(mCurrentRoundAttackerId));
        writer.write(mTableCards.attackCards().begin(), mTableCards.attackCards().end());
        writer.write(mTableCards.defendCards().begin(), mTableCards.defendCards().end());
    }

    // save observers data
    unsigned int observersCount = mGameObservers.size();
    writer.write(observersCount);
    for (std::vector<GameObserver*>::iterator it = mGameObservers.begin(); it != mGameObservers.end(); ++it) {
        (*it)->write(writer);
    }
    unlock();
}

void Engine::init(DataReader& reader, const std::vector<Player*> players, const std::vector<GameObserver*>& observers)
{
    // check that engine is not initialized yet
    assert(!mPlayerIdCounter);
    assert(mGeneratedIds.empty());
    assert(mPlayers.empty());
    assert(!mDeck);

    // read players count
    std::vector<const PlayerId*>::size_type playersCount;
    reader.read(playersCount);

    assert(players.size() == playersCount);
    // add players
    for (std::vector<Player*>::const_iterator it = players.begin(); it != players.end(); ++it) {
        add(**it);
    }

    const Card defaultCard(SUIT_CLUBS, RANK_6);
    // read each player cards
    for (std::vector<const PlayerId*>::const_iterator it = mGeneratedIds.begin(); it != mGeneratedIds.end(); ++it) {
        CardSet& playerCards = mPlayersCards[*it];
        assert(playerCards.empty());
        reader.read(playerCards, defaultCard);
        mPlayers[*it]->cardsRestored(playerCards);
    }

    // read deck
    Deck deck;
    reader.read(deck, defaultCard);
    Suit trumpSuit;
    reader.read(trumpSuit);
    deck.setTrumpSuit(trumpSuit);
    setDeck(deck);

    // read current player index
    unsigned int currentPlayerindex;
    reader.read(currentPlayerindex);
    mCurrentPlayer = mGeneratedIds[currentPlayerindex];
    // read current round index
    reader.read(mRoundIndex);

    // read current round data
    std::vector<const PlayerId*>::size_type attackersAmount;
    reader.read(attackersAmount);

    if (attackersAmount) {
        while (attackersAmount--) {
            unsigned int attackerIndex;
            reader.read(attackerIndex);
            mAttackers.push_back(mGeneratedIds[attackerIndex]);
        }
        unsigned int defenderIndex;
        reader.read(defenderIndex);
        mDefender = mGeneratedIds[defenderIndex];
        reader.read(mPassedCounter);
        unsigned int currentRoundAttackerIndex;
        reader.read(currentRoundAttackerIndex);
        mCurrentRoundAttackerId = mGeneratedIds[currentRoundAttackerIndex];

        std::vector<Card> attackCards;
        reader.read(attackCards, defaultCard);
        for (std::vector<Card>::iterator it = attackCards.begin(); it != attackCards.end(); ++it) {
            mTableCards.addAttackCard(*it);
        }

        std::vector<Card> defendCards;
        reader.read(defendCards, defaultCard);
        for (std::vector<Card>::iterator it = attackCards.begin(); it != attackCards.end(); ++it) {
            mTableCards.addDefendCard(*it);
        }
    }

    // append observers
    mGameObservers.insert(mGameObservers.end(), observers.begin(), observers.end());
    // initialize observers
    unsigned int savedObservers;
    reader.read(savedObservers);
    assert(savedObservers == mGameObservers.size());
    for (std::vector<GameObserver*>::iterator it = mGameObservers.begin(); it != mGameObservers.end(); ++it) {
        (*it)->init(reader);
    }

    std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsRestoredNotification(mTableCards));
}

void Engine::quit()
{
    lock();
    mQuit = true;
    unlock();
}

Engine::PlayerIdImplementation::PlayerIdImplementation(unsigned int id)
    : mId(id)
{}

Engine::GameStartNotification::GameStartNotification(const Suit &trumpSuit, const std::vector<const PlayerId *> &players, const CardSet &gameCards)
    : mTrumpSuit(trumpSuit)
    , mPlayers(players)
    , mGameCards(gameCards)
{}


void Engine::GameStartNotification::operator()(GameObserver*observer)
{
    observer->gameStarted(mTrumpSuit, mGameCards, mPlayers);
}

Engine::RoundStartNotification::RoundStartNotification(const std::vector<const PlayerId *> &attackers, const PlayerId *defender, unsigned int roundIndex)
    : mAttackers(attackers)
    , mDefender(defender)
    , mRoundIndex(roundIndex)
{

}

void Engine::RoundStartNotification::operator()(GameObserver *observer)
{
    observer->roundStarted(mRoundIndex, mAttackers, mDefender);
}

Engine::RoundEndNotification::RoundEndNotification(unsigned int roundIndex)
    : mRoundIndex(roundIndex)
{
}

void Engine::RoundEndNotification::operator()(GameObserver *observer)
{
    observer->roundEnded(mRoundIndex);
}

void Engine::TableCards::addAttackCard(const Card& card)
{
    mAttackCards.push_back(card);
    mAll.insert(card);
}

void Engine::TableCards::addDefendCard(const Card& card)
{
    mDefendCards.push_back(card);
    mAll.insert(card);
}

const CardSet& Engine::TableCards::all() const
{
    return mAll;
}

bool Engine::TableCards::empty() const
{
    return mAll.empty();
}

const std::vector<Card>& Engine::TableCards::attackCards() const
{
    return mAttackCards;
}

const std::vector<Card>& Engine::TableCards::defendCards() const
{
    return mDefendCards;
}

void Engine::TableCards::clear()
{
    mAttackCards.clear();
    mDefendCards.clear();
    mAll.clear();
}

bool Engine::playCurrentRound()
{
    // deal cards
    dealCards();

    lock();
    mCurrentRoundAttackerId = mAttackers[0];
    mPassedCounter = 0;
    unlock();

    CardSet& defenderCards = mPlayersCards[mDefender];
    Player& defender = *mPlayers[mDefender];

    bool defendFailed = false;
    bool quit = false;

#define CHECK_QUIT \
if (quit) { \
    return false; \
}

    const unsigned int maxAttackCards = Rules::maxAttackCards(defenderCards.size());

    for (;;) {

        if (mTableCards.attackCards().size() == maxAttackCards) {
            // defender has no more cards - defend succeeded
            break;
        }

        CardSet attackCards = Rules::getAttackCards(mTableCards.all(), mPlayersCards[mCurrentRoundAttackerId]);

        Player& currentAttacker = *mPlayers[mCurrentRoundAttackerId];
        const Card* attackCardPtr;

        if (mTableCards.empty()) {
            attackCardPtr = attackCards.empty() ? NULL : &currentAttacker.attack(mDefender, attackCards);
        } else {
            // ask for pitch even with empty attackCards - expected NULL attack card pointer
            attackCardPtr = currentAttacker.pitch(mDefender, attackCards);
        }

        if (attackCards.empty() || !attackCardPtr) {
            // player skipped the move
            lock();
            mCurrentRoundAttackerId = Rules::pickNext(mAttackers, mCurrentRoundAttackerId);
            quit = mQuit;
            unlock();

            CHECK_QUIT;

            if (mAttackers.size() > 1 && mCurrentRoundAttackerId == mAttackers[0]) {
                mPassedCounter = 0;
            }

            if (++mPassedCounter == mAttackers.size()) {
                // all attackers "passed" - round ended
                break;
            }
            continue;
        }

        assert(attackCardPtr);

        Card attackCard = *attackCardPtr;

        if(!attackCards.erase(attackCard)) {
            // invalid card returned - the card is not from attackCards
            assert(!attackCards.empty());
            // take any card
            attackCard = *attackCards.begin();
        }

        std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsDroppedNotification(mCurrentRoundAttackerId, attackCard));

        lock();
        mTableCards.addAttackCard(attackCard);
        mPlayersCards[mCurrentRoundAttackerId].erase(attackCard);
        quit = mQuit;
        unlock();

        CHECK_QUIT;

        CardSet defendCards = Rules::getDefendCards(attackCard, defenderCards, mDeck->trumpSuit());

        const Card* defendCardPtr = defender.defend(mCurrentRoundAttackerId, attackCard, defendCards);

        bool noCardsToDefend = defendCards.empty();
        bool userGrabbedCards = !defendCardPtr;
        bool invalidDefendCard = !defendCardPtr ? true : defendCards.find(*defendCardPtr) == defendCards.end();

        if(noCardsToDefend || userGrabbedCards || invalidDefendCard) {
            // defend failed
            defendFailed = true;
        } else {
            lock();
            mTableCards.addDefendCard(*defendCardPtr);
            defenderCards.erase(*defendCardPtr);
            quit = mQuit;
            unlock();
            CHECK_QUIT;
            std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsDroppedNotification(mDefender, *defendCardPtr));
        }
    }

    if (defendFailed) {
        lock();
        defenderCards.insert(mTableCards.all().begin(), mTableCards.all().end());
        defender.cardsUpdated(defenderCards);
        mCurrentRoundAttackerId = NULL;
        quit = mQuit;
        unlock();
        CHECK_QUIT;
        std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsReceivedNotification(mDefender, mTableCards.all()));
    } else {
        std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsLeftNotification(mTableCards.all()));
    }

    return !defendFailed;
}

void Engine::dealCards()
{
    // deal order:
    // 1. first attacker
    // 2. defender
    // 3. rest attackers

    std::map<const PlayerId*, unsigned int> oldCardsAmount;
    std::vector<CardSet*> cards;

    cards.push_back(&mPlayersCards[mAttackers[0]]);
    oldCardsAmount[mAttackers[0]] = cards[0]->size();

    cards.push_back(&mPlayersCards[mDefender]);
    oldCardsAmount[mDefender] = cards[1]->size();

    for(std::vector<const PlayerId*>::const_iterator it = mAttackers.begin() + 1; it != mAttackers.end(); ++it) {
        CardSet& set = mPlayersCards[*it];
        cards.push_back(&set);
        oldCardsAmount[*it] = set.size();
    }

    lock();
    Rules::deal(*mDeck, cards);
    unlock();

    for(std::map<const PlayerId*, unsigned int>::iterator it = oldCardsAmount.begin(); it != oldCardsAmount.end(); ++it) {
        const PlayerId* id = it->first;
        unsigned int cardsReceived = mPlayersCards[id].size() - it->second;
        if (cardsReceived) {
            mPlayers[id]->cardsUpdated(mPlayersCards[id]);
            std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsAmountReceivedNotification(id, cardsReceived));
        }
    }
}

Engine::CardsAmountReceivedNotification::CardsAmountReceivedNotification(const PlayerId *playerId, unsigned int cardsReceived)
    : mPlayerId(playerId)
    , mCardsReceived(cardsReceived)
{
}

void Engine::CardsAmountReceivedNotification::operator()(GameObserver* observer)
{
    observer->cardsDealed(mPlayerId, mCardsReceived);
}

Engine::CardsDroppedNotification::CardsDroppedNotification(const PlayerId *playerId, const Card &droppedCard)
    : mPlayerId(playerId)
{
    mDroppedCards.insert(droppedCard);
}

void Engine::CardsDroppedNotification::operator()(GameObserver *observer)
{
    observer->cardsDropped(mPlayerId, mDroppedCards);
}

Engine::CardsReceivedNotification::CardsReceivedNotification(const PlayerId *playerId, const CardSet &receivedCards)
    : mPlayerId(playerId)
    , mReceivedCards(receivedCards)
{
}

void Engine::CardsReceivedNotification::operator()(GameObserver *observer)
{
    observer->cardsPickedUp(mPlayerId, mReceivedCards);
}

Engine::CardsRestoredNotification::CardsRestoredNotification(const TableCards& tableCards)
    : mTableCards(tableCards)
{

}

void Engine::CardsRestoredNotification::operator ()(GameObserver* observer)
{
    observer->tableCardsRestored(mTableCards.attackCards(), mTableCards.defendCards());
}

Engine::CardsLeftNotification::CardsLeftNotification(const CardSet& tableCards)
    : mCards(tableCards)
{

}

void Engine::CardsLeftNotification::operator ()(GameObserver* observer)
{
    observer->cardsLeft(mCards);
}


}
