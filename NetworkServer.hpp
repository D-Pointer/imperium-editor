#ifndef NETWORKSERVER_HPP
#define NETWORKSERVER_HPP

#include <QObject>
#include <QTcpServer>

class NetworkServer : public QObject {

    Q_OBJECT

public:

    explicit NetworkServer (QObject *parent = 0);

    void start (qint16 port);

    bool isStarted () const {
        return m_server->isListening();
    }


signals:

    void sendMapToIpad (QTcpSocket * ipad);


private slots:

    void iPadConnected ();


private:

    // the main server
    QTcpServer * m_server;

};

#endif // NETWORKSERVER_HPP
