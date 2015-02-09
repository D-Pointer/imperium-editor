#include "Globals.hpp"
#include "Unit.hpp"
#include "Terrain.hpp"
#include "Selection.hpp"
#include "Map.hpp"
#include "House.hpp"

EditorMode editorMode = kEdit;
QList<Unit *> allUnits;
QList<House *> allHouses;
QList<Terrain *> allTerrains;
QList<Terrain *> navigationGrid;
QList<Objective *> allObjectives;
Selection * selection = new Selection();
Map * map = 0;
