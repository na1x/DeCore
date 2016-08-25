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

    // cleanup
    playerPtr = players;
    while(*playerPtr) {
        delete *playerPtr;
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

void EngineTest::TestPlayer::idCreated(const PlayerId *id)
{
    mIds.push_back(id);
}
