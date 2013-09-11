#ifndef DOT_HPP
#define DOT_HPP

#include <QGraphicsPixmapItem>

class Terrain;

class Dot : public QGraphicsPixmapItem {

public:

    Dot (const QPointF &scene_pos, Terrain * terrain);

    Terrain * getTerrain () {
        return m_terrain;
    }


protected:

    virtual QVariant itemChange (GraphicsItemChange change, const QVariant &value);

private:

    Terrain * m_terrain;
};

#endif // DOT_HPP
