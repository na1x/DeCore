#include <cstddef>

#include "round.h"
#include "rules.h"
#include "player.h"

namespace decore {

Round::Round(const std::vector<const PlayerId*>& attackers,
             const PlayerId*& defender,
             std::map<const PlayerId*, Player*>& players,
             std::vector<GameObserver*>& gameObservers,
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

void Round::play()
{
    // deal cards
    dealCards();

    const PlayerId* currentAttackerId = mAttackers[0];

    CardSet tableCards;

    CardSet attackCards;
    CardSet& defenderCards = mPlayersCards[mDefender];

    Player& defender = *mPlayers[mDefender];

    unsigned int passedCounter = 0;

    while (!(attackCards = Rules::getAttackCards(tableCards, mPlayersCards[currentAttackerId])).empty()) {

        Player& currentAttacker = *mPlayers[currentAttackerId];
        const Card* attackCardPtr;

        if (tableCards.empty()) {
            attackCardPtr = &currentAttacker.attack(mDefender, attackCards);
        } else {
            attackCardPtr = currentAttacker.pitch(mDefender, attackCards);
            if (attackCardPtr) {
                passedCounter = 0;
            } else {
                // "pass"
                if (++passedCounter == mAttackers.size()) {
                    // all attackers "passed" - round ended - defend succeeded
                    return;
                }
            }
        }

        Card attackCard = *attackCardPtr;

        if(!attackCards.erase(attackCard)) {
            // invalid card returned - the card is not from attackCards
            if (attackCards.empty()) {
                // very unexpected
                return;
            }
            // take any card
            attackCard = *attackCards.begin();
        }

        mAttackCards.push_back(attackCard);
        tableCards.insert(attackCard);
        mPlayersCards[currentAttackerId].erase(attackCard);

        CardSet defendCards = Rules::getDefendCards(attackCard, defenderCards, mDeck.trumpSuit());

        const Card* defendCardPtr = defender.defend(currentAttackerId, defendCards);

        if(defendCards.empty()
                || !defendCardPtr
                || defendCards.find(*defendCardPtr) == defendCards.end()) {
            // defend failed
            defenderCards.insert(tableCards.begin(), tableCards.end());
            return;
        } else {
            mDefendCards.push_back(*defendCardPtr);
            tableCards.insert(*defendCardPtr);
            defenderCards.erase(*defendCardPtr);
        }

        currentAttackerId = Rules::pickNext(mAttackers, currentAttackerId);
    }
}

void Round::dealCards()
{
    // deal order:
    // 1. first attacker
    // 2. defender
    // 3. rest attackers
    std::vector<CardSet*> cards;
    cards.push_back(&mPlayersCards[mAttackers[0]]);
    cards.push_back(&mPlayersCards[mDefender]);
    for(std::vector<const PlayerId*>::iterator it = mAttackers.begin() + 1; it != mAttackers.end(); ++it) {
        cards.push_back(&mPlayersCards[*it]);
    }
    Rules::deal(mDeck, cards);
}

}

