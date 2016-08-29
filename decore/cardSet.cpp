#include <algorithm>

#include "cardSet.h"

namespace decore {

CardSet::CardSet()
{
    //ctor
}

CardSet::~CardSet()
{
    //dtor
}

bool CardSet::addAll(const std::vector<Card> &cards)
{
    unsigned int oldSize = size();
    std::copy(cards.begin(), cards.end(), std::inserter(*this, begin()));
    return oldSize + cards.size() == size();
}

/**
 * @brief std::for_each function
 */
class RankFilter
{
    const Rank& mRank;
    CardSet& mCards;
public:
    RankFilter(const Rank &rank, CardSet& cards)
        : mRank(rank)
        , mCards(cards)
    {}

    void operator() (const Card& card)
    {
        if (card.rank() == mRank) {
           mCards.insert(card);
        }
    }
};

void CardSet::getCards(const Rank &rank, CardSet &cards) const
{
    std::for_each(begin(), end(), RankFilter(rank, cards));
}

/**
 * @brief std::for_each function
 */
class SuitFilter
{
    const Suit& mSuit;
    CardSet& mCards;
public:
    SuitFilter(const Suit& suit, CardSet& cards)
        : mSuit(suit)
        , mCards(cards)
    {}

    void operator() (const Card& card)
    {
        if (card.suit() == mSuit) {
            mCards.insert(card);
        }
    }
};

void CardSet::getCards(const Suit &suit, CardSet &cards) const
{
    std::for_each(begin(), end(), SuitFilter(suit, cards));
}

}
