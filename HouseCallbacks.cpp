
#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "Selection.hpp"

void EditorMainWindow::selectedHouseChanged (House * house) {
    if ( house ) {
        ui->m_stack->setCurrentIndex( EditorMainWindow::HousePage );
        ui->m_house_rotation->setEnabled( true );
        ui->m_house_rotation->setValue( (int)house->rotation() );
        ui->m_house_rotation_label->setNum( (int)house->rotation() );
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
    ui->m_house_rotation_label->setNum( ui->m_house_rotation->value() );
}



void EditorMainWindow::houseTypeChanged () {
    House * selected = selection->getSelectedHouse();
    if ( ! selected ) {
        return;
    }

    selected->setType( ui->m_house_type->currentIndex() );
}
