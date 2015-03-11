#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QMatrix>
#include <QImage>
#include <QtAlgorithms>
#include <QTcpSocket>
#include <QDebug>

#include "Serializer.hpp"
#include "Map.hpp"
#include "Globals.hpp"
#include "Selection.hpp"
#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include "Validator.hpp"
#include "VictoryConditions.hpp"

void Serializer::saveMap (Map * map, EditorMainWindow * editor) {
    qDebug() << "Serializer::saveMap: saving to:" << map->m_name;

   QFile file( map->m_name );
    if ( ! file.open( QIODevice::WriteOnly |QIODevice::Truncate ) ) {
        // failed
        QMessageBox::warning( 0, "Save Failed", "Failed to save the map: " + file.errorString(), QMessageBox::Ok );
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // save the real map to a stream
    QTextStream stream( &file );
    saveMapToStream( stream, editor );
}


void Serializer::sendMapToIpad (QTcpSocket * ipad, EditorMainWindow * editor) {
    qDebug() << "Serializer::sendMapToIpad: saving to socket";

    // first check if it is valid
    if ( ! Validator().validate( editor )) {
        qDebug() << "Serializer::sendMapToIpad: scenario not valid, but sending anyway";
    }
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // save the real map to a stream
    QTextStream stream( ipad );
    saveMapToStream( stream, editor );
}


void Serializer::saveMapToStream (QTextStream & stream, EditorMainWindow * editor) {
    float height = map->getHeight();

    // map metadata
    stream << "size "     << map->getWidth() << " " << map->getHeight() << endl
           << "id "       << editor->ui->m_id->value() << endl
           << "depend "   << editor->ui->m_depends->value() << endl
           << "time "     << editor->ui->m_time->time().hour() << " " << editor->ui->m_time->time().minute() << endl
           << "tutorial " << (editor->ui->m_tutorial->isChecked() ? "1" : "0") << endl
           << "aihint "   << editor->ui->m_aiHint->currentIndex() << endl
           << "battlesize " << editor->ui->m_battleSize->currentIndex() << endl
           << "title "    << editor->ui->m_title->text() << endl
           << "desc "     << editor->ui->m_description->toPlainText().replace( "\n", "|") << endl;

    // all victory conditions
    foreach ( VictoryCondition * condition, allVictoryConditions ) {
        stream << "victory " << condition->toString() << endl;
    }


    // sort all terrains according to z order first
    qSort( allTerrains.begin(), allTerrains.end(), terrainComparator );

    foreach ( Terrain * terrain, allTerrains ) {
        QPointF terrain_pos = terrain->pos();
        QPolygonF polygon = terrain->polygon();

        // is the terrain valid?
        if ( polygon.size() == 0 ) {
            qWarning() << "Serializer::saveMapToStream: terrain with no points, skipping it";
            continue;
        }

        QPointF pos = polygon.boundingRect().center();
        polygon.translate( -pos );
        polygon = QMatrix().rotate( terrain->rotation() ).map( polygon );
        polygon.translate( pos );

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
               << " " << unit->m_hq_id
               << " " << unit->m_men
               << " " << unit->m_weapon
               << " " << unit->m_mode
               << " " << unit->m_experience
               << " " << unit->m_ammo
               << " " << unit->m_name << endl;
    }

    // the navigation grid is saved as one single long line
    stream << "navgrid";

    // loop the entire map
    for ( int index = 0; index < navigationGrid.size(); ++index ) {
        stream << " ";
        if ( navigationGrid[ index ] != 0 ) {
            stream << navigationGrid[ index ]->m_type;
        }
        else {
            stream << kGrass;
        }
    }

    stream << endl;

    // write the final "end"
    stream << "end" << endl;

    QApplication::restoreOverrideCursor();
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
    editor->ui->m_battleSize->setCurrentIndex( kMediumBattle );

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
            editor->ui->m_size->setText( QString::number( width ) + " x " + QString::number( height ));
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

        else if ( type == "victory" ) {
            QString type = parts.takeFirst();

            if ( type == TimeBased::id() ) {
                allVictoryConditions << new TimeBased( parts.takeFirst().toInt() );
            }

            if ( type == CasualtyBased::id() ) {
                allVictoryConditions << new CasualtyBased( parts.takeFirst().toInt() );
            }
        }

        else if ( type == "tutorial" ) {
            editor->ui->m_tutorial->setChecked( parts.takeFirst().toInt() == 1 );
        }

        else if ( type == "aihint" ) {
            editor->ui->m_aiHint->setCurrentIndex( parts.takeFirst().toInt() );
        }

        else if ( type == "battlesize" ) {
            editor->ui->m_battleSize->setCurrentIndex( parts.takeFirst().toInt() );
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

            unit->m_hq_id      = parts.takeFirst().toInt();
            unit->m_men        = parts.takeFirst().toInt();
            unit->m_weapon     = (WeaponType)parts.takeFirst().toInt();
            unit->m_mode       = (UnitMode)parts.takeFirst().toInt();
            unit->m_experience = (ExperienceType)parts.takeFirst().toInt();
            unit->m_ammo       = parts.takeFirst().toInt();
            unit->m_name       = parts.join( " " );

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

        else if ( type == "navgrid" ) {
            qDebug() << "Serializer::loadMap: ignoring navigation grid";
        }
        else if ( type == "end" ) {
            qDebug() << "Serializer::loadMap: ignoring navigation grid";
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

    float treeX, treeY;

    // woods has more trees
    int delta = terrain->m_type == kWoods ? 18 : 25;
    int offset = terrain->m_type == kWoods ? 5 : 5;

    // trees are 30 px wide and high
    //const int treeSize = 30;

    float skipValue = terrain->m_type == kWoods ? 0.05 : 0.1;

    int generated = 0;

    for ( int y = terrain->boundingRect().y(); y <= terrain->boundingRect().y() + terrain->boundingRect().height(); y += delta ) {
        for ( int x = terrain->boundingRect().x(); x <= terrain->boundingRect().x() + terrain->boundingRect().width(); x += delta ) {
             // randomly skip trees
            if ( ((float)rand() / RAND_MAX) < skipValue ) {
                continue;
            }

            // how many times do we try to randomly find a suitable tree?
            int tries = terrain->m_type == kWoods ? 50 : 5;

            // try harder to find a suitable tree
            for ( int index = 0; index < tries; ++index ) {
                // a random tree type
                int tree = rand() % 5;

                // scale and rotate a bit randomly
                float scale = 0.3 + ((float)rand() / RAND_MAX) * 0.8;
                float rotation = 20 + ((float)rand() / RAND_MAX) * 40;

                //float radius = (treeSize * scale) / 2.0f;

                // offset the positions a bit
                treeX = x - offset + ((float)rand() / RAND_MAX) * offset * 2;
                treeY = y - offset + ((float)rand() / RAND_MAX) * offset * 2;

                // is the position inside the polygon? test 4 positions: above, below, left and right
                if ( terrain->contains( QPoint( treeX, treeY ) ) ) {
//                    if ( terrain->contains( QPoint( treeX - radius, treeY ) ) && terrain->contains( QPoint( treeX + radius, treeY ) ) &&
//                         terrain->contains( QPoint( treeX, treeY - radius ) ) && terrain->contains( QPoint( treeX, treeY + radius ) ) ) {
                    // save a tree
                    stream << " " << tree << " " << (int)treeX << " " << (int)toSave( treeY, mapHeight ) << " " << scale << " " << (int)rotation;
                    generated++;
                    break;
                }
            }
        }
    }

    // did we generate at least a single tree?
    if ( generated == 0 ) {
        qWarning() << "Serializer::generateTrees: terrain polygon contains no trees, adding one at the middle";

        // no, so add one at the center of the polygon
        QPointF center = terrain->boundingRect().center();

        // a random tree type
        int tree = rand() % 5;

        // scale and rotate a bit randomly
        float scale = 0.3 + ((float)rand() / RAND_MAX) * 0.8;
        float rotation = 20 + ((float)rand() / RAND_MAX) * 40;

        stream << " " << tree << " " << (int)center.x() << " " << (int)toSave( center.y(), mapHeight ) << " " << scale << " " << (int)rotation;
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

    int generated = 0;

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

    // did we generate at least a single rock?
    if ( generated == 0 ) {
        qWarning() << "Serializer::generateRocks: terrain polygon contains no rocks, adding one at the middle";

        // no, so add one at the center of the polygon
        QPointF center = terrain->boundingRect().center();

        // a random rock
       int rock = rand() % 6;

        // scale and rotate a bit randomly
        float scale = 0.8 + ((float)rand() / RAND_MAX) * 0.2;
        float rotation = 10 + ((float)rand() / RAND_MAX) * 20;

        stream << " " << rock << " " << (int)center.x() << " " << (int)toSave( center.y(), mapHeight ) << " " << scale << " " << (int)rotation;
    }

    stream << endl;
}


//void Serializer::saveNavigationGrid () {
//    // change the extension for the navigation grid
//    QString filename = map->m_name;
//    filename.replace( ".map", ".nav" );

//    qDebug() << "Serializer::saveNavigationGrid: saving to:" << filename;

//    if ( filename == map->m_name ) {
//        QMessageBox::warning( 0, "Save Failed", "Failed to save the navigation grid: same name", QMessageBox::Ok );
//        return;
//    }

//    QFile file( filename );
//    if ( ! file.open( QIODevice::WriteOnly |QIODevice::Truncate ) ) {
//        // failed
//        QMessageBox::warning( 0, "Save Failed", "Failed to save the navigation grid: " + file.errorString(), QMessageBox::Ok );
//        return;
//    }

//    int gridWidth = map->getWidth() / navigationTileSize;
//    int gridHeight = map->getHeight() / navigationTileSize;

//    unsigned char data[ gridWidth * gridHeight ];

//    // loop the entire map
//    for ( int index = 0; index < gridWidth * gridHeight; ++index ) {
//        if ( navigationGrid[ index ] != 0 ) {
//            data[ index ] = (unsigned char)navigationGrid[ index ]->m_type;
//        }
//        else {
//            data[ index ] = (unsigned char)kGrass;
//        }
//    }

//    // save it all
//    file.write( (const char *)data, gridWidth * gridHeight );
//    file.close();
//}


/*void Serializer::saveHeightmap () {
    // change the extension for the height map
    QString filename = map->m_name;
    filename.replace( ".map", "-default.png" );
    qDebug() << "Serializer::saveNavigationGrid: saving to:" << filename;

    QFile file( filename );
    if ( ! file.open( QIODevice::WriteOnly |QIODevice::Truncate ) ) {
        // failed
        QMessageBox::warning( 0, "Save Failed", "Failed to save the heightmap: " + file.errorString(), QMessageBox::Ok );
        return;
    }

    // heightma filled with a default height
    QImage heightmap( QSize( map->getWidth() / 2, map->getHeight() / 2 ), QImage::Format_Indexed8 );
    heightmap.fill( 128 );

    // set a grayscale indexed color table
    QVector<QRgb> table( 256 );
    for( int i = 0; i < 256; ++i ) {
        table[i] = qRgb(i,i,i);
    }
    heightmap.setColorTable( table );

    // hide all items
    foreach ( QGraphicsItem * item, map->items() ) {
        item->hide();
    }

    // hide all terrains that are not water
    foreach ( Terrain * terrain, allTerrains ) {
        if ( terrain->m_type == kRiver ) {
            terrain->show();

            foreach ( Dot * dot, terrain->m_dots ) {
                dot->hide();
            }
        }
    }

    // grab an lower resolution screenshot
    QImage source( QSize( map->getWidth() / 2, map->getHeight() / 2 ), QImage::Format_RGB888 );
    QPainter painter( &source );
    map->render( &painter, source.rect(), QRectF( 0, 0, map->getWidth(), map->getHeight() ) );

    // color for the water
    QRgb waterColor = Terrain::getColor( kRiver ).rgb();

    // what to save in the heightmap for water
    int waterHeight = qGray( qRgb( 0, 0, 0) );

    for(int i = 0; i < source.width(); i++) {
        for(int j = 0; j< source.height(); j++) {
            if ( source.pixel( i,j ) == waterColor ) {
                heightmap.setPixel( i,j, waterHeight ); //qGray( source.pixel(i,j) ));
            }
        }
    }

    heightmap.save( &file, "PNG" );

    // show all terrain again
    foreach ( Terrain * terrain, allTerrains ) {
        terrain->show();

        foreach ( Dot * dot, terrain->m_dots ) {
            dot->show();
        }
    }

    foreach ( QGraphicsItem * item, map->items() ) {
        item->show();
    }
}*/
