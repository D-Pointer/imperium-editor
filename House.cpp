#include <QDebug>

#include "House.hpp"

House::House (const QPointF & pos, int type, QGraphicsItem * parent) : QGraphicsPixmapItem(parent)/*, m_flipped(false)*/ {
    setType( type );

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setPos( pos );
    setZValue( 150 );
}


void House::setType (int type) {
    m_type = type;

    //qDebug() << "House::setType: new type:" << m_type;

    // new pixmap
    setPixmap( QPixmap(":/Icons/House" + QString::number( type + 1 ) + ".png") );
    setTransformOriginPoint( boundingRect().center() );
}
