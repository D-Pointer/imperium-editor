#ifndef MAP_HPP
#define MAP_HPP

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

#include "Unit.hpp"
#include "Terrain.hpp"
#include "Objective.hpp"
#include "House.hpp"

class Map : public QGraphicsScene {
    Q_OBJECT
public:
    explicit Map(QObject *parent = 0);
    
    int getWidth () const {
        return m_background->rect().width();
    }

    int getHeight () const {
        return m_background->rect().height();
    }

    QString m_name;


signals:

    void unitAdded (Unit * unit);
    void terrainAdded (Terrain * terrain);
    void objectiveAdded (Objective * objective);
    void houseAdded (House * house);


public slots:

    void setSize (int width, int height);

    
protected:

    virtual void mousePressEvent (QGraphicsSceneMouseEvent *event);


private slots:

    void selectedItemsChanged ();


private:
    
    QGraphicsRectItem * m_background;

    QList<QGraphicsLineItem *> m_grid;
};

#endif // MAP_HPP
