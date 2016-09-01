#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <vector>

#include "playerId.h"
#include "cardSet.h"
#include "gameObserver.h"
#include "playerIds.h"
#include "atomic.h"

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
class DataReader;
class DataWriter;

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
 * Generally the class is not intended to be "thread safe" and designed for single thread for the sake of simplicity.
 * All players and observers will be invoked from "inside" of playRound().
 * Only methods to be used from other thread:
 * - save()
 * - quit()
 */
class Engine
{
    /**
     * @brief Cards on the table
     */
    class TableCards
    {
        /**
         * @brief Attacker's cards
         */
        std::vector<Card> mAttackCards;
        /**
         * @brief Defender's cards
         */
        std::vector<Card> mDefendCards;
        /**
         * @brief Attacker;s and defender cards all together
         */
        CardSet mAll;
    public:
        /**
         * @brief Adds the `card` as attacker's
         * @param card card to add
         */
        void addAttackCard(const Card& card);
        /**
         * @brief Adds the `card` as defender's
         * @param card card to add
         */
        void addDefendCard(const Card& card);
        /**
         * Returns all cards
         * @return cards
         */
        const CardSet& all() const;
        /**
         * @brief Checks if empty
         * @return true if empty
         */
        bool empty() const;
        /**
         * @brief Returns attackers cards
         * @return cards
         */
        const std::vector<Card>& attackCards() const;
        /**
         * @brief Returns defender's cards
         * @return cards
         */
        const std::vector<Card>& defendCards() const;
        /**
         * @brief Resets the instance
         */
        void clear();
    };
    /**
     * @brief Generated player ids
     *
     * Stored for cleanup reasons.
     */
    PlayerIds mGeneratedIds;
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

    /**
     * @brief Round index
     */
    unsigned int mRoundIndex;

    /**
     * @brief Current round state: list of attacker ids
     */
    std::vector<const PlayerId*> mAttackers;

    /**
     * @brief Current round state: defender's id
     */
    const PlayerId* mDefender;

    /**
     * @brief Current round state: cards currently on the table
     */
    TableCards mTableCards;

    /**
     * @brief Internal data synchronization lock
     */
    mutable pthread_mutex_t mLock;

    /**
     * @brief Quit flag
     */
    Atomic<bool> mQuit;

    /**
     * @brief Current round state: current attacker
     */
    const PlayerId* mCurrentRoundAttackerId;

    /**
     * @brief Current round state: "passed" counter
     */
    unsigned int mPassedCounter;
    /**
     * @brief Current round state: max cards for attack (can't be bigger then defender's cards amount)
     */
    unsigned int mMaxAttackCards;
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
     * Game observers notified about game state changes.
     *
     * @return true if game is not ended and one more round could be played
     */
    bool playRound();
    /**
     * @brief Returns loser (only player with cards) or `NULL` in case of draw(no loser)
     *
     * Valid only if game ended.
     * @return loser id or `NULL` if draw or game not ended yet
     */
    const PlayerId* getLoser();
    /**
     * @brief Saves current state of the game into the `write`
     *
     * Saved data could be used later to construct the engine from the data so allowing to resume the game.
     * @param writer writer to save state
     */
    void save(DataWriter& writer) const;

    /**
     * @brief Initializes the instance from the 'reader'
     * @param reader contains data saved
     * @param players players
     * @param observers game observers
     */
    void init(DataReader& reader, const std::vector<Player*> players, const std::vector<GameObserver*>& observers);
    /**
     * @brief Requests quit
     *
     * Use to terminate "play round" loop from other thread.
     * The class user is responsible for stopping external players.
     * Basically this method should be invoked before all players stopped (returned from attack/pitch/defend)
     */
    void quit();

private:

    /**
     * @brief Internal implementation of PlayerId
     *
     * Note: id should be unique during one game.
     */
    class PlayerIdImplementation: public PlayerId
    {
        /**
         * Id index
         */
        const unsigned int mId;
    public:
        PlayerIdImplementation(unsigned int id);
    };

    /**
     * @brief Function for std::for_each
     */
    class GameStartNotification
    {
        const Suit& mTrumpSuit;
        const std::vector<const PlayerId*>& mPlayers;
        const CardSet mGameCards;

    public:
        GameStartNotification(const Suit& trumpSuit, const std::vector<const PlayerId*>& players, const CardSet& gameCards);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief Function for std::for_each
     */
    class RoundStartNotification
    {
        const std::vector<const PlayerId*>& mAttackers;
        const PlayerId* mDefender;
        const unsigned int mRoundIndex;

    public:
        RoundStartNotification(const std::vector<const PlayerId*>& attackers, const PlayerId* defender, unsigned int roundIndex);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief Function for std::for_each
     */
    class RoundEndNotification
    {
        const unsigned int mRoundIndex;

    public:
        RoundEndNotification(unsigned int roundIndex);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief std::for_each function
     */
    class CardsAmountReceivedNotification
    {
        const PlayerId* mPlayerId;
        const unsigned int mCardsReceived;
    public:
        CardsAmountReceivedNotification(const PlayerId* playerId, unsigned int cardsReceived);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief std::for_each function
     */
    class CardsReceivedNotification
    {
        const PlayerId* mPlayerId;
        const CardSet& mReceivedCards;
    public:
        CardsReceivedNotification(const PlayerId* playerId, const CardSet& receivedCards);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief std::for_each function
     */
    class CardsDroppedNotification
    {
        const PlayerId* mPlayerId;
        CardSet mDroppedCards;
    public:
        CardsDroppedNotification(const PlayerId* playerId, const Card& droppedCard);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief std::for_each function
     */
    class GameRestoredNotification
    {
        const std::vector<const PlayerId*>& mPlayerIds;
        const std::map<const PlayerId*, unsigned int> mPlayersCards;
        unsigned int mDeckCards;
        const Suit& mTrumpSuit;
        const TableCards& mTableCards;
    public:
        GameRestoredNotification(const std::vector<const PlayerId*>& playerIds,
            const std::map<const PlayerId*, unsigned int> playersCards,
            unsigned int deckCards,
            const Suit& trumpSuit,
            const TableCards& tableCards);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief std::for_each function
     */
    class CardsLeftNotification
    {
        const CardSet& mCards;
    public:
        CardsLeftNotification(const CardSet& tableCards);
        void operator()(GameObserver* observer);
    };

    /**
     * @brief Checks if the game is ended
     * @return true if ended
     */
    bool gameEnded() const;
    /**
     * @brief Plays current round
     * @return true if defended
     */
    bool playCurrentRound();
    /**
     * @brief Deals cards before playing round
     */
    void dealCards();
    /**
     * @brief Locks the instance
     */
    void lock() const;
    /**
     * @brief Unlocks the instance
     */
    void unlock() const;
};

}
#endif // ENGINE_H
