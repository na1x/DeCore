#ifndef DECKTRACKER_H
#define DECKTRACKER_H

#include "gameObserver.h"
#include "cardSet.h"

namespace decore {

/**
 * @brief Definition of player cards from external point of view.
 *
 * Cards in the class are:
 * - unknown, i.e. received from deck only amount of them is known
 * - known, the cards which player picked up from the table (when defend is failed)
 *
 */
class PlayerCards
{
    /**
     * @brief Cards which are known for sure
     */
    CardSet mKnownCards;
    /**
     * @brief Cards which can be guessed
     */
    unsigned int mUnknownCards;
public:
    /**
     * @brief Adds unknown cards
     * @param cardsAmount number of cards
     */
    void addUnknownCards(unsigned int cardsAmount);
    /**
     * @brief Adds known cards
     * @param cards cards
     */
    void addCards(const CardSet& cards);
    /**
     * @brief Removes cards from the set
     *
     * First the card removed from known
     * @param cards cards to remove
     */
    void removeCards(const CardSet& cards);
    /**
     * @brief Returns true if the set is empty
     * Returns true if the set is empty
     * @return total number of cards
     */
    bool empty() const;
    /**
     * @brief Returns card amount in the set
     * @return total number of cards
     */
    unsigned int size() const;
};

/**
 * @brief Game tracker.
 *
 * The main goal of the class is a helper for the game bots.
 */
class GameCardsTracker : public GameObserver
{
    /**
     * @brief Cards currently in game
     * @see gameCards()
     */
    CardSet mGameCards;
    /**
     * @brief Trump suit
     * @see trumpSuit()
     */
    Suit mTrumpSuit;
    /**
     * @brief Cards amount in the deck
     * @see gameCards
     */
    unsigned int mDeckCardsNumber;
    /**
     * @brief Cards which player's have
     * @see playerCards()
     */
    std::map<const PlayerId*, PlayerCards> mPlayersCards;
    /**
     * @brief Table cards
     */
    std::vector<Card> mAttackCards;
    /**
     * @brief Table cards
     */
    std::vector<Card> mDefendCards;
    /**
     * @brief Current round: attackers
     */
    std::vector<const PlayerId*> mAttackers;
    /**
     * @brief Current round: defender
     */
    const PlayerId* mDefender;
public:
    void gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *>& players);
    void roundStarted(unsigned int roundIndex, const std::vector<const PlayerId *> attackers, const PlayerId *defender);
    void roundEnded(unsigned int roundIndex);
    void cardsPickedUp(const PlayerId *playerId, const CardSet &cardSet);
    void cardsDealed(const PlayerId *playerId, unsigned int cardsAmount);
    void cardsLeft(const CardSet &cardSet);
    void cardsDropped(const PlayerId *playerId, const CardSet &cardSet);
    void tableCardsRestored(const std::vector<Card>& attackCards, const std::vector<Card>& defendCards);
    void write(DataWriter& writer);
    void init(DataReader& reader);

    /**
     * @brief Returns cards in the game
     * @return cards
     */
    const CardSet& gameCards() const;
    /**
     * @brief Returns trump suit
     * @return suit
     */
    const Suit& trumpSuit() const;
    /**
     * @brief Returns size of the deck
     * @return cards amount in the deck
     */
    unsigned int deckCards() const;
    /**
     * @brief Returns player cards
     * @param playerId player id
     * @return the `playerId` cards
     */
    const PlayerCards& playerCards(const PlayerId* playerId) const;
private:
    /**
     * @brief Function for std::for_each
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
