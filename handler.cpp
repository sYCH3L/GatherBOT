﻿#include "handler.h"
#include <QStringList>
#include <QSettings>

handler::handler(commands *cmd) : commands(cmd)
{

    connect(_socket, SIGNAL(readyRead()), this, SLOT(NextStep()));
    connect(_socket, SIGNAL(connected()), this, SLOT(SendInfo()));
    connect(_timer, SIGNAL(timeout()), this, SLOT(SendInfo()));
    connect(_queueTimer, SIGNAL(timeout()), this, SLOT(RunQueue()));
    connect(_delayTimer, SIGNAL(timeout()), this, SLOT(ProceedProcess()));

}
void commands::Prepear(QString chl, QString bn, QString gn)
{

    channel.append(chl);
    botname.append(bn);
    gname.append(gn);

    // Connecting to server
    _socket->connectToHost("irc.quakenet.org",6667);
    bool ok = _socket->isOpen();
    if(ok)
    {
        qDebug() << "IRC Connection Obtained";
    }
}


void commands::NextStep()
{
    _string->clear();
    _string->append(_socket->readAll());

    qDebug() << *_string;

    if (_bProcess)
        ProcessContents();

}
void commands::ProcessContents()
{
    QStringList list = _string->split(QRegExp("\\s"));

    if(list.contains("PING")) {
        int i = list.lastIndexOf("PING");
        if(list[i] == "PING") {
            _socket->write(" ");
            qDebug() << "Spacer sent";
            _socket->write((QString("PONG ") += list[i+1]).toLatin1());
            _socket->write("\r\n");
            qDebug() << "Ping Pong";
        }
    }
    this->commandHandler();
}
void commands::SendInfo()
{
    _lCount++;
    switch(_lCount)
    {
    case 1:
        _timer->start(500);
        _socket->write("CAP LS\r\n");
        break;
    case 2:
        _socket->write((QString("NICK %1\r\nUSER sycbot aww awwbot :sYCH3L minion\r\n").arg(botname)).toLatin1());
    case 3:
        _timer->stop();
        _delayTimer->start(5);
        break;
    }
}
void commands::RunQueue()
{
    if (_commandQueue->isEmpty())
        return;

    _socket->write(_commandQueue->dequeue());
}

void commands::ProceedProcess()
{
    _bProcess = true;
    _delayTimer->stop();
    delete _delayTimer;
}