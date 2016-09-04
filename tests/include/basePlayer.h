#ifndef BASEPLAYER_H
#define BASEPLAYER_H

#include "player.h"

using namespace decore;

class BasePlayer : public Player
{
    const decore::PlayerId* mId;
    /**
     * Each change in the player's cards adds new item into the vector
     */
    std::vector<CardSet> mPlayerCards;

public:
    void idCreated(const PlayerId* id);
    const Card& attack(const PlayerId* playerId, const CardSet& cardSet);
    const Card* pitch(const PlayerId* playerId, const CardSet& cardSet);
    const Card* defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet);
    void cardsUpdated(const CardSet& cardSet);
    void cardsRestored(const CardSet& cards);

    void gameStarted(const Suit& trumpSuit, const CardSet& cardSet, const std::vector<const PlayerId*>& players);
    void roundStarted(unsigned int roundIndex, const std::vector<const PlayerId*> attackers, const PlayerId* defender);
    void roundEnded(unsigned int roundIndex);
    void cardsPickedUp(const PlayerId* playerId, const CardSet& cardSet);
    void cardsDealed(const PlayerId* playerId, unsigned int cardsAmount);
    void cardsGone(const CardSet& cardSet);
    void cardsDropped(const PlayerId* playerId, const CardSet& cardSet);
    void gameRestored(const std::vector<const PlayerId*>& playerIds,
        const std::map<const PlayerId*, unsigned int>& playersCards,
        unsigned int deckCards,
        const Suit& trumpSuit,
        const std::vector<Card>& attackCards,
        const std::vector<Card>& defendCards);
    void save(DataWriter& writer);
    void init(DataReader& reader);

    const PlayerId* id() const;
    const CardSet& cards(unsigned int index) const;
    unsigned int cardSets() const;

protected:
    void removeCard(const decore::Card* card);

};

#endif /* BASEPLAYER_H */

