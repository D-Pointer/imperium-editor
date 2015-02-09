#include <QMessageBox>
#include <QTcpSocket>
#include <QDebug>

#include "NetworkServer.hpp"

NetworkServer::NetworkServer (QObject *parent) : QObject(parent), m_server(0) {
    // create the server
    m_server = new QTcpServer( this );

    connect( m_server, SIGNAL(newConnection()), this, SLOT(iPadConnected()) );
}


void NetworkServer::start (qint16 port) {
    // don't do twice
    if ( m_server->isListening() ) {
        qDebug() << "NetworkServer::start: already listening";
        return;
    }

    if ( ! m_server->listen( QHostAddress::Any, port ) ) {
        QMessageBox::critical( 0, "Server error", "Failed to listen on port " + QString::number( port ) + "\n"
                "Error: " + m_server->errorString() );
        return;
    }

    qDebug() << "NetworkServer::start: server started ok";
}


void NetworkServer::iPadConnected () {
    // get next connection
    QTcpSocket * connection = m_server->nextPendingConnection();

    qDebug() << "NetworkServer::iPadConnected: client connected:" << connection->peerName();

    // have someone else send the map
    emit sendMapToIpad( connection );

    // flush and close
    connection->flush();
    connection->disconnectFromHost();

    // nuke it later
    connect( connection, SIGNAL(disconnected()), connection, SLOT(deleteLater()) );

    qDebug() << "NetworkServer::iPadConnected: scenario sent, connection closed";
}
