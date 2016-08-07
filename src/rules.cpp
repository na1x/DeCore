#include <algorithm>

#include "rules.h"
#include "cardSet.h"

namespace decore {

class AttackCardFilter
{
    const CardSet& mPlayerCards;
    CardSet& mResult;
public:
    AttackCardFilter(const CardSet& playerCards, CardSet& result)
        : mPlayerCards(playerCards)
        , mResult(result)
    {}

    void operator()(const Card& tableCard)
    {
        mPlayerCards.getCards(tableCard.rank(), mResult);
    }
};

CardSet Rules::getAttackCards(const CardSet& tableCards, const CardSet& playerCards)
{
    if (tableCards.empty()) {
        return playerCards;
    }

    CardSet result;
    std::for_each(tableCards.begin(), tableCards.end(), AttackCardFilter(playerCards, result));
    return result;
}

class DefendCardFilter
{
    const Card& mCardToBeat;
    const Suit& mTrump;
    CardSet& mResult;

public:
    DefendCardFilter(const Card& cardToBeat, const Suit& trump, CardSet& result)
        : mCardToBeat(cardToBeat)
        , mTrump(trump)
        , mResult(result)
    {}

    void operator()(const Card& playerCard)
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
};

CardSet Rules::getDefendCards(const Card &card, const CardSet &playerCards, const Suit &trumpSuit)
{
    CardSet result;
    std::for_each(playerCards.begin(), playerCards.end(), DefendCardFilter(card, trumpSuit, result));
    return result;
}

PlayerId *Rules::pickNext(const std::vector<PlayerId*>& playersList, PlayerId* after)
{
    std::vector<PlayerId*>::const_iterator current = std::find(playersList.begin(), playersList.end(), after);

    if (playersList.end() == current) {
        return NULL;
    }

    std::vector<PlayerId*>::const_iterator next = current + 1;

    if (playersList.end() == next) {
        next = playersList.begin();
    }
    return *next;
}

}

