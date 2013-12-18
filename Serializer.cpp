#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QMatrix>
#include <QtAlgorithms>
#include <QDebug>

#include "Serializer.hpp"
#include "Map.hpp"
#include "Globals.hpp"
#include "Selection.hpp"
#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"

void Serializer::saveMap (Map * map, EditorMainWindow * editor) {
    qDebug() << "Serializer::saveMap: saving to:" << map->m_name;

    QFile file( map->m_name );
    if ( ! file.open( QIODevice::WriteOnly |QIODevice::Truncate ) ) {
        // failed
        QMessageBox::warning( 0, "Save Failed", "Failed to save the map: " + file.errorString(), QMessageBox::Ok );
        return;
    }

    QTextStream stream( &file );

    float height = map->getHeight();

    // map metadata
    stream << "size "     << map->getWidth() << " " << map->getHeight() << endl
           << "id "       << editor->ui->m_id->value() << endl
           << "depend "   << editor->ui->m_depends->value() << endl
           << "time "     << editor->ui->m_time->time().hour() << " " << editor->ui->m_time->time().minute() << endl
           << "length "   << editor->ui->m_length->value() << endl
           << "tutorial " << (editor->ui->m_tutorial->isChecked() ? "1" : "0") << endl
           << "aihint "   << editor->ui->m_aiHint->currentIndex() << endl
           << "title "    << editor->ui->m_title->text() << endl
           << "desc "     << editor->ui->m_description->toPlainText().replace( "\n", "|") << endl;

    // sort all terrains according to z order first
    qSort( allTerrains.begin(), allTerrains.end(), terrainComparator );

    foreach ( Terrain * terrain, allTerrains ) {
        QPointF terrain_pos = terrain->pos();
        QPolygonF polygon = terrain->polygon();

        QPointF pos = polygon.boundingRect().center();
        polygon.translate( -pos );
        polygon = QMatrix().rotate( terrain->rotation() ).map( polygon );
        polygon.translate( pos );

        stream << "terrain " << terrain->m_type;
        foreach ( QPointF point_pos, polygon ) {
            point_pos += terrain_pos;
            stream << " " << (int)point_pos.x() << " " << (int)toSave( point_pos.y(), height );
        }
        stream << endl;

        // scattered trees or woods?
        if ( terrain->m_type == kScatteredTrees || terrain->m_type == kWoods ) {
            // generate some random trees
            generateTrees( terrain, stream, height );
        }

        // rocks?
        else if ( terrain->m_type == kRocky ) {
            // generate some random rocks
            generateRocks( terrain, stream, height );
        }
    }

    foreach ( Objective * objective, allObjectives ) {
        QPointF objective_pos = objective->pos() + objective->boundingRect().center();
        stream << "objective " << objective->m_id << " "
               << (int)objective_pos.x() << " " << (int)toSave( objective_pos.y(), height ) << " "
               << objective->m_title << endl;
    }

    foreach ( House * house, allHouses ) {
        QPointF house_pos = house->pos() + house->boundingRect().center();
        stream << "house " << house->getType() << " "
               << (int)house_pos.x() << " " << (int)toSave( house_pos.y(), height ) << " "
               << house->rotation() << endl;
    }

    foreach ( Unit * unit, allUnits ) {
        QPointF unit_pos = unit->pos() + unit->boundingRect().center();
        stream << "unit " << unit->m_id << " " << unit->m_owner << " " << unit->m_type << " "
               << (int)unit_pos.x() << " " << (int)toSave( unit_pos.y(), height )
               << " " << unit->rotation()
               << " " << unit->m_hq_id << " " << unit->m_men << " " << unit->m_guns << " " << unit->m_mode << " "
               << unit->m_name << endl;
    }
}


Map * Serializer::loadMap (const QString & filename, EditorMainWindow * editor) {
    qDebug() << "Serializer::loadMap: reading from:" << filename;

    QFile file( filename );
    if ( ! file.open( QIODevice::ReadOnly ) ) {
        // failed
        QMessageBox::warning( 0, "Open Failed", "Failed to open the map: " + file.errorString(), QMessageBox::Ok );
        return 0;
    }

    QString line, type;
    int height;

    // set some defaults
    editor->ui->m_aiHint->setCurrentIndex( kMeeting );

    // create a new map
    Map * map = new Map;
    map->m_name = filename;

    QTextStream stream( &file );

    while ( ! stream.atEnd() ) {
        line = stream.readLine();

        // split into parts
        QStringList parts = line.split( " ", QString::SkipEmptyParts );
        if ( parts.size() == 0 ) {
            continue;
        }

        type = parts.takeFirst();

        if ( type == "size" ) {
            int width  = parts.takeFirst().toInt();
            height = parts.takeFirst().toInt();
            map->setSize( width, height );
            editor->ui->m_width->setValue( width );
            editor->ui->m_height->setValue( height );
        }

        else if ( type == "id" ) {
            editor->ui->m_id->setValue( parts.takeFirst().toInt() );
        }

        else if ( type == "depend" ) {
            editor->ui->m_depends->setValue( parts.takeFirst().toInt() );
        }

        else if ( type == "time" ) {
            int hours   = parts.takeFirst().toInt();
            int minutes = parts.takeFirst().toInt();
            editor->ui->m_time->setTime( QTime( hours, minutes ) );
        }

        else if ( type == "length" ) {
            editor->ui->m_length->setValue( parts.takeFirst().toInt() );
        }

        else if ( type == "tutorial" ) {
            editor->ui->m_tutorial->setChecked( parts.takeFirst().toInt() == 1 );
        }

        else if ( type == "aihint" ) {
            editor->ui->m_aiHint->setCurrentIndex( parts.takeFirst().toInt() );
        }

        else if ( type == "title" ) {
            editor->ui->m_title->setText( parts.join( " " ) );
        }

        else if ( type == "desc" ) {
            editor->ui->m_description->setPlainText( parts.join( " " ).replace( "|", "\n" ) );
        }

        else if ( type == "terrain" ) {
            Terrain * terrain = new Terrain;
            terrain->setType( (TerrainType)parts.takeFirst().toInt() );
            map->addItem( terrain );
            allTerrains << terrain;

            while ( ! parts.isEmpty() ) {
                float x = parts.takeFirst().toFloat();
                float y = fromSave( parts.takeFirst().toFloat(), height );
                terrain->addPoint( QPointF( x, y ) );
            }
        }

        else if ( type == "unit" ) {
            int id         = parts.takeFirst().toInt();
            Player owner   = (Player)parts.takeFirst().toInt();
            UnitType type  = (UnitType)parts.takeFirst().toInt();
            float x        = parts.takeFirst().toFloat();
            float y        = fromSave( parts.takeFirst().toFloat(), height );
            Unit * unit = new Unit( id, owner, type, QPointF( x, y ) );

            float rotation = parts.takeFirst().toFloat();

            unit->setRotation( rotation );

            unit->m_hq_id    = parts.takeFirst().toInt();
            unit->m_men      = parts.takeFirst().toInt();
            unit->m_guns     = parts.takeFirst().toInt();
            unit->m_mode     = (UnitMode)parts.takeFirst().toInt();
            unit->m_name     = parts.join( " " );

            unit->setupIcon();
            map->addItem( unit );
            allUnits << unit;
        }

        else if ( type == "objective" ) {
            int id         = parts.takeFirst().toInt();
            float x        = parts.takeFirst().toFloat();
            float y        = fromSave( parts.takeFirst().toFloat(), height );
            QString title  = parts.join( " " );
            Objective * objective = new Objective( id, title, QPointF( x, y ) );

            // fix up the position
            QPointF new_pos = objective->pos() - objective->boundingRect().center();
            objective->setPos( new_pos );

            map->addItem( objective );
            allObjectives << objective;
        }

        else if ( type == "house" ) {
            int type       = parts.takeFirst().toInt();
            float x        = parts.takeFirst().toFloat();
            float y        = fromSave( parts.takeFirst().toFloat(), height );
            float rotation = parts.takeFirst().toFloat();
            House * house = new House( QPointF( x, y ), type );

            // fix up the position
            QPointF new_pos = house->pos() - house->boundingRect().center();
            house->setPos( new_pos );
            house->setRotation( rotation );

            map->addItem( house );
            allHouses << house;
        }
    }

    return map;
}


float Serializer::fromSave (float value, int height) {
    return -(value - height);
}


float Serializer::toSave (float value, int height) {
    return -value + height;
}


bool Serializer::terrainComparator (const Terrain * t1, const Terrain * t2) {
    return t1->zValue() < t2->zValue();
}


void Serializer::generateTrees (Terrain *terrain, QTextStream &stream, float mapHeight) {
    stream << "trees";

    // woods has more trees
    int delta = terrain->m_type == kWoods ? 18 : 25;
    int offset = terrain->m_type == kWoods ? 5 : 5;

    // margin to the edge of the polygon
    int marginLeft = 12;
    int marginRight = 8;
    int marginTop = 8;
    int marginBottom = 12;

    for ( int y = terrain->boundingRect().y() + marginBottom; y <= terrain->boundingRect().y() + terrain->boundingRect().height() - marginTop - marginBottom; y += delta ) {
        for ( int x = terrain->boundingRect().x() + marginLeft; x <= terrain->boundingRect().x() + terrain->boundingRect().width() - marginRight - marginLeft; x += delta ) {
            // randomly skip trees
            if ( ((float)rand() / RAND_MAX) < 0.1 ) {
                continue;
            }

            // a random tree
            int tree = rand() % 5;

            // offset the positions a bit
            float treeX = x - offset + ((float)rand() / RAND_MAX) * offset * 2;
            float treeY = y - offset + ((float)rand() / RAND_MAX) * offset * 2;

            // is the position inside the polygon?
            if ( ! terrain->contains( QPoint( treeX, treeY ) ) ) {
                continue;
            }

            // scale and rotate a bit randomly
            float scale = 0.5 + ((float)rand() / RAND_MAX) * 0.5;
            float rotation = 10 + ((float)rand() / RAND_MAX) * 20;

            stream << " " << tree << " " << (int)treeX << " " << (int)toSave( treeY, mapHeight ) << " " << scale << " " << (int)rotation;
        }
    }

    stream << endl;
}


void Serializer::generateRocks (Terrain *terrain, QTextStream &stream, float mapHeight) {
    stream << "rocks";

    // woods has more trees
    int delta = 10;
    int offset = 4;

    // margin to the edge of the polygon
    int marginLeft = 5;
    int marginRight = 5;
    int marginTop = 5;
    int marginBottom = 5;

    for ( int y = terrain->boundingRect().y() + marginBottom; y <= terrain->boundingRect().y() + terrain->boundingRect().height() - marginTop - marginBottom; y += delta ) {
        for ( int x = terrain->boundingRect().x() + marginLeft; x <= terrain->boundingRect().x() + terrain->boundingRect().width() - marginRight - marginLeft; x += delta ) {
             // a random rock
            int rock = rand() % 6;

            // offset the positions a bit
            float rockX = x - offset + ((float)rand() / RAND_MAX) * offset * 2;
            float rockY = y - offset + ((float)rand() / RAND_MAX) * offset * 2;

            // is the position inside the polygon?
            if ( ! terrain->contains( QPoint( rockX, rockY ) ) ) {
                continue;
            }

            // scale and rotate a bit randomly
            float scale = 0.8 + ((float)rand() / RAND_MAX) * 0.2;
            float rotation = 10 + ((float)rand() / RAND_MAX) * 20;

            stream << " " << rock << " " << (int)rockX << " " << (int)toSave( rockY, mapHeight ) << " " << scale << " " << (int)rotation;
        }
    }

    stream << endl;
}

