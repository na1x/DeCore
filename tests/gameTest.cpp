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

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(!engine.playRound()); // cards for one round only

    CPPUNIT_ASSERT(player0Id == player0.mId);
    CPPUNIT_ASSERT(player1Id == player1.mId);

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

    CPPUNIT_ASSERT(engine.setDeck(deck));

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

    // deck has cards for player0 and player1 to play the round without pitch from player2
    CPPUNIT_ASSERT(1 == player2.mPlayerCards.size());
}

void GameTest::testOneRound02()
{
    // attacker has 2 cards, and goes with trump (due to CardSet sort order)
    // player two picks card
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

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(engine.playRound());
    CPPUNIT_ASSERT(!engine.playRound());
    CPPUNIT_ASSERT(engine.getLoser() == player1.mId);
    CPPUNIT_ASSERT((player0.mPlayerCards.end() - 1)->empty());
    CPPUNIT_ASSERT((player1.mPlayerCards.end() - 1)->size() == 1);
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

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(!engine.playRound());
    CPPUNIT_ASSERT(engine.getLoser() == player0.mId);
    CPPUNIT_ASSERT((player0.mPlayerCards.end() - 1)->size() == 1);
    CPPUNIT_ASSERT((player1.mPlayerCards.end() - 1)->empty());
}

void GameTest::testDraw00()
{
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

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(!engine.playRound());
    CPPUNIT_ASSERT(!engine.getLoser());
    CPPUNIT_ASSERT((player0.mPlayerCards.end() - 1)->empty());
    CPPUNIT_ASSERT((player1.mPlayerCards.end() - 1)->empty());
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

    deck.push_back(Card(SUIT_HEARTS, RANK_6));
    deck.push_back(Card(SUIT_HEARTS, RANK_10));
    deck.push_back(Card(SUIT_HEARTS, RANK_JACK));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_JACK));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_QUEEN));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_KING));

    deck.push_back(Card(SUIT_CLUBS, RANK_ACE));

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(engine.playRound());

    // one deal + three attacks
    CPPUNIT_ASSERT(player0.mPlayerCards.size() == 4);
    // one deal + six defends
    CPPUNIT_ASSERT(player1.mPlayerCards.size() == 7);
    // one deal + three attacks
    CPPUNIT_ASSERT(player2.mPlayerCards.size() == 4);

    CPPUNIT_ASSERT((player0.mPlayerCards.end() - 1)->size() == 3);
    CPPUNIT_ASSERT((player1.mPlayerCards.end() - 1)->empty());
    CPPUNIT_ASSERT((player2.mPlayerCards.end() - 1)->size() == 3);
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

    Deck deck;

    deck.push_back(Card(SUIT_SPADES, RANK_7));
    deck.push_back(Card(SUIT_SPADES, RANK_6));

    deck.push_back(Card(SUIT_HEARTS, RANK_7));
    deck.push_back(Card(SUIT_HEARTS, RANK_6));

    deck.push_back(Card(SUIT_DIAMONDS, RANK_7));
    deck.push_back(Card(SUIT_DIAMONDS, RANK_6));

    deck.push_back(Card(SUIT_CLUBS, RANK_ACE));

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(engine.playRound());

    // deal and three attacks
    CPPUNIT_ASSERT(player0.mPlayerCards.size() == 4);
    // deal and one pick up
    CPPUNIT_ASSERT(player1.mPlayerCards.size() == 2);

    CPPUNIT_ASSERT((player0.mPlayerCards.end() - 1)->size() == 1);
    CPPUNIT_ASSERT((player1.mPlayerCards.end() - 1)->size() == 6);
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

    CPPUNIT_ASSERT(engine.setDeck(deck));

    CPPUNIT_ASSERT(engine.playRound());

    // deal and two attacks
    CPPUNIT_ASSERT(player0.mPlayerCards.size() == 3);
    // deal + defend + one pick up
    CPPUNIT_ASSERT(player1.mPlayerCards.size() == 3);
    // deal and one pitch
    CPPUNIT_ASSERT(player2.mPlayerCards.size() == 2);

    CPPUNIT_ASSERT((player0.mPlayerCards.end() - 1)->size() == 2);
    CPPUNIT_ASSERT((player1.mPlayerCards.end() - 1)->size() == 6);
    CPPUNIT_ASSERT((player2.mPlayerCards.end() - 1)->size() == 2);
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

    CPPUNIT_ASSERT(!player0.mRoundsData.empty());

    const Observer::RoundData& round0 = *player0.mRoundsData[0];

    CPPUNIT_ASSERT(1 == round0.mDroppedCards.size());
    CPPUNIT_ASSERT(checkCard(round0.mDroppedCards.at(playerIds[0]), deck[0]));
    CPPUNIT_ASSERT(1 == round0.mPickedUpCards.size());
    CPPUNIT_ASSERT(checkCard(round0.mPickedUpCards.at(playerIds[1]), deck[0]));

    CPPUNIT_ASSERT(!engine.playRound());

    CPPUNIT_ASSERT(engine.getLoser() == player1.mId);

    CPPUNIT_ASSERT(2 == player0.mRoundsData.size());

    const Observer::RoundData& round1 = *player0.mRoundsData[1];

    CPPUNIT_ASSERT(2 == round1.mDroppedCards.size());
    CPPUNIT_ASSERT(checkCard(round1.mDroppedCards.at(playerIds[2]), deck[2]));
    CPPUNIT_ASSERT(checkCard(round1.mDroppedCards.at(playerIds[0]), deck[3]));
    CPPUNIT_ASSERT(round1.mPickedUpCards.empty());
}

GameTest::Observer::Observer()
    : mCurrentRoundIndex(-1)
    , mCurrentRoundData(NULL)
{

}

GameTest::Observer::~Observer()
{
    for (std::vector<const RoundData*>::iterator it = mRoundsData.begin(); it != mRoundsData.end(); ++it) {
        delete *it;
    }
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
    CardSet& set = mCurrentRoundData->mDroppedCards[playerId];
    unsigned int oldSize = set.size();
    set.insert(cardSet.begin(), cardSet.end());
    CPPUNIT_ASSERT(oldSize + cardSet.size() == set.size());
}

void GameTest::Observer::cardsPickedUp(const PlayerId *playerId, const CardSet &cardSet)
{
    mPlayersCards[playerId] += cardSet.size();
    CardSet& set = mCurrentRoundData->mPickedUpCards[playerId];
    unsigned int oldSize = set.size();
    set.insert(cardSet.begin(), cardSet.end());
    CPPUNIT_ASSERT(oldSize + cardSet.size() == set.size());
}

void GameTest::Observer::cardsDealed(const PlayerId *playerId, unsigned int cardsAmount)
{
    mPlayersCards[playerId] += cardsAmount;
    mGameCardsCount -= cardsAmount;
}

void GameTest::Observer::roundStarted(unsigned int roundIndex, const std::vector<const PlayerId *> attackers, const PlayerId *defender)
{
    CPPUNIT_ASSERT(mCurrentRoundIndex != roundIndex);
    mCurrentRoundIndex = roundIndex;
    CPPUNIT_ASSERT(!mCurrentRoundData);
    mCurrentRoundData = new RoundData();
    std::copy(attackers.begin(), attackers.end(), std::inserter(mCurrentRoundData->mPlayers, mCurrentRoundData->mPlayers.begin()));
    mCurrentRoundData->mPlayers.push_back(defender);
}

void GameTest::Observer::roundEnded(unsigned int roundIndex)
{
    CPPUNIT_ASSERT(mCurrentRoundIndex == roundIndex);
    mRoundsData.push_back(mCurrentRoundData);
    mCurrentRoundData = NULL;
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

void GameTest::TestPlayer0::cardsPickedUp(const PlayerId *playerId, const CardSet &cardSet)
{
    Observer::cardsPickedUp(playerId, cardSet);
}

void GameTest::TestPlayer0::cardsDealed(const PlayerId *playerId, unsigned int cardsAmount)
{
    Observer::cardsDealed(playerId, cardsAmount);
}

void GameTest::TestPlayer0::updateCards(const Card *card)
{
    CardSet currentCards = *(mPlayerCards.end() - 1);
    CPPUNIT_ASSERT(currentCards.erase(*card));
    mPlayerCards.push_back(currentCards);
}


void GameTest::TestPlayer0::roundStarted(unsigned int roundIndex, const std::vector<const PlayerId *> attackers, const PlayerId *defender)
{
    Observer::roundStarted(roundIndex, attackers, defender);
}

void GameTest::TestPlayer0::roundEnded(unsigned int roundIndex)
{
    Observer::roundEnded(roundIndex);
}
