#include <QMessageBox>
#include <QDebug>

#include "Validator.hpp"
#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"

Validator::Validator () {
}


bool Validator::validate (EditorMainWindow * editor) {
    return validateMetaData( editor ) &&
            validateUnits( editor ) &&
            validateObjectives( editor ) &&
            validateVictoryConditions( editor );
}


bool Validator::validateMetaData (EditorMainWindow * editor) {
    // title
    if ( editor->ui->m_title->text().isEmpty() ) {
        QMessageBox::warning( editor, "Validation Error", "The scenario must have a valid title.", QMessageBox::Ok );
        return false;
    }

    // description
    if ( editor->ui->m_description->toPlainText().isEmpty() ) {
        QMessageBox::warning( editor, "Validation Error", "The scenario must have a description.", QMessageBox::Ok );
        return false;
    }

    // meta data is valid
    return true;
}


bool Validator::validateUnits (EditorMainWindow * editor) {
    int unitCounts[2] = { 0, 0 };

    foreach (Unit * unit, allUnits ) {
        // add to the counts
        unitCounts[ unit->m_owner ]++;

        if ( unit->m_name.isEmpty() || unit->m_name == "Unnamed" ) {
            QMessageBox::warning( editor, "Validation Error", "Unit must have a valid name.", QMessageBox::Ok );
            return false;
        }
    }

    // unit counts ok?
    if ( unitCounts[0] == 0 ) {
        QMessageBox::warning( editor, "Validation Error", "No units for player 1", QMessageBox::Ok );
        return false;
    }
    if ( unitCounts[1] == 0 ) {
        QMessageBox::warning( editor, "Validation Error", "No units for player 2", QMessageBox::Ok );
        return false;
    }

    // units are valid
    return true;
}


bool Validator::validateObjectives (EditorMainWindow * editor) {
    foreach (Objective * objective, allObjectives ) {
        // objective title must be valid
        if ( objective->m_title.isEmpty() ) {
            QMessageBox::warning( editor, "Validation Error", "Objective must have a title.", QMessageBox::Ok );
            return false;
        }
    }

    // objectives are valid
    return true;
}


bool Validator::validateVictoryConditions (EditorMainWindow * editor) {
    if ( allVictoryConditions.isEmpty() ) {
        QMessageBox::warning( editor, "Validation Error", "No victory or game end conditions set.", QMessageBox::Ok );
        return false;
    }

    // we have victory conditions
    return true;
}

