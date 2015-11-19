
#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "ReinforcementPoint.hpp"
#include "Selection.hpp"

void EditorMainWindow::reinforcementPointAdded (ReinforcementPoint * point) {
    ui->m_reinforcementCount->setNum( allReinforcementPoints.size() );
}
