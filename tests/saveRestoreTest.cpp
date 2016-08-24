#include "saveRestoreTest.h"
#include "engine.h"
#include "deck.h"
#include "card.h"
#include "gameTest.h"
#include "gameCardsTracker.h"

using namespace decore;

void SaveRestoreTest::BasePlayer::idCreated(const PlayerId* id)
{
    (void)id;
}

const Card& SaveRestoreTest::BasePlayer::attack(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    return *cardSet.begin();
}

const Card* SaveRestoreTest::BasePlayer::pitch(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
    return NULL;
}

const Card* SaveRestoreTest::BasePlayer::defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
    (void) attackCard;
    return NULL;
}

void SaveRestoreTest::BasePlayer::cardsUpdated(const CardSet& cardSet)
{
    (void) cardSet;
}

void SaveRestoreTest::BasePlayer::gameStarted(const Suit& trumpSuit, const CardSet& cardSet, const std::vector<const PlayerId*>& players)
{
    (void) trumpSuit;
    (void) cardSet;
    (void) players;
}

void SaveRestoreTest::BasePlayer::roundStarted(unsigned int roundIndex, const std::vector<const PlayerId*> attackers, const PlayerId* defender)
{
    (void) roundIndex;
    (void) attackers;
    (void) defender;
}

void SaveRestoreTest::BasePlayer::roundEnded(unsigned int roundIndex)
{
    (void) roundIndex;
}

void SaveRestoreTest::BasePlayer::cardsPickedUp(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
}

void SaveRestoreTest::BasePlayer::cardsDealed(const PlayerId* playerId, unsigned int cardsAmount)
{
    (void) playerId;
    (void) cardsAmount;
}

void SaveRestoreTest::BasePlayer::cardsLeft(const CardSet& cardSet)
{
    (void) cardSet;
}

void SaveRestoreTest::BasePlayer::cardsDropped(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    (void) cardSet;
}

void SaveRestoreTest::test00()
{
    Engine engine;

    BasePlayer player0, player1;

    engine.add(player0);
    engine.add(player1);

    GameCardsTracker observer;

    engine.addGameObserver(observer);

    Deck deck;

    deck.push_back(Card(SUIT_CLUBS, RANK_6));
    deck.push_back(Card(SUIT_CLUBS, RANK_7));

    engine.setDeck(deck);

    engine.playRound();
}