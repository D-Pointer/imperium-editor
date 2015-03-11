#include "Unit.hpp"

int Unit::m_next_id = 0;

Unit::Unit (Unit * original) : QGraphicsPixmapItem(original->parentItem()) {
    m_name  = original->m_name;
    m_id    = Unit::m_next_id++;
    m_hq_id = original->m_hq_id;
    m_men   = original->m_men;
    m_ammo  = original->m_ammo;
    m_mode  = original->m_mode;
    m_owner = original->m_owner;
    m_type  = original->m_type;
    m_weapon = original->m_weapon;
    m_experience = original->m_experience;

    setupIcon();

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setPos( original->pos() - boundingRect().center() + QPointF( 20, 20 ));
    setTransformOriginPoint( boundingRect().center() );
    setZValue( 200 );
}


Unit::Unit (const QPointF & pos, QGraphicsItem * parent) : QGraphicsPixmapItem(parent) {
    m_name  = "Unnamed";
    m_id    = Unit::m_next_id++;
    m_hq_id = -1;
    m_men   = 100;
    m_ammo  = 50;
    m_mode  = kFormation;
    m_owner = kPlayer1;
    m_type  = kInfantry;
    m_weapon = kRifle;
    m_experience = kRegular;

    setupIcon();

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setPos( pos - boundingRect().center() );
    setTransformOriginPoint( boundingRect().center() );
    setZValue( 200 );
}


Unit::Unit (int id, Player owner, UnitType type, const QPointF & pos, QGraphicsItem * parent) : QGraphicsPixmapItem(parent), m_id(id), m_owner(owner), m_type(type) {
    Unit::m_next_id = qMax( id + 1, Unit::m_next_id );

    setupIcon();

    setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );

    setPos( pos - boundingRect().center() );
    setTransformOriginPoint( boundingRect().center() );
    setZValue( 200 );
}


void Unit::setupIcon () {
    switch ( m_type ) {
    case kInfantry:
        setPixmap( m_owner == kPlayer1 ? QPixmap(":/Icons/Infantry1.png") : QPixmap(":/Icons/Infantry2.png") );
        break;

    case kCavalry:
        setPixmap( m_owner == kPlayer1 ? QPixmap(":/Icons/Cavalry1.png") : QPixmap(":/Icons/Cavalry2.png") );
        break;

    case kArtillery:
        setPixmap( m_owner == kPlayer1 ? QPixmap(":/Icons/Artillery1.png") : QPixmap(":/Icons/Artillery2.png") );
        break;

    case kInfantryHeadquarter:
        setPixmap( m_owner == kPlayer1 ? QPixmap(":/Icons/InfantryHQ1.png") : QPixmap(":/Icons/InfantryHQ2.png") );
        break;

    case kCavalryHeadquarter:
        setPixmap( m_owner == kPlayer1 ? QPixmap(":/Icons/CavalryHQ1.png") : QPixmap(":/Icons/CavalryHQ2.png") );
        break;
    }
}
