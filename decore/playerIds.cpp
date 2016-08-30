#include <algorithm>
#include <cassert>

#include "playerIds.h"


namespace decore
{

unsigned int PlayerIds::index(const PlayerId* id) const
{
    std::vector<const PlayerId*>::const_iterator it = std::find(begin(), end(), id);
    assert(it != end());
    return it - begin();
}


}


