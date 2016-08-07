#include <cstddef>

#include "round.h"
#include "rules.h"
#include "player.h"

namespace decore {

const unsigned int MAX_PLAYER_CARDS = 6;

Round::Round(const std::vector<PlayerId*>& attackers,
             PlayerId*& defender,
             std::map<PlayerId*, Player*>& players,
             std::vector<GameObserver*>& gameObservers,
             Deck& deck,
             std::map<PlayerId*, CardSet>& playersCards)
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
    // 1. deal cards
    // 2. make moves
    // 3. till round complete
    // 4. check if game ended

    mDeck.deal(mPlayersCards, MAX_PLAYER_CARDS);

    PlayerId* currentAttackerId = mAttackers[0];

    mAttackers.at(0);

    CardSet tableCards;

    CardSet attackCards;

    Player& defender = *mPlayers[mDefender];

    while (!(attackCards = Rules::getAttackCards(tableCards, mPlayersCards[currentAttackerId])).empty()) {

        Player& currentAttacker = *mPlayers[currentAttackerId];
        Card attackCard = currentAttacker.attack(mDefender, attackCards);

        if(!attackCards.erase(attackCard)) {
            // invalid card returned
            if (attackCards.empty()) {
                // very unexpected
                return;
            }
            // take any card
            attackCard = *attackCards.begin();
        }

        mAttackCards.push_back(attackCard);

        CardSet defendCards = Rules::getDefendCards(attackCard, mPlayersCards[mDefender], mDeck.trumpSuit());

        const Card* defendCard = defender.defend(currentAttackerId, defendCards);

        if(defendCards.empty() || !defendCard || defendCards.find(*defendCard) == defendCards.end()) {
            mPlayersCards[mDefender].addAll(mAttackCards);
            mPlayersCards[mDefender].addAll(mDefendCards);
            return;
        } else {
            mDefendCards.push_back(*defendCard);
        }

        tableCards.insert(attackCard);
        tableCards.insert(*defendCard);

        currentAttackerId = Rules::pickNext(mAttackers, currentAttackerId);
    }
}

}

