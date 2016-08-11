#include "rulesTest.h"
#include "playerId.h"
#include "rules.h"
#include "cardSet.h"
#include "deck.h"
#include "defines.h"

class Id : public decore::PlayerId
{

};

void RulesTest::testPickNext()
{
    using namespace decore;
    std::vector<const PlayerId*> playerIds;

    const unsigned int IDS = 5;

    // add players
    for (unsigned int i = 0; i < IDS; ++i) {
        playerIds.push_back(new Id());
    }

    // check that there's no next player for not existing player id
    CPPUNIT_ASSERT(!Rules::pickNext(playerIds, NULL));

    // check pickNext
    for(std::vector<const PlayerId*>::iterator it = playerIds.begin(); it != playerIds.end(); ++it) {
        std::vector<const PlayerId*>::iterator next = it + 1;
        if (next == playerIds.end()) {
            next = playerIds.begin();
        }
        CPPUNIT_ASSERT(*next == Rules::pickNext(playerIds, *it));
    }

    // cleanup players
    for(std::vector<const PlayerId*>::iterator it = playerIds.begin(); it != playerIds.end(); ++it) {
        delete *it;
    }
}

void RulesTest::testAttackCards()
{
    using namespace decore;

    CardSet tableCards;
    CardSet playerCards;

    // empty table cards - player can attack with any card
    playerCards.insert(Card(SUIT_CLUBS, RANK_6));
    playerCards.insert(Card(SUIT_CLUBS, RANK_7));
    playerCards.insert(Card(SUIT_CLUBS, RANK_8));
    playerCards.insert(Card(SUIT_CLUBS, RANK_9));

    CardSet attackCards = Rules::getAttackCards(tableCards, playerCards);
    CPPUNIT_ASSERT(!attackCards.empty());
    CPPUNIT_ASSERT(attackCards == playerCards);

    //
    tableCards.insert(Card(SUIT_SPADES, RANK_6));
    attackCards = Rules::getAttackCards(tableCards, playerCards);
    CPPUNIT_ASSERT(1 == attackCards.size());
    CPPUNIT_ASSERT(attackCards.find(Card(SUIT_CLUBS, RANK_6)) != attackCards.end());

    //
    tableCards.insert(Card(SUIT_SPADES, RANK_9));
    attackCards = Rules::getAttackCards(tableCards, playerCards);
    CPPUNIT_ASSERT(2 == attackCards.size());
    CPPUNIT_ASSERT(attackCards.find(Card(SUIT_CLUBS, RANK_6)) != attackCards.end());
    CPPUNIT_ASSERT(attackCards.find(Card(SUIT_CLUBS, RANK_9)) != attackCards.end());
}

void RulesTest::testDefendCards()
{
    using namespace decore;

    Suit trump = SUIT_SPADES;

    CardSet playerCards;

    playerCards.insert(Card(SUIT_CLUBS, RANK_7));
    playerCards.insert(Card(SUIT_DIAMONDS, RANK_7));
    playerCards.insert(Card(SUIT_HEARTS, RANK_7));

    CardSet defendCards = Rules::getDefendCards(Card(SUIT_CLUBS, RANK_6), playerCards, trump);
    CPPUNIT_ASSERT(1 == defendCards.size());
    CPPUNIT_ASSERT(*defendCards.begin() == Card(SUIT_CLUBS, RANK_7));

    defendCards = Rules::getDefendCards(Card(SUIT_DIAMONDS, RANK_6), playerCards, trump);
    CPPUNIT_ASSERT(1 == defendCards.size());
    CPPUNIT_ASSERT(*defendCards.begin() == Card(SUIT_DIAMONDS, RANK_7));

    defendCards = Rules::getDefendCards(Card(SUIT_HEARTS, RANK_6), playerCards, trump);
    CPPUNIT_ASSERT(1 == defendCards.size());
    CPPUNIT_ASSERT(*defendCards.begin() == Card(SUIT_HEARTS, RANK_7));

    defendCards = Rules::getDefendCards(Card(SUIT_HEARTS, RANK_8), playerCards, trump);
    CPPUNIT_ASSERT(defendCards.empty());

    defendCards = Rules::getDefendCards(Card(SUIT_SPADES, RANK_6), playerCards, trump);
    CPPUNIT_ASSERT(defendCards.empty());

    // add trump to the player
    playerCards.insert(Card(SUIT_SPADES, RANK_7));

    defendCards = Rules::getDefendCards(Card(SUIT_CLUBS, RANK_6), playerCards, trump);
    CPPUNIT_ASSERT(2 == defendCards.size());
    CPPUNIT_ASSERT(defendCards.find(Card(SUIT_CLUBS, RANK_7)) != defendCards.end());
    CPPUNIT_ASSERT(defendCards.find(Card(SUIT_SPADES, RANK_7)) != defendCards.end());

    defendCards = Rules::getDefendCards(Card(SUIT_CLUBS, RANK_8), playerCards, trump);
    CPPUNIT_ASSERT(1 == defendCards.size());
    CPPUNIT_ASSERT(defendCards.find(Card(SUIT_SPADES, RANK_7)) != defendCards.end());

    defendCards = Rules::getDefendCards(Card(SUIT_SPADES, RANK_8), playerCards, trump);
    CPPUNIT_ASSERT(defendCards.empty());
}

void RulesTest::testDeal0()
{
    using namespace decore;

    Deck deck;

    deck.push_back(Card(SUIT_CLUBS, RANK_6));
    deck.push_back(Card(SUIT_CLUBS, RANK_7));
    deck.push_back(Card(SUIT_CLUBS, RANK_8));
    deck.push_back(Card(SUIT_CLUBS, RANK_9));
    deck.push_back(Card(SUIT_CLUBS, RANK_10));

    std::vector<CardSet*> cards;

    CardSet c0;
    CardSet c1;
    CardSet c2;

    cards.push_back(&c0);
    cards.push_back(&c1);
    cards.push_back(&c2);

    CPPUNIT_ASSERT(Rules::deal(deck, cards));

    CPPUNIT_ASSERT(2 == c0.size());
    CPPUNIT_ASSERT(2 == c1.size());
    CPPUNIT_ASSERT(1 == c2.size());

    CPPUNIT_ASSERT(c0.find(Card(SUIT_CLUBS, RANK_6)) != c0.end());
    CPPUNIT_ASSERT(c0.find(Card(SUIT_CLUBS, RANK_9)) != c0.end());

    CPPUNIT_ASSERT(c1.find(Card(SUIT_CLUBS, RANK_7)) != c1.end());
    CPPUNIT_ASSERT(c1.find(Card(SUIT_CLUBS, RANK_10)) != c1.end());

    CPPUNIT_ASSERT(c2.find(Card(SUIT_CLUBS, RANK_8)) != c2.end());
}

void RulesTest::testDeal1()
{
    using namespace decore;
    const unsigned int MAX_CARDS = 6;

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

    CardSet c0;
    CardSet c1;
    CardSet c2;

    std::vector<CardSet*> cards;

    cards.push_back(&c0);
    cards.push_back(&c1);
    cards.push_back(&c2);

    CPPUNIT_ASSERT(Rules::deal(deck, cards));

    CPPUNIT_ASSERT(MAX_CARDS == c0.size());
    CPPUNIT_ASSERT(MAX_CARDS == c1.size());
    CPPUNIT_ASSERT(MAX_CARDS == c2.size());

    CardSet c3;
    cards.push_back(&c3);

    CardSet c0copy(c0);
    CardSet c1copy(c1);
    CardSet c2copy(c2);

    CPPUNIT_ASSERT(Rules::deal(deck, cards));
    CPPUNIT_ASSERT(MAX_CARDS == c0.size());
    CPPUNIT_ASSERT(MAX_CARDS == c1.size());
    CPPUNIT_ASSERT(MAX_CARDS == c2.size());
    CPPUNIT_ASSERT(MAX_CARDS == c3.size());

    CPPUNIT_ASSERT(c0copy == c0);
    CPPUNIT_ASSERT(c1copy == c1);
    CPPUNIT_ASSERT(c2copy == c2);

    CPPUNIT_ASSERT(!Rules::deal(deck, cards));

    CPPUNIT_ASSERT(12 == deck.size());
}

