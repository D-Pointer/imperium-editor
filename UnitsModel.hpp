#ifndef UNITSMODEL_HH
#define UNITSMODEL_HH

#include <QAbstractTableModel>

class UnitsModel : public QAbstractTableModel {

public:
    UnitsModel (QObject *parent=0);
    ~UnitsModel ();

    int rowCount (const QModelIndex & parent = QModelIndex()) const;

    int columnCount (const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // UNITSMODEL_HH
