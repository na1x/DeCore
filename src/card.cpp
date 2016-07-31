#include "card.h"

namespace decore {

Card::Card(Suit suit, Rank rank)
    : mSuit(suit), mRank(rank)
{
}

bool Card::operator ==(const Card &other) const
{
    return mRank == other.mRank && mSuit == other.mSuit;
}

Rank Card::rank() const
{
    return mRank;
}

Suit Card::suit() const
{
    return mSuit;
}

}
