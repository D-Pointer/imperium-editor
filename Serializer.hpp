#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <QTextStream>

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

    void generateTrees (Terrain * terrain, QTextStream & stream, float mapHeight);
};


#endif // SERIALIZER_HPP
