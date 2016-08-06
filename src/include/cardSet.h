#ifndef CARDSET_H
#define CARDSET_H

#include <set>
#include <map>
#include <vector>

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
class CardSet : public std::set<Card>
{

public:
    /**
     * @brief Constructs empty card set
     */
    CardSet();
    virtual ~CardSet();

    /**
     * @brief Adds the cards to the card set
     * @param cards cards to add
     * @return true if all cards added
     */
    bool addAll(const std::vector<Card>& cards);
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
};

}

#endif // CARDSET_H
