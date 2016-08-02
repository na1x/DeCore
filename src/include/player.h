#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "gameObserver.h"

namespace decore
{

class PlayerId;
class CardSet;

/**
 * @brief The player abstraction
 */
class Player: public GameObserver
{

public:
    virtual ~Player()
    {}

    /**
     * @brief Makes first attack move
     *
     * The player is attacker and it makes first move against PlayerId.
     * The move is a mandatory move.
     * @param playerId defender player id
     * @param cardSet available cards
     * @return index in CardSet, if index out of bounds CardSet then zero index assumed
     */
    virtual unsigned int attack(PlayerId* playerId, const CardSet& cardSet) = 0;
    /**
     * @brief Makes extra attack move
     *
     * The player is attacker and it makes extra move against PlayerId.
     * The move is optional: to "pass" the move return 'false'
     * @param playerId defender player id
     * @param cardSet available cards
     * @param index out parameter, index in CardSet
     * @return true if attack performed, false if attack ended
     */
    virtual bool attack(PlayerId* playerId, const CardSet& cardSet, unsigned int& index) = 0;
    /**
     * @brief The player defends against atacker with playerId
     *
     * The move is optinal: to pick up the cards return 'false' - this means that defend is failed.
     * @param playerId attacker's player id
     * @param cardSet available cards
     * @param index out parameter, index in CardSet
     * @return true if index updated and defend performed, false means that defend is failed and table cards to be picked up
     */
    virtual bool defend(PlayerId* playerId, const CardSet& cardSet, unsigned int& index) = 0;
    /**
     * @brief Notification from engine about updated cards
     * @param cardSet new cards
     */
    virtual void cardsUpdated(const CardSet& cardSet) = 0;
};

}

#endif // PLAYER_H_INCLUDED
