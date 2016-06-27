#ifndef SCRIPTEDITOR_HPP
#define SCRIPTEDITOR_HPP

#include <QDialog>

namespace Ui {
class ScriptEditor;
}

class ScriptEditor : public QDialog {
    Q_OBJECT

public:
    explicit ScriptEditor(const QString & script, QWidget *parent = 0);
    ~ScriptEditor();

    QString getScript () const;


private:
    Ui::ScriptEditor *ui;
};

#endif // SCRIPTEDITOR_HPP
