#include "Dot.hpp"
#include "Terrain.hpp"

Dot::Dot (const QPointF & scene_pos, Terrain * terrain) : QGraphicsPixmapItem(terrain), m_terrain(terrain) {
    setPixmap( QPixmap(":/Icons/Dot.png") );
    setPos( scene_pos - boundingRect().center() );
    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges );
}


QVariant Dot::itemChange (GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged && scene()) {
        m_terrain->dotMoved( this );
        return QVariant();
    }

    return QGraphicsItem::itemChange(change, value);
}
