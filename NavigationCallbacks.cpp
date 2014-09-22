#include <QDebug>
#include <QPolygonF>
#include <QGraphicsEllipseItem>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"

void EditorMainWindow::generateNavigation () {
    qDebug() << "EditorMainWindow::generateNavigation: generating navigation mesh";

    QList<QPolygonF> terrains;

    // first make a list of all polygons for all terrains, they need to be offset with their positions
    foreach ( Terrain * terrain, allTerrains ) {
        QPointF terrainPos = terrain->pos();
        QPolygonF polygon = terrain->polygon();

        polygon.translate( terrainPos );
        terrains << polygon;
    }

    // fill the result array with nothing
    if ( navigationGrid ) {
        delete [] navigationGrid;
    }

    int gridWidth = map->getWidth() / navigationTileSize;
    int gridHeight = map->getHeight() / navigationTileSize;

    navigationGrid = new Terrain*[ gridWidth * gridHeight ];

    qDebug() << "EditorMainWindow::generateNavigation: grid size:" << gridWidth << "x" << gridHeight;

    // number of terrains found that are not open
    int foundCount = 0;

    // loop the entire map
    for ( int y = 0; y < map->getHeight(); y += navigationTileSize ) {
        for ( int x = 0; x < map->getWidth(); x += navigationTileSize ) {
            QPoint p1( x, y );
            QPoint p2( x + navigationTileSize, y );
            QPoint p3( x, y + navigationTileSize );
            QPoint p4( x + navigationTileSize, y + navigationTileSize );

            // convert to navigation grid coordinates
            int cx = x / navigationTileSize;
            int cy = y / navigationTileSize;

            int position = (gridHeight - cy - 1) * gridWidth + cx;

            // no toplevel terrain found so far
            //qDebug() << position << cx << cy;
            navigationGrid[ position ] = 0;

            // check all polygons to see if it intersects
            for ( int index = 0; index < terrains.size(); ++index ) {
                QPolygonF terrain = terrains[ index ];

                // does any point reside inside the polygon?
                if ( terrain.containsPoint( p1, Qt::OddEvenFill ) || terrain.containsPoint( p2, Qt::OddEvenFill ) ||
                     terrain.containsPoint( p3, Qt::OddEvenFill )|| terrain.containsPoint( p4, Qt::OddEvenFill ) ) {
                    Terrain * candidate = allTerrains[ index ];
                    if ( navigationGrid[ position ] == 0 || candidate->zValue() > navigationGrid[ position ]->zValue() ) {
                        navigationGrid[ position ] = candidate;
                    }
                }
            }

            if ( navigationGrid[ position ] ) {
                QGraphicsEllipseItem * marker = new QGraphicsEllipseItem( QRectF( x, y, navigationTileSize, navigationTileSize ) );
                marker->setBrush( navigationGrid[ position ]->brush() );
                marker->setPen( QPen( Qt::black ));
                marker->setZValue( 255 );
                map->addItem( marker );
                foundCount++;
            }
        }
    }

    qDebug() << "EditorMainWindow::generateNavigation: found:" << foundCount << "non open terrains";
}

