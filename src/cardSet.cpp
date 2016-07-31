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

void CardSet::shuffle()
{
    std::vector<Card> cards(mCards);
    mCards.clear();
    std::srand(std::time(NULL));
    while (!cards.empty()) {
        int index = std::rand() * (cards.size() - 1) / RAND_MAX;
        mCards.push_back(*cards.erase(cards.begin() + index));
    }
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

}
