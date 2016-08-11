#include "gameTest.h"
#include "engine.h"
#include "player.h"
#include "deck.h"
#include "defines.h"

using namespace decore;

class TestPlayer0 : public Player
{
public:
    const PlayerId* mId;
    CardSet mCards;

    void idCreated(const PlayerId*id)
    {
        mId = id;
    }

    const Card &attack(const PlayerId*, const CardSet &cardSet)
    {
        return *cardSet.begin();
    }

    const Card *pitch(const PlayerId*, const CardSet &cardSet)
    {
        return cardSet.empty() ? static_cast<Card*>(NULL) : &*cardSet.begin();
    }

    const Card *defend(const PlayerId*, const CardSet &cardSet)
    {
        return cardSet.empty() ? static_cast<Card*>(NULL) : &*cardSet.begin();
    }

    void cardsUpdated(const CardSet &cardSet)
    {
        mCards = cardSet;
    }

    void gameStarted(Suit &trumpSuit, const CardSet &cardSet)
    {
        (void)trumpSuit;
        (void)cardSet;
    }
    void cardsLeft(const CardSet &cardSet)
    {
        (void)cardSet;
    }
    void cardsDropped(const PlayerId*playerId, const CardSet &cardSet)
    {
        (void)playerId;
        (void)cardSet;
    }
    void cardsReceived(const PlayerId* playerId, const CardSet& cardSet)
    {
        (void)playerId;
        (void)cardSet;
    }
    void cardsReceived(const PlayerId* playerId, int cardsAmount)
    {
        (void)playerId;
        (void)cardsAmount;
    }
};


void GameTest::testInitialization()
{
    Engine engine;
    TestPlayer0 player0;
    TestPlayer0 player1;

    CPPUNIT_ASSERT(!engine.playRound()); // set players and cards first

    engine.add(player0);
    engine.add(player1);

    CPPUNIT_ASSERT(!engine.playRound()); // set cards

    Deck deck;

    CPPUNIT_ASSERT(!engine.setDeck(deck)); // deck should not be empty
    CPPUNIT_ASSERT(!engine.playRound()); // deck should not be empty

    Rank ranks[] = {
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_9,
        RANK_10,
        RANK_JACK,
        RANK_QUEEN,
        RANK_KING,
        RANK_ACE,
    };

    Suit suits[] = {
        SUIT_SPADES,
        SUIT_HEARTS,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };

    deck.generate(ranks, ARRAY_SIZE(ranks), suits, ARRAY_SIZE(suits));

    CPPUNIT_ASSERT(engine.setDeck(deck)); // OK

    CPPUNIT_ASSERT(engine.playRound()); // OK, at least one round played
}

void GameTest::testOneRound()
{
    Engine engine;
    TestPlayer0 player0;
    TestPlayer0 player1;

    const PlayerId* player0Id = engine.add(player0);
    const PlayerId* player1Id = engine.add(player1);

    Deck deck;

    deck.push_back(Card(SUIT_CLUBS, RANK_6)); // goes to player 0
    deck.push_back(Card(SUIT_CLUBS, RANK_7)); // goes to player 1
    deck.push_back(Card(SUIT_DIAMONDS, RANK_6)); // goes to player 0
    deck.push_back(Card(SUIT_DIAMONDS, RANK_9)); // goes to player 1, trump suit

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(!engine.playRound()); // cards for one round only

    CPPUNIT_ASSERT(player0Id == player0.mId);
    CPPUNIT_ASSERT(player1Id == player1.mId);

}
