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

    //repeated: need to fix the design
    int look_left  = 10;
    int look_right = 4;
    int look_up    = 1;
    int look_down  = 7;
    int move_left  = 9;
    int move_right = 2;
    int move_up    = 0;
    int move_down  = 6;

    float mStep = 0.2; //TODO: Need to be changeable according Player_public_informations::SPEED_TYPE

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

    void moveAtLeft()
    {
        const_cast<Tiled::Cell &>(cell()).change(move_left);
        mState = CatchChallenger::Direction_look_at_left;
    }

    void moveAtRight()
    {
        const_cast<Tiled::Cell &>(cell()).change(move_right);
        mState = CatchChallenger::Direction_look_at_right;
    }

    void moveAtUp()
    {
        const_cast<Tiled::Cell &>(cell()).change(move_up);
        mState = CatchChallenger::Direction_look_at_top;
    }

    void moveAtDown()
    {
        const_cast<Tiled::Cell &>(cell()).change(move_down);
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

    inline void stepAtTheRight()
    {
        this->setX(this->x() + mStep);
    }

    inline void stepAtTheLeft()
    {
        this->setX(this->x() - mStep);
    }

    inline void stepAtTheTop()
    {
        this->setY(this->y() - mStep);
    }

    inline void stepAtTheBottom()
    {
        this->setY(this->y() + mStep);
    }

    inline void setState(int state)
    {
        const_cast<Tiled::Cell &>(cell()).change(state);
        mState = state;
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
