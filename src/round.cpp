#include "round.h"

namespace decore {

Round::Round(const std::vector<PlayerId*>& attackers, PlayerId*& defender)
    : mAttackers(attackers)
    , mDefender(defender)
{
}

bool Round::makeMove()
{
    // not implemented
    return false;
}

}

