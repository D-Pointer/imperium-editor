#include "StartPos.hpp"

StartPos::StartPos (const QPointF & pos, QGraphicsItem * parent) : QGraphicsPixmapItem(parent) {
    setPixmap( QPixmap(":/Icons/StartPosition.png") );

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setPos( pos - boundingRect().center() );
    setTransformOriginPoint( boundingRect().center() );
    setZValue( 200 );
}
