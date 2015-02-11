#include <QMessageBox>
#include <QCloseEvent>
#include <QActionGroup>
#include <QFileDialog>
#include <QStatusBar>
#include <QDebug>

#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "Selection.hpp"
#include "Serializer.hpp"
#include "GeneratorDialog.hpp"
#include "NetworkServer.hpp"

EditorMainWindow::EditorMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::EditorMainWindow) {
    ui->setupUi(this);

    m_actionGroup = new QActionGroup( this );
    m_actionGroup->addAction( ui->m_edit_action );
    m_actionGroup->addAction( ui->m_terrain_action );
    m_actionGroup->addAction( ui->m_house_action );
    m_actionGroup->addAction( ui->m_road_action );
    m_actionGroup->addAction( ui->m_unit_action );
    m_actionGroup->addAction( ui->m_objective_action );
    m_actionGroup->setExclusive( true );

    connect( ui->m_open_action,        SIGNAL( triggered() ),       SLOT( openMap()) );
    connect( ui->m_new_action,         SIGNAL( triggered() ),       SLOT( newMap()) );
    connect( ui->m_save_action,        SIGNAL( triggered() ),       SLOT( saveMap()) );
    connect( ui->m_save_as_action,     SIGNAL( triggered() ),       SLOT( saveMapAs()) );
    connect( ui->m_quit_action,        SIGNAL( triggered() ),       SLOT( close()) );

    connect( ui->m_edit_action,        SIGNAL( triggered() ),       SLOT( editModeChanged()) );
    connect( ui->m_terrain_action,     SIGNAL( triggered() ),       SLOT( editModeChanged()) );
    connect( ui->m_unit_action,        SIGNAL( triggered() ),       SLOT( editModeChanged()) );
    connect( ui->m_house_action,       SIGNAL( triggered() ),       SLOT( editModeChanged()) );
    connect( ui->m_road_action  ,      SIGNAL( triggered() ),       SLOT( editModeChanged()) );
    connect( ui->m_objective_action,   SIGNAL( triggered() ),       SLOT( editModeChanged()) );
    connect( ui->m_delete_action,      SIGNAL( triggered() ),       SLOT( deleteSelectedItem()) );
    connect( ui->m_deselect_action,    SIGNAL( triggered() ),       SLOT( deselect()) );

    connect( ui->m_rotation,           SIGNAL(valueChanged(int)),        SLOT( unitRotated()) );
    connect( ui->m_unit_name,          SIGNAL(textChanged(QString)),     SLOT( unitNameChanged(QString)) );
    connect( ui->m_owner,              SIGNAL(currentIndexChanged(int)), SLOT( unitOwnerChanged()) );
    connect( ui->m_type,               SIGNAL(currentIndexChanged(int)), SLOT( unitTypeChanged()) );
    connect( ui->m_mode,               SIGNAL(currentIndexChanged(int)), SLOT( unitModeChanged()) );
    connect( ui->m_hq,                 SIGNAL(currentIndexChanged(int)), SLOT( unitHQChanged()) );
    connect( ui->m_men,                SIGNAL(valueChanged(int)),        SLOT( unitMenChanged()) );
    connect( ui->m_ammo,               SIGNAL(valueChanged(int)),        SLOT( unitAmmoChanged()) );
    connect( ui->m_weapon,             SIGNAL(currentIndexChanged(int)), SLOT( unitWeaponChanged()) );
    connect( ui->m_experience,         SIGNAL(currentIndexChanged(int)), SLOT( unitExperienceChanged()) );

    connect( ui->m_objective_title,    SIGNAL( textChanged(QString)),    SLOT( objectiveNameChanged(QString)) );

    connect( ui->m_house_rotation,     SIGNAL(valueChanged(int)),        SLOT( houseRotated()) );
    connect( ui->m_house_type,         SIGNAL(currentIndexChanged(int)), SLOT( houseTypeChanged()) );

    connect( ui->m_rotate_left,        SIGNAL( clicked()),             SLOT( rotateTerrain()) );
    connect( ui->m_rotate_right,       SIGNAL( clicked()),             SLOT( rotateTerrain()) );
    connect( ui->m_duplicate,          SIGNAL( clicked()),             SLOT( duplicateTerrain()) );
    connect( ui->m_flipHorizontal,     SIGNAL( clicked()),             SLOT( flipMapHorizontally()) );
    connect( ui->m_flipVertical,       SIGNAL( clicked()),             SLOT( flipMapVertically()) );
    connect( ui->m_done,               SIGNAL( clicked()),             SLOT( terrainDone()) );
    connect( ui->m_add_point,          SIGNAL( clicked()),             SLOT( addPoint()) );
    connect( ui->m_zoom_in,            SIGNAL( clicked()),             SLOT( zoomIn()) );
    connect( ui->m_zoom_out,           SIGNAL( clicked()),             SLOT( zoomOut()) );
    connect( ui->m_zoom_normal,        SIGNAL( clicked()),             SLOT( zoomNormal()) );
    connect( ui->m_terrain_type,       SIGNAL(currentIndexChanged(int)), SLOT(terrainTypeChanged()) );
    connect( selection, SIGNAL( selectedUnitChanged(Unit*)),             SLOT( selectedUnitChanged(Unit*)) );
    connect( selection, SIGNAL( selectedTerrainChanged(Terrain*)),       SLOT( selectedTerrainChanged(Terrain*)) );
    connect( selection, SIGNAL( selectedObjectiveChanged(Objective*)),   SLOT( selectedObjectiveChanged(Objective*)) );
    connect( selection, SIGNAL( selectedHouseChanged(House*)),           SLOT( selectedHouseChanged(House*)) );

    // create the network server
    m_server = new NetworkServer( this );
    connect( m_server, SIGNAL(sendMapToIpad(QTcpSocket*)), SLOT(sendMapToIpad(QTcpSocket*)) );
}


EditorMainWindow::~EditorMainWindow() {
    delete ui;
}


void EditorMainWindow::closeEvent (QCloseEvent * event) {
    if ( map ) {
        if ( QMessageBox::question( this, "Confirm", "Really quit?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) {
            event->ignore();
            return;
        }
    }

    event->accept();
}


void EditorMainWindow::newMap () {
    if ( map != 0 ) {
        if ( QMessageBox::question( this, "Confirm", "Really create a new map?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) {
            return;
        }
    }

    // show the map generation dialog
    GeneratorDialog generatorDialog;
    if ( generatorDialog.exec() == QDialog::Rejected ) {
        qDebug() << "EditorMainWindow::newMap: rejected";
        return;
    }

    if ( map ) {
        delete map;
        map = 0;
    }

    // clear some stuff
    selection->deselect();
    selection->setTerrain( 0 );
    allUnits.clear();
    allTerrains.clear();
    allObjectives.clear();
    allHouses.clear();

    navigationGrid.clear();
//    if ( navigationGrid ) {
//        delete [] navigationGrid;
//        navigationGrid = 0;
//    }

    // get the new map
    map = generatorDialog.getMap();

    takeNewMapIntoUse();

    ui->m_width->setNum( map->getWidth() );
    ui->m_height->setNum( map->getHeight() );
    ui->m_time->setTime( QTime( 12, 0 ) );
    ui->m_title->clear();
    ui->m_description->clear();
    ui->m_tutorial->setChecked( false );

    statusBar()->showMessage( "Created a new map", 3000 );

    // start the server if needed
    if ( ! m_server->isStarted() ) {
        m_server->start( 45001 );
    }
}


void EditorMainWindow::openMap () {
    if ( map != 0 ) {
        if ( QMessageBox::question( this, "Confirm", "Really open a map?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) {
            return;
        }

        delete map;
        map = 0;
    }

    QString name = QFileDialog::getOpenFileName( this, "Open Map", "/Users/chakie/Prog/Triangularizer/Scenarios/" );
    if ( name == "" ) {
        return;
    }

    // clear some stuff
    selection->deselect();
    allUnits.clear();
    allTerrains.clear();
    allObjectives.clear();
    allHouses.clear();

    navigationGrid.clear();

    ui->m_tutorial->setChecked( false );

    map = Serializer().loadMap( name, this );

    if ( map == 0 ) {
        return;
    }

    takeNewMapIntoUse();

    // start the server if needed
    if ( ! m_server->isStarted() ) {
        m_server->start( 45001 );
    }

    statusBar()->showMessage( QString("Loaded map: ") + map->m_name, 3000 );
}


void EditorMainWindow::saveMap () {
    if ( ! map ) {
        return;
    }

    if ( map->m_name == "" ) {
        saveMapAs();
    }
    else {
        // first generate the navigation grid
        generateNavigation();

        Serializer().saveMap( map, this );
        statusBar()->showMessage( QString("Saved map: ") + map->m_name, 3000 );
    }
}


void EditorMainWindow::saveMapAs () {
    if ( ! map ) {
        return;
    }

    QString name = QFileDialog::getSaveFileName( this, "Save Map As" );
    if ( name == "" ) {
        return;
    }

    map->m_name = name;

    // first generate the navigation grid
    generateNavigation();

    Serializer().saveMap( map, this );
}


void EditorMainWindow::sendMapToIpad (QTcpSocket * ipad) {
    // first generate the navigation grid
    generateNavigation();

    // serialize to the ipad
    Serializer().sendMapToIpad( ipad, this );
}


void EditorMainWindow::editModeChanged () {
    if ( ui->m_edit_action->isChecked() ) {
        editorMode = kEdit;
        ui->m_stack->setCurrentIndex( EditorMainWindow::ScenarioPage );
    }
    else if ( ui->m_terrain_action->isChecked() ) {
        editorMode = kAddTerrain;
        statusBar()->showMessage( "Click points to create terrain, click Done or hit [Esc] to finish", 5000 );
        ui->m_stack->setCurrentIndex( EditorMainWindow::TerrainPage );
    }
    else if ( ui->m_house_action->isChecked() ) {
        editorMode = kAddHouse;
        ui->m_stack->setCurrentIndex( EditorMainWindow::HousePage );
    }
    else if ( ui->m_unit_action->isChecked() ) {
        editorMode = kAddUnit;
        ui->m_stack->setCurrentIndex( EditorMainWindow::UnitPage );
    }
    else if ( ui->m_objective_action->isChecked() ) {
        editorMode = kAddObjective;
        ui->m_stack->setCurrentIndex( EditorMainWindow::ObjectivePage );
    }
    else if ( ui->m_road_action->isChecked() ) {
        editorMode = kAddRoad;
        statusBar()->showMessage( "Click points to create road, [Esc] to finish", 5000 );
        ui->m_stack->setCurrentIndex( EditorMainWindow::TerrainPage );
    }

    selection->deselect();
}


void EditorMainWindow::selectedTerrainChanged (Terrain * terrain) {
    if ( terrain == 0 ) {
        ui->m_stack->setCurrentIndex( EditorMainWindow::ScenarioPage );
        ui->m_bound_size->clear();
    }
    else {
        ui->m_stack->setCurrentIndex( EditorMainWindow::TerrainPage );
        ui->m_terrain_type->setCurrentIndex( terrain->m_type );
        ui->m_bound_size->setText( QString( "%1 x %2 m").arg( terrain->boundingRect().size().width()).arg( terrain->boundingRect().size().height()) );
    }
}


void EditorMainWindow::deleteSelectedItem () {
    QList<QGraphicsItem *> selected = map->selectedItems();

    if ( selected.size() == 0 ) {
        return;
    }

    Terrain * terrain = dynamic_cast<Terrain *>( selected.first() );
    if ( terrain ) {
        allTerrains.removeAll( terrain );
        selection->setTerrain( 0 );
        delete terrain;
        return;
    }

    Unit * unit = dynamic_cast<Unit *>( selected.first() );
    if ( unit ) {
        allUnits.removeAll( unit );
        selection->setUnit( 0 );
        delete unit;
        return;
    }

    Objective * objective = dynamic_cast<Objective *>( selected.first() );
    if ( objective ) {
        allObjectives.removeAll( objective );
        selection->setObjective( 0 );
        delete objective;
        return;
    }

    House * house = dynamic_cast<House *>( selected.first() );
    if ( house ) {
        allHouses.removeAll( house );
        selection->setHouse( 0 );
        delete house;
        return;
    }

    Dot * dot = dynamic_cast<Dot *>( selected.first() );
    if ( dot ) {
        dot->getTerrain()->removeDot( dot );
        delete dot;
        qDebug() << "EditorMainWindow::addPoint: dot!";
    }

}


void EditorMainWindow::terrainTypeChanged () {
    Terrain * selected = selection->getSelectedTerrain();
    if ( ! selected ) {
        return;
    }

    selected->setType( (TerrainType)ui->m_terrain_type->currentIndex() );
}


void EditorMainWindow::terrainDone () {
    selection->deselect();
    map->clearSelection();
}


void EditorMainWindow::addPoint () {
    QList<QGraphicsItem *> selected = map->selectedItems();

    if ( selected.size() == 0 ) {
        return;
    }

    Dot * dot = dynamic_cast<Dot *>( selected.first() );
    if ( dot ) {
        dot->getTerrain()->addPointAfter( dot );
        qDebug() << "EditorMainWindow::addPoint: dot!";
    }
}


void EditorMainWindow::rotateTerrain () {
    bool cw;
    if ( sender() == ui->m_rotate_right ) {
        cw = true;
    }
    else if ( sender() == ui->m_rotate_left ) {
        cw = false;
    }
    else {
        return;
    }

    Terrain * selected = selection->getSelectedTerrain();
    if ( ! selected ) {
        return;
    }

    selected->setTransformOriginPoint( selected->boundingRect().center() );
    selected->setRotation( selected->rotation() + 15 * (cw ? 1 : -1 ) );
}


void EditorMainWindow::duplicateTerrain () {
    Terrain * selected = selection->getSelectedTerrain();
    if ( ! selected ) {
        return;
    }

    Terrain * terrain = new Terrain( selected->polygon().translated( 20, 20 ) );
    terrain->setType( selected->m_type );
    map->addItem( terrain );
    allTerrains << terrain;

    map->clearSelection();
    selection->setTerrain( terrain );
    terrain->setSelected( true );
}


void EditorMainWindow::flipMapHorizontally () {
    foreach (Terrain * terrain, allTerrains ) {
        terrain->flipHorizontally();
    }

    foreach (Objective * objective, allObjectives ) {
        objective->setPos( map->getWidth() - objective->pos().x() - objective->boundingRect().width(), objective->pos().y() );
    }

    foreach (House * house, allHouses ) {
        house->setPos( map->getWidth() - house->pos().x() - house->boundingRect().width(), house->pos().y() );
        float old = house->rotation();
        float rotation = 180 + house->rotation();
        if ( rotation > 360 ) {
            rotation -= 360;
        }
        else if ( rotation < 0 ) {
            rotation += 360;
        }
        house->setTransform(QTransform::fromScale(-1, 1), true );//setRotation( rotation );
    }
}


void EditorMainWindow::flipMapVertically () {
    foreach (Terrain * terrain, allTerrains ) {
        terrain->flipVertically();
    }

    foreach (Objective * objective, allObjectives ) {
        objective->setPos( objective->pos().x(), map->getHeight() - objective->pos().y() - objective->boundingRect().height() );
    }

    foreach (House * house, allHouses ) {
        house->setPos( house->pos().x(), map->getHeight() - house->pos().y() - house->boundingRect().height() );
        float old = house->rotation();
        float rotation = 180 - house->rotation();
        if ( rotation > 360 ) {
            rotation -= 360;
        }
        else if ( rotation < 0 ) {
            rotation += 360;
        }

        house->setTransform(QTransform::fromScale(1, -1), true );//setRotation( rotation );
    }
}


void EditorMainWindow::zoomIn () {
    ui->m_view->scale( 1.1, 1.1 );
}


void EditorMainWindow::zoomOut () {
    ui->m_view->scale( 0.9, 0.9 );
}


void EditorMainWindow::zoomNormal () {
    ui->m_view->setTransform( QTransform::fromScale( 1.0f, 1.0f ));
}


void EditorMainWindow::deselect () {
    selection->deselect();
    map->clearSelection();

    editorMode = kEdit;
    ui->m_edit_action->setChecked( true );

    ui->m_stack->setCurrentIndex( EditorMainWindow::ScenarioPage );
}


void EditorMainWindow::takeNewMapIntoUse () {
    ui->m_view->setScene( map );

    connect( map, SIGNAL(unitAdded(Unit*)), SLOT( unitAdded(Unit *)) );

    // now we can save
    ui->m_save_action->setEnabled( true );
    ui->m_save_as_action->setEnabled( true );

    // enable toolbar and tab
    ui->m_stack->setEnabled( true );
    ui->m_toolbar->setEnabled( true );

    ui->m_width->setEnabled( true );
    ui->m_height->setEnabled( true );
    ui->m_time->setEnabled( true );
    ui->m_length->setEnabled( true );
    ui->m_tutorial->setEnabled( true );
    ui->m_view->setEnabled( true );
    ui->m_zoom_in->setEnabled( true );
    ui->m_zoom_normal->setEnabled( true );
    ui->m_zoom_out->setEnabled( true );

    ui->m_hq->clear();
    ui->m_hq->addItem( "No HQ", -1 );

    foreach ( Unit * unit, allUnits ) {
        ui->m_hq->addItem( unit->m_name, unit->m_id );
    }

    updateUnitStats();
}
