
#include <QMessageBox>
#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"

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
