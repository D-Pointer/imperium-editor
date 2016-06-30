#include <QDebug>

#include "Validator.hpp"
#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"

Validator::Validator () {
}


bool Validator::validate (EditorMainWindow * editor, QString &result) {
    bool ok = validateMetaData( editor, result );
    ok &= validateUnits( editor, result );
    ok &= validateObjectives( editor, result );
    ok &= validateVictoryConditions( editor, result );

    if ( ok ) {
 result += "Scenario is ok.\n";
    }

    return ok;
}


bool Validator::validateMetaData (EditorMainWindow * editor, QString &result) {
    bool ok = true;

    // title
    if ( editor->ui->m_title->text().isEmpty() ) {
        result += "The scenario must have a valid title.\n";
        ok = false;
    }

    // description
    if ( editor->ui->m_description->toPlainText().isEmpty() ) {
        result += "The scenario must have a description.\n";
        ok = false;
    }

    return ok;
}


bool Validator::validateUnits (EditorMainWindow * editor, QString &result) {
    if ( editor->ui->m_mapType == kMultiplayer ) {
        return true;
    }

    int unitCounts[2] = { 0, 0 };

    bool ok = true;

    foreach (Unit * unit, allUnits ) {
        // add to the counts
        unitCounts[ unit->m_owner ]++;

        if ( unit->m_name.isEmpty() || unit->m_name == "Unnamed" ) {
            result += "Unit must have a valid name.\n";
            ok = false;
        }
    }

    // unit counts ok?
    if ( unitCounts[0] == 0 ) {
        result += "No units for player 1.\n";
        ok = false;
    }
    if ( unitCounts[1] == 0 ) {
        result += "No units for player 2.\n";
        ok = false;
    }

    return ok;
}


bool Validator::validateObjectives (EditorMainWindow * editor, QString &result) {
    bool ok = true;

    foreach (Objective * objective, allObjectives ) {
        // objective title must be valid
        if ( objective->m_title.isEmpty() ) {
            result += "Objective must have a title.\n";
            ok = false;
        }
    }

    return ok;
}


bool Validator::validateVictoryConditions (EditorMainWindow * editor, QString &result) {
    bool ok = true;
    if ( allVictoryConditions.isEmpty() ) {
        result += "No victory or game end conditions set.\n";
        ok = false;
    }

    return ok;
}

