#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <QList>

class Selection;
class Unit;
class Terrain;
class Objective;
class Map;
class House;

// battle size for multiplayer games
typedef enum {
    kSmallBattle,
    kMediumBattle,
    kLargeBattle,
    kNotIncluded,
} BattleSizeType;

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
    kRifle,
    kLightCannon,
    kHeavyCannon,
    kMachineGun,
    kFlamethrower
} WeaponType;


typedef enum {
    kGreen,
    kRegular,
    kVeteran,
    kElite,
} ExperienceType;

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


static const int navigationTileSize = 10;

extern EditorMode editorMode;
extern QList<House *> allHouses;
extern QList<Unit *> allUnits;
extern QList<Objective *> allObjectives;
extern QList<Terrain *> allTerrains;
extern Terrain ** navigationGrid;
extern Selection * selection;
extern Map * map;

#endif // GLOBALS_HPP
