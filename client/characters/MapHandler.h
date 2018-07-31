#ifndef MAPHANDLER_H
#define MAPHANDLER_H

#include "../base/render/MapVisualiserThread.h"

namespace Character {

typedef std::unordered_map<std::string, MapVisualiserThread::Map_full*> Map;

/**
 * Defined to get a readable mapper handler, to use the most common functions either
 */
class MapHandler
{
protected:

    Map m_map;

public:

    map();

    bool exists(const std::string& mapId)
    {
        return m_map.find(mapId) != m_map.cend();
    }

//    MapVisualiserThread::Map_full* operator[] (const std::string& mapId)
//    {
//        return m_map.at(mapId);
//    }

    MapVisualiserThread::Map_full* at(const std::string& mapId)
    {
        return m_map.at(mapId);
    }

    void clear()
    {
        m_map.clear();
    }

    void erase(const std::string& mapId)
    {
        m_map.erase(mapId);
    }
};

} // namespace Character

#endif // MAPHANDLER_H
