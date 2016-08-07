#ifndef RULES_H
#define RULES_H

#include <vector>

#include "card.h"

namespace decore {

class CardSet;
class PlayerId;

/**
 * @brief Game rules
 *
 * To avoid populating game rules all over namespace's classes this class is designed as a game rules global junk.
 * Game rules is not intended to change so static reference looks OK.
 */
class Rules {
public:
    /**
     * @brief Returns possible cards for attack move
     *
     * Return all playerCards if tableCards empty
     * @param tableCards cards on the game table
     * @param playerCards attacker's cards
     * @return possible cards
     */
    static CardSet getAttackCards(const CardSet& tableCards, const CardSet& playerCards);
    /**
     * @brief Returns possible cards for defend move
     * @param card card to beat
     * @param playerCards defender's cards
     * @param trumpSuit trump suit
     * @return possible cards
     */
    static CardSet getDefendCards(const Card& card, const CardSet& playerCards, const Suit& trumpSuit);
    /**
     * @brief Returns next player in the player queue after the player
     * @param playersList list of the player
     * @param after defines player id
     * @return next player id
     */
    static PlayerId* pickNext(const std::vector<PlayerId*>& playersList, PlayerId* after);
};

}

#endif // RULES_H
