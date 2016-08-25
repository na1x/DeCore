#include <algorithm>
#include <assert.h>

#include "engine.h"
#include "player.h"
#include "rules.h"
#include "deck.h"
#include "dataWriter.h"

namespace decore {

Engine::Engine()
    : mDeck(NULL)
    , mPlayerIdCounter(0)
    , mCurrentPlayer(NULL)
    , mRoundIndex(0)
    , mDefender(NULL)
{
}

Engine::~Engine()
{
    delete mDeck;
    for(std::vector<const PlayerId*>::iterator it = mGeneratedIds.begin(); it != mGeneratedIds.end(); ++it) {
        delete *it;
    }
}

PlayerId *Engine::add(Player & player)
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

    if (!mCurrentPlayer) {
        // if no current player - pick first one
        mCurrentPlayer = mGeneratedIds[0];
    }

    if (gameEnded()) {
        return false;
    }

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

    std::for_each(mGameObservers.begin(), mGameObservers.end(), RoundStartNotification(mAttackers, mDefender, mRoundIndex));

    bool defended = playCurrentRound();

    std::for_each(mGameObservers.begin(), mGameObservers.end(), RoundEndNotification(mRoundIndex));

    mRoundIndex++;

    // if attack failed "next move" goes to defender
    // or to next player after the defender otherwise
    mCurrentPlayer = defended ? mDefender : Rules::pickNext(mGeneratedIds, mDefender);

    // cleanup
    mAttackers.clear();
    mDefender = NULL;
    mTableCards.clear();

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

void Engine::save(DataWriter& writer) const
{
    // TODO: add synchronization
#define SAVE_CARD(writer, card) \
    writer.write(static_cast<unsigned int>(card.rank())); \
    writer.write(static_cast<unsigned int>(card.suit()));

    // save players count
    writer.write(mGeneratedIds.size());
    // save each player cards
    for (std::vector<const PlayerId*>::const_iterator it = mGeneratedIds.begin(); it != mGeneratedIds.end(); ++it) {
        const CardSet& playerCards = mPlayersCards.at(*it);
        // save cards amount
        writer.write(playerCards.size());
        // save all player cards
        for (CardSet::iterator it = playerCards.begin(); it != playerCards.end(); ++it) {
            const Card card = *it;
            SAVE_CARD(writer, card);
        }
    }
    assert(mDeck); // save called too early - nothing to save actually because the game has not been even started
    // save deck
    writer.write(mDeck->size());
    for (Deck::const_iterator it = mDeck->begin(); it != mDeck->end(); ++it) {
        const Card& card = *it;
        SAVE_CARD(writer, card);
    }
    // save current player
    assert(mCurrentPlayer);
    writer.write(std::find(mGeneratedIds.begin(), mGeneratedIds.end(), mCurrentPlayer) - mGeneratedIds.begin());
    // save current round index
    writer.write(mRoundIndex);
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

unsigned int Engine::TableCards::attackCards() const
{
    return mAttackCards.size();
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

    const PlayerId* currentAttackerId = mAttackers[0];

    CardSet& defenderCards = mPlayersCards[mDefender];

    Player& defender = *mPlayers[mDefender];

    unsigned int passedCounter = 0;

    bool defendFailed = false;

    const unsigned int maxAttackCards = Rules::maxAttackCards(defenderCards.size());

    for (;;) {

        if (mTableCards.attackCards() == maxAttackCards) {
            // defender has no more cards - defend succeeded
            break;
        }

        CardSet attackCards = Rules::getAttackCards(mTableCards.all(), mPlayersCards[currentAttackerId]);

        Player& currentAttacker = *mPlayers[currentAttackerId];
        const Card* attackCardPtr;

        if (mTableCards.empty()) {
            attackCardPtr = attackCards.empty() ? NULL : &currentAttacker.attack(mDefender, attackCards);
        } else {
            // ask for pitch with empty attackCards
            attackCardPtr = currentAttacker.pitch(mDefender, attackCards);
        }

        if (attackCards.empty() || !attackCardPtr) {
            // player skipped the move
            currentAttackerId = Rules::pickNext(mAttackers, currentAttackerId);

            if (mAttackers.size() > 1 && currentAttackerId == mAttackers[0]) {
                passedCounter = 0;
            }

            if (++passedCounter == mAttackers.size()) {
                // all attackers "passed"
                break;
            }
            continue;
        }

        Card attackCard = *attackCardPtr;

        if(!attackCards.erase(attackCard)) {
            // invalid card returned - the card is not from attackCards
            assert(!attackCards.empty());
            // take any card
            attackCard = *attackCards.begin();
        }

        std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsDroppedNotification(currentAttackerId, attackCard));
        mTableCards.addAttackCard(attackCard);
        mPlayersCards[currentAttackerId].erase(attackCard);

        CardSet defendCards = Rules::getDefendCards(attackCard, defenderCards, mDeck->trumpSuit());

        const Card* defendCardPtr = defender.defend(currentAttackerId, attackCard, defendCards);

        bool noCardsToDefend = defendCards.empty();
        bool userGrabbedCards = !defendCardPtr;
        bool invalidDefendCard = !defendCardPtr ? true : defendCards.find(*defendCardPtr) == defendCards.end();

        if(noCardsToDefend || userGrabbedCards || invalidDefendCard) {
            // defend failed
            defendFailed = true;
        } else {
            mTableCards.addDefendCard(*defendCardPtr);
            defenderCards.erase(*defendCardPtr);
            std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsDroppedNotification(mDefender, *defendCardPtr));
        }
    }

    if (defendFailed) {
        defenderCards.insert(mTableCards.all().begin(), mTableCards.all().end());
        defender.cardsUpdated(defenderCards);
        std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsReceivedNotification(mDefender, mTableCards.all()));
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

    Rules::deal(*mDeck, cards);

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

}
