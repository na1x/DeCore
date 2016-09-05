#ifndef OBSERVER_H
#define OBSERVER_H

#include "gameObserver.h"
#include "cardSet.h"
#include "suit.h"
#include "playerId.h"
#include "playerIds.h"

class Observer : public decore::GameObserver
{

public:
    class RoundData
    {
    public:
        decore::PlayerIds mPlayers;
        std::map<const decore::PlayerId*, decore::CardSet> mDroppedCards;
        std::map<const decore::PlayerId*, decore::CardSet> mPickedUpCards;
    };

    decore::PlayerIds mPlayers;
    decore::CardSet mGameCards;
    unsigned int mGameCardsCount;
    decore::Suit mTrumpSuit;
    std::map<const decore::PlayerId*, unsigned int> mPlayersCards;
    std::vector<const RoundData*> mRoundsData;
    unsigned int mCurrentRoundIndex;

    Observer();
    ~Observer();
    void gameStarted(const decore::Suit &trumpSuit, const decore::CardSet &cardSet, const std::vector<const decore::PlayerId *>& players);
    void cardsGone(const decore::CardSet &cardSet);
    void cardsDropped(const decore::PlayerId *playerId, const decore::CardSet &cardSet);
    void cardsPickedUp(const decore::PlayerId* playerId, const decore::CardSet& cardSet);
    void cardsDealed(const decore::PlayerId* playerId, unsigned int cardsAmount);
    void roundStarted(unsigned int roundIndex, const std::vector<const decore::PlayerId *> attackers, const decore::PlayerId *defender);
    void roundEnded(unsigned int roundIndex);
    void gameRestored(const std::vector<const decore::PlayerId*>& playerIds,
        const std::map<const decore::PlayerId*, unsigned int>& playersCards,
        unsigned int deckCards,
        const decore::Suit& trumpSuit,
        const std::vector<decore::Card>& attackCards,
        const std::vector<decore::Card>& defendCards);
    void save(decore::DataWriter& writer);

    void init(decore::DataReader& reader);

private:
    void saveMap(decore::DataWriter& writer, const std::map<const decore::PlayerId*, decore::CardSet>& map);
    void loadMap(decore::DataReader& reader, std::map<const decore::PlayerId*, decore::CardSet>& map);

    RoundData* mCurrentRoundData;
    bool mRestored;
};

#endif /* OBSERVER_H */

