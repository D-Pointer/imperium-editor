#ifndef START_POS_HPP
#define START_POS_HPP

#include <QGraphicsPixmapItem>

#import "Globals.hpp"

class StartPos : public QGraphicsPixmapItem {

public:
    StartPos (const QPointF & pos, QGraphicsItem * parent=0);
};

#endif

