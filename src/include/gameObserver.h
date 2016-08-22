#ifndef GAMEOBSERVER_H_INCLUDED
#define GAMEOBSERVER_H_INCLUDED

#include <suit.h>
#include <vector>

namespace decore
{

class PlayerId;
class CardSet;

/**
 * @brief Game flow observer
 *
 * The interface notified about game flow events from "external" point of view.
 * - after deal it notified about how muh cards each player received
 * - on player attack/defend it "sees" which card player dropped
 * - etc
 *
 * Main purpose of the interface is UI update (with conjunction of local Player implementation).
 */
class GameObserver
{

public:
    virtual ~GameObserver()
    {}
    /**
     * @brief Game is started
     * @param trumpSuit trump suit
     * @param cardSet full game card set
     * @param players players in the game
     */
    virtual void gameStarted(const Suit& trumpSuit, const CardSet& cardSet, const std::vector<const PlayerId*>& players) = 0;
    /**
     * @brief New round started
     * @param roundIndex index of the started round
     * @param attackers list of attackers' ids
     * @param defender defender's id
     */
    virtual void roundStarted(unsigned int roundIndex, const std::vector<const PlayerId*> attackers, const PlayerId* defender) = 0;
    /**
     * @brief Current round ended
     * @param roundIndex index of the ended round
     */
    virtual void roundEnded(unsigned int roundIndex) = 0;
    /**
     * @brief Some player picked up cards (defend failed)
     * @param playerId player id
     * @param cardSet cards
     */
    virtual void cardsPickedUp(const PlayerId* playerId, const CardSet& cardSet) = 0;
    /**
     * @brief Some player received card from the deck
     * @param playerId player
     * @param cardsAmount cards amount
     */
    virtual void cardsDealed(const PlayerId* playerId, unsigned int cardsAmount) = 0;
    /**
     * @brief Cards left game
     * @param cardSet cards
     */
    virtual void cardsLeft(const CardSet& cardSet) = 0;
    /**
     * @brief Player dropped cards to the table either during attack or defend
     * @param playerId player id
     * @param cardSet cards
     */
    virtual void cardsDropped(const PlayerId* playerId, const CardSet& cardSet) = 0;
};

}

#endif // GAMEOBSERVER_H_INCLUDED
