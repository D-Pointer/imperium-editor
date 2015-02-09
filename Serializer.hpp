#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <QTextStream>

#include "Map.hpp"

class EditorMainWindow;
class QTcpSocket;

class Serializer {
public:

    void saveMap (Map * map, EditorMainWindow * editor);

    void sendMapToIpad (QTcpSocket * ipad, EditorMainWindow * editor);

    Map * loadMap (const QString & filename, EditorMainWindow * editor);


private:

    void saveMapToStream (QTextStream & stream, EditorMainWindow * editor);


    float fromSave (float value, int height);
    float toSave (float value, int height);

    static bool terrainComparator (const Terrain * t1, const Terrain * t2);

    void generateTrees (Terrain * terrain, QTextStream & stream, float mapHeight);
    void generateRocks (Terrain * terrain, QTextStream & stream, float mapHeight);

    //void saveNavigationGrid ();
    //void saveNavigationGrid (QTextStream & stream);

    //void saveHeightmap ();
};


#endif // SERIALIZER_HPP
