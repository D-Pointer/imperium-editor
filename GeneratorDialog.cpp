
#include <math.h>

#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QStack>
#include <QList>
#include <QPainter>

#include "GeneratorDialog.hpp"
#include "ui_GeneratorDialog.h"
#include "Map.hpp"
#include "Perlin.hpp"

GeneratorDialog::GeneratorDialog(QWidget *parent) : QDialog(parent), ui(new Ui::GeneratorDialog), m_map(0) {
    ui->setupUi(this);

    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()) );
}


GeneratorDialog::~GeneratorDialog() {
    delete ui;
}


void GeneratorDialog::ok () {
    qDebug() << "GeneratorDialog::ok";

    m_width = ui->m_width->value() + 20;
    m_height = ui->m_height->value() + 20;

    // create a new map, but use the final size
    m_map = new Map;
    m_map->setSize( ui->m_width->value(), ui->m_height->value() );

    // generate stuff?
    if ( ! ui->m_autoGenerate->isChecked() ) {
        // nope, we're done
        accept();
        return;
    }

    // clear all state
    m_forests.clear();

    QImage image( m_width, m_height, QImage::Format_ARGB32 );


    // perlin noise
    generatePerlin( image );
    showImage( image );


    // determine what is forest
    findForests( image );
    showImage( image );

    // vectorize the forests
    traceForests( image );

    QImage forestsImage( m_width, m_height, QImage::Format_ARGB32 );
    forestsImage.fill( Qt::white );

    QPainter p( &forestsImage );
    p.setPen( QColor( Qt::darkGreen ) );

    foreach ( const QPolygonF & forest, m_forests ) {
        p.drawPolygon( forest );
    }
    p.end();

    showImage( forestsImage );


    // simplify the forest polygons
    for ( int index = 0; index < m_forests.size(); ++index ) {
        simplifyForest( m_forests[ index ] );
    }

    QImage simplifiedImage( m_width, m_height, QImage::Format_ARGB32 );
    simplifiedImage.fill( Qt::white );

    QPainter p2( &simplifiedImage );
    p2.setPen( QColor( Qt::darkGreen ) );

    foreach ( const QPolygonF & forest, m_forests ) {
        p2.drawPolygon( forest );
    }
    p2.end();

    showImage( simplifiedImage );


    // finally create the map
    createMap();

    //findEdgePoints( image );
    // we're done
    //accept();
}


void GeneratorDialog::generatePerlin (QImage & image) {
    // parameters for the perlin noise generator
    int octave = 2;
    int frequency = 5;

    // noise generator
    Perlin perlin( octave, frequency, 1, qrand() );


    // fill with black
    image.fill( qRgba( 0, 0, 0, 255 ) );

    for ( int y = 0; y < m_height - 20; ++y ) {
        for ( int x = 0; x < m_width - 20; ++x ) {
            int value = 127 + ( perlin.Get( (float)x / m_width, (float)y / m_height ) * 128 );

            //qDebug() << x << y << value;
            image.setPixel( x + 10, y + 10, qRgba( value, value, value, 255 ));
        }
    }
}


void GeneratorDialog::findForests (QImage & image) {
    int elevation = 200;
    float wantedPercentage = ui->m_woods->value();

    int totalPixels = m_width * m_height;
    int needed = (int)( totalPixels * (wantedPercentage / 100.0f) );
    int count;

    do {
        count = 0;

        for ( int y = 0; y < m_height; ++y ) {
            for ( int x = 0; x < m_width; ++x ) {
                int pixel = image.pixel( x, y );

                if ( qRed(pixel) >= elevation ) {
                    image.setPixel( x, y, qRgba( qRed(pixel), qGreen(pixel), qBlue(pixel), 0 ));
                    count++;
                }
            }
        }

        elevation--;
    } while ( count < needed );

    qDebug() << "GeneratorDialog::findForests: elevation " << elevation << "found" << count;

    // make all forests gree and the rest white
    for ( int y = 0; y < m_height; ++y ) {
        for ( int x = 0; x < m_width; ++x ) {
            if ( qAlpha( image.pixel( x, y ) ) == 0 ) {
                image.setPixel( x, y, qRgba( 0, 50, 0, 255 ));
            }
            else {
                image.setPixel( x, y, qRgba( 0, 0, 0, 255 ));
            }
        }
    }
}


void GeneratorDialog::traceForests (QImage & image) {
    int color = 60;

    // now fill all the forests with own colors
    for ( int y = 0; y < m_height; ++y ) {
        for ( int x = 0; x < m_width; ++x ) {
            if ( qGreen( image.pixel( x, y ) ) == 50 ) {
                fill( image, x, y, color, 50 );

                // trace this color
                traceOneForest( image, x, y, color, 0 );
                color += 2;

                if ( color == 64 ) {
                    return;
                }
                // DEBUG: make only one forest
                //return;
            }
        }
    }

    qDebug() << "GeneratorDialog::traceForests: found" << m_forests.size() << "forests";
}


void GeneratorDialog::fill (QImage & image, int x, int y, int newColor, int oldColor) {
    if ( newColor == oldColor ) {
        // avoid infinite loop
        return;
    }

    QStack<int> stack;

    // start case
    stack.push( index( x, y ) );

    while ( ! stack.isEmpty() ) {
        // convert index back to pixel coords
        fromIndex( stack.pop(), x, y );

        // peform real filling
        image.setPixel( x, y, qRgba( 0, newColor, 0, 255 ));

        if ( x + 1 < m_width && qGreen( image.pixel( x + 1, y )) == oldColor ) {
            stack.push( index( x + 1, y ) );
        }

        if ( x > 0  && qGreen( image.pixel( x - 1, y )) == oldColor ) {
            stack.push( index( x - 1, y ) );
        }

        if ( y + 1 < m_height && qGreen( image.pixel( x, y + 1 )) == oldColor ) {
            stack.push( index( x, y + 1 ) );
        }

        if ( y > 0 && qGreen( image.pixel( x, y - 1 )) == oldColor ) {
            stack.push( index( x, y - 1 ) );
        }
    }
}


void GeneratorDialog::traceOneForest (QImage & image, int startX, int startY, int forestColor, int backgroundColor) {

    // a path with a starting pos
    QList<int> points;
    points << index( startX, startY );

    qDebug() << "GeneratorDialog::traceOneForest: adding first point:" << startX << startY;

    bool found = false;

    // parameters
    const float radius = 3;
    const int angleDelta = 2;

    float distance;

    do {
        // not found a match yet
        found = false;

        int startColor;
        int lastX, lastY;
        int tmpX, tmpY;
        fromIndex( points.last(), lastX, lastY );

        for ( int angle = 0; angle < 360 - angleDelta; angle += angleDelta ) {
            int x = lastX + (int)( cos( angle * M_PI / 180.0f ) * radius );
            int y = lastY + (int)( sin( angle * M_PI / 180.0f ) * radius );

            // still inside?
            Q_ASSERT ( x >= 0 || x < m_width || y >= 0 || y < m_height );

            // already tried that one?
            //if ( points.contains( index( x1, y1 ) ) ) {
            //    continue;
            //}

            int color = qGreen( image.pixel( x, y ) );

            // first point we test?
            if ( angle == 0 ) {
                // yes, just get the starting color and then we're done
                startColor = color;
                continue;
            }

            // have we flipped color?
            if ( color == startColor ) {
                // not yet, same as the start, next angle
                continue;
            }

            qDebug() << "GeneratorDialog::traceOneForest: edge:" << x << y;
            qDebug() << "GeneratorDialog::traceOneForest: angle:" << angle;
            qDebug() << "GeneratorDialog::traceOneForest: last:" << lastX << lastY;

            // we have a changed color. if this is the second point then just add it
            if ( points.size() == 1 ) {
                found = true;
                points << index( x, y );
                qDebug() << "GeneratorDialog::traceOneForest: adding second point:" << x << y;

                // done with this edge
                break;
            }

            // are we close enough to the previous node of the path?
            fromIndex( points[ points.size() - 2 ], tmpX, tmpY );
            distance = sqrtf( (x - tmpX) * (x - tmpX)  +  (y - tmpY) * (y - tmpY) );
            if ( distance <= radius * 0.9 ) {
                qDebug() << "GeneratorDialog::traceOneForest: too close to second to last:" << tmpX << tmpY;
                startColor = color;
                continue;
            }


            // are we close enough to the first node of the path?
            distance = sqrtf( (x - startX) * (x - startX)  +  (y - startY) * (y - startY) );
            if ( distance <= radius * 0.9 ) {
                // close enough, do we have enough points?
                if ( points.size() > 2 ) {
                    qDebug() << "GeneratorDialog::traceOneForest: found starting point";
                    break;
                }
            }

            // seems we found a point
            found = true;
            points << index( x, y );
            qDebug() << "GeneratorDialog::traceOneForest: adding point:" << x << y;
            break;
        }
    } while ( found );

    /*
        for ( int angle = 0; angle < 360 - angleDelta; angle += angleDelta ) {
            int endX1 = startX + (int)( cos( angle * M_PI / 180.0f ) * radius );
            int endY1 = startY + (int)( sin( angle * M_PI / 180.0f ) * radius );
            int endX2 = startX + (int)( cos( (angle + angleDelta) * M_PI / 180.0f ) * radius );
            int endY2 = startY + (int)( sin( (angle + angleDelta) * M_PI / 180.0f ) * radius );

            // still inside?
            if ( endX1 < 0 || endX1 >= m_width || endY1 < 0 || endY1 >= m_height ||
                 endX2 < 0 || endX2 >= m_width || endY2 < 0 || endY2 >= m_height ) {
                continue;
            }

            // already tried that one?
            if ( points.contains( index( endX1, endY1 ) ) ) {
                continue;
            }

            int color1 = qGreen( image.pixel( endX1, endY1 ) );
            int color2 = qGreen( image.pixel( endX2, endY2 ) );

            // did the color change from the last color?
            if ( color1 != color2 && ( color1 == forestColor || color2 == forestColor )) {
                // we have a color change, try to rule out a 180 degree turn
                if ( points.size() > 1 ) {
                    int tmpX, tmpY;

                    if ( points.size() > 2 ) {
                        // are we close enough to the first node of the path?
                        fromIndex( points.first(), tmpX, tmpY );

                        if ( qAbs( endX1 - startX ) <= radius * 1.5 && qAbs( endY1 - startY ) <= radius * 1.5 ) {
                            // same point we came from
                            qDebug() << "GeneratorDialog::traceOneForest: first point found:" << endX1 << endY1 << "==" << startX << startY;
                            qDebug() << "GeneratorDialog::traceOneForest: points:" << points.size();
                            //continue;
                            break;
                        }
                    }

                    // are we close enough to the first node of the path?
                    fromIndex( points.at( points.size() - 2 ), tmpX, tmpY );

                    if ( qAbs( endX1 - tmpX ) <= radius / 2 && qAbs( endY1 - tmpY ) <= radius / 2 ) {
                        // same point we came from
                        continue;
                    }
                }

                //qDebug() << "angle:" << angle << "path size:" << points.size() << "edge:" << x << y << "->" << endX1 << endY1;
                startX = endX1;
                startY = endY1;
                found = true;

                points << index( startX, startY );

                // done with this edge
                break;
            }
        }
    } while ( found );
*/
    // only add in the ones that are decent sized
    if ( points.size() <= 5 ) {
        qDebug() << "forest size:" << points.size() << "is too small, ignoring forest";
        return;
    }

    // create a polygon from the forest
    QPolygonF forest;
    int tmpX, tmpY;

    for ( int index = 0; index < points.size() - 1; index++ ) {
        fromIndex( points[ index ], tmpX, tmpY );

        forest << QPointF( tmpX, tmpY );
    }

    // and add the first point once more to close
    fromIndex( points.first(), startX, startY );
    forest << QPointF( startX, startY );

    m_forests << forest;
}


void GeneratorDialog::simplifyForest (QPolygonF &forest) {
    int startSize = forest.size();
    int currentIndex = 0;
    bool found ;

    // max distance for the mid point from the line between two points for it to be cut
    const float maxDistance = 2;

    do {
        // nothing to remove found yet
        found = false;

        while ( currentIndex < forest.size() - 2 ) {
            QPointF start = forest.at( currentIndex );
            QPointF mid = forest.at( currentIndex + 1 );
            QPointF end = forest.at( currentIndex + 2 );

            // get the distance from the mid point to the line start -> end
            float px = end.x() - start.x();
            float py = end.y() - start.y();

            float squared = px*px + py*py;

            float u = ((mid.x() - start.x()) * px + (mid.y() - start.y()) * py) / squared;

            if ( u > 1 ) {
                u = 1;
            }
            else if ( u < 0 ) {
                u = 0;
            }

            float dx = start.x() + u * px - mid.x();
            float dy = start.y() + u * py - mid.y();

            float distance = sqrtf( dx * dx + dy * dy );

            //qDebug() << start << mid << end << "distance: " << distance;

            // close enough to be removed?
            if ( distance < maxDistance ) {
                // yes, remove it and do not increment the current index, that way we check the next available end point from the
                // same start -> can simplify long straights into one
                forest.remove( currentIndex + 1 );
                found = true;
            }
            else {
                // not found, move to the next
                currentIndex++;
            }
        }
    }
    while( found );

    qDebug() << "GeneratorDialog::simplifyForest: start size:" << startSize << ", end size:" << forest.size();
}


void GeneratorDialog::showImage (const QImage & image) {
    QLabel * label = new QLabel();
    label->setPixmap( QPixmap::fromImage( image ) );
    label->show();
}


void GeneratorDialog::createMap () {
    // add in all
    foreach ( const QPolygonF & forest, m_forests ) {
        Terrain * terrain = new Terrain( forest );
        terrain->setType( kWoods );

        m_map->addItem( terrain );
        allTerrains << terrain;
    }
}


/*void GeneratorDialog::traceForests (QImage & image) {
    for ( int y = 0; y < m_height - 1; ++y ) {
        for ( int x = 0; x < m_width; ++x ) {
            if ( qGreen( image.pixel( x, y ) ) != qGreen( image.pixel( x, y + 1 ) ) ) {
                image.setPixel( x, y, qRgb( 255, 0, 0 ));
            }
        }
    }

    for ( int y = 0; y < m_height ; ++y ) {
        for ( int x = 0; x < m_width - 1; ++x ) {
            if ( qGreen( image.pixel( x, y ) ) != qGreen( image.pixel( x + 1, y ) ) ) {
                image.setPixel( x, y, qRgb( 255, 0, 0 ));
            }
        }
    }

    // make the rest white
    for ( int y = 0; y < m_height; ++y ) {
        for ( int x = 0; x < m_width; ++x ) {
            if ( qRed( image.pixel( x, y ) ) != 255 ) {
                image.setPixel( x, y, qRgb( 250, 250, 250 ));
            }
        }
    }
}


void GeneratorDialog::findEdgePoints (const QImage & image) {
    int last1 = -100;
    int last2 = -100;

    // upper and lower edges
    for ( int x = 0; x < m_width; ++x ) {
        if ( qRed( image.pixel( x, 0 ) ) == 255 ) {
            // was the last position also an edge point?
            if ( last1 < x - 1 ) {
                // found a position
                traceEdge( image, x, 0 );
            }

            last1 = x;
        }
        if ( qRed( image.pixel( x, m_height - 1 ) ) == 255 ) {
            // was the last position also an edge point?
            if ( last2 < x - 1 ) {
                // found a position
                traceEdge( image, x, m_height - 1 );
            }

            last2 = x;
        }
    }

    last1 = -100;
    last2 = -100;

    // left and right edges, 1px from the top and bottom
    for ( int y = 1; y < m_height - 1; ++y ) {
        if ( qRed( image.pixel( 0, y ) ) == 255 ) {
            // was the last position also an edge point?
            if ( last1 < y - 1 ) {
                // found a position
                traceEdge( image, 0, y );
            }

            last1 = y;
        }
        if ( qRed( image.pixel( m_width - 1, y ) ) == 255 ) {
            // was the last position also an edge point?
            if ( last2 < y - 1 ) {
                // found a position
                traceEdge( image, m_width - 1, y );
            }

            last2 = y;
        }
    }
}


void GeneratorDialog::traceEdge (const QImage & image, int x, int y) {
    // is this point already used?
    if ( m_used.contains( index(x, y) ) ) {
        qDebug() << "GeneratorDialog::traceEdge: already used:" << x << y;
        return;
    }

    qDebug() << "GeneratorDialog::traceEdge: tracing from start:" << x << y;
    m_used << index(x, y);

    const int radius = 10;


}
*/
