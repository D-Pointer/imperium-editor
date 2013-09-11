#ifndef SELECTION_HH
#define SELECTION_HH

#include <QObject>

#include "Unit.hpp"
#include "Terrain.hpp"
#include "Objective.hpp"
#include "Globals.hpp"
#include "Map.hpp"

class Selection : public QObject {
    Q_OBJECT

public:

    Selection () : QObject(), m_unit(0), m_terrain(0) {

    }

    void setUnit (Unit * unit) {
        m_unit = unit;
        emit selectedUnitChanged( m_unit );
    }

    void setTerrain (Terrain * terrain) {
        m_terrain = terrain;
        emit selectedTerrainChanged( m_terrain );
    }

    void setObjective (Objective * objective) {
        m_objective = objective;
        emit selectedObjectiveChanged( m_objective );
    }

    Unit * getSelectedUnit () {
        return m_unit;
    }

    Terrain * getSelectedTerrain () {
        return m_terrain;
    }

    Objective * getSelectedObjective () {
        return m_objective;
    }



    void deselect () {
        if ( m_unit ) {
            setUnit( 0 );
        }

        if ( m_terrain ) {
            setTerrain( 0 );
        }

        if ( m_objective ) {
            setObjective( 0 );
        }

        if ( map ) {
            map->clearSelection();
        }
    }

signals:

    void selectedUnitChanged (Unit * unit);
    void selectedTerrainChanged (Terrain * terrain);
    void selectedObjectiveChanged (Objective * objective);


private:
    Unit * m_unit;
    Terrain * m_terrain;
    Objective * m_objective;
};

#endif // SELECTION_HH
