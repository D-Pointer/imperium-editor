#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "Objective.hpp"
#include "Selection.hpp"



void EditorMainWindow::selectedObjectiveChanged (Objective * objective) {
    qDebug() << "EditorMainWindow::selectedObjectiveChanged";

    if ( objective == 0 ) {
         ui->m_objective_title->clear();
    }
    else {
        ui->m_stack->setCurrentIndex( EditorMainWindow::ObjectivePage );
        ui->m_objective_title->setEnabled( true );
        ui->m_objective_title->setText( objective->m_title );
    }
    qDebug() << "EditorMainWindow::selectedObjectiveChanged: current index:" << ui->m_stack->currentIndex();
}


void EditorMainWindow::objectiveNameChanged (const QString & name) {
    // any selected unit?
    Objective * selected = selection->getSelectedObjective();
    if ( ! selected ) {
        return;
    }

    selected->m_title = name;
}

