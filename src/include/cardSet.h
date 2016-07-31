#ifndef CARDSET_H
#define CARDSET_H

#include <vector>
#include <map>

#include "card.h"
#include "playerId.h"

namespace decore
{

class CardSet
{

private:
    std::vector<Card> mCards;

public:
    CardSet();
    virtual ~CardSet();

    void add(const Card& card);
    unsigned int size() const;
    bool empty() const;
    bool deal(std::map<PlayerId*, CardSet>&, unsigned int);
    void shuffle();
    void generate(const Rank *ranks, unsigned int ranksSize, const Suit *suits, unsigned int suitsSize);
    const Card* get(unsigned int index) const;
    bool operator == (const CardSet& other) const;
};

}

#endif // CARDSET_H
