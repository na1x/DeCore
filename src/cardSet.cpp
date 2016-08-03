#include <cstdlib>
#include <ctime>

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

void CardSet::add(const Card& card)
{
    mCards.push_back(card);
}

unsigned int CardSet::size() const
{
    return mCards.size();
}

bool CardSet::empty() const
{
    return mCards.empty();
}

bool CardSet::deal(std::map<PlayerId *, CardSet> &playersCards, unsigned int maxCards)
{
    if (mCards.empty()) {
        return false;
    }

    unsigned int cardsAmount = mCards.size();

    for (std::map<PlayerId *, CardSet>::iterator it = playersCards.begin(); it != playersCards.end(); ++it) {
        CardSet& playerCards = it->second;
        if (playerCards.size() >= maxCards) {
            continue;
        }
        playerCards.add(*mCards.erase(mCards.begin()));
        if (mCards.empty()) {
            break;
        }
    }

    return cardsAmount != mCards.size();
}

unsigned int CardSet::shuffle()
{
    std::vector<Card> cards(mCards);
    std::vector<Card> origin(mCards);
    mCards.clear();
    std::srand(std::time(NULL));
    while (!cards.empty()) {
        int index = std::rand() * (cards.size() - 1) / RAND_MAX;
        mCards.push_back(*cards.erase(cards.begin() + index));
    }

    unsigned int notShuffledCards = 0;

    for(unsigned int i = 0, end = mCards.size(); i < end; ++i) {
        if (origin.at(i) == mCards.at(i)) {
            notShuffledCards++;
        }
    }
    return notShuffledCards;
}

void CardSet::generate(const Rank *ranks, unsigned int ranksSize, const Suit *suits, unsigned int suitsSize)
{
    mCards.clear();
    for(unsigned int suitIndex = 0; suitIndex < suitsSize; ++suitIndex) {
        for(unsigned int rankIndex = 0; rankIndex < ranksSize; ++rankIndex) {
            mCards.push_back(Card(suits[suitIndex], ranks[rankIndex]));
        }
    }
}

const Card* CardSet::get(unsigned int index) const
{
    return index < mCards.size() ? &mCards[index] : NULL;
}

bool CardSet::operator ==(const CardSet &other) const
{
    return mCards == other.mCards;
}

void CardSet::getCards(const Rank &rank, CardSet &cards) const
{
    class RankFilter : public Condition
    {
        const Rank& mRank;
    public:
        RankFilter(const Rank &rank)
            : mRank(rank)
        {}

        bool test(const Card& card) const
        {
            return card.rank() == mRank;
        }
    };

    filter((RankFilter)rank, cards);
}

void CardSet::getCards(const Suit &suit, CardSet &cards) const
{
    class SuitFilter : public Condition
    {
        const Suit& mSuit;
    public:
        SuitFilter(const Suit& suit)
            : mSuit(suit)
        {}

        bool test(const Card& card) const
        {
            return card.suit() == mSuit;
        }
    };

    filter((SuitFilter)suit, cards);
}

void CardSet::intersect(const CardSet &with, CardSet &cards) const
{
    class Filter : public Condition
    {
        const Card& mCard;
    public:
        Filter(const Card& card)
            : mCard(card)
        {}

        bool test(const Card& card) const
        {
            return card.suit() == mCard.suit() || card.rank() == mCard.rank();
        }
    };

    for(std::vector<Card>::const_iterator it = mCards.begin(); it != mCards.end(); ++it) {
        with.filter((Filter)*it, cards);
    }
}

void CardSet::filter(const CardSet::Condition &condition, CardSet& cards) const
{
    for(std::vector<Card>::const_iterator it = mCards.begin(); it != mCards.end(); ++it) {
        if (condition.test(*it)) {
            cards.add(*it);
        }
    }
}

}
