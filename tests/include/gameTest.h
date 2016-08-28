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
    CPPUNIT_TEST(testOneRound00);
    CPPUNIT_TEST(testOneRound01);
    CPPUNIT_TEST(testOneRound02);
    CPPUNIT_TEST(testOneRound03);
    CPPUNIT_TEST(testDraw00);
    CPPUNIT_TEST(testThreePlayers00);
    CPPUNIT_TEST(testPitch00);
    CPPUNIT_TEST(testPitch01);
    CPPUNIT_TEST(testMoveTransfer00);
    CPPUNIT_TEST_SUITE_END();

public:
    void testInitialization();
    void testOneRound00();
    void testOneRound01();
    void testOneRound02();
    void testOneRound03();
    void testDraw00();
    void testThreePlayers00();
    void testPitch00();
    void testPitch01();
    void testMoveTransfer00();

private:
    class Observer : public decore::GameObserver
    {

    public:
        class RoundData
        {
        public:
            std::vector<const decore::PlayerId*> mPlayers;
            std::map<const decore::PlayerId*, decore::CardSet> mDroppedCards;
            std::map<const decore::PlayerId*, decore::CardSet> mPickedUpCards;
        };

        std::vector<const decore::PlayerId*> mPlayers;
        decore::CardSet mGameCards;
        unsigned int mGameCardsCount;
        decore::Suit mTrumpSuit;
        std::map<const decore::PlayerId*, unsigned int> mPlayersCards;
        std::vector<const RoundData*> mRoundsData;
        unsigned int mCurrentRoundIndex;

        Observer();
        ~Observer();
        void gameStarted(const decore::Suit &trumpSuit, const decore::CardSet &cardSet, const std::vector<const decore::PlayerId *>& players);
        void cardsLeft(const decore::CardSet &cardSet);
        void cardsDropped(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        void cardsPickedUp(const decore::PlayerId* playerId, const decore::CardSet& cardSet);
        void cardsDealed(const decore::PlayerId* playerId, unsigned int cardsAmount);
        void roundStarted(unsigned int roundIndex, const std::vector<const decore::PlayerId *> attackers, const decore::PlayerId *defender);
        void roundEnded(unsigned int roundIndex);
        void tableCardsRestored(const std::vector<decore::Card>& attackCards, const std::vector<decore::Card>& defendCards);


    private:
        RoundData* mCurrentRoundData;
    };

    class TestPlayer0 : public decore::Player, public Observer
    {
    public:
        const decore::PlayerId* mId;
        std::vector<decore::CardSet> mPlayerCards;

        TestPlayer0();
        void idCreated(const decore::PlayerId *id);
        const decore::Card &attack(const decore::PlayerId *, const decore::CardSet &cardSet);
        const decore::Card *pitch(const decore::PlayerId *, const decore::CardSet &cardSet);
        const decore::Card *defend(const decore::PlayerId *, const decore::Card& card, const decore::CardSet &cardSet);
        void cardsUpdated(const decore::CardSet &cardSet);
        void roundStarted(unsigned int roundIndex, const std::vector<const decore::PlayerId *> attackers, const decore::PlayerId *defender);
        void roundEnded(unsigned int roundIndex);
        void cardsRestored(const decore::CardSet& cards);

        void gameStarted(const decore::Suit &trumpSuit, const decore::CardSet &cardSet, const std::vector<const decore::PlayerId *>& players);
        void cardsLeft(const decore::CardSet &cardSet);
        void cardsDropped(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        void cardsPickedUp(const decore::PlayerId* playerId, const decore::CardSet& cardSet);
        void cardsDealed(const decore::PlayerId* playerId, unsigned int cardsAmount);
        void tableCardsRestored(const std::vector<decore::Card>& attackCards, const std::vector<decore::Card>& defendCards);


    private:
        void removeCard(const decore::Card* card);
    };
};

#endif // GAMETEST_H
