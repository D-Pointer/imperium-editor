#-------------------------------------------------
#
# Project created by QtCreator 2012-07-23T18:12:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImperiumEditor
TEMPLATE = app


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
    HouseCallbacks.cpp

HEADERS  += EditorMainWindow.hpp \
    Map.hpp \
    Globals.hpp \
    Unit.hpp \
    Selection.hpp \
    Serializer.hpp \
    Terrain.hpp \
    Dot.hpp \
    Objective.hpp \
    House.hpp

FORMS    += EditorMainWindow.ui

RESOURCES += \
    Icons.qrc
