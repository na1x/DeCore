#ifndef SAVERESTORETEST_H
#define SAVERESTORETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "player.h"

using namespace decore;

class SaveRestoreTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SaveRestoreTest);
    CPPUNIT_TEST(test00);
    CPPUNIT_TEST_SUITE_END();

public:
    void test00();

private:

    class BasePlayer : public Player
    {
        void idCreated(const PlayerId* id);
        const Card& attack(const PlayerId* playerId, const CardSet& cardSet);
        const Card* pitch(const PlayerId* playerId, const CardSet& cardSet);
        const Card* defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet);
        void cardsUpdated(const CardSet& cardSet);

        void gameStarted(const Suit& trumpSuit, const CardSet& cardSet, const std::vector<const PlayerId*>& players);
        void roundStarted(unsigned int roundIndex, const std::vector<const PlayerId*> attackers, const PlayerId* defender);
        void roundEnded(unsigned int roundIndex);
        void cardsPickedUp(const PlayerId* playerId, const CardSet& cardSet);
        void cardsDealed(const PlayerId* playerId, unsigned int cardsAmount);
        void cardsLeft(const CardSet& cardSet);
        void cardsDropped(const PlayerId* playerId, const CardSet& cardSet);
    };
};

#endif /* SAVERESTORETEST_H */

