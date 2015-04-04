#ifndef VICTORYCONDITIONSDIALOG_HPP
#define VICTORYCONDITIONSDIALOG_HPP

#include <QDialog>

namespace Ui {
class VictoryConditionsDialog;
}

class VictoryConditionsDialog : public QDialog {
    Q_OBJECT

public:
    explicit VictoryConditionsDialog (QWidget *parent = 0);
    ~VictoryConditionsDialog ();

    virtual void accept ();


private:

    Ui::VictoryConditionsDialog *ui;
};

#endif // VICTORYCONDITIONSDIALOG_HPP
