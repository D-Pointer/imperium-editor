#include <QDebug>
#include <QPolygonF>
#include <QGraphicsEllipseItem>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"

void EditorMainWindow::generateNavigation () {
    qDebug() << "EditorMainWindow::generateNavigation: generating navigation mesh";

    statusBar()->showMessage( "Generating navigation grid..." );

    QList<QPolygonF> terrains;

    // first make a list of all polygons for all terrains, they need to be offset with their positions
    foreach ( Terrain * terrain, allTerrains ) {
        QPointF terrainPos = terrain->pos();
        QPolygonF polygon = terrain->polygon();

        polygon.translate( terrainPos );
        terrains << polygon;
    }

    int gridWidth = map->getWidth() / navigationTileSize;
    int gridHeight = map->getHeight() / navigationTileSize;

    // set up an empty navigation grid
    navigationGrid = new Terrain*[ gridWidth * gridHeight ];
    for ( int index = 0; index < gridWidth * gridHeight; ++index ) {
        navigationGrid[ index ] = 0;
    }

    qDebug() << "EditorMainWindow::generateNavigation: grid size:" << gridWidth << "x" << gridHeight << "=" << gridWidth * gridHeight;

    // loop the entire map
    for ( int y = 0; y < gridHeight; ++y ) {
        for ( int x = 0; x < gridWidth; ++x ) {
            QPoint p1( x * navigationTileSize, y * navigationTileSize );
            QPoint p2( x * navigationTileSize + navigationTileSize, y * navigationTileSize);
            QPoint p3( x * navigationTileSize, y * navigationTileSize + navigationTileSize );
            QPoint p4( x * navigationTileSize + navigationTileSize, y * navigationTileSize + navigationTileSize );

            int position = (gridHeight - y - 1) * gridWidth + x;

            // no toplevel terrain found so far
            //qDebug() << position << x << y;
            navigationGrid[ position ] = 0;

            // check all polygons to see if it intersects
            for ( int index = 0; index < terrains.size(); ++index ) {
                QPolygonF terrain = terrains[ index ];

                // does any point reside inside the polygon?
                if ( terrain.containsPoint( p1, Qt::OddEvenFill ) || terrain.containsPoint( p2, Qt::OddEvenFill ) ||
                     terrain.containsPoint( p3, Qt::OddEvenFill ) || terrain.containsPoint( p4, Qt::OddEvenFill ) ) {
                    Terrain * candidate = allTerrains[ index ];
                    if ( navigationGrid[ position ] == 0 || candidate->zValue() > navigationGrid[ position ]->zValue() ) {
                        navigationGrid[ position ] = candidate;
                    }
                }
            }

            // show debugging balls
//            if ( navigationGrid[ position ] ) {
//                QGraphicsEllipseItem * marker = new QGraphicsEllipseItem( QRectF( x * navigationTileSize, y * navigationTileSize, navigationTileSize, navigationTileSize ) );
//                marker->setBrush( navigationGrid[ position ]->brush() );
//                marker->setPen( QPen( Qt::black ));
//                marker->setZValue( 255 );
//                map->addItem( marker );
//            }
        }
    }

    // we now have a navigation map
    statusBar()->showMessage( "Navigation grid done", 3000 );

    qDebug() << "EditorMainWindow::generateNavigation: navigation grid done";
}

