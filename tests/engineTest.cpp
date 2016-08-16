#include <algorithm>

#include "engineTest.h"
#include "engine.h"
#include "player.h"
#include "rules.h"

using namespace decore;

void EngineTest::testAddPlayers()
{
    Engine engine;

    // set used to check uniqueness of ids
    std::set<const PlayerId*> playerIds;

    Player* players[] = {
        new TestPlayer(),
        new TestPlayer(),
        new TestPlayer(),
        new TestPlayer(),
        NULL
    };

    // add players
    Player** playerPtr = players;
    while(*playerPtr) {
        const PlayerId* added = engine.add(**playerPtr);
        CPPUNIT_ASSERT(added);
        CPPUNIT_ASSERT(playerIds.insert(added).second);
        playerPtr++;
    }
}

void EngineTest::testAddDuplicatedPlayers()
{
    Engine engine;
    TestPlayer player;
    const PlayerId* id0, *id1;
    CPPUNIT_ASSERT(id0 = engine.add(player));
    CPPUNIT_ASSERT(id1 = engine.add(player));
    CPPUNIT_ASSERT(id0 != id1);
    std::vector<const PlayerId*>& ids = player.mIds;
    CPPUNIT_ASSERT(std::find(ids.begin(), ids.end(), id0) != ids.end());
    CPPUNIT_ASSERT(std::find(ids.begin(), ids.end(), id1) != ids.end());
}



void EngineTest::TestPlayer::gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *> players)
{
    (void)trumpSuit;
    (void)cardSet;
    (void)players;
}

void EngineTest::TestPlayer::cardsLeft(const CardSet &cardSet)
{
    (void)cardSet;
}

void EngineTest::TestPlayer::cardsDropped(const PlayerId *playerId, const CardSet &cardSet)
{
    (void)cardSet;
    (void)playerId;
}

void EngineTest::TestPlayer::cardsReceived(const PlayerId *playerId, const CardSet &cardSet)
{
    (void)cardSet;
    (void)playerId;
}

void EngineTest::TestPlayer::cardsReceived(const PlayerId *playerId, unsigned int cardsAmount)
{
    (void)cardsAmount;
    (void)playerId;
}

void EngineTest::TestPlayer::idCreated(const PlayerId *id)
{
    mIds.push_back(id);
}

const Card &EngineTest::TestPlayer::attack(const PlayerId *playerId, const CardSet &cardSet)
{
    (void)playerId;
    return *cardSet.begin();
}

const Card *EngineTest::TestPlayer::pitch(const PlayerId *playerId, const CardSet &cardSet)
{
    (void)playerId;
    return cardSet.empty() ? NULL : &*cardSet.begin();
}

const Card *EngineTest::TestPlayer::defend(const PlayerId *playerId, const CardSet &cardSet)
{
    (void)playerId;
    return cardSet.empty() ? NULL : &*cardSet.begin();
}

void EngineTest::TestPlayer::cardsUpdated(const CardSet &cardSet)
{
    (void)cardSet;
}
