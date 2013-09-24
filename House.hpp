#ifndef HOUSE_HPP
#define HOUSE_HPP

#include <QGraphicsPixmapItem>

class House : public QGraphicsPixmapItem {

public:

    House (const QPointF & pos, int type, QGraphicsItem * parent=0);

    int getType () const {
        return m_type;
    }

    void setType (int type);


private:
    // house type, 1..5
    int m_type;
};

#endif // HOUSE_HPP
