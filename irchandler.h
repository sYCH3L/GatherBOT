#ifndef IRCHANDLER_H
#define IRCHANDLER_H

#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QStringList>
#include "player.h"

struct Slicer
{
    QStringList* list;
    QString* str;
    inline void Cleanup() { delete list; delete str; }
};

class IRCHandler : public QTcpSocket
{
    Q_OBJECT

public:
    IRCHandler(QObject* parent = 0);
    void PingHandler(Slicer &data);
    void InviteHandler(Slicer &data);
    void JoinHandler(Slicer &data);
    void WhoHandler(Slicer &data);
    void WhoisHandler(Slicer &data);

private:
    bool _authed;
    bool _initDatabaseLoading;
    PlayerMap _players;
    QString _activeChannel;
    QSqlDatabase _database;

private slots:
    void PerformMasterCleanup();
    void ReadSocket();
};

#endif // IRCHANDLER_H
