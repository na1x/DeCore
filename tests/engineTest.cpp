#include "engineTest.h"
#include "engine.h"
#include "player.h"
#include "rules.h"

using namespace decore;

class TestEngine: public decore::Engine {
};

class TestPlayer: public decore::Player {

    void idCreated(PlayerId*)
    {

    }

    void gameStarted(Suit &, const CardSet &)
    {

    }

    void cardsReceived(PlayerId*, const CardSet&)
    {

    }

    void cardsReceived(PlayerId*, int)
    {

    }

    void cardsLeft(const CardSet&)
    {

    }

    void cardsDropped(PlayerId*, const CardSet&)
    {

    }

    const Card& attack(PlayerId*, const CardSet& set)
    {
        // dont care
        return *set.begin();
    }

    const Card* pitch(PlayerId*, const CardSet&)
    {
        // dont care
        return NULL;
    }

    Card* defend(PlayerId*, const CardSet&)
    {
        // dont care
        return NULL;
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
    CPPUNIT_ASSERT(!Rules::pickNext(playerIds, NULL));

    // check pickNext
    for(std::vector<PlayerId*>::iterator it = playerIds.begin(); it != playerIds.end(); ++it) {
        std::vector<PlayerId*>::iterator next = it + 1;
        if (next == playerIds.end()) {
            next = playerIds.begin();
        }
        CPPUNIT_ASSERT(*next == Rules::pickNext(playerIds, *it));
    }

    // cleanup players
    playerPtr = players;

    while(*playerPtr) {
        delete *playerPtr;
        playerPtr++;
    }

}

