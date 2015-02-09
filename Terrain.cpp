#include <QBrush>
#include <QDebug>
#include <QGraphicsScene>

#include "Terrain.hpp"
#include "Map.hpp"

Terrain::Terrain (QGraphicsItem *parent) : QGraphicsPolygonItem(parent) {
    setType( kWoods );
    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges );
}


Terrain::Terrain (const QPolygonF & polygon, QGraphicsItem *parent) : QGraphicsPolygonItem(parent)/*, m_polygon(polygon)*/ {
    setPolygon( polygon );
    setType( kWoods );
    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges );

    foreach ( const QPointF & pos, polygon ) {
        createDot( pos );
    }
}


void Terrain::addPoint (const QPointF & position, int index) {
    QPolygonF m_polygon = polygon();

    // specific position?
    if ( index == -1 ) {
        // just append
        m_polygon << mapFromScene( position );
    }
    else {
        m_polygon.insert( index, mapFromScene( position ) );
    }

    setPolygon( m_polygon );

    createDot( mapFromScene( position ), index );
}


void Terrain::addPointAfter (Dot * dot) {
    int index = m_dots.indexOf( dot );
    if ( index == -1 ) {
        qWarning() << "Terrain::addPointAfter: dot not found";
        return;
    }

    QPolygonF m_polygon = polygon();

    QPointF pos1 = dot->pos() + dot->boundingRect().center();
    QPointF pos2;
    if ( index == m_polygon.size() - 1 ) {
        pos2 = m_polygon[0];
    }
    else {
        pos2 = m_polygon[index + 1];
    }

    QPointF new_pos = (pos1 + pos2) / 2;

    m_polygon.insert( index + 1, new_pos );
    setPolygon( m_polygon );

    Dot * new_dot = new Dot( new_pos, this );
    m_dots.insert( index + 1, new_dot );
}


void Terrain::removeDot (Dot * dot) {
    int index = m_dots.indexOf( dot );
    if ( index == -1 ) {
        qWarning() << "Terrain::removeDot: dot not found";
        return;
    }
    QPolygonF m_polygon = polygon();

    m_polygon.remove( index );
    m_dots.removeAt( index );

    setPolygon( m_polygon );
}


void Terrain::setType (TerrainType type) {
    m_type = type;
    setBrush( QBrush( getColor( type ) ) );

    QList<qreal> zOrders;
    zOrders << 55  // woods
            << 50  // field
            << 0   // grass
            << 100 // road
            << 75  // river
            << 150 // roof
            << 60  // swamp
            << 70  // rocky
            << 60  // beach
            << 80  // ford
            << 40; // trees

    setZValue( zOrders[ type ]);
}


void Terrain::dotMoved (Dot * dot) {
    int index = m_dots.indexOf( dot );

    if ( index == -1 ) {
        qWarning() << "Terrain::dotMoved: dot not found";
        return;
    }
    QPolygonF poly = polygon();

    poly[ index ] = dot->pos() + dot->boundingRect().center();
    setPolygon( poly );
}


void Terrain::flipHorizontally () {
    float width = map->getWidth();

    QPolygonF poly = polygon();

    // loop all points and flip them
    for ( int index = 0; index < poly.size(); ++index ) {
        QPointF & point( poly[index] );
        point.setX( width - point.x() );
    }

    setPolygon( poly );

    // get rif of all dots and recreate them
    qDeleteAll( m_dots );
    m_dots.clear();

    foreach ( const QPointF & pos, poly ) {
        createDot( pos );
    }
}


void Terrain::flipVertically () {
    float height = map->getHeight();

    QPolygonF poly = polygon();

    // loop all points and flip them
    for ( int index = 0; index < poly.size(); ++index ) {
        QPointF & point( poly[index] );
        point.setY( height - point.y() );
    }

    setPolygon( poly );

    // get rif of all dots and recreate them
    qDeleteAll( m_dots );
    m_dots.clear();

    foreach ( const QPointF & pos, poly ) {
        createDot( pos );
    }
}


QColor Terrain::getColor (TerrainType type) {
    QList<QColor> colors;
    colors << Qt::darkGreen
           << QColor( 218, 181, 71 )
           << Qt::green
           << Qt::darkRed
           << QColor( 0, 255, 255 )
           << Qt::gray
           << Qt::darkMagenta
           << Qt::darkGray
           << Qt::darkYellow
           << Qt::blue
           << QColor(90, 175, 0);

    return colors[ type ];
}


QVariant Terrain::itemChange (GraphicsItemChange change, const QVariant &value) {
    if (change == ItemSelectedHasChanged && scene()) {
        //        foreach ( Dot * dot, m_dots ) {
        //            dot->setVisible( isSelected() );
        //        }

        return QVariant();
    }

    else if (change == ItemPositionChange && scene() ) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        QPointF delta = newPos - pos();
        QPolygonF m_polygon = polygon();
        //m_polygon.translate( delta );
        //setPolygon( m_polygon );
        //qDebug() << delta << pos() << polygon();

        //         foreach (Dot * dot, m_dots ) {
        //             dot->translate( delta.x(), delta.y() );
        //         }

        //return QPointF(0, 0);
        return value;
    }
    else if (change == ItemPositionHasChanged && scene() ) {
        //         // value is the new position.
        //         QPolygonF m_polygon = polygon();
        //         m_polygon.translate( pos() );
        //         setPolygon( m_polygon );
        //         qDebug() << pos() << polygon();

        //         //return QPointF(0, 0);
        //         return value;
    }

    else if ( change == ItemSceneHasChanged ) {
        foreach (Dot * dot, m_dots ) {
            if ( ! dot->scene() ) {
                scene()->addItem( dot );
            }
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void Terrain::createDot (const QPointF & scene_pos, int index) {
    Dot * dot = new Dot( scene_pos, this );
    dot->setZValue( 200 );

    // specific position?
    if ( index == -1 ) {
        // just append
        m_dots << dot;
    }
    else {
        m_dots.insert( index, dot );
    }
}
