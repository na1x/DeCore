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

    void cardsDopped(const PlayerId&, const CardSet&)
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

    // not existing player id
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

