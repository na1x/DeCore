#include <map>
#include <cstddef>
#include <ctime>
#include <cstdlib>
#include <cassert>

#include "deck.h"

namespace decore {

void Deck::push_back(const Card &card)
{
    std::vector<Card>::push_back(card);
    mTrumpSuit = card.suit();
}

void Deck::generate(const Rank *ranks, unsigned int ranksSize, const Suit *suits, unsigned int suitsSize)
{
    clear();
    for(unsigned int suitIndex = 0; suitIndex < suitsSize; ++suitIndex) {
        for(unsigned int rankIndex = 0; rankIndex < ranksSize; ++rankIndex) {
            push_back(Card(suits[suitIndex], ranks[rankIndex]));
        }
    }
    if (!empty()) {
        mTrumpSuit = (end() - 1)->suit();
    }
}

unsigned int Deck::shuffle()
{
    std::vector<Card> cards(*this);
    std::vector<Card> origin(*this);
    clear();
    std::srand(std::time(NULL));
    while (!cards.empty()) {
        unsigned int index = std::rand() * (cards.size() - 1) / RAND_MAX;
        assert(index < cards.size());
        Card card = cards[index];
        cards.erase(cards.begin() + index);
        push_back(card);
    }

    if (!empty()) {
        mTrumpSuit = (end() - 1)->suit();
    }

    unsigned int notShuffledCards = 0;

    for(unsigned int i = 0, end = size(); i < end; ++i) {
        if (origin.at(i) == at(i)) {
            notShuffledCards++;
        }
    }
    return notShuffledCards;
}

const Suit& Deck::trumpSuit() const
{
    return mTrumpSuit;
}

void Deck::setTrumpSuit(const Suit& trumpSuit)
{
    mTrumpSuit = trumpSuit;
}

}
