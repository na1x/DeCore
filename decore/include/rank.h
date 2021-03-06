#ifndef RANK_H
#define RANK_H

namespace decore {

/**
 * @brief The Rank enum
 *
 * Note: order does matter.
 */
enum Rank
{
    /** 6 */
    RANK_6,
    /** 7 */
    RANK_7,
    /** 8 */
    RANK_8,
    /** 9 */
    RANK_9,
    /** 10 */
    RANK_10,
    /** Jack */
    RANK_JACK,
    /** Queen */
    RANK_QUEEN,
    /** King */
    RANK_KING,
    /** Ace */
    RANK_ACE,
    /** It's better not to use in game */
    RANK_LAST
};

}
#endif // RANK_H
