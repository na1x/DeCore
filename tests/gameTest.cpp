#include "gameTest.h"
#include "engine.h"
#include "player.h"
#include "deck.h"
#include "defines.h"

using namespace decore;

#define MAX_CARDS (6)

GameTest::TestPlayer0::TestPlayer0()
{

}

void GameTest::TestPlayer0::idCreated(const PlayerId*id)
{
    mId = id;
}

const Card &GameTest::TestPlayer0::attack(const PlayerId*, const CardSet &cardSet)
{
    const Card& res = *cardSet.begin();
    updateCards(&res);
    return res;
}

const Card *GameTest::TestPlayer0::pitch(const PlayerId*, const CardSet &cardSet)
{
    if (cardSet.empty()) {
        return NULL;
    }

    const Card* res = &*cardSet.begin();
    updateCards(res);
    return res;
}

const Card *GameTest::TestPlayer0::defend(const PlayerId*, const Card &card, const CardSet &cardSet)
{
    (void)card;

    if (cardSet.empty()) {
        return NULL;
    }

    const Card* res = &*cardSet.begin();
    updateCards(res);
    return res;
}

void GameTest::TestPlayer0::cardsUpdated(const CardSet &cardSet)
{
    mPlayerCards.push_back(cardSet);
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
    TestPlayer0 player0, player1, player2;

    std::vector<TestPlayer0*> players;
    players.push_back(&player0);
    players.push_back(&player1);
    players.push_back(&player2);

    std::vector<const PlayerId*> playerIds;
    for(std::vector<TestPlayer0*>::iterator it = players.begin(); it != players.end(); ++ it) {
        playerIds.push_back(engine.add(**it));
    }

    Observer observer;

    engine.addGameObserver(observer);

    Deck deck;

    // cards preset for one round
    // first attacker (player0) has cards for 6 moves
    // defender (player1) has cards for successful defend
    // player2 has trumps only, but it should not receive move for attack
    deck.push_back(Card(SUIT_CLUBS, RANK_6));
    deck.push_back(Card(SUIT_CLUBS, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_6));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_6));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_7));

    deck.push_back(Card(SUIT_HEARTS, RANK_6));
    deck.push_back(Card(SUIT_HEARTS, RANK_8));
    deck.push_back(Card(SUIT_SPADES, RANK_8));

    deck.push_back(Card(SUIT_CLUBS, RANK_8));
    deck.push_back(Card(SUIT_CLUBS, RANK_9));
    deck.push_back(Card(SUIT_SPADES, RANK_9));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_9));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_10));
    deck.push_back(Card(SUIT_SPADES, RANK_10));

    deck.push_back(Card(SUIT_HEARTS, RANK_10));
    deck.push_back(Card(SUIT_HEARTS, RANK_JACK));
    deck.push_back(Card(SUIT_SPADES, RANK_JACK));

    deck.push_back(Card(SUIT_SPADES, RANK_ACE));

    CPPUNIT_ASSERT(engine.setDeck(deck)); // OK

    // check game started
    std::vector<Card> observerGameCards, deckCards;
    std::copy(observer.mGameCards.begin(), observer.mGameCards.end(), std::back_inserter(observerGameCards));
    std::copy(deck.begin(), deck.end(), std::back_inserter(deckCards));
    // if deck is shuffled the observer should not receive same cardset
    CPPUNIT_ASSERT(deckCards != observerGameCards);
    CPPUNIT_ASSERT(deckCards.size() == observerGameCards.size());

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

    CPPUNIT_ASSERT(engine.playRound());

    for(std::vector<TestPlayer0*>::iterator it = players.begin(); it != players.end(); ++ it) {
        TestPlayer0& player = **it;
        CPPUNIT_ASSERT(!player.mPlayerCards.empty());
        // check deal
        CPPUNIT_ASSERT(player.mPlayerCards[0].size() == MAX_CARDS);
        // compare last player's update in the round with observer's
        CPPUNIT_ASSERT(observer.mPlayersCards[player.mId] == player.mPlayerCards[player.mPlayerCards.size() - 1].size());
    }

    // deck has for player0 and player1 to play the round without pitch from player2
    CPPUNIT_ASSERT(1 == player2.mPlayerCards.size());
}

void GameTest::Observer::gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *> players)
{
    mPlayers = players;
    mTrumpSuit = trumpSuit;
    mGameCards = cardSet;
    for(std::vector<const PlayerId*>::iterator it = mPlayers.begin(); it != mPlayers.end(); ++it) {
        mPlayersCards[*it] = 0;
    }
    mGameCardsCount = mGameCards.size();
}

void GameTest::Observer::cardsLeft(const CardSet &cardSet)
{
    mGameCardsCount -= cardSet.size();
}

void GameTest::Observer::cardsDropped(const PlayerId *playerId, const CardSet &cardSet)
{
    mPlayersCards[playerId] -= cardSet.size();
}

void GameTest::Observer::cardsReceived(const PlayerId *playerId, const CardSet &cardSet)
{
    mPlayersCards[playerId] += cardSet.size();
}

void GameTest::Observer::cardsReceived(const PlayerId *playerId, unsigned int cardsAmount)
{
    mPlayersCards[playerId] += cardsAmount;
    mGameCardsCount -= cardsAmount;
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

void GameTest::TestPlayer0::updateCards(const Card *card)
{
    CardSet currentCards = *(mPlayerCards.end() - 1);
    CPPUNIT_ASSERT(currentCards.erase(*card));
    mPlayerCards.push_back(currentCards);
}
