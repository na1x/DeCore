#ifndef DECKTRACKER_H
#define DECKTRACKER_H

#include "gameObserver.h"
#include "cardSet.h"
#include "playerIds.h"
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
     * @brief Default ctor
     */
    PlayerCards();
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
    /**
     * @brief Returns amount of unknown cards
     * @return amount of unknown cards
     */
    unsigned int unknownCards() const;
    /**
     * @brief Returns known cards
     * @return known cards
     */
    const CardSet& knownCards() const;
};

/**
 * @brief Game tracker.
 *
 * The class gathers information about game cards which is attentive game observer can collect,
 * so the main goal of the class is a helper for the game bots.
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
    /**
     * @brief Player ids in the game
     */
    PlayerIds mPlayerIds;
    /**
     * @brief The cards which left the game
     */
    CardSet mGoneCards;
    /**
     * @brief Last round index
     */
    unsigned int mLastRoundIndex;
#ifndef NDEBUG
    /**
     * @brief For internal validation
     */
    std::map<const PlayerId*, unsigned int> mRestoredPlayerCards;
    /**
     * @brief For internal validation
     */
    std::vector<Card> mRestoredAttackCards;
    /**
     * @brief For internal validation
     */
    std::vector<Card> mRestoredDefendCards;
#endif // NDEBUG
public:
    GameCardsTracker();

    void gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *>& players);
    void roundStarted(unsigned int roundIndex, const std::vector<const PlayerId *> attackers, const PlayerId *defender);
    void roundEnded(unsigned int roundIndex);
    void cardsPickedUp(const PlayerId *playerId, const CardSet &cardSet);
    void cardsDealed(const PlayerId *playerId, unsigned int cardsAmount);
    void cardsGone(const CardSet &cardSet);
    void cardsDropped(const PlayerId *playerId, const CardSet &cardSet);
    void save(DataWriter& writer);
    void init(DataReader& reader);
    void gameRestored(const std::vector<const PlayerId*>& playerIds,
        const std::map<const PlayerId*, unsigned int>& playersCards,
        unsigned int deckCards,
        const Suit& trumpSuit,
        const std::vector<Card>& attackCards,
        const std::vector<Card>& defendCards);
    void quit();

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

    /**
     * @brief Returns player ids
     * @return player ids
     */
    const PlayerIds& playerIds() const;

    /**
     * @brief Returns gone cards
     * @return gone cards
     */
    const CardSet& goneCards() const;

    /**
     * @brief Returns last round index
     * @return last round index
     */
    unsigned int lastRoundIndex() const;

    /**
     * @brief Returns mAttackCards
     * @return cards
     */
    const std::vector<Card>& attackCards() const;

    /**
     * @brief Returns mDefendCards
     * @return cards
     */
    const std::vector<Card>& defendCards() const;
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
