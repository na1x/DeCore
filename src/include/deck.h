#ifndef DECK_H
#define DECK_H

#include <vector>
#include <map>

#include "cardSet.h"

namespace decore {

class PlayerId;

/**
 * @brief The deck
 */
class Deck : public std::vector<Card>
{
    Suit mTrumpSuit;

public:
    /**
     * @brief Append card to the end of the deck
     *
     * Trump suit of the deck is updated to the suit of the card
     * @param card card to append
     */
    void push_back(const Card& card);
    /**
     * @brief Generates card set
     *
     * Trump suit is taken from the last card.
     * @param ranks card ranks for generator
     * @param ranksSize size of ranks
     * @param suits card suits for generator
     * @param suitsSize size of suits
     */
    void generate(const Rank *ranks, unsigned int ranksSize, const Suit *suits, unsigned int suitsSize);
    /**
     * @brief Shuffles cards in the set
     * @return amount of not shuffled cards
     */
    unsigned int shuffle();
    /**
     * @brief Returns trump suit
     * @return trump suit
     */
    const Suit& trumpSuit() const;
    /**
     * @brief Updates trump suit for the deck
     *
     * Note: the trump suit also updated in generate() and push_back()
     */
    void setTrumpSuit(const Suit& trumpSuit);
};

}

#endif // DECK_H

