#ifndef VICTORY_CONDITIONS_HPP
#define VICTORY_CONDITIONS_HPP

#include <QString>

#include "Globals.hpp"

class VictoryCondition {
public:

    virtual ~VictoryCondition() {
    }

    virtual QString toString () const {
        return "";
    }
};


class DestroyUnit : public VictoryCondition {
public:

    DestroyUnit (int unitId) : m_unitId(unitId) {

    }

    static QString id () {
        return "destroy";
    }

    virtual QString toString () const {
        return QString("destroy %1").arg( m_unitId);
    }

    int m_unitId;
};

class TimeBased : public VictoryCondition {
public:

    TimeBased (int duration) : m_duration(duration) {

    }

    static QString id () {
        return "time";
    }

    virtual QString toString () const {
        return QString("time %1").arg( m_duration );
    }

    int m_duration;
};


class HoldAllObjectivesBased : public VictoryCondition {
public:

    HoldAllObjectivesBased (Player player, int duration) : m_player(player), m_duration(duration) {

    }

    static QString id () {
        return "hold";
    }

    virtual QString toString () const {
        return QString("hold %1 %1").arg( m_player ).arg( m_duration );
    }

    Player m_player;
    int m_duration;
};


class CasualtyBased : public VictoryCondition {
public:

    CasualtyBased (int percentage) : m_percentage(percentage) {

    }

    static QString id () {
        return "casualty";
    }

    virtual QString toString () const {
        return QString("casualty %1").arg( m_percentage );
    }

    int m_percentage;
};


class EscortUnitBased : public VictoryCondition {
public:

    EscortUnitBased (int unitId, int objectiveId) : m_unitId(unitId), m_objectiveId(objectiveId) {

    }

    static QString id () {
        return "escort";
    }

    virtual QString toString () const {
        return QString("escort %1 %2").arg( m_unitId ).arg( m_objectiveId );
    }

    int m_unitId;
    int m_objectiveId;
};



#endif // VICTORYCONDITIONS_HPP

