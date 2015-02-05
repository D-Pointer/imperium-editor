#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "Objective.hpp"
#include "Selection.hpp"



void EditorMainWindow::selectedObjectiveChanged (Objective * objective) {
    if ( objective == 0 ) {
        ui->m_stack->setCurrentIndex( EditorMainWindow::ScenarioPage );
        //ui->m_tabs->setCurrentWidget( ui->m_map_tab );
        //ui->m_unit_tab->setDisabled( true );
        //ui->m_terrain_tab->setDisabled( true );
        //ui->m_objective_tab->setDisabled( true );
        ui->m_objective_title->clear();
    }
    else {
        ui->m_stack->setCurrentIndex( EditorMainWindow::ObjectivePage );
        //ui->m_tabs->setCurrentWidget( ui->m_objective_tab );
        //ui->m_objective_tab->setEnabled( true );
        ui->m_objective_title->setEnabled( true );
        ui->m_objective_title->setText( objective->m_title );
    }
}


void EditorMainWindow::objectiveNameChanged (const QString & name) {
    // any selected unit?
    Objective * selected = selection->getSelectedObjective();
    if ( ! selected ) {
        return;
    }

    selected->m_title = name;
}

