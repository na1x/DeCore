#ifndef CARDSET_H
#define CARDSET_H

#include <vector>
#include <map>

#include "card.h"
#include "playerId.h"

namespace decore
{

/**
 * @brief The card set
 *
 * - Initially created empty could be filled with cards, see generate()
 * - Any amount of cards could be added with add()
 */
class CardSet
{
    /**
     * @brief Abstract condition
     */
    class Condition {
    public:
        /**
         * @brief Tests the card
         * @param card card to test
         * @return true if condition passed
         */
        virtual bool test(const Card& card) const = 0;
    };

    /**
     * Cards in the set
     *
     * TODO: replace with std::set
     */
    std::vector<Card> mCards;

public:
    /**
     * @brief Constructs empty card set
     */
    CardSet();
    virtual ~CardSet();

    /**
     * @brief Adds the card to the card set
     *
     * Note: The `card` is just added without check if same card already exist in the set
     * @param card card to add
     */
    void add(const Card& card);
    /**
     * @brief Returns cards amount in the set
     * @return cards amount
     * @see empty()
     */
    unsigned int size() const;
    /**
     * @brief Tests if the card set is empty
     * @return true if the card set is empty
     */
    bool empty() const;
    /**
     * @brief Deals cards from the set to the players up to maxCards
     *
     * If any player already has maxCards then its card set won't be updated
     * @param playersCards set of cards to update
     * @param maxCards max target amount of player cards
     * @return true if any card set is updated
     */
    bool deal(std::map<PlayerId *, CardSet> &playersCards, unsigned int maxCards);
    /**
     * @brief Shuffles cards in the set
     * @return amount of not shuffled cards
     */
    unsigned int shuffle();
    /**
     * @brief Generates card set
     * @param ranks card ranks for generator
     * @param ranksSize size of ranks
     * @param suits card suits for generator
     * @param suitsSize size of suits
     */
    void generate(const Rank *ranks, unsigned int ranksSize, const Suit *suits, unsigned int suitsSize);
    /**
     * @brief Retuns ptr to the card at index
     * @param index card index
     * @return Card ptr or null
     */
    const Card* get(unsigned int index) const;
    /**
     * @brief operator ==
     * @param other other card set
     * @return true if equal
     */
    bool operator == (const CardSet& other) const;
    /**
     * @brief Appends all cards with the `rank` from the card set to `cards`
     * @param rank rank
     * @param cards destination card set
     */
    void getCards(const Rank& rank, CardSet& cards) const;
    /**
     * @brief Appends all cards with the `suit` from the card set to `cards`
     * @param suit suit
     * @param cards destination card set
     */
    void getCards(const Suit& suit, CardSet& cards) const;
    /**
     * @brief Appends cards from `with` to `cards` only if in this set exists card with same suit OR rank
     *
     * @param with card set to "intersect" with
     * @param cards destination card set
     */
    void intersect(const CardSet& with, CardSet& cards) const;

private:
    void filter(const Condition& condition, CardSet& cards) const;
};

}

#endif // CARDSET_H
