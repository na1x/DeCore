#include "gameTest.h"
#include "engine.h"
#include "player.h"
#include "deck.h"
#include "defines.h"

using namespace decore;

void GameTest::TestPlayer0::idCreated(const PlayerId*id)
{
    mId = id;
}

const Card &GameTest::TestPlayer0::attack(const PlayerId*, const CardSet &cardSet)
{
    return *cardSet.begin();
}

const Card *GameTest::TestPlayer0::pitch(const PlayerId*, const CardSet &cardSet)
{
    return cardSet.empty() ? static_cast<Card*>(NULL) : &*cardSet.begin();
}

const Card *GameTest::TestPlayer0::defend(const PlayerId*, const CardSet &cardSet)
{
    return cardSet.empty() ? static_cast<Card*>(NULL) : &*cardSet.begin();
}

void GameTest::TestPlayer0::cardsUpdated(const CardSet &cardSet)
{
    mPlayerCards = cardSet;
}

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

void GameTest::testObservers()
{
    Engine engine;
    TestPlayer0 player0;
    TestPlayer0 player1;

    std::vector<const PlayerId*> playerIds;
    playerIds.push_back(engine.add(player0));
    playerIds.push_back(engine.add(player1));

    Observer observer;

    engine.addGameObserver(observer);

    Deck deck;

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

    deck.shuffle();

    CPPUNIT_ASSERT(engine.setDeck(deck)); // OK

    // check game started
    std::vector<Card> observerGameCards, deckCards;
    std::copy(observer.mGameCards.begin(), observer.mGameCards.end(), std::back_inserter(observerGameCards));
    std::copy(deck.begin(), deck.end(), std::back_inserter(deckCards));
    // if deck is shuffled the observer should not receive same cardset
    CPPUNIT_ASSERT(deckCards != observerGameCards);

    // check all observers, players are observers also
    std::vector<Observer*> observers;
    observers.push_back(&observer);
    observers.push_back(&player0);
    observers.push_back(&player1);
    for(std::vector<Observer*>::iterator it = observers.begin(); it != observers.end(); ++it) {
        Observer& currentObserver = **it;
        CPPUNIT_ASSERT(deck.trumpSuit() == currentObserver.mTrumpSuit);
        CPPUNIT_ASSERT(currentObserver.mPlayers == observer.mPlayers);
        std::vector<Card> currentObserverGameCards;
        std::copy(currentObserver.mGameCards.begin(), currentObserver.mGameCards.end(), std::back_inserter(currentObserverGameCards));
        CPPUNIT_ASSERT(currentObserverGameCards == observerGameCards);
    }

    CPPUNIT_ASSERT(engine.playRound()); // OK, at least one round played
}


void GameTest::Observer::gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *> players)
{
    mPlayers = players;
    mTrumpSuit = trumpSuit;
    mGameCards = cardSet;
}

void GameTest::Observer::cardsLeft(const CardSet &cardSet)
{
    (void)cardSet;
}

void GameTest::Observer::cardsDropped(const PlayerId *playerId, const CardSet &cardSet)
{
    (void)playerId;
    (void)cardSet;
}

void GameTest::Observer::cardsReceived(const PlayerId *playerId, const CardSet &cardSet)
{
    (void)playerId;
    (void)cardSet;
}

void GameTest::Observer::cardsReceived(const PlayerId *playerId, unsigned int cardsAmount)
{
    (void)playerId;
    (void)cardsAmount;
}


void GameTest::TestPlayer0::gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *> players)
{
    Observer::gameStarted(trumpSuit, cardSet, players);
}

void GameTest::TestPlayer0::cardsLeft(const CardSet &cardSet)
{
    Observer::cardsLeft(cardSet);
}

void GameTest::TestPlayer0::cardsDropped(const PlayerId *playerId, const CardSet &cardSet)
{
    Observer::cardsDropped(playerId, cardSet);
}

void GameTest::TestPlayer0::cardsReceived(const PlayerId *playerId, const CardSet &cardSet)
{
    Observer::cardsReceived(playerId, cardSet);
}

void GameTest::TestPlayer0::cardsReceived(const PlayerId *playerId, unsigned int cardsAmount)
{
    Observer::cardsReceived(playerId, cardsAmount);
}
