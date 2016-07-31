#ifndef GAMEOBSERVER_H_INCLUDED
#define GAMEOBSERVER_H_INCLUDED

namespace decore
{

class PlayerId;
class CardSet;

class GameObserver
{

public:

    virtual void cardsReceived(const PlayerId&, const CardSet&) = 0;
    virtual void cardsReceived(const PlayerId&, int) = 0;
    virtual void cardsLeft(const CardSet&) = 0;
    virtual void cardsDopped(const PlayerId&, const CardSet&) = 0;
};

}

#endif // GAMEOBSERVER_H_INCLUDED
