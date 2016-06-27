#include "ScriptEditor.hpp"
#include "ui_ScriptEditor.h"

ScriptEditor::ScriptEditor(const QString & script, QWidget *parent) : QDialog(parent), ui(new Ui::ScriptEditor) {
    ui->setupUi(this);
    ui->m_script->setPlainText( script );
}


ScriptEditor::~ScriptEditor () {
    delete ui;
}


QString ScriptEditor::getScript () const {
    return ui->m_script->toPlainText();
}
