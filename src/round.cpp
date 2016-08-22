#include <cstddef>
#include <algorithm>
#include <cassert>

#include "round.h"
#include "rules.h"
#include "player.h"

namespace decore {

Round::Round(const std::vector<const PlayerId*>& attackers,
             const PlayerId*& defender,
             std::map<const PlayerId*, Player*>& players,
             const std::vector<GameObserver*>& gameObservers,
             Deck& deck,
             std::map<const PlayerId*, CardSet>& playersCards)
    : mAttackers(attackers)
    , mDefender(defender)
    , mPlayers(players)
    , mGameObservers(gameObservers)
    , mDeck(deck)
    , mPlayersCards(playersCards)
{
}

bool Round::play()
{
    // deal cards
    dealCards();

    class TableCards
    {
        std::vector<Card> mAttackCards;
        std::vector<Card> mDefendCards;
        CardSet mAll;
    public:
        void addAttackCard(const Card& card)
        {
            mAttackCards.push_back(card);
            mAll.insert(card);
        }

        void addDefendCard(const Card& card)
        {
            mDefendCards.push_back(card);
            mAll.insert(card);
        }

        const CardSet& all() const
        {
            return mAll;
        }
        bool empty() const
        {
            return mAll.empty();
        }
        unsigned int attackCards() const
        {
            return mAttackCards.size();
        }
    } tableCards;

    const PlayerId* currentAttackerId = mAttackers[0];

    CardSet attackCards;
    CardSet& defenderCards = mPlayersCards[mDefender];

    Player& defender = *mPlayers[mDefender];

    unsigned int passedCounter = 0;

    bool defendFailed = false;

    unsigned int maxAttackCards = Rules::maxAttackCards(defenderCards.size());

    for (;;) {

        if (tableCards.attackCards() == maxAttackCards) {
            // defender has no more cards - defend succeeded
            break;
        }

        attackCards = Rules::getAttackCards(tableCards.all(), mPlayersCards[currentAttackerId]);

        Player& currentAttacker = *mPlayers[currentAttackerId];
        const Card* attackCardPtr;

        if (tableCards.empty()) {
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
        tableCards.addAttackCard(attackCard);
        mPlayersCards[currentAttackerId].erase(attackCard);

        CardSet defendCards = Rules::getDefendCards(attackCard, defenderCards, mDeck.trumpSuit());

        const Card* defendCardPtr = defender.defend(currentAttackerId, attackCard, defendCards);

        bool noCardsToDefend = defendCards.empty();
        bool userGrabbedCards = !defendCardPtr;
        bool invalidDefendCard = defendCards.find(*defendCardPtr) == defendCards.end();

        if(noCardsToDefend || userGrabbedCards || invalidDefendCard) {
            // defend failed
            defendFailed = true;
        } else {
            tableCards.addDefendCard(*defendCardPtr);
            defenderCards.erase(*defendCardPtr);
            std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsDroppedNotification(mDefender, *defendCardPtr));
        }
    }

    if (defendFailed) {
        defenderCards.insert(tableCards.all().begin(), tableCards.all().end());
        defender.cardsUpdated(defenderCards);
        std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsReceivedNotification(mDefender, tableCards.all()));
    }

    return !defendFailed;
}

void Round::dealCards()
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

    Rules::deal(mDeck, cards);

    for(std::map<const PlayerId*, unsigned int>::iterator it = oldCardsAmount.begin(); it != oldCardsAmount.end(); ++it) {
        const PlayerId* id = it->first;
        unsigned int cardsReceived = mPlayersCards[id].size() - it->second;
        if (cardsReceived) {
            mPlayers[id]->cardsUpdated(mPlayersCards[id]);
            std::for_each(mGameObservers.begin(), mGameObservers.end(), CardsAmountReceivedNotification(id, cardsReceived));
        }
    }
}

Round::CardsAmountReceivedNotification::CardsAmountReceivedNotification(const PlayerId *playerId, unsigned int cardsReceived)
    : mPlayerId(playerId)
    , mCardsReceived(cardsReceived)
{
}

void Round::CardsAmountReceivedNotification::operator()(GameObserver* observer)
{
    observer->cardsDealed(mPlayerId, mCardsReceived);
}

Round::CardsDroppedNotification::CardsDroppedNotification(const PlayerId *playerId, const Card &droppedCard)
    : mPlayerId(playerId)
{
    mDroppedCards.insert(droppedCard);
}

void Round::CardsDroppedNotification::operator()(GameObserver *observer)
{
    observer->cardsDropped(mPlayerId, mDroppedCards);
}

Round::CardsReceivedNotification::CardsReceivedNotification(const PlayerId *playerId, const CardSet &receivedCards)
    : mPlayerId(playerId)
    , mReceivedCards(receivedCards)
{
}

void Round::CardsReceivedNotification::operator()(GameObserver *observer)
{
    observer->cardsPickedUp(mPlayerId, mReceivedCards);
}


}

