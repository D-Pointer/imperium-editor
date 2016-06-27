
#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "StartPos.hpp"
#include "Selection.hpp"

void EditorMainWindow::startPosAdded (StartPos * startPos) {
    ui->m_start_position_count->setNum( allStartPositions.size() );
}
