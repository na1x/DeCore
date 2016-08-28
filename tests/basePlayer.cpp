#include <cstddef>

#include "basePlayer.h"
#include "cardSet.h"

using namespace decore;

void BasePlayer::idCreated(const PlayerId* id)
{
    (void)id;
}

const Card& BasePlayer::attack(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    return *cardSet.begin();
}

const Card* BasePlayer::pitch(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
    return cardSet.empty() ? NULL : &*cardSet.begin();
}

const Card* BasePlayer::defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
    (void) attackCard;
    return cardSet.empty() ? NULL : &*cardSet.begin();
}

void BasePlayer::cardsUpdated(const CardSet& cardSet)
{
    (void) cardSet;
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

void BasePlayer::cardsLeft(const CardSet& cardSet)
{
    (void) cardSet;
}

void BasePlayer::cardsDropped(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
}

void BasePlayer::tableCardsRestored(const std::vector<Card>& attackCards, const std::vector<Card>& defendCards)
{
    (void) attackCards;
    (void) defendCards;
}

void BasePlayer::write(DataWriter& writer)
{
    // just save something
    int value = 0;
    writer.write(value);
}

void BasePlayer::init(DataReader& reader)
{
    // just read previously saved
    int value;
    reader.read(value);
}

