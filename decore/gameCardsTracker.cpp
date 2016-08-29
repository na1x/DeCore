#include <algorithm>
#include <cassert>

#include "gameCardsTracker.h"

namespace decore {


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

void GameCardsTracker::gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *>& players)
{
    mGameCards = cardSet;
    mTrumpSuit = trumpSuit;
    mDeckCardsNumber = cardSet.size();
    for(std::vector<const PlayerId*>::const_iterator it = players.begin(); it != players.end(); ++it) {
        mPlayersCards[*it] = PlayerCards();
    }
}

void GameCardsTracker::roundStarted(unsigned int, const std::vector<const PlayerId*> attackers, const PlayerId* defender)
{
    mAttackers = attackers;
    mDefender = defender;
}

void GameCardsTracker::roundEnded(unsigned int )
{
    mAttackers.clear();
    mDefender = NULL;
    mAttackCards.clear();
    mDefendCards.clear();
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

void GameCardsTracker::cardsLeft(const CardSet &cardSet)
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

void GameCardsTracker::tableCardsRestored(const std::vector<Card>& attackCards, const std::vector<Card>& defendCards)
{
    mAttackCards = attackCards;
    mDefendCards = defendCards;
}

void GameCardsTracker::write(DataWriter& writer)
{
    assert(false);
    // TODO: implement
}

void GameCardsTracker::init(DataReader& reader)
{
    assert(false);
    // TODO: implement
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


GameCardsTracker::CardRemover::CardRemover(CardSet &cards)
    : mCards(cards)
{
}

void GameCardsTracker::CardRemover::operator()(const Card &card)
{
     assert(1 == mCards.erase(card));
}

}

