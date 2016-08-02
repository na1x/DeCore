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
             CardSet& deck,
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

    CardSet attackCards = Rules::getAttackCards(mTableCards, set);

    unsigned int attackIndex = mPlayers[currentAttacker]->attack(mDefender, attackCards);

    // fix index
    if (attackIndex >= attackCards.size()) {
        attackIndex = 0;
    }

    CardSet defendCards = Rules::getDefendCards(*attackCards.get(attackIndex), set);

    unsigned int defendIndex = -1;

    if(mPlayers[mDefender]->defend(currentAttacker, defendCards, defendIndex)) {

    } else {

    }
}

}

