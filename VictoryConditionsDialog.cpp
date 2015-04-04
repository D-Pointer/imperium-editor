#include <QMessageBox>
#include <QList>
#include <QDebug>

#include "VictoryConditionsDialog.hpp"
#include "ui_VictoryConditionsDialog.h"
#include "Globals.hpp"
#include "VictoryConditions.hpp"
#include "Unit.hpp"

VictoryConditionsDialog::VictoryConditionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VictoryConditionsDialog) {
    ui->setupUi(this);

    // set up all player 2's units
    ui->m_unitCombo->blockSignals( true );
    ui->m_unitCombo->clear();

    foreach ( Unit * unit, allUnits ) {
        if ( unit->m_owner == kPlayer2 ) {
            ui->m_unitCombo->addItem( unit->m_name, unit->m_id );
        }
    }

    ui->m_unitCombo->blockSignals( false );


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

        // hold all objectives based?
        HoldAllObjectivesBased * holdAllBased = dynamic_cast<HoldAllObjectivesBased *>( condition );
        if ( holdAllBased ) {
            ui->m_holdGroup->setChecked( true );
            ui->m_playerId->setCurrentIndex( (int)holdAllBased->m_player );
            ui->m_holdDuration->setValue( holdAllBased->m_duration );
        }

        // destroy unit based?
        DestroyUnit * destroyUnitBased = dynamic_cast<DestroyUnit *>( condition );
        if ( destroyUnitBased ) {
            ui->m_destroyGroup->setChecked( true );

            int index = ui->m_unitCombo->findData( destroyUnitBased->m_unitId, Qt::UserRole );
            if ( index != -1 ) {
                ui->m_unitCombo->setCurrentIndex( index );
            }
            else {
                ui->m_unitCombo->setCurrentIndex( 0 );
            }
        }
    }
}


VictoryConditionsDialog::~VictoryConditionsDialog () {
    delete ui;
}


void VictoryConditionsDialog::accept () {
    // at least one must be checked
    if ( ! ui->m_timeGroup->isChecked() &&
         ! ui->m_casualtiesGroup->isChecked() &&
         ! ui->m_holdGroup->isChecked() &&
         ! ui->m_destroyGroup->isChecked() ) {
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

    if ( ui->m_holdGroup->isChecked() ) {
        allVictoryConditions << new HoldAllObjectivesBased( (Player)ui->m_playerId->currentIndex(), ui->m_holdDuration->value() );
    }

    if ( ui->m_destroyGroup->isChecked() ) {
        int unitId = ui->m_unitCombo->itemData( ui->m_unitCombo->currentIndex() ).toInt();

        allVictoryConditions << new DestroyUnit( unitId);
    }

    // we're done
    QDialog::accept();
}
