#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <QList>

class Selection;
class Unit;
class Terrain;
class Objective;
class Map;
class House;
class VictoryCondition;
class ReinforcementPoint;
class StartPos;

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
    kAddRoad,
    kAddRiver,
    kAddStartPos,
    kAddReinforcementPoint,
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
    kFlamethrower,
    kRifleMk2,
    kSubmachineGun,
    kMortar,
    kHowitzer,
    kSniperRifle
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
extern QList<ReinforcementPoint *> allReinforcementPoints;
extern QList<StartPos *> allStartPositions;
extern Terrain ** navigationGrid;
extern QList<VictoryCondition *> allVictoryConditions;
extern Selection * selection;
extern Map * map;
extern QString script;

#endif // GLOBALS_HPP
