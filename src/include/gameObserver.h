#ifndef GAMEOBSERVER_H_INCLUDED
#define GAMEOBSERVER_H_INCLUDED

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

    /**
     * @brief Some player picked up cards (defend failed)
     * @param playerId player id
     * @param cardSet cards
     */
    virtual void cardsReceived(const PlayerId& playerId, const CardSet& cardSet) = 0;
    /**
     * @brief Some player received card from the deck
     * @param playerId player
     * @param cardsAmount cards amount
     */
    virtual void cardsReceived(const PlayerId& playerId, int cardsAmount) = 0;
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
    virtual void cardsDropped(const PlayerId& playerId, const CardSet& cardSet) = 0;
};

}

#endif // GAMEOBSERVER_H_INCLUDED
