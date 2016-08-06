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
    std::vector<PlayerId*> mAttackers;
    PlayerId* mDefender;
    std::map<PlayerId*, Player*>& mPlayers;
    std::vector<GameObserver*>& mGameObservers;
    Deck& mDeck;
    std::vector<Card> mAttackCards;
    std::vector<Card> mDefendCards;
    std::map<PlayerId*, CardSet>& mPlayersCards;

public:
    Round(const std::vector<PlayerId*>& attackers,
          PlayerId*& defender,
          std::map<PlayerId*, Player*>& players,
          std::vector<GameObserver*>& gameObservers,
          Deck& deck,
          std::map<PlayerId*, CardSet>& playersCards);

    /**
     * @brief Plays current round
     */
    void play();
};

}


#endif // ROUND_H
