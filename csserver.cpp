#include "csserver.h"
#include <QString>

csserver::csserver()
{
    _csocket = new QTcpSocket();
    _return = new QString();
}
bool csserver::ServerConnect(int port, QString ip,QString rcon)
{
    _csocket->connectToHost(ip,port);
    if(_csocket->waitForConnected(1000))
    {
        _csocket->write(makePacket(3,rcon,true));
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
    _csocket->write(makePacket(2,command,false));
    _csocket->waitForBytesWritten(10000);
    if(_csocket->bytesAvailable() == 0)
    {

        return false;
    }
    else
    {
        _return->clear();
        _return->append(_csocket->readAll().toHex());

        qDebug() << *_return;
        return true;
    }
}

QByteArray csserver::makePacket(int type, QString command, bool rcon) // rewrite..
{
    // Don't ever bother to ask, done with wireshark analyze
    QByteArray temp;
    QString a;
    int c;
    if(rcon)
    {
        c  = command.length() + 10 + QString("369098752").length();
    }
    else
    {
    c = command.length() + 9;
    }
    QString b;

    temp.insert(0,c);
    if(rcon)
    {
    temp.insert(4,2);
    }
    temp.insert(8,type);
    for(int i = 0; i < command.length(); i++)
    {
        b = command[i];
        if(b == " ") { b = "X"; }
        temp.insert(12+i,b);
    }
    temp.append('\0');
    if(rcon)
    {
        temp.append("369098752");
    }
    temp.replace(' ','\0');
    temp.replace("X"," ");
    temp.append('\0');
    return temp;

}
