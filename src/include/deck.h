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
public:
    /**
     * @brief Generates card set
     * @param ranks card ranks for generator
     * @param ranksSize size of ranks
     * @param suits card suits for generator
     * @param suitsSize size of suits
     */
    void generate(const Rank *ranks, unsigned int ranksSize, const Suit *suits, unsigned int suitsSize);
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
};

}

#endif // DECK_H

