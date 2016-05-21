#ifndef CSSERVER_H
#define CSSERVER_H

#include <QTcpSocket>
#include <QByteArray>

class csserver
{
public:
    explicit csserver();
    virtual ~csserver();
    QTcpSocket *_csocket;
    QString *_return;
    bool ServerConnect(int port, QString ip, QString rcon);
    bool sendCommand(QString command);
    QByteArray makePacket(int type, QString command,bool rcon);

signals:

public slots:

};

#endif // CSSERVER_H
