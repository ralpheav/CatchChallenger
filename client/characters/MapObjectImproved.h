#ifndef MAPOBJECTIMPROVED_H
#define MAPOBJECTIMPROVED_H

#include "../tiled/tiled_mapobject.h"
#include "../tiled/tiled_tile.h"

namespace Character {

/**
 * Defined for the MapObject Improved
 */
class MapObjectImproved : public Tiled::MapObject
{

public:

    MapObjectImproved() : Tiled::MapObject() {}

    MapObjectImproved(
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


    inline void setState(int state)
    {
        const_cast<Tiled::Cell &>(cell()).change(state);
        mState = state;
    }

    inline int getState()
    {
        return mState;
    }

    inline void updateTileSet(const Tiled::Tileset* newTileSet, int tileId = -1, const std::string name = "")
    {
        if (!name.empty()) {
            this->setName(QString::fromStdString(name));
        }

        Tiled::Cell cell = this->cell();

        if (tileId == -1) {
            tileId = cell.tile->id();
        }

        cell.tile = newTileSet->tileAt(tileId);//this way the tileset probably is going to be loose, need a better way

        if (cell.tile == NULL) {
            std::cerr << "Tile NULL before contructor" << std::endl;
        }

        this->setCell(cell);
    }

private:

    int mState;
};

} // namespace Character

#endif // MAPOBJECTIMPROVED_H
