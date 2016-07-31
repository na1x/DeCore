#ifndef ROUND_H
#define ROUND_H

#include <vector>

namespace decore {

class PlayerId;

class Round
{
    std::vector<PlayerId*> mAttackers;
    PlayerId* mDefender;

public:
    Round(const std::vector<PlayerId*>& attackers, PlayerId*& defender);

    bool makeMove();
};

}


#endif // ROUND_H
