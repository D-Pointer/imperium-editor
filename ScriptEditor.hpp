#ifndef SCRIPTEDITOR_HPP
#define SCRIPTEDITOR_HPP

#include <QDialog>

namespace Ui {
class ScriptEditor;
}

class ScriptEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptEditor(QWidget *parent = 0);
    ~ScriptEditor();

private:
    Ui::ScriptEditor *ui;
};

#endif // SCRIPTEDITOR_HPP
