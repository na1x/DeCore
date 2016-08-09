#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <set>
#include <vector>

#include "playerId.h"
#include "cardSet.h"

/**
 * @mainpage DeCore
 *
 * The "DeCore" is a Russian card game engine.
 *
 * Developed with c++ the engine is aimed to be cross-platform and portable (where c++ is available).
 */
namespace decore
{

class Player;
class GameObserver;
class Deck;

/**
 * @brief The game engine class
 *
 * Main game flow is here.
 * Usage of the class:
 * - create instance - Engine()
 * - add players (at least two players) - add()
 * - add game observers (optional) - addGameObserver()
 * - set deck (mandatory) - setDeck()
 * - call playRound till it returns true - playRound()
 *
 * The class is not intended to be "thread safe" and designed for single thread for the sake of simplicity.
 * All players and observers will be invoked from "inside" of playRound().
 */
class Engine
{
    /**
     * @brief Generated player ids
     *
     * Stored for cleanup reasons.
     */
    std::vector<const PlayerId*> mGeneratedIds;
    /**
     * @brief Players added to the game
     *
     * Each player identified by unique id
     */
    std::map<const PlayerId*, Player*> mPlayers;
    /**
     * @brief Map of each player cards
     */
    std::map<const PlayerId*, CardSet> mPlayersCards;
    /**
     * @brief Game flow observers
     */
    std::vector<GameObserver*> mGameObservers;
    /**
     * @brief Cards left in the deck.
     *
     */
    Deck* mDeck;
    /**
     * @brief Player id generation counter
     */
    int mPlayerIdCounter;
    /**
     * @brief Attacker's player id
     */
    const PlayerId* mCurrentPlayer;

public:
    /**
     * @brief Ctor
     */
    Engine();
    /**
     * @brief Dtor
     */
    virtual ~Engine();

    /**
     * @brief Adds player to the game.
     *
     * Adds player to the game. Order of adding players defines moves order: i.e. first added player will make first move,
     * Adding same player more than once is allowed but game flow in the case will be unexpected.
     * @param player player instance to add
     * @return id for the player or NULL if player not added (for example if game started already)
     */
    PlayerId* add(Player& player);
    /**
     * @brief Adds game observer
     *
     * Adding duplicated observer is OK: each observer considered as unique though, i.e. observer added twice will receive twice amount of notifications.
     * @param observer observer to add
     */
    void addGameObserver(GameObserver& observer);
    /**
     * @brief Sets cards to the game
     *
     * After players added the game is to be initiated with card set. Suit of the last card in the card set will be trump for the game.
     *
     * @param deck cards for the game, typically full and shuffled deck
     * @return true if set, false in case of any errors
     */
    bool setDeck(const Deck& deck);
    /**
     * @brief Plays one round
     *
     * Synchronously plays game round.
     * Game observers notified about game state chamges.
     *
     * @return true if game is not ended and one more round could be played
     */
    bool playRound();

private:
    /**
     * @brief Checks if the game is ended
     * @return true if ended
     */
    bool gameEnded() const;
};

}
#endif // ENGINE_H
