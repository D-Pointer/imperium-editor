#ifndef UNIT_HPP
#define UNIT_HPP

#include <QGraphicsPixmapItem>

#import "Globals.hpp"

class Unit : public QGraphicsPixmapItem {

public:
    Unit (Unit * original);
    Unit (const QPointF & pos, QGraphicsItem * parent=0);
    Unit (int id, Player owner, UnitType type, const QPointF & pos, QGraphicsItem * parent=0);

    void setupIcon ();

    QString m_name;
    int m_men;
    int m_ammo;
    WeaponType m_weapon;
    int m_id;
    int m_hq_id;
    UnitMode m_mode;
    Player m_owner;
    UnitType m_type;
    ExperienceType m_experience;

    static int m_next_id;
};

#endif // UNIT_HPP
