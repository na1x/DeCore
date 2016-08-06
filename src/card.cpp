#include "card.h"

namespace decore {

Card::Card(const Suit &suit, const Rank &rank)
    : mSuit(suit), mRank(rank)
{
}

bool Card::operator ==(const Card &other) const
{
    return mRank == other.mRank && mSuit == other.mSuit;
}

bool Card::operator <(const Card &other) const
{
    return mSuit < other.mSuit || mRank < other.mRank;
}

const Rank& Card::rank() const
{
    return mRank;
}

const Suit& Card::suit() const
{
    return mSuit;
}

}
