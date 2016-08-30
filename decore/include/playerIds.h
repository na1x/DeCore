#ifndef PLAYERIDS_H
#define PLAYERIDS_H

#include <vector>

namespace decore
{

class PlayerId;

/**
 * @brief Array of PlayerId
 */
class PlayerIds : public std::vector<const PlayerId*>
{
public:
    /**
     * @brief Returns index of the player id in the array
     * @param id player id
     * @return index
     */
    unsigned int index(const PlayerId* id) const;
};

}

#endif /* PLAYERIDS_H */

