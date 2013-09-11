#include "Globals.hpp"
#include "Unit.hpp"
#include "Terrain.hpp"
#include "Selection.hpp"
#include "Map.hpp"

EditorMode editorMode = kEdit;
QList<Unit *> allUnits;
QList<Terrain *> allTerrains;
QList<Objective *> allObjectives;
Selection * selection = new Selection();
Map * map = 0;
