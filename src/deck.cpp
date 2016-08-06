#include <map>
#include <cstddef>
#include <ctime>
#include <cstdlib>

#include "deck.h"

namespace decore {

bool Deck::deal(std::map<PlayerId *, CardSet> &playersCards, unsigned int maxCards)
{
    unsigned int cardsAmount = size();

    for (std::map<PlayerId *, CardSet>::iterator it = playersCards.begin(); it != playersCards.end() && !empty(); ++it) {
        CardSet& playerCards = it->second;
        if (playerCards.size() >= maxCards) {
            continue;
        }
        playerCards.insert(*erase(begin()));
    }

    return cardsAmount != size();
}

void Deck::generate(const Rank *ranks, unsigned int ranksSize, const Suit *suits, unsigned int suitsSize)
{
    clear();
    for(unsigned int suitIndex = 0; suitIndex < suitsSize; ++suitIndex) {
        for(unsigned int rankIndex = 0; rankIndex < ranksSize; ++rankIndex) {
            push_back(Card(suits[suitIndex], ranks[rankIndex]));
        }
    }
}

unsigned int Deck::shuffle()
{
    std::vector<Card> cards(*this);
    std::vector<Card> origin(*this);
    clear();
    std::srand(std::time(NULL));
    while (!cards.empty()) {
        int index = std::rand() * (cards.size() - 1) / RAND_MAX;
        push_back(*cards.erase(cards.begin() + index));
    }

    unsigned int notShuffledCards = 0;

    for(unsigned int i = 0, end = size(); i < end; ++i) {
        if (origin.at(i) == at(i)) {
            notShuffledCards++;
        }
    }
    return notShuffledCards;
}

}
