#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "Map.hpp"

class EditorMainWindow;

class Serializer {
public:

    void saveMap (Map * map, EditorMainWindow * editor);

    Map * loadMap (const QString & filename, EditorMainWindow * editor);


private:

    float fromSave (float value, int height);
    float toSave (float value, int height);

    static bool terrainComparator (const Terrain * t1, const Terrain * t2);
};


#endif // SERIALIZER_HPP
