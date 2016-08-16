#ifndef ROUND_H
#define ROUND_H

#include <vector>
#include <map>

#include "card.h"
#include "deck.h"
#include "cardSet.h"

namespace decore {

class PlayerId;
class GameObserver;
class Player;

/**
 * @brief One game round
 *
 * The class is related to Engine only and could be it's part undoubtedly. But separated to keep Engine simple and avoid unwished dependencies inside the engine.
 */
class Round
{
    /**
     * @brief List of attacker ids
     */
    const std::vector<const PlayerId*> mAttackers;
    /**
     * @brief Defender's id
     */
    const PlayerId* mDefender;
    /**
     * @brief Reference to playerId = player map
     */
    std::map<const PlayerId*, Player*>& mPlayers;
    /**
     * @brief GameObservers list
     */
    const std::vector<GameObserver*>& mGameObservers;
    /**
     * @brief Reference to dek
     */
    Deck& mDeck;
    /**
     * @brief Cards per playerId
     */
    std::map<const PlayerId*, CardSet>& mPlayersCards;

public:
    /**
     * @brief Ctor
     * @param attackers reference to attackers ids
     * @param defender defender's id
     * @param players all players by player id map
     * @param gameObservers list of game observers
     * @param deck reference to deck
     * @param playersCards reference to "player card by player id" map
     */
    Round(const std::vector<const PlayerId*>& attackers,
          const PlayerId*& defender,
          std::map<const PlayerId*, Player*>& players,
          const std::vector<GameObserver*>& gameObservers,
          Deck& deck,
          std::map<const PlayerId*, CardSet>& playersCards);

    /**
     * @brief Plays round
     */
    void play();

private:

    /**
     * @brief std::for_each function
     */
    class CardsAmountReceivedNotification
    {
        const PlayerId* mPlayerId;
        const unsigned int mCardsReceived;
    public:
        CardsAmountReceivedNotification(const PlayerId* playerId, unsigned int cardsReceived);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief std::for_each function
     */
    class CardsReceivedNotification
    {
        const PlayerId* mPlayerId;
        const CardSet& mReceivedCards;
    public:
        CardsReceivedNotification(const PlayerId* playerId, const CardSet& receivedCards);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief std::for_each function
     */
    class CardsDroppedNotification
    {
        const PlayerId* mPlayerId;
        CardSet mDroppedCards;
    public:
        CardsDroppedNotification(const PlayerId* playerId, const Card& droppedCard);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief Deals cards
     */
    void dealCards();
};

}


#endif // ROUND_H
