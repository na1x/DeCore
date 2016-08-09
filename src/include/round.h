#ifndef ROUND_H
#define ROUND_H

#include <vector>
#include <map>

#include "card.h"
#include "deck.h"
#include "cardSet.h"

namespace decore {

class PlayerId;
class GameObserver;
class Player;

class Round
{
    std::vector<const PlayerId*> mAttackers;
    const PlayerId* mDefender;
    std::map<const PlayerId*, Player*>& mPlayers;
    std::vector<GameObserver*>& mGameObservers;
    Deck& mDeck;
    std::vector<Card> mAttackCards;
    std::vector<Card> mDefendCards;
    std::map<const PlayerId*, CardSet>& mPlayersCards;

public:
    Round(const std::vector<const PlayerId*>& attackers,
          const PlayerId*& defender,
          std::map<const PlayerId*, Player*>& players,
          std::vector<GameObserver*>& gameObservers,
          Deck& deck,
          std::map<const PlayerId*, CardSet>& playersCards);

    /**
     * @brief Plays current round
     */
    void play();

private:
    void dealCards();
};

}


#endif // ROUND_H
