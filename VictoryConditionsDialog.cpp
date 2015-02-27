#include <QMessageBox>
#include <QList>
#include <QDebug>

#include "VictoryConditionsDialog.hpp"
#include "ui_VictoryConditionsDialog.h"
#include "Globals.hpp"
#include "VictoryConditions.hpp"

VictoryConditionsDialog::VictoryConditionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VictoryConditionsDialog) {
    ui->setupUi(this);

    // check all conditions
    foreach ( VictoryCondition * condition, allVictoryConditions ) {
        // time based?
        TimeBased * timeBased = dynamic_cast<TimeBased *>( condition );
        if ( timeBased ) {
            ui->m_timeGroup->setChecked( true );
            ui->m_duration->setValue( timeBased->m_duration );
        }

        // casualty based?
        CasualtyBased * casualtyBased = dynamic_cast<CasualtyBased *>( condition );
        if ( casualtyBased ) {
            ui->m_casualtiesGroup->setChecked( true );
            ui->m_percentage->setValue( casualtyBased->m_percentage );
        }
    }
}


VictoryConditionsDialog::~VictoryConditionsDialog () {
    delete ui;
}


void VictoryConditionsDialog::accept () {
    // at least one must be checked
    if ( ! ui->m_timeGroup->isChecked() && ! ui->m_casualtiesGroup->isChecked() ) {
        QMessageBox::warning( this, "Error", "At least one victory condition must be enabled.", QMessageBox::Ok );
        return;
    }

    // get rid of all old victory conditions
    qDeleteAll( allVictoryConditions );
    allVictoryConditions.clear();

    if ( ui->m_timeGroup->isChecked() ) {
        allVictoryConditions << new TimeBased( ui->m_duration->value() );
    }

    if ( ui->m_casualtiesGroup->isChecked() ) {
        allVictoryConditions << new CasualtyBased( ui->m_percentage->value() );
    }

    // we're done
    QDialog::accept();
}
