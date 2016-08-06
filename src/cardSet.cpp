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
    for(std::vector<Card>::const_iterator it = cards.begin(); it != cards.end(); ++it) {
        if (!insert(*it).second) {
            return false;
        }
    }
    return true;
}

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

class CardFilter
{
    const Card& mCard;
    CardSet& mCards;
public:
    CardFilter(const Card& card, CardSet& cards)
        : mCard(card)
        , mCards(cards)
    {}

    void operator() (const Card& card)
    {
        if(card.suit() == mCard.suit() || card.rank() == mCard.rank()) {
            mCards.insert(card);
        }
    }
};

void CardSet::intersect(const CardSet &with, CardSet &cards) const
{
    for(std::set<Card>::const_iterator it = begin(); it != end(); ++it) {
        std::for_each(with.begin(), with.end(), CardFilter(*it, cards));
    }
}

}
