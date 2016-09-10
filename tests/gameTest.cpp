#include "gameTest.h"
#include "engine.h"
#include "player.h"
#include "deck.h"
#include "defines.h"
#include "gameCardsTracker.h"
#include "observer.h"

using namespace decore;

#define MAX_CARDS (6)

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

void GameTest::testOneRound00()
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

    GameCardsTracker tracker;

    engine.addGameObserver(tracker);

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(!engine.playRound()); // cards for one round only

    CPPUNIT_ASSERT(player0Id == player0.id());
    CPPUNIT_ASSERT(player1Id == player1.id());

    CPPUNIT_ASSERT(!tracker.deckCards());
    CPPUNIT_ASSERT(tracker.playerCards(player0.id()).empty());
    CPPUNIT_ASSERT(tracker.playerCards(player1.id()).empty());
}

void GameTest::testOneRound01()
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
    GameCardsTracker tracker;

    engine.addGameObserver(observer);
    engine.addGameObserver(tracker);

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

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(19 == tracker.deckCards());

    // check game started
    std::vector<Card> observerGameCards, deckCards;
    std::copy(observer.gameCards().begin(), observer.gameCards().end(), std::back_inserter(observerGameCards));
    std::copy(deck.begin(), deck.end(), std::back_inserter(deckCards));
    // if deck is shuffled the observer should not receive same cards
    CPPUNIT_ASSERT(deckCards != observerGameCards);
    CPPUNIT_ASSERT(deckCards.size() == observerGameCards.size());

    // check all observers, players are observers also
    std::vector<Observer*> observers;
    observers.push_back(&observer);
    observers.push_back(&player0);
    observers.push_back(&player1);
    for(std::vector<Observer*>::iterator it = observers.begin(); it != observers.end(); ++it) {
        Observer& currentObserver = **it;
        CPPUNIT_ASSERT(deck.trumpSuit() == currentObserver.trumpSuit());
        CPPUNIT_ASSERT(currentObserver.players() == observer.players());
        std::vector<Card> currentObserverGameCards;
        std::copy(currentObserver.gameCards().begin(), currentObserver.gameCards().end(), std::back_inserter(currentObserverGameCards));
        CPPUNIT_ASSERT(currentObserverGameCards == observerGameCards);
    }

    CPPUNIT_ASSERT(engine.playRound());

    for(std::vector<TestPlayer0*>::iterator it = players.begin(); it != players.end(); ++ it) {
        TestPlayer0& player = **it;
        CPPUNIT_ASSERT(player.cardSets() != 0);
        // check deal
        CPPUNIT_ASSERT(player.cards(0).size() == MAX_CARDS);
        // compare last player's update in the round with observer
        CPPUNIT_ASSERT(observer.playerCards(player.id()) == player.cards(player.cardSets() - 1).size());
    }

    // deck has cards for player0 and player1 to play the round without pitch from player2
    CPPUNIT_ASSERT(1 == player2.cardSets());

    CPPUNIT_ASSERT(tracker.playerCards(player0.id()).empty());
    CPPUNIT_ASSERT(tracker.playerCards(player1.id()).empty());
    CPPUNIT_ASSERT(MAX_CARDS == tracker.playerCards(player2.id()).size());

    CPPUNIT_ASSERT(tracker.deckCards() == 1);
}

void GameTest::testOneRound02()
{
    // attacker has 2 cards, and goes with trump (due to CardSet sort order)
    // player two picks the card
    // game ended on second round - player one goes with SUIT_CLUBS, RANK_6
    // player two beats and loses with one card remained
    Engine engine;
    TestPlayer0 player0, player1;

    std::vector<TestPlayer0*> players;
    players.push_back(&player0);
    players.push_back(&player1);

    std::vector<const PlayerId*> playerIds;
    for(std::vector<TestPlayer0*>::iterator it = players.begin(); it != players.end(); ++ it) {
        playerIds.push_back(engine.add(**it));
    }

    Deck deck;

    deck.push_back(Card(SUIT_CLUBS, RANK_6));
    deck.push_back(Card(SUIT_CLUBS, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_6));

    GameCardsTracker tracker;

    engine.addGameObserver(tracker);

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(engine.playRound());
    CPPUNIT_ASSERT(!engine.playRound());
    CPPUNIT_ASSERT(1 == tracker.lastRoundIndex());
    CPPUNIT_ASSERT(engine.getLoser() == player1.id());
    CPPUNIT_ASSERT(player0.cards(player0.cardSets() - 1).empty());
    CPPUNIT_ASSERT(player1.cards(player1.cardSets() - 1).size() == 1);
    CPPUNIT_ASSERT(tracker.deckCards() == 0);
    CPPUNIT_ASSERT(tracker.playerCards(player0.id()).empty());
    CPPUNIT_ASSERT(1 == tracker.playerCards(player1.id()).size());
}

void GameTest::testOneRound03()
{
    // attacker has 2 cards, defender has one card
    // defender beats
    // attacker left with one card
    Engine engine;
    TestPlayer0 player0, player1;

    std::vector<TestPlayer0*> players;
    players.push_back(&player0);
    players.push_back(&player1);

    std::vector<const PlayerId*> playerIds;
    for(std::vector<TestPlayer0*>::iterator it = players.begin(); it != players.end(); ++ it) {
        playerIds.push_back(engine.add(**it));
    }

    Deck deck;

    deck.push_back(Card(SUIT_SPADES, RANK_6));
    deck.push_back(Card(SUIT_SPADES, RANK_7));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_6));

    GameCardsTracker tracker;

    engine.addGameObserver(tracker);

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(!engine.playRound());
    CPPUNIT_ASSERT(engine.getLoser() == player0.id());
    CPPUNIT_ASSERT(player0.cards(player0.cardSets() - 1).size() == 1);
    CPPUNIT_ASSERT(player1.cards(player1.cardSets() - 1).empty());

    CPPUNIT_ASSERT(tracker.playerCards(player0.id()).size() == 1);
    CPPUNIT_ASSERT(tracker.playerCards(player1.id()).empty());
    CPPUNIT_ASSERT(!tracker.deckCards());
}

void GameTest::testDraw00()
{
    Engine engine;
    TestPlayer0 player0, player1;

    GameCardsTracker cardTracker;

    std::vector<TestPlayer0*> players;
    players.push_back(&player0);
    players.push_back(&player1);

    std::vector<const PlayerId*> playerIds;
    for(std::vector<TestPlayer0*>::iterator it = players.begin(); it != players.end(); ++ it) {
        playerIds.push_back(engine.add(**it));
    }

    Deck deck;

    deck.push_back(Card(SUIT_SPADES, RANK_6));
    deck.push_back(Card(SUIT_SPADES, RANK_7));

    engine.addGameObserver(cardTracker);

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(!engine.playRound());
    CPPUNIT_ASSERT(!engine.getLoser());
    CPPUNIT_ASSERT(player0.cards(player0.cardSets() - 1).empty());
    CPPUNIT_ASSERT(player1.cards(player1.cardSets() - 1).empty());

    CPPUNIT_ASSERT(cardTracker.playerCards(player0.id()).empty());
    CPPUNIT_ASSERT(cardTracker.playerCards(player1.id()).empty());
    CPPUNIT_ASSERT(!cardTracker.deckCards());
}

void GameTest::testThreePlayers00()
{
    // three players, play one round
    // both player0 and player2 attacks
    // player1 defends
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

    GameCardsTracker gameCardsTracker;

    engine.addGameObserver(gameCardsTracker);

    Deck deck;

    deck.push_back(Card(SUIT_SPADES, RANK_6));
    deck.push_back(Card(SUIT_SPADES, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_9));

    deck.push_back(Card(SUIT_HEARTS, RANK_6));
    deck.push_back(Card(SUIT_HEARTS, RANK_7));
    deck.push_back(Card(SUIT_HEARTS, RANK_9));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_6));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_9));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_10));

    deck.push_back(Card(SUIT_SPADES, RANK_8));
    deck.push_back(Card(SUIT_SPADES, RANK_10));
    deck.push_back(Card(SUIT_SPADES, RANK_JACK));

    deck.push_back(Card(SUIT_HEARTS, RANK_ACE));
    deck.push_back(Card(SUIT_HEARTS, RANK_10));
    deck.push_back(Card(SUIT_HEARTS, RANK_JACK));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_JACK));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_QUEEN));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_KING));

    deck.push_back(Card(SUIT_CLUBS, RANK_ACE));

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(19 == gameCardsTracker.deckCards());

    CPPUNIT_ASSERT(engine.playRound());

    // one deal + three attacks
    CPPUNIT_ASSERT(player0.cardSets() == 4);
    // one deal + six defends
    CPPUNIT_ASSERT(player1.cardSets() == 7);
    // one deal + three attacks
    CPPUNIT_ASSERT(player2.cardSets() == 4);

    CPPUNIT_ASSERT(player0.cards(player0.cardSets() - 1).size() == 3);
    CPPUNIT_ASSERT(player1.cards(player1.cardSets() - 1).empty());
    CPPUNIT_ASSERT(player2.cards(player2.cardSets() - 1).size() == 3);

    CPPUNIT_ASSERT(1 == gameCardsTracker.deckCards());
}

void GameTest::testPitch00()
{
    // attacker has three cards for the first move
    // defender has no cards to beat first card
    // ensure that attacker is allowed to pitch all cards
    Engine engine;
    TestPlayer0 player0, player1;

    std::vector<TestPlayer0*> players;
    players.push_back(&player0);
    players.push_back(&player1);

    std::vector<const PlayerId*> playerIds;
    for(std::vector<TestPlayer0*>::iterator it = players.begin(); it != players.end(); ++ it) {
        playerIds.push_back(engine.add(**it));
    }

    GameCardsTracker gameCardsTracker;

    engine.addGameObserver(gameCardsTracker);

    Deck deck;

    deck.push_back(Card(SUIT_SPADES, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_6));

    deck.push_back(Card(SUIT_HEARTS, RANK_7));
    deck.push_back(Card(SUIT_HEARTS, RANK_6));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_7));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_6));

    deck.push_back(Card(SUIT_CLUBS, RANK_ACE));

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(7 == gameCardsTracker.deckCards());

    CPPUNIT_ASSERT(engine.playRound());

    // deal and three attacks
    CPPUNIT_ASSERT(player0.cardSets() == 4);
    // deal and one pick up
    CPPUNIT_ASSERT(player1.cardSets() == 2);

    CPPUNIT_ASSERT(player0.cards(player0.cardSets() - 1).size() == 1);
    CPPUNIT_ASSERT(player1.cards(player1.cardSets() - 1).size() == 6);

    CPPUNIT_ASSERT(gameCardsTracker.playerCards(player0.id()).size() == 1);
    CPPUNIT_ASSERT(gameCardsTracker.playerCards(player1.id()).size() == 6);
}

void GameTest::testPitch01()
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

    Deck deck;

    deck.push_back(Card(SUIT_SPADES, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_8));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_7));

    deck.push_back(Card(SUIT_HEARTS, RANK_7));
    deck.push_back(Card(SUIT_HEARTS, RANK_6));
    deck.push_back(Card(SUIT_HEARTS, RANK_8));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_9));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_6));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_8));

    deck.push_back(Card(SUIT_CLUBS, RANK_ACE));

    GameCardsTracker tracker;

    engine.addGameObserver(tracker);

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(tracker.deckCards() == 10);
    CPPUNIT_ASSERT(engine.playRound());

    // deal and two attacks
    CPPUNIT_ASSERT(player0.cardSets() == 3);
    // deal + defend + one pick up
    CPPUNIT_ASSERT(player1.cardSets() == 3);
    // deal and one pitch
    CPPUNIT_ASSERT(player2.cardSets() == 2);

    CPPUNIT_ASSERT(player0.cards(player0.cardSets() - 1).size() == 2);
    CPPUNIT_ASSERT(player1.cards(player1.cardSets() - 1).size() == MAX_CARDS);
    CPPUNIT_ASSERT(player2.cards(player2.cardSets() - 1).size() == 2);

    CPPUNIT_ASSERT(tracker.playerCards(player0.id()).size() == 2);
    CPPUNIT_ASSERT(tracker.playerCards(player1.id()).size() == MAX_CARDS);
    CPPUNIT_ASSERT(tracker.playerCards(player2.id()).size() == 2);
}

static bool checkCard(const CardSet& set, const Card& card)
{
    return !set.empty() && set.find(card) != set.end();
}

void GameTest::testMoveTransfer00()
{
    // play two rounds
    // first round failed to attack
    // play one more round
    // check that "move" transfered to player2
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

    Deck deck;

    deck.push_back(Card(SUIT_SPADES, RANK_8));
    deck.push_back(Card(SUIT_SPADES, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_6));

    deck.push_back(Card(SUIT_SPADES, RANK_9));

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(engine.playRound());

    CPPUNIT_ASSERT(0 != player0.rounds());

    const Observer::RoundData& round0 = *player0.roundData(0);

    CPPUNIT_ASSERT(1 == round0.mDroppedCards.size());
    CPPUNIT_ASSERT(checkCard(round0.mDroppedCards.at(playerIds[0]), deck[0]));
    CPPUNIT_ASSERT(1 == round0.mPickedUpCards.size());
    CPPUNIT_ASSERT(checkCard(round0.mPickedUpCards.at(playerIds[1]), deck[0]));

    CPPUNIT_ASSERT(!engine.playRound());

    CPPUNIT_ASSERT(engine.getLoser() == player1.id());

    CPPUNIT_ASSERT(2 == player0.rounds());

    const Observer::RoundData& round1 = *player0.roundData(1);

    CPPUNIT_ASSERT(2 == round1.mDroppedCards.size());
    CPPUNIT_ASSERT(checkCard(round1.mDroppedCards.at(playerIds[2]), deck[2]));
    CPPUNIT_ASSERT(checkCard(round1.mDroppedCards.at(playerIds[0]), deck[3]));
    CPPUNIT_ASSERT(round1.mPickedUpCards.empty());
}

void GameTest::testMoveTransfer01()
{
    // player0 has one card
    // player1 has no cards to defend
    // move goes to player2
    // ensure that player2 attacks player1 and not player0, because latter has no cards to defend
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

    deck.push_back(Card(SUIT_SPADES, RANK_8));
    deck.push_back(Card(SUIT_SPADES, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_6));

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(engine.playRound());
    CPPUNIT_ASSERT(observer.rounds() == 1);
    const Observer::RoundData& firstRound = *observer.roundData(0);
    CPPUNIT_ASSERT(firstRound.mDroppedCards.size() == 1);
    CPPUNIT_ASSERT(firstRound.mDroppedCards.find(player0.id()) != firstRound.mDroppedCards.end());
    CPPUNIT_ASSERT(firstRound.mPickedUpCards.size() == 1);
    CPPUNIT_ASSERT(firstRound.mPickedUpCards.find(player1.id()) != firstRound.mPickedUpCards.end());
    CPPUNIT_ASSERT(!engine.playRound());
    CPPUNIT_ASSERT(observer.rounds() == 2);
    const Observer::RoundData& secondRound = *observer.roundData(1);
    CPPUNIT_ASSERT(secondRound.mDroppedCards.size() == 2);
    CPPUNIT_ASSERT(secondRound.mDroppedCards.find(player2.id()) != secondRound.mDroppedCards.end());
    CPPUNIT_ASSERT(secondRound.mDroppedCards.find(player1.id()) != secondRound.mDroppedCards.end());
    CPPUNIT_ASSERT(secondRound.mPickedUpCards.size() == 0);
}

void GameTest::testInvalidCards00()
{
    // player attacks with card which is not from suggested set
    Card invalidCard(SUIT_CLUBS, RANK_ACE);
    AttackWithInvalidCardPlayer player0(invalidCard);

    BasePlayer player1;

    GameCardsTracker tracker;
    Observer observer;

    playRound(player0, player1, tracker, observer);

    const CardSet& goneCards = tracker.goneCards();
    CPPUNIT_ASSERT(goneCards.find(player0.invalidCard()) == goneCards.end());
    CPPUNIT_ASSERT(0 != observer.rounds());
    CPPUNIT_ASSERT(observer.roundData(0)->mDroppedCards.at(player0.id()).find(invalidCard) == observer.roundData(0)->mDroppedCards.at(player0.id()).end());
}

void GameTest::testInvalidCards01()
{
    // player attacks with card which value is from suggested set but it is not located in the set
    Card invalidCard(SUIT_DIAMONDS, RANK_8);
    AttackWithInvalidCardPlayer player0(invalidCard);

    BasePlayer player1;

    GameCardsTracker tracker;
    Observer observer;

    playRound(player0, player1, tracker, observer);
    // invalid card should not left the game
    CPPUNIT_ASSERT(tracker.goneCards().find(player0.invalidCard()) == tracker.goneCards().end());
    CPPUNIT_ASSERT(0 != observer.rounds());
    CPPUNIT_ASSERT(observer.roundData(0)->mDroppedCards.at(player0.id()).find(invalidCard) == observer.roundData(0)->mDroppedCards.at(player0.id()).end());
}

void GameTest::testInvalidCards02()
{
    // player defends with card is not in suggested set
    Card invalidCard(SUIT_HEARTS, RANK_10);
    DefendWithInvalidCardPlayer player1(invalidCard);

    BasePlayer player0;

    GameCardsTracker tracker;
    Observer observer;

    playRound(player0, player1, tracker, observer);
    // invalid card should not left the game
    CPPUNIT_ASSERT(tracker.goneCards().find(player1.invalidCard()) == tracker.goneCards().end());
    CPPUNIT_ASSERT(0 != observer.rounds());
    CPPUNIT_ASSERT(observer.roundData(0)->mDroppedCards.find(player1.id()) == observer.roundData(0)->mDroppedCards.end());
}

void GameTest::testInvalidCards03()
{
    // player defends with card which value is from suggested set but it is not located in the set
    Card invalidCard(SUIT_SPADES, RANK_9);
    DefendWithInvalidCardPlayer player1(invalidCard);

    BasePlayer player0;

    GameCardsTracker tracker;
    Observer observer;

    playRound(player0, player1, tracker, observer);
    // invalid card should not left the game
    CPPUNIT_ASSERT(tracker.goneCards().find(player1.invalidCard()) == tracker.goneCards().end());
    CPPUNIT_ASSERT(0 != observer.rounds());
    CPPUNIT_ASSERT(observer.roundData(0)->mDroppedCards.find(player1.id()) == observer.roundData(0)->mDroppedCards.end());
}

void GameTest::fullFlow()
{
    TestPlayer0 player0, player1;
    Engine engine;
    Observer observer;

    engine.add(player0);
    engine.add(player1);

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

    deck.generate(ranks, sizeof(ranks) / sizeof(ranks[0]), suits, sizeof(suits) / sizeof(suits[0]));

    engine.setDeck(deck);

    while(engine.playRound());
    CPPUNIT_ASSERT(observer.rounds() == 10);
    CPPUNIT_ASSERT(engine.getLoser() == player1.id());
}


void GameTest::TestPlayer0::gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *> &players)
{
    Observer::gameStarted(trumpSuit, cardSet, players);
    BasePlayer::gameStarted(trumpSuit, cardSet, players);
}

void GameTest::TestPlayer0::cardsGone(const CardSet &cardSet)
{
    Observer::cardsGone(cardSet);
    BasePlayer::cardsGone(cardSet);
}

void GameTest::TestPlayer0::cardsDropped(const PlayerId *playerId, const CardSet &cardSet)
{
    Observer::cardsDropped(playerId, cardSet);
    BasePlayer::cardsDropped(playerId, cardSet);
}

void GameTest::TestPlayer0::cardsPickedUp(const PlayerId *playerId, const CardSet &cardSet)
{
    Observer::cardsPickedUp(playerId, cardSet);
    BasePlayer::cardsPickedUp(playerId, cardSet);
}

void GameTest::TestPlayer0::cardsDealed(const PlayerId *playerId, unsigned int cardsAmount)
{
    Observer::cardsDealed(playerId, cardsAmount);
    BasePlayer::cardsDealed(playerId, cardsAmount);
}

void GameTest::TestPlayer0::gameRestored(const std::vector<const PlayerId*>& playerIds,
        const std::map<const PlayerId*, unsigned int>& playersCards,
        unsigned int deckCards,
        const Suit& trumpSuit,
        const std::vector<Card>& attackCards,
        const std::vector<Card>& defendCards)
{
    Observer::gameRestored(playerIds, playersCards, deckCards, trumpSuit, attackCards, defendCards);
    BasePlayer::gameRestored(playerIds, playersCards, deckCards, trumpSuit, attackCards, defendCards);
}

void GameTest::TestPlayer0::save(decore::DataWriter& writer)
{
    Observer::save(writer);
    BasePlayer::save(writer);
}

void GameTest::TestPlayer0::init(decore::DataReader& reader)
{
    Observer::init(reader);
    BasePlayer::init(reader);
}

void GameTest::TestPlayer0::quit()
{
    Observer::quit();
}

GameTest::AttackWithInvalidCardPlayer::AttackWithInvalidCardPlayer(const Card& card)
    : mInvalidCard(card)
    , mDontCheckCardExist(false)
{
}

const Card& GameTest::AttackWithInvalidCardPlayer::attack(const PlayerId* playerId, const CardSet& cardSet)
{
    if (mDontCheckCardExist || cardSet.find(mInvalidCard) != cardSet.end()) {
        removeCard(&mInvalidCard);
        return mInvalidCard;
    }
    return BasePlayer::attack(playerId, cardSet);
}

const Card& GameTest::AttackWithInvalidCardPlayer::invalidCard()
{
    return mInvalidCard;
}

void GameTest::AttackWithInvalidCardPlayer::cardsUpdated(const CardSet& cardSet)
{
    mDontCheckCardExist= cardSet.find(mInvalidCard) == cardSet.end();
}


GameTest::DefendWithInvalidCardPlayer::DefendWithInvalidCardPlayer(const Card& card)
    : mInvalidCard(card)
    , mDontCheckCardExist(false)
{

}

const Card* GameTest::DefendWithInvalidCardPlayer::defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet)
{
    if (mDontCheckCardExist || cardSet.find(mInvalidCard) != cardSet.end()) {
        removeCard(&mInvalidCard);
        return &mInvalidCard;
    }
    return BasePlayer::defend(playerId, attackCard, cardSet);
}


const Card& GameTest::DefendWithInvalidCardPlayer::invalidCard()
{
    return mInvalidCard;
}

void GameTest::DefendWithInvalidCardPlayer::cardsUpdated(const CardSet& cardSet)
{
    mDontCheckCardExist= cardSet.find(mInvalidCard) == cardSet.end();
}


void GameTest::TestPlayer0::roundStarted(unsigned int roundIndex, const std::vector<const PlayerId *> attackers, const PlayerId *defender)
{
    Observer::roundStarted(roundIndex, attackers, defender);
    BasePlayer::roundStarted(roundIndex, attackers, defender);
}

void GameTest::TestPlayer0::roundEnded(unsigned int roundIndex)
{
    Observer::roundEnded(roundIndex);
    BasePlayer::roundEnded(roundIndex);
}

void GameTest::playRound(Player& player0, Player& player1, GameCardsTracker& tracker, GameObserver& observer)
{
    Engine engine;

    std::vector<Player*> players;
    players.push_back(&player0);
    players.push_back(&player1);

    for(std::vector<Player*>::iterator it = players.begin(); it != players.end(); ++ it) {
        CPPUNIT_ASSERT(engine.add(**it));
    }

    Deck deck;

    deck.push_back(Card(SUIT_SPADES, RANK_6));
    deck.push_back(Card(SUIT_SPADES, RANK_7));

    deck.push_back(Card(SUIT_CLUBS, RANK_6));
    deck.push_back(Card(SUIT_CLUBS, RANK_7));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_8));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_9));

    deck.push_back(Card(SUIT_SPADES, RANK_8));
    deck.push_back(Card(SUIT_SPADES, RANK_9));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_ACE));

    engine.addGameObserver(tracker);
    engine.addGameObserver(observer);

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(tracker.deckCards() == 9);
    engine.playRound();
}
