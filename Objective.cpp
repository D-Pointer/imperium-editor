#include "Objective.hpp"

int Objective::m_next_id = 0;

Objective::Objective (const QPointF & pos, QGraphicsItem * parent) : QGraphicsPixmapItem(parent) {
    m_title = "Unnamed";
    m_id    = Objective::m_next_id++;

    setPixmap( QPixmap(":/Icons/Objective.png") );

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setPos( pos - boundingRect().center() );
    setTransformOriginPoint( boundingRect().center() );
    setZValue( 210 );
}


Objective::Objective (int id, const QString & title, const QPointF & pos, QGraphicsItem * parent) : QGraphicsPixmapItem(parent), m_id(id), m_title(title) {
    Objective::m_next_id = qMax( id + 1, Objective::m_next_id );

    setPixmap( QPixmap(":/Icons/Objective.png") );

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setPos( pos );
    setTransformOriginPoint( boundingRect().center() );
    setZValue( 210 );
}

