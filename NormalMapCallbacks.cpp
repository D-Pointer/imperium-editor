#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <Qlabel>
#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"

void EditorMainWindow::updateNormalMap () {
    QString heightmapImageFilename = QFileDialog::getOpenFileName( this, "Open height map", "/Users/chakie/Prog/Triangularizer/Scenarios/", tr("Height maps (*.png)") );
    if ( heightmapImageFilename == "" ) {
        return;
    }

    QImage heightmapImage;
    if ( ! heightmapImage.load( heightmapImageFilename, "PNG") ) {
        QMessageBox::critical( this, "Failed to open height map", "Could not load the heightmap:\n" + heightmapImageFilename );
        return;
    }

    QLabel * l1 = new QLabel;
    l1->setPixmap( QPixmap::fromImage( heightmapImage ) );
    l1->show();
    l1->setWindowTitle( "Original heightmap image");

    qDebug() << "EditorMainWindow::updateNormalMap: loaded heightmap image from:" << heightmapImageFilename;

    int width = heightmapImage.width();
    int height = heightmapImage.height();

    // create a raw pixel array
    unsigned char heightmap[ width * height ];

    for ( int x = 0; x < width; x++) {
        for ( int y = 0; y < height; y++) {
            QRgb pixel = heightmapImage.pixel( x, y );

            // save a grayscale version of the pixel
            heightmap[ y * width + x ] = (unsigned char)qGray( pixel );
        }
    }

    // create a raw pixel array for the normal map
    QImage debugImage( heightmap, width, height, QImage::Format_Indexed8 );
    QLabel * l3 = new QLabel;
    l3->setPixmap( QPixmap::fromImage( debugImage ) );
    l3->show();
    l3->setWindowTitle( "Heightmap");


    qDebug() << "EditorMainWindow::updateNormalMap: created 8 bit heightmap";

    QString heightMapFilename = heightmapImageFilename;
    QString normalMapFilename = heightmapImageFilename;
    heightMapFilename.replace( ".png", ".heightmap" );
    normalMapFilename.replace( ".png", "-normalmap.png" );

    qDebug() << "EditorMainWindow::updateNormalMap: saving heightmap to:" << heightMapFilename;

    // save the updated 8 bit heightmap to a new file
    QFile file( heightMapFilename );
    if ( ! file.open( QIODevice::WriteOnly |QIODevice::Truncate ) ) {
        QMessageBox::warning( 0, "Saving heightmap failed", "Failed to save the heightmap: " + file.errorString(), QMessageBox::Ok );
        return;
    }

    file.write( (const char*)heightmap, width * height );
    file.close();

    qDebug() << "EditorMainWindow::updateNormalMap: heightmap saved ok";

    // create a raw pixel array for the normal map
    QImage normalMapImage( width, height, QImage::Format_RGBA8888 );

    for ( int x = 0; x < width; x++) {
        for ( int y = 0; y < height; y++) {
            unsigned char center = heightmap[ y * width + x ];
            unsigned char left   = center;
            unsigned char right  = center;
            unsigned char up     = center;
            unsigned char down   = center;

            if ( x > 0 ) {
                left = heightmap[ y * width + x - 1 ];
            }

            if ( x < width - 1 ) {
                right = heightmap[ y * width + x + 1 ];
            }

            if ( y > 0 ) {
                up = heightmap[ (y - 1) * width + x ];
            }

            if ( y < height - 1 ) {
                down = heightmap[ (y + 1) * width + x ];
            }

            int deltaX = 128 + (left - right) * 10; //( 128 + left - right) / 2;
            int deltaY = 128 + (up - down) * 10; //( 128 + up - down ) / 2;
            if ( x < 100 && y < 100 ) {
                qDebug() << x << y << deltaX << deltaY;
            }

            //qDebug() << x << y << "," << center << "->" << left << right << up << down << deltaX << deltaY << qRgba( deltaX, deltaY, 255, 255 );
            normalMapImage.setPixel( x, y, qRgba( deltaX & 255, deltaY & 255, 255, 255 ) );
        }
    }

    QLabel * l2 = new QLabel;
    l2->setPixmap( QPixmap::fromImage( normalMapImage ) );
    l2->show();
    l2->setWindowTitle( "Normal map");

    qDebug() << "EditorMainWindow::updateNormalMap: saving normal map to:" << normalMapFilename;

    // create an image from the data
    if ( ! normalMapImage.save( normalMapFilename, "PNG" ) ) {
        QMessageBox::warning( 0, "Saving normal map failed", "Failed to save the normal map to:\n" + normalMapFilename, QMessageBox::Ok );
        return;
    }

    qDebug() << "EditorMainWindow::updateNormalMap: normal map saved ok";
}

