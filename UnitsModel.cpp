#include <QColor>
#include "UnitsModel.hpp"

UnitsModel::UnitsModel(QObject *parent) : QAbstractTableModel(parent) {

}


UnitsModel::~UnitsModel() {

}


int UnitsModel::rowCount (const QModelIndex & parent) const {
    Q_UNUSED( parent );
    return 10;
}


int UnitsModel::columnCount (const QModelIndex & parent) const {
    Q_UNUSED( parent );
    return 5;
}


QVariant UnitsModel::data(const QModelIndex & index, int role) const {
    // custom color
    if ( role == Qt::ForegroundRole ) {
        if ( index.row() < 5 ) {
            return QColor( Qt::darkBlue );
        }
        else {
            return QColor( Qt::darkRed );
        }
    }

    // and now only handle the display role
    if ( role != Qt::DisplayRole ) {
        return QVariant();
    }

    return QString("Unit %1 %2").number( index.row() ).number( index.column() );

}


QVariant UnitsModel::headerData (int section, Qt::Orientation orientation, int role) const {
    if ( role != Qt::DisplayRole || orientation == Qt::Vertical ) {
        return QAbstractTableModel::headerData( section, orientation, role );
    }

    switch ( section ) {
    case 0:
        return "Name";
        break;

    case 1:
        return "Owner";
        break;

    case 2:
        return "Type";
        break;

    case 3:
        return "Men";
        break;

    case 4:
        return "Weapon";
        break;
    }

    return QAbstractTableModel::headerData( section, orientation, role );
}



Qt::ItemFlags UnitsModel::flags (const QModelIndex &index) const {
       return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

