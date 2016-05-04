#include "ScriptEditor.hpp"
#include "ui_ScriptEditor.h"

ScriptEditor::ScriptEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptEditor)
{
    ui->setupUi(this);
}

ScriptEditor::~ScriptEditor()
{
    delete ui;
}
