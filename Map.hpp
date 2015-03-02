#ifndef MAP_HPP
#define MAP_HPP

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QDebug>

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

    void setRiverWidths (int minWidth, int maxWidth) {
        m_minRiverWidth = minWidth;
        m_maxRiverWidth = maxWidth;
    }

    int getRiverMinWidth () const {
        return m_minRiverWidth;
    }

    int getRiverMaxWidth () const {
        return m_maxRiverWidth;
    }

    bool addPointToRoadOrRiver (Terrain * road, const QPointF & pos, float width=-1);

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

    // river dimensions
    int m_minRiverWidth;
    int m_maxRiverWidth;
};

#endif // MAP_HPP
