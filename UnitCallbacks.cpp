#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "Unit.hpp"
#include "Selection.hpp"


void EditorMainWindow::unitAdded (Unit * unit) {
    if ( unit->m_type == kHeadquarter ) {
        ui->m_hq->addItem( unit->m_name, unit->m_id );
    }

    updateUnitStats();
}


void EditorMainWindow::selectedUnitChanged (Unit * unit) {
    if ( unit == 0 ) {
        ui->m_tabs->setCurrentWidget( ui->m_map_tab );
        ui->m_unit_tab->setDisabled( true );
        ui->m_terrain_tab->setDisabled( true );
        ui->m_objective_tab->setDisabled( true );

        ui->m_unit_name->clear();
        ui->m_unit_id->clear();
    }
    else {
        ui->m_tabs->setCurrentWidget( ui->m_unit_tab );
        ui->m_unit_tab->setEnabled( true );

        ui->m_unit_name->setText( unit->m_name );
        ui->m_unit_id->setNum( unit->m_id );
        ui->m_men->setValue( unit->m_men );
        ui->m_guns->setValue( unit->m_guns );
        ui->m_mode->setCurrentIndex( unit->m_mode );
        ui->m_owner->setCurrentIndex( unit->m_owner );
        ui->m_rotation->setValue( (int)unit->rotation() );
        ui->m_type->setCurrentIndex( unit->m_type );

        int index = ui->m_hq->findData( unit->m_hq_id, Qt::UserRole );
        if ( index != -1 ) {
            ui->m_hq->setCurrentIndex( index );
        }
        else {
            ui->m_hq->setCurrentIndex( 0 );
        }
    }
}


void EditorMainWindow::unitRotated() {
    // any selected unit?
    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    qDebug() << "EditorMainWindow::rotateUnit: angle:" << ui->m_rotation->value() << selected->rotation();
    selected->setRotation( ui->m_rotation->value() );
}


void EditorMainWindow::unitNameChanged (const QString & name) {
    // any selected unit?
    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    selected->m_name = name;
}


void EditorMainWindow::unitTypeChanged () {
    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    selected->m_type  = (UnitType)ui->m_type->currentIndex();
    selected->setupIcon();

    refreshHqList();
}


void EditorMainWindow::unitOwnerChanged () {
    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    selected->m_owner = (Player)ui->m_owner->currentIndex();
    selected->setupIcon();

    refreshHqList();

    updateUnitStats();
}


void EditorMainWindow::unitHQChanged () {
    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    selected->m_hq_id = ui->m_hq->itemData( ui->m_hq->currentIndex() ).toInt();
}


void EditorMainWindow::unitModeChanged () {
    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    selected->m_mode  = (UnitMode)ui->m_mode->currentIndex();
    selected->setupIcon();
}


void EditorMainWindow::unitMenChanged () {
    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    selected->m_men = ui->m_men->value();

    updateUnitStats();
}


void EditorMainWindow::unitGunsChanged () {
    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    selected->m_guns  = ui->m_guns->value();

    updateUnitStats();
}


void EditorMainWindow::refreshHqList () {
    ui->m_hq->blockSignals( true );

    ui->m_hq->clear();

    Unit * selected = selection->getSelectedUnit();
    if ( ! selected ) {
        return;
    }

    ui->m_hq->addItem( "No HQ", -1 );

    foreach ( Unit * unit, allUnits ) {
        if ( unit->m_owner == selected->m_owner && unit->m_type == kHeadquarter ) {
            ui->m_hq->addItem( unit->m_name, unit->m_id );
        }
    }

    int index = ui->m_hq->findData( selected->m_hq_id, Qt::UserRole );
    if ( index != -1 ) {
        ui->m_hq->setCurrentIndex( index );
    }
    else {
        ui->m_hq->setCurrentIndex( 0 );
    }

    ui->m_hq->blockSignals( false );
}


void EditorMainWindow::updateUnitStats () {
    int units[2] = {0, 0 };
    int men[2] = {0, 0 };
    int guns[2] = {0, 0 };

    foreach ( Unit * unit, allUnits ) {
        units[ unit->m_owner]++;
        men[ unit->m_owner] += unit->m_men;
        guns[ unit->m_owner] += unit->m_guns;
    }

    ui->m_unit_stats1->setText( QString("%1 units, %2 men, %3 guns").arg(units[kPlayer1]).arg(men[kPlayer1]).arg(guns[kPlayer1]) );
    ui->m_unit_stats2->setText( QString("%1 units, %2 men, %3 guns").arg(units[kPlayer2]).arg(men[kPlayer2]).arg(guns[kPlayer2]) );
}
