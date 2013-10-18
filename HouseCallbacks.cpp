
#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "Selection.hpp"

void EditorMainWindow::selectedHouseChanged (House * house) {
    if ( house == 0 ) {
        ui->m_tabs->setCurrentWidget( ui->m_map_tab );
        ui->m_unit_tab->setDisabled( true );
        ui->m_terrain_tab->setDisabled( true );
        ui->m_objective_tab->setDisabled( true );
        ui->m_houses_tab->setDisabled( true );
    }
    else {
        ui->m_tabs->setCurrentWidget( ui->m_houses_tab );
        ui->m_houses_tab->setEnabled( true );
        ui->m_house_rotation->setEnabled( true );
        ui->m_house_rotation->setValue( (int)house->rotation() );
        ui->m_house_type->setCurrentIndex( house->getType() );
    }
}



void EditorMainWindow::houseRotated() {
    // any selected house?
    House * selected = selection->getSelectedHouse();
    if ( ! selected ) {
        return;
    }

    selected->setRotation( ui->m_house_rotation->value() );
}



void EditorMainWindow::houseTypeChanged () {
    House * selected = selection->getSelectedHouse();
    if ( ! selected ) {
        return;
    }

    selected->setType( ui->m_house_type->currentIndex() );
}
