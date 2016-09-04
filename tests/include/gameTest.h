#ifndef GAMETEST_H
#define GAMETEST_H
#include <cppunit/extensions/HelperMacros.h>
#include "gameObserver.h"
#include "cardSet.h"
#include "player.h"
#include "basePlayer.h"
#include "gameCardsTracker.h"

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
        void cardsGone(const decore::CardSet &cardSet);
        void cardsDropped(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
        void cardsPickedUp(const decore::PlayerId* playerId, const decore::CardSet& cardSet);
        void cardsDealed(const decore::PlayerId* playerId, unsigned int cardsAmount);
        void roundStarted(unsigned int roundIndex, const std::vector<const decore::PlayerId *> attackers, const decore::PlayerId *defender);
        void roundEnded(unsigned int roundIndex);
        void gameRestored(const std::vector<const decore::PlayerId*>& playerIds,
            const std::map<const decore::PlayerId*, unsigned int>& playersCards,
            unsigned int deckCards,
            const decore::Suit& trumpSuit,
            const std::vector<decore::Card>& attackCards,
            const std::vector<decore::Card>& defendCards);
        void save(decore::DataWriter& writer);
        void init(decore::DataReader& reader);

    private:
        RoundData* mCurrentRoundData;
    };

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
    };

    class AttackWithInvalidCardPlayer : public BasePlayer
    {
        const Card mInvalidCard;
        bool mDontCheckCardExist;
    public:
        AttackWithInvalidCardPlayer(Suit suit, Rank rank);
        const Card& attack(const PlayerId* playerId, const CardSet& cardSet);
        const Card& invalidCard();
        void cardsUpdated(const CardSet& cardSet);
    };

    class DefendWithInvalidCardPlayer : public BasePlayer
    {
        const Card mInvalidCard;
        bool mDontCheckCardExist;
    public:
        DefendWithInvalidCardPlayer(Suit suit, Rank rank);
        const Card* defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet);
        const Card& invalidCard();
        void cardsUpdated(const CardSet& cardSet);
    };
    static void playRound(Player& player0, Player& player1, GameCardsTracker& observer);
};

#endif // GAMETEST_H
