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
     * The player is attacker and it makes first move against PlayerId
     * @param playerId
     * @param cardSet
     * @return index in CardSet
     */
    virtual int attack(const PlayerId& playerId, const CardSet& cardSet) = 0;
    /**
     * @brief Makes extra attack move
     *
     * The player is attacker and it makes extra move against PlayerId
     * @param playerId defender player id
     * @param cardSet available cards
     * @param index out parameter, index in CardSet
     * @return true if attack performed, false if attack ended
     */
    virtual bool attack(const PlayerId& playerId, const CardSet& cardSet, int& index) = 0;
    /**
     * @brief Defends
     * @param playerId attacker's player id
     * @param cardSet available cards
     * @param index out parameter, index in CardSet
     * @return true if index updated and defend performed, false means that defend is failed and table cards to be picked up
     */
    virtual bool defend(const PlayerId& playerId, const CardSet& cardSet, int& index) = 0;
    /**
     * @brief Notification from engine about updated cards
     * @param cardSet new cards
     */
    virtual void cardsUpdated(const CardSet& cardSet) = 0;
};

}

#endif // PLAYER_H_INCLUDED
