#ifndef CARD_H_INCLUDED
#define CARD_H_INCLUDED

#include "suit.h"
#include "rank.h"

namespace decore {

/**
 * @brief The card class
 *
 * Game card class.
 */
class Card
{
    /**
     * @brief Suit of the card
     */
    Suit mSuit;
    /**
     * @brief Rank of the card
     */
    Rank mRank;

public:
    /**
     * @brief Card ctor
     * @param suit suit of the card
     * @param rank rank of the card
     */
    Card(Suit suit, Rank rank);
    /**
     * @brief operator ==
     *
     * Tests the card against other card
     * @param other other card instance
     * @return true if the cards equal
     */
    bool operator == (const Card& other) const;
    /**
     * @brief Rank getter
     * @return rank of the card
     */
    Rank rank() const;
    /**
     * @brief Suit getter
     * @return suit of the card
     */
    Suit suit() const;
};

}
#endif // CARD_H_INCLUDED
