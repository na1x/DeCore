#ifndef DECKTRACKER_H
#define DECKTRACKER_H

#include "gameObserver.h"
#include "cardSet.h"

namespace decore {

/**
 * Definition of player cards from external point of view.
 *
 * Cards in the class are:
 * - unknown, i.e. received from deck only amount of them is known
 * - known, the cards which player picked up from the table (when defend is failed)
 *
 */
class PlayerCards
{
    /**
     * Cards which are known for sure
     */
    CardSet mKnownCards;
    /**
     * Cards which can be guessed
     */
    unsigned int mUnknownCards;
public:
    /**
     * Adds unknown cards
     * @param cardsAmount number of cards
     */
    void addUnknownCards(unsigned int cardsAmount);
    /**
     * Adds known cards
     * @param cards cards
     */
    void addCards(const CardSet& cards);
    /**
     * Removes cards from the set
     *
     * First the card removed from known
     * @param cards cards to remove
     */
    void removeCards(const CardSet& cards);
    /**
     * Returns true if the set is empty
     * @return total number of cards
     */
    bool empty() const;
    /**
     * Returns card amount in the set
     * @return total number of cards
     */
    unsigned int size() const;
};

/**
 * Game tracker.
 *
 * The main goal of the class is a helper for the game bots.
 */
class GameCardsTracker : public GameObserver
{
    /**
     * Cards currently in game
     * @see gameCards()
     */
    CardSet mGameCards;
    /**
     * Trump suit
     * @see trumpSuit()
     */
    Suit mTrumpSuit;
    /**
     * Cards amount in the deck
     * @see gameCards
     */
    unsigned int mDeckCardsNumber;
    /**
     * Cards which player's have
     * @see playerCards()
     */
    std::map<const PlayerId*, PlayerCards> mPlayersCards;
public:
    void gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *>& players);
    void roundStarted(unsigned int roundIndex, const std::vector<const PlayerId *> attackers, const PlayerId *defender);
    void roundEnded(unsigned int roundIndex);
    void cardsPickedUp(const PlayerId *playerId, const CardSet &cardSet);
    void cardsDealed(const PlayerId *playerId, unsigned int cardsAmount);
    void cardsLeft(const CardSet &cardSet);
    void cardsDropped(const PlayerId *playerId, const CardSet &cardSet);

    /**
     * Returns cards in the game
     * @return cards
     */
    const CardSet& gameCards() const;
    /**
     * Returns trump suit
     * @return suit
     */
    const Suit& trumpSuit() const;
    /**
     * Returns size of the deck
     * @return cards amount in the deck
     */
    unsigned int deckCards() const;
    /**
     * Returns player cards
     * @param playerId player id
     * @return the `playerId` cards
     */
    const PlayerCards& playerCards(const PlayerId* playerId) const;
private:
    /**
     * Function for std::for_each
     */
    class CardRemover
    {
        CardSet& mCards;
    public:
        CardRemover(CardSet& cards);
        void operator()(const Card& card);
    };
};

}

#endif // DECKTRACKER_H
