#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <set>

#include "playerId.h"
#include "cardSet.h"

namespace decore
{

class Player;
class GameObserver;

/**
 * @brief The game engine class
 *
 * Main game flow is here.
 * Usage of the class:
 * - create instance
 * - add players (at least two players)
 * - add game observers (optional)
 * - set cards (mandatory)
 * - call playRound till it returns true
 * Note: for unit testing protected visibility is used instead of private
 */
class Engine
{
    /**
     * @brief Generated player ids
     *
     * Stored for cleanup reasons.
     */
    std::vector<PlayerId*> mGeneratedIds;
    /**
     * @brief Players added to the game
     *
     * Each player identified by unique id
     */
    std::map<PlayerId*, Player*> mPlayers;
    /**
     * @brief Map of each player cards
     */
    std::map<PlayerId*, CardSet> mPlayersCards;
    /**
     * @brief Game flow observers
     */
    std::set<GameObserver*> mGameObservers;
    /**
     * @brief Cards left in the deck.
     */
    CardSet* mDeckCards;
    /**
     * @brief Player id generation counter
     */
    int mPlayerIdCounter;
    /**
     * @brief Attacker's player id
     */
    PlayerId* mCurrentPlayer;
    /**
     * @brief Trump suit
     *
     * Note: Contains junk by design if cards not set
     */
    Suit mTrumpSuit;

public:
    Engine();
    virtual ~Engine();

    /**
     * @brief Adds player to the game.
     *
     * Adds player to the game. Order of adding players defines moves order: i.e. first added player will make first move,
     *
     * @param player player instance to add
     * @return id for the player or NULL if player not added (for example if game started already)
     */
    PlayerId* add(Player& player);
    /**
     * @brief Adds game observer
     * @param observer observer to add
     */
    void addGameObserver(GameObserver& observer);
    /**
     * @brief Sets cards to the game
     *
     * After players added the game is to be initiated with card set. Suit of the last card in the card set will be trump for the game.
     *
     * @param cardset cards for the game, typically full and shuffled deck
     * @return true if set, false in case of any errors
     */
    bool setCards(const CardSet& cardset);
    /**
     * @brief Plays one round
     *
     * Synchronously plays game round.
     * Game observers notified about game state chamges.
     *
     * @return true if game is not ended and one more round could be played
     */
    bool playRound();

protected:
    /**
     * @brief Returns next player in the player queue after the player
     * @param after defines player id
     * @return next player id
     */
    PlayerId* pickNext(PlayerId* after);
    /**
     * @brief Checks if the game is ended
     * @return true if ended
     */
    bool gameEnded() const;
};

}
#endif // ENGINE_H
