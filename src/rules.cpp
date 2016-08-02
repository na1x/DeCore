#include <algorithm>


#include "rules.h"
#include "cardSet.h"

namespace decore {

CardSet Rules::getAttackCards(const CardSet& tableCards, const CardSet& playerCards)
{
    // TODO: implement
    (void)tableCards;
    return playerCards;
}

CardSet Rules::getDefendCards(const Card &card, const CardSet &playerCards)
{
    // TODO: implement
    (void)card;
    return playerCards;
}

PlayerId *Rules::pickNext(const std::vector<PlayerId*>& playersList, PlayerId* after)
{
    std::vector<PlayerId*>::const_iterator current = std::find(playersList.begin(), playersList.end(), after);

    if (playersList.end() == current) {
        return NULL;
    }

    std::vector<PlayerId*>::const_iterator next = current + 1;

    if (playersList.end() == next) {
        next = playersList.begin();
    }
    return *next;
}

}

