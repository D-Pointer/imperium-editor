#ifndef OBJECTIVE_HPP
#define OBJECTIVE_HPP

#include <QGraphicsPixmapItem>

#import "Globals.hpp"

class Objective : public QGraphicsPixmapItem {

public:
    Objective (const QPointF & pos, QGraphicsItem * parent=0);
    Objective (int id, const QString & title, const QPointF & pos, QGraphicsItem * parent=0);

    int m_id;
    QString m_title;

    static int m_next_id;
};

#endif // Objective_HPP
