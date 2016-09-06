#include <algorithm>
#include <cassert>

#include "gameCardsTracker.h"

namespace decore
{

PlayerCards::PlayerCards()
    : mUnknownCards(0)
{

}

void PlayerCards::addUnknownCards(unsigned int cardsAmount)
{
    mUnknownCards += cardsAmount;
}

void PlayerCards::addCards(const CardSet& cards)
{
    for(CardSet::const_iterator it = cards.begin(); it != cards.end(); ++it) {
        bool inserted = mKnownCards.insert(*it).second;
        assert(inserted);
    }
}

void PlayerCards::removeCards(const CardSet& cards)
{
    for(CardSet::const_iterator it = cards.begin(); it != cards.end(); ++it) {
        if (mKnownCards.find(*it) != mKnownCards.end()) {
            bool erased = mKnownCards.erase(*it);
            assert(erased);
        } else {
            assert(mUnknownCards);
            mUnknownCards--;
        }
    }
}

bool PlayerCards::empty() const
{
    return !mUnknownCards && mKnownCards.empty();
}

unsigned int PlayerCards::size() const
{
    return mUnknownCards + mKnownCards.size();
}

unsigned int PlayerCards::unknownCards() const
{
    return mUnknownCards;
}

const CardSet& PlayerCards::knownCards() const
{
    return mKnownCards;
}

GameCardsTracker::GameCardsTracker()
    : mDefender(NULL)
    , mLastRoundIndex(0)
{

}

void GameCardsTracker::gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *>& players)
{
    mGameCards = cardSet;
    mTrumpSuit = trumpSuit;
    mDeckCardsNumber = cardSet.size();
    mPlayerIds.insert(mPlayerIds.begin(), players.begin(), players.end());
    for(std::vector<const PlayerId*>::const_iterator it = players.begin(); it != players.end(); ++it) {
        mPlayersCards[*it] = PlayerCards();
    }
}

void GameCardsTracker::roundStarted(unsigned int roundIndex, const std::vector<const PlayerId*> attackers, const PlayerId* defender)
{
    mAttackers = attackers;
    mDefender = defender;
    mLastRoundIndex = roundIndex;
}

void GameCardsTracker::roundEnded(unsigned int roundIndex)
{
    mAttackers.clear();
    mDefender = NULL;
    mAttackCards.clear();
    mDefendCards.clear();
    assert(mLastRoundIndex == roundIndex);
    mLastRoundIndex = roundIndex;
}

void GameCardsTracker::cardsPickedUp(const PlayerId* playerId, const CardSet &cards)
{
    assert(mPlayersCards.find(playerId) != mPlayersCards.end());
    mPlayersCards[playerId].addCards(cards);
    // the cardSet is picked up from table cards
    // ensure that proper cards removed
    assert(cards.size() == mAttackCards.size() + mDefendCards.size());
#ifndef NDEBUG
    for (CardSet::const_iterator it = cards.begin(); it != cards.end(); ++it) {
        assert(std::find(mAttackCards.begin(), mAttackCards.end(), *it) != mAttackCards.end()
                || std::find(mDefendCards.begin(), mDefendCards.end(), *it) != mDefendCards.end());
    }
#endif // NDEBUG
}

void GameCardsTracker::cardsDealed(const PlayerId* playerId, unsigned int cardsAmount)
{
    mDeckCardsNumber -= cardsAmount;
    assert(mPlayersCards.find(playerId) != mPlayersCards.end());
    mPlayersCards[playerId].addUnknownCards(cardsAmount);
}

void GameCardsTracker::cardsGone(const CardSet &cardSet)
{
    std::for_each(cardSet.begin(), cardSet.end(), CardRemover(mGameCards));
    // the cardSet is left from table cards
    // ensure that proper cards removed
    assert(cardSet.size() == mAttackCards.size() + mDefendCards.size());
#ifndef NDEBUG
    for (CardSet::const_iterator it = cardSet.begin(); it != cardSet.end(); ++it) {
        assert(std::find(mAttackCards.begin(), mAttackCards.end(), *it) != mAttackCards.end()
                || std::find(mDefendCards.begin(), mDefendCards.end(), *it) != mDefendCards.end());
    }
#endif // NDEBUG
    mGoneCards.insert(cardSet.begin(), cardSet.end());
}

void GameCardsTracker::cardsDropped(const PlayerId* playerId, const CardSet &cards)
{
    assert(mPlayersCards.find(playerId) != mPlayersCards.end());
    mPlayersCards[playerId].removeCards(cards);
    std::vector<Card> * dst;
    if (playerId == mDefender) {
        dst = &mDefendCards;
    } else {
        dst = &mAttackCards;;
    }
    dst->insert(dst->end(), cards.begin(), cards.end());
}

void GameCardsTracker::save(DataWriter& writer)
{
    writer.write(mGameCards.begin(), mGameCards.end());
    writer.write(mTrumpSuit);

    unsigned int playersCount = mPlayersCards.size();
    writer.write(playersCount);
    for (std::map<const PlayerId*, PlayerCards>::iterator it = mPlayersCards.begin(); it != mPlayersCards.end(); ++it) {
        writer.write(mPlayerIds.index(it->first));
        PlayerCards& cards = it->second;
        writer.write(cards.unknownCards());
        writer.write(cards.knownCards().begin(), cards.knownCards().end());
    }

    writer.write(mGoneCards.begin(), mGoneCards.end());
    writer.write(mLastRoundIndex);
    writer.write(mAttackCards.begin(), mAttackCards.end());
    writer.write(mDefendCards.begin(), mDefendCards.end());
}

void GameCardsTracker::init(DataReader& reader)
{
    const Card defaultCard(SUIT_LAST, RANK_LAST);

    reader.read(mGameCards, defaultCard);
    reader.read(mTrumpSuit);

    unsigned int playersCount;
    reader.read(playersCount);
    while (playersCount--) {
        unsigned int playerIndex;
        reader.read(playerIndex);
        unsigned int unknownCards;
        reader.read(unknownCards);
        CardSet knownCards;
        reader.read(knownCards, defaultCard);
        PlayerCards cards;
        cards.addCards(knownCards);
        cards.addUnknownCards(unknownCards);
        assert(mPlayerIds.size() > playerIndex);
        mPlayersCards[mPlayerIds[playerIndex]] = cards;
    }

    reader.read(mGoneCards, defaultCard);
    reader.read(mLastRoundIndex);
    reader.read(mAttackCards, defaultCard);
    reader.read(mDefendCards, defaultCard);

    // check data consistency
#ifndef NDEBUG
    for (std::map<const PlayerId*, unsigned int>::const_iterator it = mRestoredPlayerCards.begin(); it != mRestoredPlayerCards.end(); ++it) {
        assert(mPlayersCards.at((*it).first).size() == (*it).second);
    }
    assert(mRestoredAttackCards.size() == mAttackCards.size());
    assert(mRestoredDefendCards.size() == mDefendCards.size());
    for (unsigned int i = 0; i < mRestoredAttackCards.size(); i++) {
        mAttackCards.at(i) == mRestoredAttackCards.at(i);
    }
    for (unsigned int i = 0; i < mRestoredDefendCards.size(); i++) {
        mDefendCards.at(i) == mRestoredDefendCards.at(i);
    }
#endif // NDEBUG
}

void GameCardsTracker::gameRestored(const std::vector<const PlayerId*>& playerIds,
        const std::map<const PlayerId*, unsigned int>& playersCards,
        unsigned int deckCards,
        const Suit& trumpSuit,
        const std::vector<Card>& attackCards,
        const std::vector<Card>& defendCards)
{
    mPlayerIds.insert(mPlayerIds.begin(), playerIds.begin(), playerIds.end());
#ifndef NDEBUG
    mRestoredPlayerCards = playersCards;
    mRestoredAttackCards = attackCards;
    mRestoredDefendCards = defendCards;
#endif
    mDeckCardsNumber = deckCards;
    mTrumpSuit = trumpSuit;
}

void GameCardsTracker::quit()
{
    // do nothing
}

const CardSet &GameCardsTracker::gameCards() const
{
    return mGameCards;
}

const Suit &GameCardsTracker::trumpSuit() const
{
    return mTrumpSuit;
}

unsigned int GameCardsTracker::deckCards() const
{
    return mDeckCardsNumber;
}

const PlayerCards& GameCardsTracker::playerCards(const PlayerId* playerId) const
{
    assert(mPlayersCards.find(playerId) != mPlayersCards.end());
    return mPlayersCards.at(playerId);
}

const PlayerIds& GameCardsTracker::playerIds() const
{
    return mPlayerIds;
}

const CardSet& GameCardsTracker::goneCards() const
{
    return mGoneCards;
}

unsigned int GameCardsTracker::lastRoundIndex() const
{
    return mLastRoundIndex;
}

const std::vector<Card>& GameCardsTracker::attackCards() const
{
    return mAttackCards;
}

const std::vector<Card>& GameCardsTracker::defendCards() const
{
    return mDefendCards;
}

GameCardsTracker::CardRemover::CardRemover(CardSet &cards)
    : mCards(cards)
{
}

void GameCardsTracker::CardRemover::operator()(const Card &card)
{
     assert(1 == mCards.erase(card));
}

}

