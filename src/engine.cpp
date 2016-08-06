#include <algorithm>

#include "engine.h"
#include "round.h"
#include "player.h"
#include "rules.h"
#include "deck.h"

namespace decore {

const unsigned int MAX_PLAYER_CARDS = 6;


class PlayerIdImplementation: public PlayerId
{
    const int mId;
public:
    PlayerIdImplementation(int id)
        : mId(id)
    {}
};

Engine::Engine()
    : mDeck(NULL)
    , mPlayerIdCounter(0)
    , mCurrentPlayer(NULL)
{
}

Engine::~Engine()
{
    delete mDeck;
    for(std::vector<PlayerId*>::iterator it = mGeneratedIds.begin(); it != mGeneratedIds.end(); ++it) {
        delete *it;
    }
}

PlayerId *Engine::add(Player & player)
{
    if (mDeck) {
        return NULL;
    }

    PlayerId* id = new PlayerIdImplementation(mPlayerIdCounter++);

    // initialize player related data
    mGeneratedIds.push_back(id);
    mPlayers[id] = &player;
    mPlayersCards[id] = CardSet();

    // add this player to game observers
    addGameObserver(player);

    player.idCreated(id);

    return id;
}

bool Engine::setDeck(const Deck &deck)
{
    if (mDeck) {
        // already set
        return false;
    }

    if (deck.empty()) {
        // nothing to do with empty card set
        return false;
    }

    if (mPlayers.size() < 2) {
        // add more players
        return false;
    }

    mDeck = new Deck(deck);

    return true;
}

bool Engine::playRound()
{
    if (!mDeck) {
        // no cards set
        return true;
    }

    if (mGeneratedIds.size() < 2) {
        // too few players - at least two should be set
        return true;
    }

    if (!mCurrentPlayer) {
        // if no current player - pick first one
        mCurrentPlayer = mGeneratedIds[0];
    }

    // prepare round data
    std::vector<PlayerId*> attackers;
    // pick current player as first attacker
    attackers.push_back(mCurrentPlayer);
    // pick next player as defender
    PlayerId* defender = Rules::pickNext(mGeneratedIds, mCurrentPlayer);
    // gather rest players as additional attackers
    PlayerId* attacker = defender;
    while((attacker = Rules::pickNext(mGeneratedIds, attacker)) != mCurrentPlayer) {
        attackers.push_back(attacker);
    }

    mDeck->deal(mPlayersCards, MAX_PLAYER_CARDS);

    Round(attackers, defender, mPlayers, mGameObservers, *mDeck, mPlayersCards).play();

    return !gameEnded();
}

void Engine::addGameObserver(GameObserver &observer)
{
    mGameObservers.push_back(&observer);
}

bool Engine::gameEnded() const
{
    // check no game cards left
    // check that less than one player have cards

    if (!mDeck->empty()) {
        return false;
    }

    unsigned int playersWithCards = 0;
    for(std::map<PlayerId*, CardSet>::const_iterator it = mPlayersCards.begin(); it != mPlayersCards.end(); ++it) {
        if(!it->second.empty()) {
            playersWithCards++;
        }
    }

    return !playersWithCards;
}

}
