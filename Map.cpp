#include <QBrush>
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "Map.hpp"
#include "Globals.hpp"
#include "Unit.hpp"
#include "Objective.hpp"
#include "Selection.hpp"

Map::Map(QObject *parent) : QGraphicsScene(parent), m_background(0), m_traceBackground(0), m_minRiverWidth(20), m_maxRiverWidth(30) {
    m_background = new QGraphicsRectItem( 0, 0, 1024, 768, 0 );
    m_background->setBrush( QBrush( Qt::green ) );
    m_background->setZValue( 10 );
    addItem( m_background );

    m_name = "";

    connect( this, SIGNAL( selectionChanged() ), SLOT( selectedItemsChanged()) );

    // set a default size
    setSize( 1024, 768 );
}


void Map::setSize (int width, int height) {
    m_background->setRect( 0, 0, width, height );
    //qDebug() << "Map::setSize: size:" << width << height;

    // nuke all old lines
    qDeleteAll( m_grid );
    m_grid.clear();

    QGraphicsLineItem * line;

    for ( int y = 0; y < height; y += 100 ) {
       line = new QGraphicsLineItem( 0, y, width, y );
       line->setZValue( 20 );
       addItem( line );
       if ( y % 500 == 0 ) {
           line->setPen( QPen( Qt::black ) );
       }
       else {
           line->setPen( QPen( Qt::lightGray ) );
       }
       m_grid << line;
    }

    for ( int x = 0; x < width; x += 100 ) {
        line = new QGraphicsLineItem( x, 0, x, height );
        line->setZValue( 20 );
        addItem( line );
        if ( x % 500 == 0 ) {
            line->setPen( QPen( Qt::black ) );
        }
        else {
            line->setPen( QPen( Qt::lightGray ) );
        }
        m_grid << line;
    }
}


void Map::mousePressEvent (QGraphicsSceneMouseEvent *event) {
    Unit * unit = 0;
    Terrain * terrain = 0;
    Objective * objective = 0;
    House * house = 0;
    ReinforcementPoint * reinforcementPoint = 0;
    StartPos * startPos = 0;
    float width;

    switch ( editorMode ) {
    case kEdit:
        QGraphicsScene::mousePressEvent( event );
        break;

    case kAddTerrain:
        // any old terrain?
        terrain = selection->getSelectedTerrain();

        if ( terrain == 0 ) {
            terrain = new Terrain;
            addItem( terrain );
            allTerrains << terrain;
        }

        // add the clicked position
        terrain->addPoint( event->scenePos() );
        clearSelection();

        emit terrainAdded( terrain );
        terrain->setSelected( true );
        selection->setTerrain( terrain );
        break;

    case kAddRoad:
        terrain = selection->getSelectedTerrain();

        if ( terrain == 0 ) {
            terrain = new Terrain;
            terrain->setType( kRoad );
            addItem( terrain );
            allTerrains << terrain;
        }

        // add the clicked position
        addPointToRoadOrRiver( terrain, event->scenePos() );
        clearSelection();

        emit terrainAdded( terrain );
        terrain->setSelected( true );
        selection->setTerrain( terrain );
        break;

    case kAddRiver:
        terrain = selection->getSelectedTerrain();

        if ( terrain == 0 ) {
            terrain = new Terrain;
            terrain->setType( kRiver );
            addItem( terrain );
            allTerrains << terrain;
        }

        // river width is random between the limits
        width = m_minRiverWidth + qrand() % ( m_maxRiverWidth - m_minRiverWidth );

        // add the clicked position
        addPointToRoadOrRiver( terrain, event->scenePos(), width );
        clearSelection();

        emit terrainAdded( terrain );
        terrain->setSelected( true );
        selection->setTerrain( terrain );
        break;

    case kAddUnit:
        qDebug() << "Map::mousePressEvent: edit unit";
        unit = new Unit( event->scenePos() );
        addItem( unit );
        allUnits << unit;

        clearSelection();

        emit unitAdded( unit );

        selection->setUnit( unit );
        unit->setSelected( true );
        break;

    case kAddObjective:
        qDebug() << "Map::mousePressEvent: add objective";
        objective = new Objective( event->scenePos() );
        addItem( objective );
        allObjectives << objective;

        clearSelection();

        emit objectiveAdded( objective );

        selection->setObjective( objective );
        objective->setSelected( true );
        break;

    case kAddStartPos:
        qDebug() << "Map::mousePressEvent: add start position";
        startPos = new StartPos( event->scenePos() );
        addItem( startPos );
        allStartPositions << startPos;

        clearSelection();

        emit startPosAdded( startPos );

        selection->setStartPos( startPos );
        startPos->setSelected( true );
        break;

    case kAddReinforcementPoint:
        qDebug() << "Map::mousePressEvent: add reinforcement point";
        reinforcementPoint = new ReinforcementPoint( event->scenePos() );
        addItem( reinforcementPoint );
        allReinforcementPoints << reinforcementPoint;

        clearSelection();

        emit reinforcementPointAdded( reinforcementPoint );

        selection->setReinforcementPoint( reinforcementPoint );
        reinforcementPoint->setSelected( true );
        break;

    case kAddHouse:
        qDebug() << "Map::mousePressEvent: add house";
        int houseId = qrand() % 5;
        int angle = 0; //qrand() % 365;
        house = new House( event->scenePos(), houseId );
        house->setRotation( angle );

        // offset the house a bit
        house->setPos( house->pos() - house->boundingRect().center() );
        addItem( house );
        allHouses << house;

        clearSelection();

        emit houseAdded( house );

        selection->setHouse( house );
        house->setSelected( true );
        break;
     }
}


bool Map::addPointToRoadOrRiver (Terrain * road, const QPointF & pos, float width) {
    Q_ASSERT( road->m_type == kRoad );

    QPolygonF polygon = road->polygon();

    if ( width < 0 ) {
        // no width, assume road and use a fixed width
        width = 10;
    }

    // how many points does it have now?
    if ( polygon.size() == 0 ) {
        // add the first point
        road->addPoint( QPointF( pos.x(), pos.y() + width ) );
        road->addPoint( pos );

        // return true if inside and false if not
        return pos.x() >= 0 && pos.x() < getWidth() && pos.y() >= 0 && pos.y() < getHeight();
    }
    else {
        // add in two points, first append the clicked position
        road->addPoint( pos );

        // get back the now changed polygon
        polygon = road->polygon();

        // now create a line from the two last positions to get an angle
        QLineF line( road->mapToScene( polygon[ polygon.size() -2 ] ),
                road->mapToScene( polygon.last() ));

        // get a normal vector
        QLineF normal = line.normalVector().unitVector();

        // make it as tall as the road is wide
        QPointF pos2 = pos + (normal.p1() - normal.p2()) * width;

        // add it first. the polygon expands at the end and at the beginning
        road->addPoint( pos2, 0 );

        return ! ( ( pos.x() < 0 || pos.x() >= getWidth() || pos.y() < 0 || pos.y() >= getHeight() ) &&
                ( pos2.x() < 0 || pos2.x() >= getWidth() || pos2.y() < 0 || pos2.y() >= getHeight() ) );
    }
}


void Map::setBackground (const QPixmap & pixmap) {
    QPixmap scaled = pixmap.scaled( getWidth(), getHeight() );

    if ( ! m_traceBackground ) {
        m_traceBackground = new QGraphicsPixmapItem( scaled );
        m_traceBackground->setZValue( 10 );
        addItem( m_traceBackground );

        // make the old green background transparent
        m_background->setBrush( QBrush( Qt::transparent) );
    }
    else {
        m_traceBackground->setPixmap( scaled );
    }
}


void Map::selectedItemsChanged () {
    QList<QGraphicsItem *> selected = selectedItems();

    if ( selected.size() == 0 ) {
        selection->setUnit( 0 );
        selection->setTerrain( 0 );
        return;
    }

    // a unit?
    Unit * unit = dynamic_cast<Unit *>( selected.first() );
    if ( unit ) {
        selection->setUnit( unit );
        return;
    }

    // a terrain?
    Terrain * terrain = dynamic_cast<Terrain *>( selected.first() );
    if ( terrain ) {
        selection->setTerrain( terrain );
        return;
    }

    // an objective?
    Objective * objective = dynamic_cast<Objective *>( selected.first() );
    if ( objective ) {
        selection->setObjective( objective );
        return;
    }

    // a house?
    House * house = dynamic_cast<House *>( selected.first() );
    if ( house ) {
        selection->setHouse( house );
        return;
    }
}
