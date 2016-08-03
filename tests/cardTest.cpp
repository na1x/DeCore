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
    unsigned int ranksSize = sizeof(ranks) / sizeof(Rank);
    Suit suits[] = {
        SUIT_SPADES,
        SUIT_HEARTS,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };
    unsigned int suitsSize = sizeof(suits) / sizeof(Suit);

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

    CPPUNIT_ASSERT(set.deal(cards, 6));

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

    Suit suits[] = {
        SUIT_SPADES,
        SUIT_HEARTS,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };

    set.generate(ranks, sizeof(ranks) / sizeof(Rank), suits, sizeof(suits) / sizeof(Suit));

    CardSet original(set);

    unsigned int notShuffled = set.shuffle();

    unsigned int notShuffledActual = 0;

    for(unsigned int i = 0, end = set.size(); i < end; ++i) {
        if (*original.get(i) == *set.get(i)) {
            ++notShuffledActual;
        }
    }

    CPPUNIT_ASSERT(original.size() == set.size());
    CPPUNIT_ASSERT(!(original == set));
    char buf[1024];
    snprintf(buf, sizeof(buf), "notShuffledActual %u, notShuffled %u", notShuffledActual, notShuffled);
    CPPUNIT_ASSERT_MESSAGE(buf, notShuffledActual == notShuffled);
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

void CardTest::testGetByRank()
{
    using namespace decore;

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
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };

    CardSet set;

    set.generate(ranks, sizeof(ranks) / sizeof(Rank), suits, sizeof(suits) / sizeof(Suit));

    CardSet result;

    set.getCards(RANK_6, result);

    // three suits
    CPPUNIT_ASSERT(3 == result.size());

    set.getCards(RANK_7, result);

    // yet more three suits - cards appended
    CPPUNIT_ASSERT(6 == result.size());
}

void CardTest::testGetBySuit()
{
    using namespace decore;

    Rank ranks[] = {
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_9,
        RANK_10,
    };

    Suit suits[] = {
        SUIT_SPADES,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };

    CardSet set;

    set.generate(ranks, sizeof(ranks) / sizeof(Rank), suits, sizeof(suits) / sizeof(Suit));

    CardSet result;

    set.getCards(SUIT_SPADES, result);

    // five cards for each suits
    CPPUNIT_ASSERT(5 == result.size());

    CPPUNIT_ASSERT(result.get(0)->suit() == SUIT_SPADES);

    set.getCards(SUIT_DIAMONDS, result);

    // yet more five cards
    CPPUNIT_ASSERT(10 == result.size());

    CPPUNIT_ASSERT(result.get(result.size() - 1)->suit() == SUIT_DIAMONDS);
}

void CardTest::testIntersection()
{
    using namespace decore;

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

    Suit suits0[] = {
        SUIT_SPADES,
    };

    CardSet set0;
    set0.generate(ranks, sizeof(ranks) / sizeof(Rank), suits0, sizeof(suits0) / sizeof(Suit));

    Suit suits1[] = {
        SUIT_HEARTS,
    };

    CardSet set1;
    set1.generate(ranks, sizeof(ranks) / sizeof(Rank), suits1, sizeof(suits1) / sizeof(Suit));

    CardSet result0;
    CardSet result1;

    set0.intersect(set1, result0);
    set1.intersect(set0, result1);

    CPPUNIT_ASSERT(result0.size() == result1.size());
}
