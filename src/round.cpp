#include <cstddef>

#include "round.h"
#include "rules.h"
#include "cardSet.h"
#include "player.h"

namespace decore {

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

    PlayerId* currentAttacker = mAttackers.at(0);

    const CardSet& set = mPlayersCards[currentAttacker];

    CardSet tableCards;
    tableCards.addAll(mAttackCards);
    tableCards.addAll(mDefendCards);

    CardSet attackCards = Rules::getAttackCards(tableCards, set);

    Card attackCard = mPlayers[currentAttacker]->attack(mDefender, attackCards);

    if(!attackCards.erase(attackCard)) {
        // invalid card returned
        if (attackCards.empty()) {
            // very unexpected
            return;
        }
        attackCard = *attackCards.begin();
    }

    CardSet defendCards = Rules::getDefendCards(attackCard, set);

    Card* defendCard = mPlayers[mDefender]->defend(currentAttacker, defendCards);
    if(defendCard) {

    } else {

    }
}

}

