#ifndef GAMETEST_H
#define GAMETEST_H
#include <cppunit/extensions/HelperMacros.h>
#include "gameObserver.h"
#include "cardSet.h"
#include "player.h"

class GameTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GameTest);
    CPPUNIT_TEST(testInitialization);
    CPPUNIT_TEST(testOneRound);
    CPPUNIT_TEST(testObservers);
    CPPUNIT_TEST_SUITE_END();

public:
    void testInitialization();
    void testOneRound();
    void testObservers();

private:
    class Observer : public decore::GameObserver
    {
    public:
        std::vector<const decore::PlayerId*> mPlayers;
        decore::CardSet mGameCards;
        unsigned int mGameCardsCount;
        decore::Suit mTrumpSuit;
        std::map<const decore::PlayerId*, unsigned int> mPlayersCards;

        void gameStarted(const decore::Suit &trumpSuit, const decore::CardSet &cardSet, const std::vector<const decore::PlayerId *> players);
        void cardsLeft(const decore::CardSet &cardSet);
        void cardsDropped(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        void cardsReceived(const decore::PlayerId* playerId, const decore::CardSet& cardSet);
        void cardsReceived(const decore::PlayerId* playerId, unsigned int cardsAmount);
    };

    class TestPlayer0 : public decore::Player, public Observer
    {
    public:
        const decore::PlayerId* mId;
        std::vector<decore::CardSet> mPlayerCards;
        unsigned int mCardsUpdatedCounter;

        TestPlayer0();
        void idCreated(const decore::PlayerId *id);
        const decore::Card &attack(const decore::PlayerId *, const decore::CardSet &cardSet);
        const decore::Card *pitch(const decore::PlayerId *, const decore::CardSet &cardSet);
        const decore::Card *defend(const decore::PlayerId *, const decore::CardSet &cardSet);
        void cardsUpdated(const decore::CardSet &cardSet);

        void gameStarted(const decore::Suit &trumpSuit, const decore::CardSet &cardSet, const std::vector<const decore::PlayerId *> players);
        void cardsLeft(const decore::CardSet &cardSet);
        void cardsDropped(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        void cardsReceived(const decore::PlayerId* playerId, const decore::CardSet& cardSet);
        void cardsReceived(const decore::PlayerId* playerId, unsigned int cardsAmount);
    };
};

#endif // GAMETEST_H
