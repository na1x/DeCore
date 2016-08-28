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
 *
 * About player's cards:
 * The cards maintained by engine and there's no way(and no need) for the player to get info about all its cards.
 * To track the cards:
 * - remember list of the cards in Player::cardsUpdated()
 * - remove cards from the list in Player::attack(), Player::pitch() and Player::defend()
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
    virtual void idCreated(const PlayerId* id) = 0;
    /**
     * @brief Makes first attack move
     *
     * The player is attacker and it makes first move against `playerId`.
     * The move is a mandatory move.
     * @param playerId defender player id
     * @param cardSet available cards
     * @return Card from the `cardSet`
     */
    virtual const Card& attack(const PlayerId* playerId, const CardSet& cardSet) = 0;
    /**
     * @brief Makes extra attack move
     *
     * The player is attacker and it makes extra move against `playerId`.
     * The move is optional: to "pass" the move return NULL
     * @param playerId defender player id
     * @param cardSet available cards
     * @return Card from the `cardSet` if attack performed, NULL if attack ended
     */
    virtual const Card* pitch(const PlayerId* playerId, const CardSet& cardSet) = 0;
    /**
     * @brief The player defends against attacker with playerId
     *
     * The move is optional: to pick up the cards return NULL - this means that defend is failed.
     * Note: the method is invoked even if cardSet is empty - returned value ignored in the case
     * @param playerId attacker's player id
     * @param attackCard card to beat
     * @param cardSet available cards
     * @return Card from the `cardSet` if index updated and defend performed, NULL means that defend is failed and table cards to be picked up by this player
     */
    virtual const Card* defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet) = 0;
    /**
     * @brief Notification from engine about updated cards
     *
     * Invoked when user receives cards:
     * - from the deck
     * - from the table (grabbed)
     *
     * For example: the player has 5 cards, after the deal he will have 6 cards; for this case the method will be invoked with `cardSet`.size == 6, which includes 5 "old" cards and 1 new card.
     * @param cardSet full set of the cards the player have.
     */
    virtual void cardsUpdated(const CardSet& cardSet) = 0;
    /**
     * @brief Notification from engine about restored cards
     *
     * Invoked during restoring game
     * @param cards cards
     */
    virtual void cardsRestored(const CardSet& cards) = 0;
};

}

#endif // PLAYER_H_INCLUDED
