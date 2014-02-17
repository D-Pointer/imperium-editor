#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <QList>

class Selection;
class Unit;
class Terrain;
class Objective;
class Map;
class House;

typedef enum {
    kEdit,
    kAddTerrain,
    kAddHouse,
    kAddUnit,
    kAddObjective,
    kAddRoad
} EditorMode;

typedef enum {
    kFormation,
    kColumn
} UnitMode;

typedef enum {
    kPlayer1,
    kPlayer2
} Player;

typedef enum {
    kInfantry,
    kCavalry,
    kArtillery,
    kInfantryHeadquarter,
    kCavalryHeadquarter
} UnitType;

typedef enum {
    kWoods = 0,
    kField,
    kGrass,
    kRoad,
    kRiver,
    kRoof,
    kSwamp,
    kRocky,
    kBeach,
    kFord,
    kScatteredTrees
} TerrainType;

typedef enum {
    kPlayer1Attacks = 0,
    kMeeting,
    kPlayer2Attacks
} AIHint;


extern EditorMode editorMode;
extern QList<House *> allHouses;
extern QList<Unit *> allUnits;
extern QList<Objective *> allObjectives;
extern QList<Terrain *> allTerrains;
extern Selection * selection;
extern Map * map;

#endif // GLOBALS_HPP
