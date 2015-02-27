#ifndef VICTORY_CONDITIONS_HPP
#define VICTORY_CONDITIONS_HPP

#include <QString>

class VictoryCondition {
public:

    virtual ~VictoryCondition() {
    }

    virtual QString toString () const {
        return "";
    }
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



#endif // VICTORYCONDITIONS_HPP

