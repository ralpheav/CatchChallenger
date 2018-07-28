#ifndef PLAYER_H
#define PLAYER_H

#include "../../general/base/GeneralStructures.h"
#include "../tiled/tiled_mapobject.h"
#include "../tiled/tiled_tile.h"

namespace Character {

/**
 * Defined for the player only
 */
class Player : public Tiled::MapObject
{
protected:

    int look_left  = 10;
    int look_right = 4;
    int look_up    = 1;
    int look_down  = 7;
    int move_left  = 9;
    int move_right = 2;
    int move_up    = 0;
    int move_down  = 6;

public:

    Player() : Tiled::MapObject() {}

    Player(
            const QString &name,
            const QString &type,
            const QPointF &pos,
            const QSizeF &size
            ) : Tiled::MapObject(
                    name,
                    type,
                    pos,
                    size
                    ) {}

    void lookAtLeft()
    {
        const_cast<Tiled::Cell &>(cell()).change(look_left);
        mState = CatchChallenger::Direction_look_at_left;
    }

    void lookAtRight()
    {
        const_cast<Tiled::Cell &>(cell()).change(look_right);
        mState = CatchChallenger::Direction_look_at_right;
    }

    void lookAtUp()
    {
        const_cast<Tiled::Cell &>(cell()).change(look_up);
        mState = CatchChallenger::Direction_look_at_top;
    }

    void lookAtDown()
    {
        const_cast<Tiled::Cell &>(cell()).change(look_down);
        mState = CatchChallenger::Direction_look_at_bottom;
    }

    inline bool isLookingLeft()
    {
        return mState == CatchChallenger::Direction_look_at_left;
    }

    inline bool isLookingRight()
    {
        return mState == CatchChallenger::Direction_look_at_right;
    }

    inline bool isLookingUp()
    {
        return mState == CatchChallenger::Direction_look_at_top;
    }

    inline bool isLookingDown()
    {
        return mState == CatchChallenger::Direction_look_at_bottom;
    }

    inline CatchChallenger::Direction getState()
    {
        return static_cast<CatchChallenger::Direction>(mState);
    }

private:

    int mState;
};

} // namespace Character

#endif // PLAYER_H
