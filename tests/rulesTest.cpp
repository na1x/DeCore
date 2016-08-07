#include "rulesTest.h"
#include "playerId.h"
#include "rules.h"
#include "cardSet.h"

class Id : public decore::PlayerId
{

};

void RulesTest::testPickNext()
{
    using namespace decore;
    std::vector<PlayerId*> playerIds;

    const unsigned int IDS = 5;

    // add players
    for (unsigned int i = 0; i < IDS; ++i) {
        playerIds.push_back(new Id());
    }

    // check that there's no next player for not existing player id
    CPPUNIT_ASSERT(!Rules::pickNext(playerIds, NULL));

    // check pickNext
    for(std::vector<PlayerId*>::iterator it = playerIds.begin(); it != playerIds.end(); ++it) {
        std::vector<PlayerId*>::iterator next = it + 1;
        if (next == playerIds.end()) {
            next = playerIds.begin();
        }
        CPPUNIT_ASSERT(*next == Rules::pickNext(playerIds, *it));
    }

    // cleanup players
    for(std::vector<PlayerId*>::iterator it = playerIds.begin(); it != playerIds.end(); ++it) {
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

