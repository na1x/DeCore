#include <cstddef>
#include <cassert>

#include "observer.h"
#include "cardTest.h"

using namespace decore;

Observer::Observer()
    : mCurrentRoundIndex(-1)
    , mCurrentRoundData(NULL)
    , mRestored(false)
{

}

Observer::~Observer()
{
    for (std::vector<const RoundData*>::iterator it = mRoundsData.begin(); it != mRoundsData.end(); ++it) {
        delete *it;
    }
    delete mCurrentRoundData;
}

void Observer::gameStarted(const Suit &trumpSuit, const CardSet &cardSet, const std::vector<const PlayerId *> &players)
{
    assert(mPlayers.empty());
    mPlayers.insert(mPlayers.begin(), players.begin(), players.end());
    mTrumpSuit = trumpSuit;
    mGameCards = cardSet;
    for(std::vector<const PlayerId*>::iterator it = mPlayers.begin(); it != mPlayers.end(); ++it) {
        mPlayersCards[*it] = 0;
    }
    mGameCardsCount = mGameCards.size();
}

void Observer::cardsGone(const CardSet &cardSet)
{
    mGameCardsCount -= cardSet.size();
}

void Observer::cardsDropped(const PlayerId *playerId, const CardSet &cardSet)
{
    mPlayersCards[playerId] -= cardSet.size();
    if (mCurrentRoundData->mDroppedCards.find(playerId) == mCurrentRoundData->mDroppedCards.end()) {
        mCurrentRoundData->mDroppedCards[playerId] = CardSet();
    }
    CardSet& set = mCurrentRoundData->mDroppedCards[playerId];
    unsigned int oldSize = set.size();
    set.insert(cardSet.begin(), cardSet.end());
    assert(oldSize + cardSet.size() == set.size());
}

void Observer::cardsPickedUp(const PlayerId *playerId, const CardSet &cardSet)
{
    mPlayersCards[playerId] += cardSet.size();
    CardSet& set = mCurrentRoundData->mPickedUpCards[playerId];
    unsigned int oldSize = set.size();
    set.insert(cardSet.begin(), cardSet.end());
    assert(oldSize + cardSet.size() == set.size());
}

void Observer::cardsDealed(const PlayerId *playerId, unsigned int cardsAmount)
{
    mPlayersCards[playerId] += cardsAmount;
    mGameCardsCount -= cardsAmount;
}

void Observer::roundStarted(unsigned int roundIndex, const std::vector<const PlayerId *> attackers, const PlayerId *defender)
{
    assert(mRestored || mCurrentRoundIndex != roundIndex);
    mRestored = false;
    mCurrentRoundIndex = roundIndex;
    assert(!mCurrentRoundData);
    mCurrentRoundData = new RoundData();
    std::copy(attackers.begin(), attackers.end(), std::inserter(mCurrentRoundData->mPlayers, mCurrentRoundData->mPlayers.begin()));
    mCurrentRoundData->mPlayers.push_back(defender);
}

void Observer::roundEnded(unsigned int roundIndex)
{
    assert(mCurrentRoundIndex == roundIndex);
    mRoundsData.push_back(mCurrentRoundData);
    mCurrentRoundData = NULL;
}

void Observer::gameRestored(const std::vector<const PlayerId*>& playerIds,
    const std::map<const PlayerId*, unsigned int>& playersCards,
    unsigned int deckCards,
    const Suit& trumpSuit,
    const std::vector<Card>& attackCards,
    const std::vector<Card>& defendCards)
{
    (void) playersCards;
    (void) deckCards;
    (void) trumpSuit;
    (void) attackCards;
    (void) defendCards;
    assert(mPlayers.empty());
    mPlayers.insert(mPlayers.begin(), playerIds.begin(), playerIds.end());
}

void Observer::saveMap(DataWriter& writer, const std::map<const PlayerId*, CardSet>& map){
    writer.write(static_cast<unsigned int>(map.size()));
    for (std::map<const decore::PlayerId*, decore::CardSet>::const_iterator it = map.begin(); it != map.end(); ++it) {
        writer.write(mPlayers.index(it->first));
        writer.write(it->second.begin(), it->second.end());
    }
}

void Observer::loadMap(DataReader& reader, std::map<const PlayerId*, CardSet>& map)
{
    static const Card defaultCard(SUIT_LAST, RANK_LAST);
    unsigned int mapSize;
    reader.read(mapSize);
    while (mapSize--) {
        unsigned int playerIndex;
        reader.read(playerIndex);
        CardSet cards;
        reader.read(cards, defaultCard);
        map[mPlayers[playerIndex]] = cards;
    }
}

void Observer::save(DataWriter& writer)
{
    writer.write(mGameCards.begin(), mGameCards.end());
    writer.write(mGameCardsCount);
    writer.write(mTrumpSuit);
    writer.write(static_cast<unsigned int>(mPlayers.size()));
    for (PlayerIds::const_iterator it = mPlayers.begin(); it != mPlayers.end(); ++it) {
        unsigned int playerIndex = mPlayers.index(*it);
        writer.write(playerIndex);
        writer.write(mPlayersCards.at(*it));
    }
    writer.write(static_cast<unsigned int>(mRoundsData.size()));
    for (std::vector<const RoundData*>::const_iterator it = mRoundsData.begin(); it != mRoundsData.end(); ++it) {
        const RoundData& roundData = **it;
        writer.write(static_cast<unsigned int>(roundData.mPlayers.size()));
        for(PlayerIds::const_iterator it = roundData.mPlayers.begin(); it != roundData.mPlayers.end(); ++it) {
            writer.write(mPlayers.index(*it));
        }
        saveMap(writer, roundData.mDroppedCards);
        saveMap(writer, roundData.mPickedUpCards);
    }
    writer.write(mCurrentRoundIndex);
}

void Observer::init(DataReader& reader)
{
    static const Card defaultCard(SUIT_LAST, RANK_LAST);
    reader.read(mGameCards, defaultCard);
    reader.read(mGameCardsCount);
    reader.read(mTrumpSuit);
    unsigned int playerAmount;
    reader.read(playerAmount);
    while (playerAmount--) {
        unsigned int playerIndex;
        reader.read(playerIndex);
        unsigned int cardsAmount;
        reader.read(cardsAmount);
        mPlayersCards[mPlayers[playerIndex]] = cardsAmount;
    }
    unsigned int rounds;
    reader.read(rounds);
    while (rounds--) {
        unsigned int players;
        reader.read(players);
        RoundData roundData;
        while (players--) {
            unsigned int playerIndex;
            reader.read(playerIndex);
            roundData.mPlayers.push_back(mPlayers[playerIndex]);
        }
        loadMap(reader, roundData.mDroppedCards);
        loadMap(reader, roundData.mPickedUpCards);
    }
    reader.read(mCurrentRoundIndex);
    mRestored = true;
}
