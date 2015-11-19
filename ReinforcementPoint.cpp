#include "ReinforcementPoint.hpp"

ReinforcementPoint::ReinforcementPoint (const QPointF & pos, QGraphicsItem * parent) : QGraphicsPixmapItem(parent) {
    setPixmap( QPixmap(":/Icons/ReinforcementPoint.png") );

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setPos( pos - boundingRect().center() );
    setTransformOriginPoint( boundingRect().center() );
    setZValue( 200 );
}
