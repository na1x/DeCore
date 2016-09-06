#include <cstddef>
#include <cppunit/TestAssert.h>

#include "basePlayer.h"
#include "cardSet.h"

using namespace decore;

void BasePlayer::idCreated(const PlayerId* id)
{
    mId = id;
}

const Card& BasePlayer::attack(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    const Card& res = *cardSet.begin();
    removeCard(&res);
    return res;
}

const Card* BasePlayer::pitch(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;

    if (cardSet.empty()) {
        return NULL;
    }

    const Card* res = &*cardSet.begin();
    removeCard(res);
    return res;
}

const Card* BasePlayer::defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet)
{
    (void) playerId;
    (void) attackCard;

    if (cardSet.empty()) {
        return NULL;
    }

    const Card* res = &*cardSet.begin();
    removeCard(res);
    return res;
}

void BasePlayer::cardsUpdated(const CardSet& cardSet)
{
    mPlayerCards.push_back(cardSet);
}

void BasePlayer::cardsRestored(const CardSet& cards)
{
    (void) cards;
}

void BasePlayer::gameStarted(const Suit& trumpSuit, const CardSet& cardSet, const std::vector<const PlayerId*>& players)
{
    (void) trumpSuit;
    (void) cardSet;
    (void) players;
}

void BasePlayer::roundStarted(unsigned int roundIndex, const std::vector<const PlayerId*> attackers, const PlayerId* defender)
{
    (void) roundIndex;
    (void) attackers;
    (void) defender;
}

void BasePlayer::roundEnded(unsigned int roundIndex)
{
    (void) roundIndex;
}

void BasePlayer::cardsPickedUp(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
}

void BasePlayer::cardsDealed(const PlayerId* playerId, unsigned int cardsAmount)
{
    (void) playerId;
    (void) cardsAmount;
}

void BasePlayer::cardsGone(const CardSet& cardSet)
{
    (void) cardSet;
}

void BasePlayer::cardsDropped(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
}

void BasePlayer::gameRestored(const std::vector<const PlayerId*>& playerIds,
        const std::map<const PlayerId*, unsigned int>& playersCards,
        unsigned int deckCards,
        const Suit& trumpSuit,
        const std::vector<Card>& attackCards,
        const std::vector<Card>& defendCards)
{
    (void) playerIds;
    (void) playersCards;
    (void) deckCards;
    (void) trumpSuit;
    (void) attackCards;
    (void) defendCards;
}

void BasePlayer::save(DataWriter& writer)
{
    unsigned int cardSets = mPlayerCards.size();
    writer.write(cardSets);
    for (std::vector<CardSet>::iterator it = mPlayerCards.begin(); it != mPlayerCards.end(); ++it) {
        CardSet& cards = *it;
        writer.write(cards.begin(), cards.end());
    }
}

void BasePlayer::init(DataReader& reader)
{
    unsigned int cardSets;
    reader.read(cardSets);
    const Card defaultCards(SUIT_LAST, RANK_LAST);
    while (cardSets--) {
        CardSet cards;
        reader.read(cards, defaultCards);
        mPlayerCards.push_back(cards);
    }
}

void BasePlayer::quit()
{
    // do nothing
}

const PlayerId* BasePlayer::id() const
{
    return mId;
}

unsigned int BasePlayer::cardSets() const
{
    return mPlayerCards.size();
}


const CardSet& BasePlayer::cards(unsigned int index) const
{
    CPPUNIT_ASSERT(index < mPlayerCards.size());
    return mPlayerCards.at(index);
}

void BasePlayer::removeCard(const decore::Card* card)
{
    if (mPlayerCards.empty()) {
        return;
    }
    CardSet currentCards = *(mPlayerCards.end() - 1);
    currentCards.erase(*card);
    mPlayerCards.push_back(currentCards);
}
