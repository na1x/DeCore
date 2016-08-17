#ifndef ENGINETEST_H
#define ENGINETEST_H
#include <cppunit/extensions/HelperMacros.h>
#include "player.h"

class EngineTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(EngineTest);
    CPPUNIT_TEST(testAddPlayers);
    CPPUNIT_TEST(testAddDuplicatedPlayers);
    CPPUNIT_TEST_SUITE_END();

public:
    void testAddPlayers();
    void testAddDuplicatedPlayers();

private:
    class TestPlayer : public decore::Player
    {
    public:
        std::vector<const decore::PlayerId*> mIds;

        void gameStarted(const decore::Suit &trumpSuit, const decore::CardSet &cardSet, const std::vector<const decore::PlayerId *> players);
        void cardsLeft(const decore::CardSet &cardSet);
        void cardsDropped(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        void cardsReceived(const decore::PlayerId* playerId, const decore::CardSet& cardSet);
        void cardsReceived(const decore::PlayerId* playerId, unsigned int cardsAmount);

        void idCreated(const decore::PlayerId *id);
        const decore::Card &attack(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        const decore::Card *pitch(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        const decore::Card *defend(const decore::PlayerId *playerId, const decore::Card& card, const decore::CardSet &cardSet);
        void cardsUpdated(const decore::CardSet &cardSet);
    };

};

#endif // ENGINETEST_H
