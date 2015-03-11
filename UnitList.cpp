#include "UnitList.hpp"
#include "ui_UnitList.h"
#include "UnitsModel.hpp"

UnitList::UnitList(QWidget *parent) : QWidget(parent), ui(new Ui::UnitList) {
    ui->setupUi(this);

    ui->unitsTable->setModel( new UnitsModel( ui->unitsTable ) );
}


UnitList::~UnitList () {
    delete ui;
}
