#include <algorithm>

#include "rules.h"
#include "cardSet.h"
#include "deck.h"

namespace decore {

const unsigned int MAX_PLAYER_CARDS = 6;

Rules::AttackCardFilter::AttackCardFilter(const CardSet& playerCards, CardSet& result)
    : mPlayerCards(playerCards)
    , mResult(result)
{}

void Rules::AttackCardFilter::operator()(const Card& tableCard)
{
    mPlayerCards.getCards(tableCard.rank(), mResult);
}

CardSet Rules::getAttackCards(const CardSet& tableCards, const CardSet& playerCards)
{
    if (tableCards.empty()) {
        return playerCards;
    }

    CardSet result;
    std::for_each(tableCards.begin(), tableCards.end(), AttackCardFilter(playerCards, result));
    return result;
}

Rules::DefendCardFilter::DefendCardFilter(const Card& cardToBeat, const Suit& trump, CardSet& result)
    : mCardToBeat(cardToBeat)
    , mTrump(trump)
    , mResult(result)
{}

void Rules::DefendCardFilter::operator()(const Card& playerCard)
{
    if (mCardToBeat.suit() == playerCard.suit()) {
        // if suits are equal - take highest card
        if (mCardToBeat.rank() < playerCard.rank()) {
            mResult.insert(playerCard);
        }
    } else if (playerCard.suit() == mTrump) {
        // if player card is a trump - take it
        mResult.insert(playerCard);
    }
}

CardSet Rules::getDefendCards(const Card &card, const CardSet &playerCards, const Suit &trumpSuit)
{
    CardSet result;
    std::for_each(playerCards.begin(), playerCards.end(), DefendCardFilter(card, trumpSuit, result));
    return result;
}

const PlayerId *Rules::pickNext(const std::vector<const PlayerId*>& playersList, const PlayerId* after)
{
    std::vector<const PlayerId*>::const_iterator current = std::find(playersList.begin(), playersList.end(), after);

    if (playersList.end() == current) {
        return NULL;
    }

    std::vector<const PlayerId*>::const_iterator next = current + 1;

    if (playersList.end() == next) {
        next = playersList.begin();
    }
    return *next;
}

bool Rules::deal(Deck& deck, const std::vector<CardSet*> &cards)
{
    unsigned int cardsAmount = deck.size();

    while (!deck.empty()) {
        unsigned int playersToDeal = cards.size();
        for (std::vector<CardSet*>::const_iterator it = cards.begin(); it != cards.end(); ++it) {
            CardSet& playerCards = **it;
            if (playerCards.size() >= MAX_PLAYER_CARDS) {
                playersToDeal--;
                continue;
            }
            Card card = *deck.begin();
            playerCards.insert(card);
            deck.erase(deck.begin());
            if (deck.empty()) {
                break;
            }
        }
        if (!playersToDeal) {
            break;
        }
    }

    return cardsAmount != deck.size();
}

unsigned int Rules::maxAttackCards(unsigned int defenderCardsAmount)
{
    return std::min(defenderCardsAmount, MAX_PLAYER_CARDS);
}

}

