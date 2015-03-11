#ifndef UNITLIST_HPP
#define UNITLIST_HPP

#include <QWidget>

namespace Ui {
class UnitList;
}

class UnitList : public QWidget
{
    Q_OBJECT

public:
    explicit UnitList(QWidget *parent = 0);
    ~UnitList();

private:
    Ui::UnitList *ui;
};

#endif // UNITLIST_HPP
