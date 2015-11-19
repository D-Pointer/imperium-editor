#ifndef REINFORCEMENTPOINT_HH
#define REINFORCEMENTPOINT_HH

#include <QGraphicsPixmapItem>

#import "Globals.hpp"

class ReinforcementPoint : public QGraphicsPixmapItem {

public:
    ReinforcementPoint (const QPointF & pos, QGraphicsItem * parent=0);
};

#endif

