
#include <cmath>

#include <QMessageBox>
#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "Selection.hpp"

void EditorMainWindow::riverMinWidthChanged (int width) {
    if ( width > ui->m_maxRiverWidth->value() ) {
        QMessageBox::warning( this, "Bad width", "The minimum width must be smaller or equal to the maximum width!", QMessageBox::Ok );
        ui->m_minRiverWidth->setValue( ui->m_maxRiverWidth->value() );
    }

    // new widths
    if ( map ) {
        map->setRiverWidths( ui->m_minRiverWidth->value(), ui->m_maxRiverWidth->value() );
    }
}


void EditorMainWindow::riverMaxWidthChanged (int width) {
    if ( width < ui->m_minRiverWidth->value() ) {
        QMessageBox::warning( this, "Bad width", "The minimum width must be larger or equal to the minimum width!", QMessageBox::Ok );
        ui->m_maxRiverWidth->setValue( ui->m_minRiverWidth->value() );
    }

    // new widths
    if ( map ) {
        map->setRiverWidths( ui->m_minRiverWidth->value(), ui->m_maxRiverWidth->value() );
    }
}


void EditorMainWindow::addRiver () {
    QPointF pos, pos2;
    float direction, distance;

    Terrain * river = new Terrain;
    river->setType( kRiver );
    map->addItem( river );
    allTerrains << river;

    int minWidth = ui->m_minRiverWidth->value();
    int maxWidth = ui->m_maxRiverWidth->value();

    // river width is random between the limits
    int riverWidth = minWidth + qrand() % ( maxWidth - minWidth );

    const float minLength = 20;
    const float maxLength = 50;

    // starting point up/down or left/right?
    switch ( qrand() % 4 ) {
    case 0:
        // left
        pos = QPointF( 0, qrand() % map->getHeight() );
        pos2 = QPointF( 0, pos.y() - riverWidth );
        direction = 0;
        break;

    case 1:
        // right
        pos = QPointF( map->getWidth(), qrand() % map->getHeight() );
        pos2 = QPointF( map->getWidth(), pos.y() + riverWidth );
        direction = 180;
        break;

    case 2:
        // top
        pos = QPointF( qrand() % map->getWidth(), 0 );
        pos2 = QPointF( pos.x() + riverWidth, 0 );
        direction = 90;
        break;

    case 3:
        // bottom
        pos = QPointF( qrand() % map->getWidth(), map->getHeight() );
        pos2 = QPointF( pos.x() - riverWidth, map->getHeight() );
        direction = 270;
        break;
    }

    // add the clicked position
    river->addPoint( pos );
    river->addPoint( pos2 );

    // the first point is longer and less deviating
    bool firstPoint = true;

    while ( true ) {
        // how far does this leg go and in what direction?
        if ( firstPoint ) {
            distance = 30 + qrand() % 20;
            direction += -5 + qrand() % 10;
            firstPoint = false;
        }
        else {
            float delta = -30 + qrand() % 60;
            direction += delta;

            distance = minLength + std::abs( delta ) / maxLength * (maxLength - minLength );
        }

        // deltas
        float x = distance * cosf( direction / 180.0f * M_PI );
        float y = distance * sinf( direction / 180.0f * M_PI );
        pos += QPointF( x, y );

        // the river can change with 5 m per step
        riverWidth += -5 + (qrand() % 10);
        if ( riverWidth < minWidth ) {
            riverWidth = minWidth;
        }
        else if ( riverWidth > maxWidth ) {
            riverWidth = maxWidth;
        }

        bool riverHit = false;

        // was a river hit? we check this before adding the point so that the check doesn't
        // match the added point
        foreach ( Terrain * terrain, allTerrains ) {
            if ( terrain->m_type != kRiver ) {
                continue;
            }

            if ( terrain->polygon().containsPoint( pos, Qt::OddEvenFill ) ) {
                // hit a river
                riverHit = true;
                break;
            }
        }

        // add the clicked position
        if ( ! map->addPointToRoadOrRiver( river, pos, riverWidth ) ) {
            // both positions are no longer inside
            break;
        }

        if ( riverHit ) {
            break;
        }
    }

    // bac to edit mode
//    editorMode = kEdit;
//    ui->m_edit_action->setChecked( true );
}
