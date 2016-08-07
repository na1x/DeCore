#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "gameObserver.h"

namespace decore
{

class PlayerId;
class CardSet;
class Card;

/**
 * @brief The player abstraction
 */
class Player: public GameObserver
{

public:
    virtual ~Player()
    {}

    /**
     * @brief The player gets its id.
     * @param id
     */
    virtual void idCreated(PlayerId* id) = 0;
    /**
     * @brief Makes first attack move
     *
     * The player is attacker and it makes first move against `playerId`.
     * The move is a mandatory move.
     * @param playerId defender player id
     * @param cardSet available cards
     * @return Card from the `cardSet`
     */
    virtual const Card& attack(PlayerId* playerId, const CardSet& cardSet) = 0;
    /**
     * @brief Makes extra attack move
     *
     * The player is attacker and it makes extra move against `playerId`.
     * The move is optional: to "pass" the move return NULL
     * @param playerId defender player id
     * @param cardSet available cards
     * @return Card from the `cardSet` if attack performed, NULL if attack ended
     */
    virtual const Card* pitch(PlayerId* playerId, const CardSet& cardSet) = 0;
    /**
     * @brief The player defends against atacker with playerId
     *
     * The move is optinal: to pick up the cards return NULL - this means that defend is failed.
     * Note: the method is invoked even if cardSet is empty - returned value ignored in the case
     * @param playerId attacker's player id
     * @param cardSet available cards
     * @return Card from the `cardSet` if index updated and defend performed, NULL means that defend is failed and table cards to be picked up by this player
     */
    virtual const Card* defend(PlayerId* playerId, const CardSet& cardSet) = 0;
    /**
     * @brief Notification from engine about updated cards
     * @param cardSet new cards
     */
    virtual void cardsUpdated(const CardSet& cardSet) = 0;
};

}

#endif // PLAYER_H_INCLUDED
