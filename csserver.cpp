#include "csserver.h"
#include <QString>

csserver::csserver()
{
    _csocket = new QTcpSocket();
    _return = new QString();
}
csserver::~csserver()
{
    delete _csocket;
    delete _return;
}

bool csserver::ServerConnect(int port, QString ip,QString rcon)
{
    _csocket->connectToHost(ip,port);
    if(_csocket->waitForConnected(1000))
    {
        _csocket->write(makePacket(3,rcon));
        _csocket->waitForBytesWritten(100000);
        _csocket->waitForReadyRead(1000);
        if(_csocket->bytesAvailable() > 0)
        {
            return true;
        }
        else
        {
            _return->clear();
            _return->append(_csocket->readAll().toHex());

            qDebug() << *_return;
        }
    }
    return false;

}

bool csserver::sendCommand(QString command)
{
    _csocket->write(makePacket(2,command));
    _csocket->waitForBytesWritten(10000);

    if(_csocket->bytesAvailable() == 0)
    {

        return false;
    }
    _return->clear();
    _return->append(_csocket->readAll().toHex());

    qDebug() << *_return;
    return true;
}

QByteArray csserver::makePacket(int type, QString command)
{
    int size = 13 + command.length() + 1;
    QByteArray Packet;
    Packet.resize(size);
    Packet[0] = command.length() + 9;
    Packet[4] = 0;
    Packet[8] = type;
    for(int i = 0; i < command.length(); i++)
    {
       Packet.insert(12+i,QString(command[i]));
    }
    return Packet;

}
