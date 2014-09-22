#include <QDebug>

#include "House.hpp"

House::House (const QPointF & pos, int type, QGraphicsItem * parent) : QGraphicsPixmapItem(parent) {
    setType( type );

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setTransformOriginPoint( boundingRect().center() );
    setPos( pos ); //- boundingRect().center() );
    setZValue( 150 );
}


void House::setType (int type) {
    m_type = type;

    //qDebug() << "House::setType: new type:" << m_type;

    // new pixmap
    setPixmap( QPixmap(":/Icons/House" + QString::number( type + 1 ) + ".png") );
}
