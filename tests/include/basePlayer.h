#ifndef BASEPLAYER_H
#define BASEPLAYER_H

#include "player.h"

using namespace decore;

class BasePlayer : public Player
{
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
    void cardsLeft(const CardSet& cardSet);
    void cardsDropped(const PlayerId* playerId, const CardSet& cardSet);
    void tableCardsRestored(const std::vector<Card>& attackCards, const std::vector<Card>& defendCards);
    void write(DataWriter& writer);
    void init(DataReader& reader);


};

#endif /* BASEPLAYER_H */

