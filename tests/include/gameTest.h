#ifndef GAMETEST_H
#define GAMETEST_H
#include <cppunit/extensions/HelperMacros.h>
#include "gameObserver.h"
#include "cardSet.h"
#include "player.h"
#include "basePlayer.h"
#include "gameCardsTracker.h"
#include "observer.h"

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
    CPPUNIT_TEST(testInvalidCards00);
    CPPUNIT_TEST(testInvalidCards01);
    CPPUNIT_TEST(testInvalidCards02);
    CPPUNIT_TEST(testInvalidCards03);
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
    void testInvalidCards00();
    void testInvalidCards01();
    void testInvalidCards02();
    void testInvalidCards03();

private:
    class TestPlayer0 : public BasePlayer, public Observer
    {
    public:

        void roundStarted(unsigned int roundIndex, const std::vector<const decore::PlayerId *> attackers, const decore::PlayerId *defender);
        void roundEnded(unsigned int roundIndex);
        void gameStarted(const decore::Suit &trumpSuit, const decore::CardSet &cardSet, const std::vector<const decore::PlayerId *>& players);
        void cardsGone(const decore::CardSet &cardSet);
        void cardsDropped(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        void cardsPickedUp(const decore::PlayerId* playerId, const decore::CardSet& cardSet);
        void cardsDealed(const decore::PlayerId* playerId, unsigned int cardsAmount);
        void gameRestored(const std::vector<const decore::PlayerId*>& playerIds,
            const std::map<const decore::PlayerId*, unsigned int>& playersCards,
            unsigned int deckCards,
            const decore::Suit& trumpSuit,
            const std::vector<decore::Card>& attackCards,
            const std::vector<decore::Card>& defendCards);
        void save(decore::DataWriter& writer);
        void init(decore::DataReader& reader);
        void quit();
    };

    class AttackWithInvalidCardPlayer : public BasePlayer
    {
        const Card mInvalidCard;
        bool mDontCheckCardExist;
    public:
        AttackWithInvalidCardPlayer(const Card& card);
        const Card& attack(const PlayerId* playerId, const CardSet& cardSet);
        const Card& invalidCard();
        void cardsUpdated(const CardSet& cardSet);
    };

    class DefendWithInvalidCardPlayer : public BasePlayer
    {
        const Card mInvalidCard;
        bool mDontCheckCardExist;
    public:
        DefendWithInvalidCardPlayer(const Card& card);
        const Card* defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet);
        const Card& invalidCard();
        void cardsUpdated(const CardSet& cardSet);
    };
    static void playRound(Player& player0, Player& player1, GameCardsTracker& tracker, GameObserver& observer);
};

#endif // GAMETEST_H
