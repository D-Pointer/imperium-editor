#ifndef EDITORMAINWINDOW_HPP
#define EDITORMAINWINDOW_HPP

class QActionGroup;

#include <QMainWindow>

#include "Map.hpp"
#include "Globals.hpp"
#include "Terrain.hpp"
#include "Serializer.hpp"

namespace Ui {
class EditorMainWindow;
}

class EditorMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit EditorMainWindow(QWidget *parent = 0);
    ~EditorMainWindow();
    
protected:

    virtual void closeEvent(QCloseEvent *);

private slots:

    void newMap ();
    void openMap ();
    void saveMap ();
    void saveMapAs ();
    void updateNormalMap ();

    void editModeChanged ();
    void selectedTerrainChanged (Terrain * terrain);
    void deleteSelectedItem ();
    void terrainTypeChanged ();
    void terrainDone ();
    void addPoint ();
    void rotateTerrain ();
    void duplicateTerrain ();

    void flipMapHorizontally ();
    void flipMapVertically ();


    void unitAdded (Unit * unit);
    void selectedUnitChanged (Unit * unit);
    void unitRotated ();
    void unitNameChanged (const QString & name);
    void unitTypeChanged ();
    void unitOwnerChanged ();
    void unitHQChanged ();
    void unitModeChanged ();
    void unitMenChanged ();
    void unitAmmoChanged ();
    void unitWeaponChanged ();
    void unitExperienceChanged ();
    void refreshHqList ();
    void updateUnitStats ();
    void updateUnitWeaponCount ();

    void selectedObjectiveChanged (Objective * objective);
    void objectiveNameChanged (const QString & name);

    void selectedHouseChanged (House * house);
    void houseRotated();
    void houseTypeChanged ();

    void zoomIn ();
    void zoomOut ();
    void zoomNormal ();

    void deselect ();

    void generateNavigation ();


private:

    friend class Serializer;

    void takeNewMapIntoUse ();

    QActionGroup * m_actionGroup;

    Ui::EditorMainWindow *ui;

    enum StackPage {
        ScenarioPage,
        TerrainPage,
        HousePage,
        UnitPage,
        ObjectivePage
    };
};

#endif // EDITORMAINWINDOW_HPP
