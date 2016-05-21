#ifndef STATSHANDLER_H
#define STATSHANDLER_H

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QStringList>
#include <QTimer>
#include <csserver.h>

class statshandler : public csserver
{
public:
    statshandler();
    virtual ~statshandler();
    QSqlDatabase db;

    QSqlQuery query;

    int lookup_rank, user_rank, pr_rank, rank, nrank, elo, wins, loses, sec, gamecounter, port;

    bool game_started, capt2_ac,map_picked,players_picked,timed_out,pick_stage,capt1_rdy,capt2_rdy;

    QString capt1,capt2,cmap,pturn,gamelist,tempM,stringA,channel, botname, gname, ip;

    QStringList maps;

    QList<QString> playerList,CT,T;

    QTimer *ztimer;

protected:
    bool vouch(QString name, QString auth,QString vauth, QString vname);
    bool unvouchq(QString name,QString vauth, QString vname);
    bool showStats(QString name, QString vname, QString vauth);
    bool lookup(QString name, QString auth);
    bool isUser(QString name);
    bool promote(QString name, QString vauth, QString vname);
    bool demote(QString name,  QString vauth, QString vname);
    bool kick(QString vauth, QString vname);
    bool startGame(QString ss, QString vauth, QString vname);
    bool checkAccept(QString vauth, QString vname);
    bool MapVoting(QString vauth, QString vname, QString map);
    bool checkDecline(QString vauth, QString vname);
    bool checkPool(QString vauth, QString vname);
    bool signIn(QString vauth, QString vname);
    bool signOut(QString vauth, QString vname);
    bool isReady(QString vauth, QString vname);
    bool pickPlayer(QString name, QString vauth, QString vname);
    bool AddGame(QString ct, QString t, QString map);
    bool checkCapt(QString vauth, QString vname);
    bool forceClose(QString vauth, QString vname);
    bool swapPlayer(int gamenumber, QString ply1, QString ply2, QString vauth, QString vname);
    bool checkGame(int gamenumber);
    bool gamesList(QString vauth, QString vname);
    bool checkConnect(QString vauth, QString vname,int port, QString ip, QString rcon);
    bool sendCommandCheck(QString vauth,QString vname,QString cmd1);
    bool selectServer(QString vauth, QString vname, int idz);
    bool giveAuth(QString vauth, QString vname);
    bool checkSteamid(QString vauth, QString vname);
    bool closeGame(QString vauth, QString vname, int gnumber);
    bool showInfo(QString vauth, QString vname, int gnumber);
    bool hasActive(QString vname);
    void addLastGame(QString tempct, QString tempt);
    void giveServ();
    int getServer();
    QString checkStatus(int gnumber);
    QString checkCT(int gnumber);
    QString checkT(int gnumber);
    QString generator();
    QString findAuth(QString vauth, QString vname, QString name);
private:
    int reg;
public slots:
    void CloseCl();
};

#endif // STATSHANDLER_H
