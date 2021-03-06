#-------------------------------------------------
#
# Project created by QtCreator 2012-07-23T18:12:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImperiumEditor
TEMPLATE = app
ICON = Icons/Icon.icns

#QMAKE_MAC_SDK = macosx10.9

SOURCES += main.cpp\
        EditorMainWindow.cpp \
    Map.cpp \
    Globals.cpp \
    Unit.cpp \
    Serializer.cpp \
    Terrain.cpp \
    Dot.cpp \
    Objective.cpp \
    UnitCallbacks.cpp \
    ObjectiveCallbacks.cpp \
    House.cpp \
    HouseCallbacks.cpp \
    Perlin.cpp \
    GeneratorDialog.cpp \
    NavigationCallbacks.cpp \
    #NormalMapCallbacks.cpp \
    NetworkServer.cpp \
    Validator.cpp \
    VictoryConditionsDialog.cpp \
    VictoryConditions.cpp \
    RiverCallbacks.cpp \
    UnitList.cpp \
    UnitsModel.cpp \
    ReinforcementPoint.cpp \
    ReinforcementPointCallbacks.cpp \
    ScriptEditor.cpp \
    StartPos.cpp \
    StartPosCallbacks.cpp

HEADERS  += EditorMainWindow.hpp \
    Map.hpp \
    Globals.hpp \
    Unit.hpp \
    Selection.hpp \
    Serializer.hpp \
    Terrain.hpp \
    Dot.hpp \
    Objective.hpp \
    House.hpp \
    Perlin.hpp \
    GeneratorDialog.hpp \
    NetworkServer.hpp \
    Validator.hpp \
    Version.hpp \
    VictoryConditionsDialog.hpp \
    VictoryConditions.hpp \
    UnitList.hpp \
    UnitsModel.hpp \
    ReinforcementPoint.hpp \
    ScriptEditor.hpp \
    StartPos.hpp

FORMS    += EditorMainWindow.ui \
    GeneratorDialog.ui \
    VictoryConditionsDialog.ui \
    UnitList.ui \
    ScriptEditor.ui

RESOURCES += \
    Icons.qrc
