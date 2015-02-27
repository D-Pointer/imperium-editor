
#include <math.h>

#include <QDebug>
#include <QImage>
#include <time.h>

#include <QPixmap>
#include <QStack>
#include <QList>
#include <QPainter>
#include <QMessageBox>

#include "GeneratorDialog.hpp"
#include "ui_GeneratorDialog.h"
#include "Map.hpp"
#include "Perlin.hpp"

const int GeneratorDialog::s_margin = 20;
const int GeneratorDialog::s_tileSize = 10;

GeneratorDialog::GeneratorDialog(QWidget *parent) : QDialog(parent), ui(new Ui::GeneratorDialog), m_map(0) {
    ui->setupUi(this);

    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(ok()) );
}


GeneratorDialog::~GeneratorDialog() {
    delete ui;
}


void GeneratorDialog::ok () {
    qDebug() << "GeneratorDialog::ok";

    if ( ui->m_width->value() % 100 != 0 ) {
        QMessageBox::warning( this, "Size error", "The width must be a multiple of 100", QMessageBox::Ok );
        return;
    }

    if ( ui->m_height->value() % 100 != 0 ) {
        QMessageBox::warning( this, "Size error", "The height must be a multiple of 100", QMessageBox::Ok );
        return;
    }

    // create a new map, but use the final size
    m_map = new Map;
    m_map->setSize( ui->m_width->value(), ui->m_height->value() );

    // generate stuff?
    if ( ! ui->m_autoGenerate->isChecked() ) {
        // nope, we're done
        accept();
        return;
    }

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    QApplication::processEvents();

    // use a larger width to allow for some margins where the woods can "swell out"
    m_width = ui->m_width->value() + s_margin * 2;
    m_height = ui->m_height->value() + s_margin * 2;

    // clear all state
    m_forests.clear();

    QImage image( m_width, m_height, QImage::Format_ARGB32 );

    // perlin noise
    generatePerlin( image );
    //showImage( image, "Noise" );

    // determine what is forest
    findForests( image );

    // cleanup garbage
    cleanup( image );

    // trace
    QList<QLine> lines = trace( image );

    // find polygons
    buildPolygons( lines, image );

    // white pen
//    QPainter painter( &image );
//    painter.setPen( QPen( QBrush( Qt::white ), 1 ) );
//    painter.drawRect( s_margin, s_margin, ui->m_width->value(), ui->m_height->value() );
//    painter.end();

    // all rivers
    //generateRivers( image );
    //showImage( image, "Rivers" );

    // finally create the map
    createMap();

    QApplication::restoreOverrideCursor();

    // we're done
    accept();
}


void GeneratorDialog::generatePerlin (QImage & image) {
    // parameters for the perlin noise generator
    int octave = ui->m_octave->value();
    int frequency = ui->m_frequency->value();
    int amplitude = 1;
    int seed = qrand();

    qsrand( time( 0 ) );

    // noise generator
    Perlin perlin( octave, frequency, amplitude, seed );


    // fill with black
    image.fill( qRgba( 0, 0, 0, 255 ) );

    for ( int y = 0; y < m_height - s_margin; ++y ) {
        for ( int x = 0; x < m_width - s_margin; ++x ) {
            int value = 127 + ( perlin.Get( (float)x / m_width, (float)y / m_height ) * 128 );

            //qDebug() << x << y << value;
            image.setPixel( x + s_margin / 2, y + s_margin / 2, qRgba( value, value, value, 255 ));
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
                // woods
                image.setPixel( x, y, qRgba( 128, 128, 128, 255 ));
            }
            else {
                // not woods
                image.setPixel( x, y, qRgba( 0, 0, 0, 255 ));
            }
        }
    }
}


void GeneratorDialog::cleanup (QImage & image) {
    // check each woods pixel to see if they have enough woods neighbours. this will remove stray pixels
    // that are alone or have one one neighbour
    for ( int y = 0; y < m_height; ++y ) {
        for ( int x = 0; x < m_width; ++x ) {
            // is this woods?
            if ( qRed( image.pixel( x, y ) ) == 128 ) {
                // woods pixel, does it have enough friends around?
                int woods = 0, empty = 0;

                // loop the 9 pixels around
                for ( int y2 = qMax( 0, y - 1); y2 <= qMin( m_height - 1, y + 1); ++y2 ) {
                    for ( int x2 = qMax( 0, x - 1); x2 <= qMin( m_width - 1, x + 1); ++x2 ) {
                        if ( x2 == x && y2 == y ) {
                            continue;
                        }

                        if ( qRed( image.pixel( x2, y2 ) ) == 128 ) {
                            woods++;
                        }
                        else {
                            empty++;
                        }
                    }
                }

                //qDebug() << x << y << woods << empty;

                // enough around?
                if ( woods < 2) {
                    // too few woods around this pixel, clear it
                    image.setPixel( x, y, qRgba( 0, 0, 0, 255 ));
                }
            }
        }
    }
}


QList<QLine>  GeneratorDialog::trace (QImage & image) {
    // uses marching cubes, see:
    // http://en.wikipedia.org/wiki/Marching_squares

    const int halfTile = s_tileSize / 2;

    int tiles = 0, edges = 0, outside = 0, inside = 0;

    QList<QLine> lines;

    // loop all tiles and give a value 0..15 based on which corners are in woods and which not
    for ( int y = 0; y < m_height - s_tileSize; y += s_tileSize ) {
        for ( int x = 0; x < m_width - s_tileSize; x += s_tileSize ) {
            int value = 0;

            // up left
            if ( qRed( image.pixel( x, y ) ) == 128 ) {
                value += 1;
            }

            // up right
            if ( qRed( image.pixel( x + s_tileSize, y ) ) == 128 ) {
                value += 2;
            }

            // down left
            if ( qRed( image.pixel( x, y + s_tileSize ) ) == 128 ) {
                value += 8;
            }

            // down right
            if ( qRed( image.pixel( x + s_tileSize, y + s_tileSize ) ) == 128 ) {
                value += 4;
            }

            tiles++;

            // 0 and 15 are fully inside or fully outside, ignore them
            if ( value == 0 ) {
                outside++;
                continue;
            }
            else if ( value == 15 ) {
                inside++;
                continue;
            }

            edges++;

            // create lines based on the value
            // note: values 0 and 15 skipped above
            switch ( value ) {
            case 1:
            case 14:
                lines << QLine( x + 0, y + halfTile,
                                x + halfTile, y + 0 );
                break;

            case 2:
            case 13:
                lines << QLine( x + halfTile, y + 0,
                                x + s_tileSize, y + halfTile );
                break;

            case 4:
            case 11:
                lines << QLine( x + halfTile, y + s_tileSize,
                                x + s_tileSize, y + halfTile );
                break;

            case 7:
            case 8:
                lines << QLine( x + 0, y + halfTile,
                                x + halfTile, y + s_tileSize );
                break;

            case 3:
            case 12:
                lines << QLine( x + 0, y + halfTile,
                                x + s_tileSize, y + halfTile );
                break;

            case 6:
            case 9:
                lines << QLine( x + halfTile, y + 0,
                                x + halfTile, y + s_tileSize );
                break;

            case 5:
                // two lines
                lines << QLine( x + 0, y + halfTile,
                                x + halfTile, y + 0 );
                lines << QLine( x + halfTile, y + s_tileSize,
                                x + s_tileSize, y + halfTile );
                break;

            case 10:
                // two lines
                lines << QLine( x + 0, y + halfTile,
                                x + halfTile, y + s_tileSize );
                lines << QLine( x + halfTile, y + 0,
                                x + s_tileSize, y + halfTile );
                break;
            }
        }
    }

    qDebug() << "GeneratorDialog::trace: tiles:" << tiles << "edges:" << edges << "inside:" << inside << "outside:" << outside;

    QPainter painter( &image );

    // white pen
    painter.setPen( QPen( QBrush( Qt::white ), 1 ) );

    // draw each line
    foreach ( QLine line, lines ) {
        painter.drawLine( line );
    }

    painter.end();

    return lines;
}


void GeneratorDialog::buildPolygons (QList<QLine> & lines, QImage & image) {
    qDebug() << "GeneratorDialog::buildPolygons: building polygons from" << lines.size() << "lines";

    while ( lines.size() > 0 ) {
        QLine line = lines.takeFirst();

        // a new polygon starts with this line
        QPolygon polygon;
        polygon << line.p1();
        polygon << line.p2();

        QPoint last = line.p1();
        QPoint current = line.p2();

        //qDebug() << "GeneratorDialog::buildPolygons: starting a new polygon, now:" << m_forests.size();
        //qDebug() << "GeneratorDialog::buildPolygons: start:" << current << "last:" << last;

        bool lastFound = false;

        // loop until we find a line that connects to the current/last points
        while ( ! lastFound ) {
            //qDebug() << "GeneratorDialog::buildPolygons: current:" << current << "last:" << last << "found:" << lastFound;

            QMutableListIterator<QLine> iter( lines );
            while ( iter.hasNext() ) {
                QLine line = iter.next();

                // connects to the current?
                if ( line.p1() == current ) {
                    // last line?
                    if ( line.p2() == last ) {
                        //qDebug() << "GeneratorDialog::buildPolygons: last found:" << line;
                        lastFound = true;
                        iter.remove();
                        break;
                    }

                    polygon << line.p2();
                    current = line.p2();
                    iter.remove();
                    //qDebug() << "GeneratorDialog::buildPolygons: added point:" << line.p2() << "from line:" << line << "now:" << polygon.size();
                }

                else if ( line.p2() == current ) {
                    // last line?
                    if ( line.p1() == last ) {
                        //qDebug() << "GeneratorDialog::buildPolygons: last found:" << line;
                        lastFound = true;
                        iter.remove();
                        break;
                    }

                    polygon << line.p1();
                    current = line.p1();
                    iter.remove();
                    //qDebug() << "GeneratorDialog::buildPolygons: added point:" << line.p1() << "from line:" << line << "now:" << polygon.size();
                }
            }
        }

        // simplify the polygon
        simplifyPolygon( polygon );

        // now add the polygon, if we add it when created a copy gets added to the polygon list and it'll
        // only contain start and end
        m_forests << QPolygonF( polygon );
    }

    QPainter painter( &image );
    painter.setPen( QPen( QBrush( Qt::red ), 2 ) );

    // draw each polygon
    foreach ( QPolygonF polygon, m_forests ) {
        painter.drawPolygon( polygon );
    }

    painter.end();

}


void GeneratorDialog::simplifyPolygon (QPolygon & polygon) {
    int currentIndex = 0;
    bool found ;

    //qDebug() << "GeneratorDialog::simplifyPolygon: start size:" << polygon.size();

    // max distance for the mid point from the line between two points for it to be cut
    const float maxDistance = 2;

    do {
        // nothing to remove found yet
        found = false;

        while ( currentIndex < polygon.size() - 1 ) {
            int realIndex = currentIndex % polygon.size();

            QPointF start = polygon.at( realIndex );
            QPointF mid = polygon.at( realIndex + 1 );
            QPointF end = polygon.at( realIndex + 2 );

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
                polygon.remove( realIndex + 1 );
                found = true;
            }
            else {
                // not found, move to the next
                currentIndex++;
            }
        }
    }
    while( found );

    //qDebug() << "GeneratorDialog::simplifyPolygon: end size:" << polygon.size();
}


void GeneratorDialog::generateRivers (QImage & image) {
    for ( int index = 0; index < ui->m_rivers->value(); ++index ) {
        qDebug() << "GeneratorDialog::generateRivers: starting river" << index;
        generateOneRiver( image );
    }
}


void GeneratorDialog::generateOneRiver (QImage &image) {
    QPointF start1, start2;
    float direction;
    QList<QPointF> points;

    QPainter painter( &image );
    painter.setPen( QPen( QBrush( Qt::yellow ), 3 ) );

    bool inWoods;

    // starting point
    do {
        inWoods = false;

        // up/down or left/right?
        switch ( qrand() % 4 ) {
        case 0:
            // left
            start1.setX( s_margin );
            start1.setY( s_margin + ( qrand() % ui->m_height->value() ) );
            start2.setX( start1.x() );
            start2.setY( start1.y() + 10 + qrand() % 30 );
            direction = 0;
            break;

        case 1:
            // right
            start1.setX( s_margin + ui->m_width->value() );
            start1.setY( s_margin + ( qrand() % ui->m_height->value() ) );
            start2.setX( start1.x() );
            start2.setY( start1.y() + 10 + qrand() % 30 );
            direction = 180;
            break;

        case 2:
            // top
            start1.setX( s_margin + ( qrand() % ui->m_width->value() ) );
            start1.setY( s_margin );
            start2.setX( start1.x() + 10 + qrand() % 30 );
            start2.setY( start1.y());
            direction = 270;
            break;

        case 3:
            // bottom
            start1.setX( s_margin + ( qrand() % ui->m_width->value() ) );
            start1.setY( s_margin +ui->m_height->value()  );
            start2.setX( start1.x() + 10 + qrand() % 30 );
            start2.setY( start1.y());
            direction = 90;
            break;
        }

        // inside any of the forests?
        foreach ( QPolygonF forest, m_forests ) {
            if ( forest.containsPoint( start1, Qt::OddEvenFill ) || forest.containsPoint( start2, Qt::OddEvenFill ) ) {
                // in woods
                qDebug() << "GeneratorDialog::generateRivers: starting point in woods";
                inWoods = true;
                break;
            }
        }
    }
    while ( inWoods );

    qDebug() << "starting point ok";

    painter.drawPoint( start1 );
    painter.drawPoint( start2 );

    points << start1;

    // starting position found
    bool finished = false;
    while ( ! finished ) {

        int tries = 0;
        inWoods = false;
        QPointF candidate;

        // try to find a position not in woods
        do {
            // how far does this leg go
            float distance = 10 + qrand() % 30;

            // new direction
            direction += -30 + qrand() % 60;

            // deltas
            float x = distance * cosf( direction / 180.0f * M_PI );
            float y = distance * sinf( direction / 180.0f * M_PI );
            candidate = points.last() + QPointF( x, y );

            // outside?
            if ( candidate.x() < s_margin || candidate.x() > s_margin + ui->m_width->value() ||
                 candidate.y() < s_margin || candidate.y() > s_margin + ui->m_height->value() ) {
                finished = true;
                break;
            }

            // not outside yet, hit woods?
            foreach ( const QPolygonF & forest, m_forests ) {
                if ( forest.containsPoint( candidate, Qt::OddEvenFill )) {
                    // in woods
                    qDebug() << "GeneratorDialog::generateRivers: point in woods";
                    inWoods = true;
                    tries++;
                    break;
                }
            }
        }
        while ( inWoods && tries < 30);

        if ( inWoods ) {
            // this did not work out, back off a bit
            points.removeLast();
            qDebug() << "backing off";
        }
        else {
            points << candidate;
            painter.drawPoint( points.last() );
        }
    }
}


void GeneratorDialog::showImage (const QImage & image, const QString & title) {
    QLabel * label = new QLabel();
    label->setPixmap( QPixmap::fromImage( image ) );
    label->setWindowTitle( title );
    label->show();
}


void GeneratorDialog::createMap () {
    // add in all
    foreach ( const QPolygonF & forest, m_forests ) {
        Terrain * terrain = new Terrain( forest.translated( -s_margin, -s_margin ) );
        terrain->setType( kWoods );

        m_map->addItem( terrain );
        allTerrains << terrain;
    }
}



