#include <QBrush>
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "Map.hpp"
#include "Globals.hpp"
#include "Unit.hpp"
#include "Objective.hpp"
#include "Selection.hpp"

Map::Map(QObject *parent) : QGraphicsScene(parent), m_background(0) {
    m_background = new QGraphicsRectItem( 0, 0, 1024, 768, 0, this );
    m_background->setBrush( QBrush( Qt::green ) );

    m_name = "";

    connect( this, SIGNAL( selectionChanged() ), SLOT( selectedItemsChanged()) );

    // set a default size
    setSize( 1024, 768 );
}


void Map::setSize (int width, int height) {
    m_background->setRect( 0, 0, width, height );
    qDebug() << "Map::setSize: size:" << width << height;

    // nuke all old lines
    qDeleteAll( m_grid );
    m_grid.clear();

    QGraphicsLineItem * line;

    for ( int y = 0; y < height; y += 100 ) {
       line = new QGraphicsLineItem( 0, y, width, y, m_background );
       if ( y % 500 == 0 ) {
           line->setPen( QPen( Qt::black ) );
       }
       else {
           line->setPen( QPen( Qt::lightGray ) );
       }
       m_grid << line;
    }

    for ( int x = 0; x < width; x += 100 ) {
        line = new QGraphicsLineItem( x, 0, x, height, m_background );
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

    switch ( editorMode ) {
    case kEdit:
        QGraphicsScene::mousePressEvent( event );
        break;

    case kAddTerrain:
        // any old terrain?
        terrain = selection->getSelectedTerrain();

//        if ( terrain && terrain->m_polygon.count() < 3 ) {
//            // not a complete polygon, nuke it
//            delete selection->getSelectedTerrain();
//            selection->setTerrain( 0 );
//            terrain = 0;
//        }

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
}
