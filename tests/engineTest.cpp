#include "engineTest.h"
#include "engine.h"
#include "player.h"

using namespace decore;

class TestEngine: public decore::Engine {
public:
    PlayerId* pickNext(PlayerId* after)
    {
        return Engine::pickNext(after);
    }
};

class TestPlayer: public decore::Player {

    void cardsReceived(const PlayerId&, const CardSet&)
    {

    }

    void cardsReceived(const PlayerId&, int)
    {

    }

    void cardsLeft(const CardSet&)
    {

    }

    void cardsDropped(const PlayerId&, const CardSet&)
    {

    }

    int attack(const PlayerId&, const CardSet&)
    {
        // dont care
        return 0;
    }

    bool attack(const PlayerId&, const CardSet&, int&)
    {
        // dont care
        return false;
    }

    bool defend(const PlayerId&, const CardSet&, int&)
    {
        // dont care
        return false;
    }

    void cardsUpdated(const CardSet&)
    {

    }

};

void EngineTest::testAddPlayers()
{
    TestEngine engine;

    // set used to check uniqueness of ids
    std::set<PlayerId*> playerIds;

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
        PlayerId* added = engine.add(**playerPtr);
        CPPUNIT_ASSERT(added);
        CPPUNIT_ASSERT(playerIds.insert(added).second);
        playerPtr++;
    }
}

void EngineTest::testAddDuplicatedPlayers()
{
    TestEngine engine;
    TestPlayer player;
    PlayerId* id0, *id1;
    CPPUNIT_ASSERT(id0 = engine.add(player));
    CPPUNIT_ASSERT(id1 = engine.add(player));
    CPPUNIT_ASSERT(id0 != id1);
}

void EngineTest::testPickNext()
{
    TestEngine engine;

    std::vector<PlayerId*> playerIds;

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
        PlayerId* added = engine.add(**playerPtr);
        CPPUNIT_ASSERT(added);
        playerIds.push_back(added);
        playerPtr++;
    }

    // check that there's no next player for not existing player id
    CPPUNIT_ASSERT(!engine.pickNext(NULL));

    // check pickNext
    for(std::vector<PlayerId*>::iterator it = playerIds.begin(); it != playerIds.end(); ++it) {
        std::vector<PlayerId*>::iterator next = it + 1;
        if (next == playerIds.end()) {
            next = playerIds.begin();
        }
        CPPUNIT_ASSERT(*next == engine.pickNext(*it));
    }

    // cleanup players
    playerPtr = players;

    while(*playerPtr) {
        delete *playerPtr;
        playerPtr++;
    }

}

