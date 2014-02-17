#ifndef EDITORMAINWINDOW_HPP
#define EDITORMAINWINDOW_HPP

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
    //void sizeChanged ();
    void editModeChanged ();
    void selectedTerrainChanged (Terrain * terrain);
    void deleteSelectedItem ();
    void terrainTypeChanged ();
    void addPoint ();
    void rotateTerrain ();
    void duplicateTerrain ();

    void unitAdded (Unit * unit);
    void selectedUnitChanged (Unit * unit);
    void unitRotated ();
    void unitNameChanged (const QString & name);
    void unitTypeChanged ();
    void unitOwnerChanged ();
    void unitHQChanged ();
    void unitModeChanged ();
    void unitMenChanged ();
    void unitGunsChanged ();
    void refreshHqList ();
    void updateUnitStats ();

    void selectedObjectiveChanged (Objective * objective);
    void objectiveNameChanged (const QString & name);

    void selectedHouseChanged (House * house);
    void houseRotated();
    void houseTypeChanged ();

    void zoomIn ();
    void zoomOut ();
    void zoomNormal ();

    void deselect ();


private:

    friend class Serializer;

    void takeNewMapIntoUse ();

    Ui::EditorMainWindow *ui;
};

#endif // EDITORMAINWINDOW_HPP
