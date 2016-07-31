#include "cardTest.h"
#include "cardSet.h"
#include "card.h"

void CardTest::testCreate()
{
    decore::CardSet set;
    CPPUNIT_ASSERT(!set.size());
    CPPUNIT_ASSERT(set.empty());
}

void CardTest::testGenerate()
{
    using namespace decore;
    CardSet set;

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
    unsigned int ranksSize = sizeof(ranks);
    Suit suits[] = {
        SUIT_SPADES,
        SUIT_HEARTS,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };
    unsigned int suitsSize = sizeof(suits);

    set.generate(ranks, ranksSize, suits, suitsSize);

    CPPUNIT_ASSERT(ranksSize * suitsSize == set.size());
}

void CardTest::testAdd()
{
    using namespace decore;
    CardSet set;
    Card cardToAdd = Card(SUIT_CLUBS, RANK_6);
    Card notAddedCard = Card(SUIT_CLUBS, RANK_7);
    set.add(cardToAdd);
    CPPUNIT_ASSERT(!set.empty());
    CPPUNIT_ASSERT(1 == set.size());

    std::map<PlayerId *, CardSet> cards;
    PlayerId id;
    cards[&id] = CardSet();

    set.deal(cards, 6);

    const CardSet& dealCards = cards[&id];
    CPPUNIT_ASSERT(1 == dealCards.size());
    CPPUNIT_ASSERT(set.empty());
    CPPUNIT_ASSERT(*dealCards.get(0) == cardToAdd);
    CPPUNIT_ASSERT(!(*dealCards.get(0) == notAddedCard));
}

void CardTest::testShuffle()
{
    using namespace decore;
    CardSet set;

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
    unsigned int ranksSize = sizeof(ranks);
    Suit suits[] = {
        SUIT_SPADES,
        SUIT_HEARTS,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };
    unsigned int suitsSize = sizeof(suits);

    set.generate(ranks, ranksSize, suits, suitsSize);

    CPPUNIT_ASSERT(ranksSize * suitsSize == set.size());

    CardSet original(set);

    set.shuffle();

    CPPUNIT_ASSERT(original.size() == set.size());
    CPPUNIT_ASSERT(!(original == set));
}

void CardTest::testGet()
{
    using namespace decore;
    CardSet set;
    set.add(Card(SUIT_CLUBS, RANK_6));
    CPPUNIT_ASSERT(!set.empty());
    CPPUNIT_ASSERT(1 == set.size());
    CPPUNIT_ASSERT(set.get(0));
    CPPUNIT_ASSERT(!set.get(1));
}
