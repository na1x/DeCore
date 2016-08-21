#include <algorithm>

#include "engine.h"
#include "round.h"
#include "player.h"
#include "rules.h"
#include "deck.h"

namespace decore {

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
    , mRoundIndex(0)
{
}

Engine::~Engine()
{
    delete mDeck;
    for(std::vector<const PlayerId*>::iterator it = mGeneratedIds.begin(); it != mGeneratedIds.end(); ++it) {
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

    CardSet cards;

    cards.insert(deck.begin(), deck.end());

    std::for_each(mGameObservers.begin(), mGameObservers.end(), GameStartNotification(mDeck->trumpSuit(), mGeneratedIds, cards));

    return true;
}

bool Engine::playRound()
{
    if (!mDeck) {
        // no cards set
        return false;
    }

    if (mGeneratedIds.size() < 2) {
        // too few players - at least two should be set
        return false;
    }

    if (!mCurrentPlayer) {
        // if no current player - pick first one
        mCurrentPlayer = mGeneratedIds[0];
    }

    if (gameEnded()) {
        return false;
    }

    // prepare round data
    std::vector<const PlayerId*> attackers;
    // pick current player as first attacker
    attackers.push_back(mCurrentPlayer);
    // pick next player as defender
    const PlayerId* defender = Rules::pickNext(mGeneratedIds, mCurrentPlayer);
    // gather rest players as additional attackers
    const PlayerId* attacker = defender;
    while((attacker = Rules::pickNext(mGeneratedIds, attacker)) != mCurrentPlayer) {
        attackers.push_back(attacker);
    }

    Round round(attackers, defender, mPlayers, mGameObservers, *mDeck, mPlayersCards);

    std::for_each(mGameObservers.begin(), mGameObservers.end(), RoundStartNotification(attackers, defender, mRoundIndex));

    bool defended = round.play();

    std::for_each(mGameObservers.begin(), mGameObservers.end(), RoundEndNotification(mRoundIndex));

    mRoundIndex++;

    // if attack failed "next move" goes to defender
    // or to next player after the defender overwise
    mCurrentPlayer = defended ? defender : Rules::pickNext(mGeneratedIds, defender);

    return !gameEnded();
}

const PlayerId *Engine::getLoser()
{
    std::vector<const PlayerId*> playersWithCards;
    for(std::map<const PlayerId*, CardSet>::const_iterator it = mPlayersCards.begin(); it != mPlayersCards.end(); ++it) {
        if(!it->second.empty()) {
            playersWithCards.push_back(it->first);
        }
    }
    return playersWithCards.empty() ? NULL : 1 == playersWithCards.size() ? playersWithCards[0] : NULL;
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
    for(std::map<const PlayerId*, CardSet>::const_iterator it = mPlayersCards.begin(); it != mPlayersCards.end(); ++it) {
        if(!it->second.empty()) {
            playersWithCards++;
        }
    }

    return playersWithCards < 2;
}

Engine::GameStartNotification::GameStartNotification(const Suit &trumpSuit, const std::vector<const PlayerId *> &players, const CardSet &gameCards)
    : mTrumpSuit(trumpSuit)
    , mPlayers(players)
    , mGameCards(gameCards)
{}

void Engine::GameStartNotification::operator()(GameObserver*observer)
{
    observer->gameStarted(mTrumpSuit, mGameCards, mPlayers);
}

Engine::RoundStartNotification::RoundStartNotification(const std::vector<const PlayerId *> &attackers, const PlayerId *defender, unsigned int roundIndex)
    : mAttackers(attackers)
    , mDefender(defender)
    , mRoundIndex(roundIndex)
{

}

void Engine::RoundStartNotification::operator()(GameObserver *observer)
{
    observer->roundStarted(mRoundIndex, mAttackers, mDefender);
}

Engine::RoundEndNotification::RoundEndNotification(unsigned int roundIndex)
    : mRoundIndex(roundIndex)
{
}

void Engine::RoundEndNotification::operator()(GameObserver *observer)
{
    observer->roundEnded(mRoundIndex);

}

}
