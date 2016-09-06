#include <algorithm>
#include <assert.h>
#include <stdbool.h>

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
    , mMaxAttackCards(0)
    , mDefendFailed(false)
    , mPickAttackCardFromTable(false)
    , mCurrentRoundIndex(NULL)
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

void Engine::lock() const
{
    pthread_mutex_lock(&mLock);
}

void Engine::unlock() const
{
    pthread_mutex_unlock(&mLock);
}

bool Engine::findByPtr(const CardSet& cards, const Card* card)
{
    for (CardSet::const_iterator it = cards.begin(); it != cards.end(); ++it) {
        if (&*it == card) {
            return true;
        }
    }
    return false;
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

    if (mAttackers.empty()) {
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
    }

    unlock();

    bool defended = playCurrentRound();

    lock();
    mDefendFailed = false;
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
    if (mQuit.get()) {
        return true;
    }
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

void Engine::save(DataWriter& writer) const
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
    writer.write(mGeneratedIds.index(mCurrentPlayer));
    // save current round index
    writer.write(mRoundIndex);

    // save bool if round is running
    writer.write(static_cast<bool>(mCurrentRoundIndex));
    // save current round related data
    if (mCurrentRoundIndex) {
        // save attackers
        writer.write(mAttackers.size());
        for (std::vector<const PlayerId*>::const_iterator it = mAttackers.begin(); it != mAttackers.end(); ++it) {
            writer.write(mGeneratedIds.index(*it));
        }
        assert(mDefender);
        writer.write(mGeneratedIds.index(mDefender));
        writer.write(mPassedCounter);
        assert(mCurrentRoundAttackerId);
        writer.write(mGeneratedIds.index(mCurrentRoundAttackerId));
        writer.write(mTableCards.attackCards().begin(), mTableCards.attackCards().end());
        writer.write(mTableCards.defendCards().begin(), mTableCards.defendCards().end());
        writer.write(mMaxAttackCards);
        writer.write(mDefendFailed);
    }

    // save observers data
    unsigned int observersCount = mGameObservers.size();
    writer.write(observersCount);
    for (std::vector<GameObserver*>::const_iterator it = mGameObservers.begin(); it != mGameObservers.end(); ++it) {
        unsigned int observerDataStart = writer.position();
        (*it)->save(writer);
        writer.write(static_cast<unsigned int>(writer.position() - observerDataStart));
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
    assert(mGameObservers.empty());

    // read players count
    std::vector<const PlayerId*>::size_type playersCount;
    reader.read(playersCount);

    assert(players.size() == playersCount);
    // add players
    for (std::vector<Player*>::const_iterator it = players.begin(); it != players.end(); ++it) {
        add(**it);
    }

    const Card defaultCard(SUIT_LAST, RANK_LAST);
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

    bool roundRunning;
    reader.read(roundRunning);
    if (roundRunning) {
        mCurrentRoundIndex = &mRoundIndex;
        // read current round data
        std::vector<const PlayerId*>::size_type attackersAmount;
        reader.read(attackersAmount);

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
        for (std::vector<Card>::iterator it = defendCards.begin(); it != defendCards.end(); ++it) {
            mTableCards.addDefendCard(*it);
        }
        reader.read(mMaxAttackCards);
        reader.read(mDefendFailed);
        mPickAttackCardFromTable = !mDefendFailed && attackCards.size() == defendCards.size() + 1;
    }

    // append observers
    mGameObservers.insert(mGameObservers.end(), observers.begin(), observers.end());

    std::map<const PlayerId*, unsigned int> playersCards;
    for (PlayerIds::const_iterator it = mGeneratedIds.begin(); it != mGeneratedIds.end(); ++it) {
        playersCards[*it] = mPlayersCards[*it].size();
    }
    std::for_each(mGameObservers.begin(), mGameObservers.end(), GameRestoredNotification(mGeneratedIds, playersCards, mDeck->size(), mDeck->trumpSuit(), mTableCards));

    // initialize observers
    unsigned int savedObservers;
    reader.read(savedObservers);
    assert(savedObservers == mGameObservers.size());
    for (std::vector<GameObserver*>::iterator it = mGameObservers.begin(); it != mGameObservers.end(); ++it) {
        unsigned int observerDataStart = reader.position();
        (*it)->init(reader);
        unsigned int actualObserverDataSize = reader.position() - observerDataStart;
        unsigned int expectedObserverDataSize;
        reader.read(expectedObserverDataSize);
        assert(actualObserverDataSize == expectedObserverDataSize);
    }
}

void Engine::quit()
{
    if (!mQuit.setAndGet(true)) {
        for (std::vector<GameObserver*>::iterator it = mGameObservers.begin(); it != mGameObservers.end(); ++it) {
            (*it)->quit();
        }
    }
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
#define CHECK_QUIT \
if (mQuit.get()) { \
    return false; \
}

    lock();
    if (!mCurrentRoundIndex) {
        mCurrentRoundIndex = &mRoundIndex;
        unlock();
        std::for_each(mGameObservers.begin(), mGameObservers.end(), RoundStartNotification(mAttackers, mDefender, mRoundIndex));
        // deal cards
        dealCards();
    } else {
        unlock();
    }

    lock();

    if (!mCurrentRoundAttackerId) {
        mCurrentRoundAttackerId = mAttackers[0];
        mPassedCounter = 0;
    }

    CardSet& defenderCards = mPlayersCards[mDefender];
    Player& defender = *mPlayers[mDefender];

    if (!mMaxAttackCards) {
        mMaxAttackCards = Rules::maxAttackCards(defenderCards.size());
    }

    unlock();

    for (;;) {

        if (mTableCards.attackCards().size() == mMaxAttackCards) {
            // defender has no more cards - defend succeeded
            break;
        }

        const Card* attackCardPtr;

        if (mPickAttackCardFromTable) {
            assert(!mTableCards.attackCards().empty());
            attackCardPtr = &*(mTableCards.attackCards().end() - 1);
        } else {
            CardSet attackCards = Rules::getAttackCards(mTableCards.all(), mPlayersCards[mCurrentRoundAttackerId]);

            Player& currentAttacker = *mPlayers[mCurrentRoundAttackerId];

            if (mTableCards.empty()) {
                attackCardPtr = attackCards.empty() ? NULL : &currentAttacker.attack(mDefender, attackCards);
            } else {
                // ask for pitch even with empty attackCards - expected NULL attack card pointer
                attackCardPtr = currentAttacker.pitch(mDefender, attackCards);
            }

            // check if quit requested and only after that transfer move to defender
            CHECK_QUIT;

            if (attackCards.empty() || !attackCardPtr) {
                lock();
                // player skipped the move - pick next attacker
                mCurrentRoundAttackerId = Rules::pickNext(mAttackers, mCurrentRoundAttackerId);
                // if more than one attacker and we have first attacker again - reset pass counter
                if (mAttackers.size() > 1 && mCurrentRoundAttackerId == mAttackers[0]) {
                    mPassedCounter = 0;
                }
                mPassedCounter++;
                unlock();

                if (mPassedCounter == mAttackers.size()) {
                    // all attackers "passed" - round ended
                    break;
                }
                continue;
            }

            assert(attackCardPtr);

            if(!findByPtr(attackCards, attackCardPtr)) {
                // invalid card returned - the card is not from attackCards
                assert(!attackCards.empty());
                // take any card
                attackCardPtr = &*attackCards.begin();
            }

            Card attackCard = *attackCardPtr;

            lock();
            mTableCards.addAttackCard(attackCard);
            mPlayersCards[mCurrentRoundAttackerId].erase(attackCard);
            unlock();
            CHECK_QUIT;
            std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsDroppedNotification(mCurrentRoundAttackerId, attackCard));
            // the card is removed from the `attackCards` and is added to `mTableCards`, so update its pointer
            attackCardPtr = &*std::find(mTableCards.attackCards().begin(), mTableCards.attackCards().end(), attackCard);
        }

        if (mDefendFailed) {
            continue;
        }

        CardSet defendCards = Rules::getDefendCards(*attackCardPtr, defenderCards, mDeck->trumpSuit());

        const Card* defendCardPtr = defender.defend(mCurrentRoundAttackerId, *attackCardPtr, defendCards);

        bool noCardsToDefend = defendCards.empty();
        bool userGrabbedCards = !defendCardPtr;
        bool invalidDefendCard = !findByPtr(defendCards, defendCardPtr);

        lock();
        mPickAttackCardFromTable = false;
        unlock();

        if(noCardsToDefend || userGrabbedCards || invalidDefendCard) {
            // defend failed
            lock();
            mDefendFailed = true;
            unlock();
        } else {
            lock();
            mTableCards.addDefendCard(*defendCardPtr);
            defenderCards.erase(*defendCardPtr);
            unlock();
            CHECK_QUIT;
            std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsDroppedNotification(mDefender, *defendCardPtr));
        }
    }

    if (mDefendFailed) {
        lock();
        defenderCards.insert(mTableCards.all().begin(), mTableCards.all().end());
        defender.cardsUpdated(defenderCards);
        unlock();
        CHECK_QUIT;
        std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsReceivedNotification(mDefender, mTableCards.all()));
    } else {
        std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsGoneNotification(mTableCards.all()));
    }

    // cleanup
    lock();
    mCurrentRoundAttackerId = NULL;
    mMaxAttackCards = 0;
    mCurrentRoundIndex = NULL;
    unlock();
    CHECK_QUIT;

    std::for_each(mGameObservers.begin(), mGameObservers.end(), RoundEndNotification(mRoundIndex));

    return !mDefendFailed;
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

Engine::GameRestoredNotification::GameRestoredNotification(const std::vector<const PlayerId*>& playerIds,
            const std::map<const PlayerId*, unsigned int> playersCards,
            unsigned int deckCards,
            const Suit& trumpSuit,
            const TableCards& tableCards)
    : mPlayerIds(playerIds)
    , mPlayersCards(playersCards)
    , mDeckCards(deckCards)
    , mTrumpSuit(trumpSuit)
    , mTableCards(tableCards)
{

}

void Engine::GameRestoredNotification::operator ()(GameObserver* observer)
{
    observer->gameRestored(mPlayerIds, mPlayersCards, mDeckCards, mTrumpSuit, mTableCards.attackCards(), mTableCards.defendCards());
}

Engine::CardsGoneNotification::CardsGoneNotification(const CardSet& tableCards)
    : mCards(tableCards)
{

}

void Engine::CardsGoneNotification::operator ()(GameObserver* observer)
{
    observer->cardsGone(mCards);
}


}
