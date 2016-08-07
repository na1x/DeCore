#include "cardTest.h"
#include "cardSet.h"
#include "card.h"
#include "deck.h"

#define ARRAY_SIZE(x) \
    sizeof(x) / sizeof(x[0])
#define GENERATE(x, y) \
    generate(x, ARRAY_SIZE(x), y, ARRAY_SIZE(y));

static decore::CardSet generate(const Rank *ranks, unsigned int ranksSize, const Suit *suits, unsigned int suitsSize)
{
    decore::CardSet res;
    for(unsigned int suitIndex = 0; suitIndex < suitsSize; ++suitIndex) {
        for(unsigned int rankIndex = 0; rankIndex < ranksSize; ++rankIndex) {
            res.insert(decore::Card(suits[suitIndex], ranks[rankIndex]));
        }
    }
    return res;
}

void CardTest::testCreate()
{
    decore::CardSet set;
    CPPUNIT_ASSERT(!set.size());
    CPPUNIT_ASSERT(set.empty());
}

void CardTest::testGenerate()
{
    using namespace decore;
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
    unsigned int ranksSize = ARRAY_SIZE(ranks);
    Suit suits[] = {
        SUIT_SPADES,
        SUIT_HEARTS,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };
    unsigned int suitsSize = ARRAY_SIZE(suits);

    deck.generate(ranks, ranksSize, suits, suitsSize);

    CPPUNIT_ASSERT(ranksSize * suitsSize == deck.size());
}

void CardTest::testSetAddAll()
{
    using namespace decore;
    std::vector<Card> cards;
    cards.push_back(Card(SUIT_CLUBS, RANK_6));
    cards.push_back(Card(SUIT_CLUBS, RANK_7));
    cards.push_back(Card(SUIT_CLUBS, RANK_8));
    cards.push_back(Card(SUIT_CLUBS, RANK_9));

    CardSet set0;
    // add card and vector which includes the card
    CPPUNIT_ASSERT(set0.insert(Card(SUIT_CLUBS, RANK_6)).second);
    CPPUNIT_ASSERT(!set0.addAll(cards));

    CardSet set1;
    // add same vector twice
    CPPUNIT_ASSERT(set1.addAll(cards));
    CPPUNIT_ASSERT(set1.size() == cards.size());
    CPPUNIT_ASSERT(!set1.addAll(cards));

    // add vector with not unique cards
    cards.push_back(Card(SUIT_CLUBS, RANK_9));
    cards.push_back(Card(SUIT_CLUBS, RANK_9));
    CardSet set2;
    CPPUNIT_ASSERT(!set2.addAll(cards));

}

void CardTest::testShuffle()
{
    using namespace decore;
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

    Deck original(deck);

    unsigned int notShuffled = deck.shuffle();

    unsigned int notShuffledActual = 0;

    for(unsigned int i = 0, end = deck.size(); i < end; ++i) {
        if (original[i] == deck[i]) {
            ++notShuffledActual;
        }
    }

    CPPUNIT_ASSERT(original.size() == deck.size());
    CPPUNIT_ASSERT(!(original == deck));
    char buf[1024];
    snprintf(buf, sizeof(buf), "notShuffledActual %u, notShuffled %u", notShuffledActual, notShuffled);
    CPPUNIT_ASSERT_MESSAGE(buf, notShuffledActual == notShuffled);
}

void CardTest::testGet()
{
    using namespace decore;
    CardSet set;
    Card card(SUIT_CLUBS, RANK_6);
    set.insert(card);
    CPPUNIT_ASSERT(!set.empty());
    CPPUNIT_ASSERT(1 == set.size());
    CPPUNIT_ASSERT(1 == set.erase(card));
    CPPUNIT_ASSERT(!set.erase(card));
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

    CardSet set = GENERATE(ranks, suits);

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

    CardSet set = GENERATE(ranks, suits);

    CardSet result;

    set.getCards(SUIT_SPADES, result);

    // five cards for each suits
    CPPUNIT_ASSERT(5 == result.size());

    CPPUNIT_ASSERT(result.begin()->suit() == SUIT_SPADES);

    set.getCards(SUIT_DIAMONDS, result);

    // yet more five cards and one is removed
    CPPUNIT_ASSERT(10 == result.size());
    CPPUNIT_ASSERT((--result.end())->suit() == SUIT_DIAMONDS);
}
