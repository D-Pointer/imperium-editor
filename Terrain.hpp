#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <QGraphicsPolygonItem>
#include <QGraphicsPixmapItem>
#include <QPolygonF>

#include "Globals.hpp"
#include "Dot.hpp"

class Terrain : public QGraphicsPolygonItem {

public:

    Terrain (QGraphicsItem * parent=0);
    Terrain (const QPolygonF & polygon, QGraphicsItem *parent=0);

    void addPoint (const QPointF & position, int index=-1);

    void addPointAfter (Dot * dot);
    void removeDot (Dot * dot);

    void setType (TerrainType type);

    void dotMoved (Dot * dot);

    void flipHorizontally ();
    void flipVertically ();

    static QColor getColor (TerrainType type);

    TerrainType m_type;

    QList<Dot *> m_dots;


protected:

    virtual QVariant itemChange (GraphicsItemChange change, const QVariant &value);


private:

    void createDot (const QPointF & scene_pos);
};

#endif // TERRAIN_HPP
