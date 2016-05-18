#include "statshandler.h"
#include <QSqlError>
#include <QSqlRecord>


statshandler::statshandler()
{
    reg = 0;
    pr_rank = 0;
    lookup_rank = 0;
    loses = 0;
    wins = 0;
    nrank = 0;
    rank = 0;
    gamecounter = 0;
    game_started = false;
    map_picked = false;
    players_picked = false;
    pick_stage = false;
    capt1_rdy = false;
    capt2_rdy = false;
    ztimer = new QTimer();

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("sql2.freemysqlhosting.net");
    db.setDatabaseName("sql2107771");
    db.setUserName("sql2107771");
    db.setPassword("yM9!mW7!");
    db.setPort(3306);

    if (!db.open() && !db.isValid()) {
        qDebug() << db.lastError().text();
        exit(EXIT_FAILURE);
    } else {
        qDebug("MySQL Connection was successful");
    }

    lookup_q = QSqlQuery(db);
    lookup_qq = QSqlQuery(db);
    vouch_q = QSqlQuery(db);
    stats_q = QSqlQuery(db);
    unvouch_q = QSqlQuery(db);
    promote_q = QSqlQuery(db);
    demote_q = QSqlQuery(db);
    addg_q = QSqlQuery(db);
    checkGame_q = QSqlQuery(db);
    updateGame_q = QSqlQuery(db);
    gameList_q = QSqlQuery(db);
    serverList_q = QSqlQuery(db);
    checkID = QSqlQuery(db);
    updateID = QSqlQuery(db);
    closegame_q = QSqlQuery(db);
    checkTeams_q = QSqlQuery(db);
    lastG_q = QSqlQuery(db);
    countG_q = QSqlQuery(db);
    setLast_q = QSqlQuery(db);
    selectMap_q = QSqlQuery(db);
    selectServer_q = QSqlQuery(db);
    updateServer_q = QSqlQuery(db);



    updateID.prepare("UPDATE users SET authkey = :aux WHERE name = :name and auth = :auth");
    checkID.prepare("SELECT steamid FROM users WHERE name = :name and auth = :auth");
    lookup_q.prepare("SELECT * FROM users WHERE name = :name AND auth = :auth");
    lookup_qq.prepare("SELECT * FROM users WHERE name = :name");
    stats_q.prepare("SELECT * FROM users WHERE name = :name");
    vouch_q.prepare("INSERT INTO users (name, auth, rank) "
                    "VALUES (:user, :auth, :rnk)");
    unvouch_q.prepare("DELETE FROM users WHERE name = :name");
    closegame_q.prepare("UPDATE games SET status=:abc WHERE id=:idg");
    promote_q.prepare("UPDATE users SET rank=:newrnk WHERE name = :name");
    demote_q.prepare("UPDATE users SET rank=:newrnk WHERE name = :name");
    addg_q.prepare("INSERT INTO games (id, map, CT, T, server_ID) "
                   "VALUES (:id, :map, :ct, :t, :sid)");
    checkGame_q.prepare("SELECT * FROM games WHERE id = :idz AND status = :stz");
    updateGame_q.prepare("UPDATE games SET CT=:ct, T=:t WHERE id = :id");
    gameList_q.prepare("SELECT id,map FROM games WHERE status = :stza");
    serverList_q.prepare("SELECT * FROM servers WHERE id = :idzz");
    checkTeams_q.prepare("SELECT * FROM games WHERE id = :idn");
    lastG_q.prepare("SELECT * FROM games WHERE id = :idn");
    setLast_q.prepare("UPDATE users SET lastgame=:dd WHERE name = :name");
    countG_q.prepare("SELECT COUNT(*) AS g FROM games");
    selectMap_q.prepare("SELECT mapname FROM maplist");
    selectServer_q.prepare("SELECT * FROM servers WHERE usable = :us");
    updateServer_q.prepare("UPDATE servers SET usable=:no WHERE id=:idz");

    countG_q.exec();
    countG_q.next();

    gamecounter = countG_q.value(0).toInt();

    selectMap_q.exec();
    while(selectMap_q.next())
    {
        maps.append(selectMap_q.value(selectMap_q.record().indexOf("mapname")).toString());
    }

}
statshandler::~statshandler()
{
    db.close();
}

bool statshandler::gamesList(QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 0)
    {
        gameList_q.bindValue(":stza", "open");
        gameList_q.exec();
        gamelist.clear();
        while(gameList_q.next())
        {
            QString map = gameList_q.value(1).toString();
            QString gn = gameList_q.value(0).toString();
            gamelist += QString("%1[%2]").arg(map).arg(gn);
            gamelist += ",";
        }
        return true;
    }
    return false;
}

bool statshandler::checkCapt(QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 0 && game_started == true)
    {
        return true;
    }
    return false;
}
bool statshandler::checkConnect(QString vauth, QString vname, int port, QString ip, QString rcon)
{
    if(lookup(vname, vauth) && lookup_rank > 4)
    {
        if(ServerConnect(port,ip,rcon))
        {
            return true;
        }
    }
    return false;
}
bool statshandler::sendCommandCheck(QString vauth, QString vname, QString cmd1)
{
    if(lookup(vname, vauth) && lookup_rank > 4 && _csocket->state() != 0)
    {
        if(sendCommand(cmd1))
        {
            return true;
        }
    }
        return false;
}
bool statshandler::checkSteamid(QString vauth, QString vname)
{
    checkID.bindValue(":name", vname);
    checkID.bindValue(":auth", vauth);
    checkID.exec();
    return false;
}

QString statshandler::generator()
{
    QString s;
    const char alphanum[] =
           "0123456789"
           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
           "abcdefghijklmnopqrstuvwxyz";

       for (int i = 0; i < 5; ++i) {
           s += alphanum[rand() % (sizeof(alphanum) - 1)];
       }
   return s;
}

bool statshandler::giveAuth(QString vauth, QString vname)
{
    if(lookup(vname,vauth) && lookup_rank > 0)
    {
        stringA = generator();
        updateID.bindValue(":aux",stringA);
        updateID.bindValue(":name",vname);
        updateID.bindValue(":auth",vauth);
        updateID.exec();
        return true;
    }
    return false;
}

bool statshandler::selectServer(QString vauth, QString vname, int idz)
{
    if(lookup(vname,vauth) && lookup_rank > 3)
    {
        serverList_q.bindValue(":idzz", idz);
        serverList_q.exec();
        serverList_q.next();


        QString ip = serverList_q.value(1).toString();
        int port = serverList_q.value(2).toInt();
        QString rcon = serverList_q.value(3).toString();

        if(ServerConnect(port,ip,rcon))
        {
            return true;
        }
    }
        return false;
}


bool statshandler::swapPlayer(int gamenumber, QString ply1, QString ply2, QString vauth, QString vname)
{
    qDebug() << gamenumber;
    if(checkGame(gamenumber) && isUser(ply1) && isUser(ply2) && lookup(vname, vauth) && lookup_rank > 2 && hasActive(ply2))
    {
        checkGame_q.bindValue(":idz", gamenumber);
        checkGame_q.bindValue(":stz","open");
        checkGame_q.exec();
        checkGame_q.next();

        QString ct = checkGame_q.value(2).toString();
        QString t = checkGame_q.value(3).toString();

        if(ct.contains(ply1) && !ct.contains(ply2))
        {
            ct.replace(ply1,ply2);
            updateGame_q.bindValue(":ct",ct);
            updateGame_q.bindValue(":t",t);
            updateGame_q.bindValue(":id",gamenumber);
            if(updateGame_q.exec()) { return true; }
        }
        else if(t.contains(ply1) && !ct.contains(ply2))
        {
            t.replace(ply1,ply2);
            updateGame_q.bindValue(":ct",ct);
            updateGame_q.bindValue(":t",t);
            updateGame_q.bindValue(":id", gamenumber);
            if(updateGame_q.exec()) { return true; }
        }
    }
    return false;

}


bool statshandler::checkGame(int gamenumber)
{
    checkGame_q.bindValue(":idz", gamenumber);
    checkGame_q.bindValue(":stz","open");
    checkGame_q.exec();
    if(checkGame_q.next()){return true;}
    return false;
}
int statshandler::getServer()
{
    selectServer_q.bindValue(":us","yes");

    if(!selectServer_q.exec())
    {
        return -1;
    }

    int fieldNo = selectServer_q.record().indexOf("id");
    int id = selectMap_q.value(fieldNo).toInt();

    return id;
}


bool statshandler::AddGame(QString ct, QString t, QString map)
{
    gamecounter++;
    addg_q.bindValue(":id",gamecounter);
    addg_q.bindValue(":map",map);
    addg_q.bindValue(":ct",ct);
    addg_q.bindValue(":t",t);
    int id = getServer();
    if(id == -1)
    {
        return false;
    }
    addg_q.bindValue(":sid",id);
    if(addg_q.exec())
    {
        return true;
    }
    return false;

}

bool statshandler::pickPlayer(QString name, QString vauth, QString vname)
{
    if(lookup(vname,vauth) && lookup_rank > 0 && game_started == true && map_picked == true && capt2_ac == true && pturn == vname && playerList.contains(name) && isUser(name) && pr_rank > 0)
    {
        QString a = capt1;
        QString b = capt2;
        if(a == vname)
        {
            CT.append(name);
            playerList.removeOne(name);
            pturn = b;
            return true;
        }
        else if(b == vname)
        {
            T.append(name);
            playerList.removeOne(name);
            pturn = a;
            return true;
        }
        else if(CT.count() > 4 && T.count() > 4)
        {
            //game starting
            return true;
        }
    }
    return false;
}
bool statshandler::forceClose(QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 2 && game_started == true)
    {
        CloseCl();
        return true;
    }
    return false;
}

bool statshandler::isReady(QString vauth, QString vname)
{
    QString a = capt1;
    QString b = capt2;

    if(lookup(vname,vauth) && lookup_rank > 0 && game_started == true && map_picked == true && capt2_ac == true && (a == vname or b == vname) && playerList.count() > 7)
    {
        if(a == vname)
        {
            capt1_rdy = true;
        }
        else if(b == vname)
        {
            capt2_rdy = true;
        }
        if(capt1_rdy == true && capt2_rdy == true)
        {
            pick_stage = true;
            pturn = capt1;
            CT.append(capt1);
            T.append(capt2);
        }
        return true;
    }
    return false;
}

bool statshandler::startGame(QString ss, QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 0 && isUser(ss) && pr_rank > 0 && game_started == false && hasActive(ss) && hasActive(vname))
    {
        //starting gamemode competive
        capt2_ac = false;
        capt1 = vname;
        capt2 = ss;
        game_started = true;
        ztimer->start(120000);
        return true;
    }
    return false;

}
bool statshandler::signIn(QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 0 && game_started == true
            && map_picked == true && playerList.contains(vname) == false && pick_stage == false && hasActive(vname))
    {
        playerList.append(vname);
        return true;
    }
    return false;
}
bool statshandler::signOut(QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 0 && game_started == true
            && map_picked == true && playerList.contains(vname) == false && pick_stage == false)
    {
        playerList.removeOne(vname);
        return true;
    }
    return false;
}

bool statshandler::checkPool(QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 0 && game_started == true)
    {
        return true;
    }
    return false;
}

bool statshandler::checkAccept(QString vauth, QString vname)
{
    if(lookup(vname,vauth) && lookup_rank > 0 && vname == capt2 && game_started == true && capt2_ac == false)
    {
        capt2_ac = true;
        ztimer->stop();
        ztimer->start(120000);
        return true;
    }
    return false;
}
bool statshandler::checkDecline(QString vauth, QString vname)
{
    if(lookup(vname,vauth) && lookup_rank > 0 && vname == capt2
            && game_started == true && capt2_ac == false)
    {
        capt2_ac = false;
        game_started = false;
        ztimer->stop();
        return true;
    }
    return false;
}

bool statshandler::MapVoting(QString vauth, QString vname, QString map)
{

    if(lookup(vname,vauth) && lookup_rank > 0 && capt2 == vname
            && game_started == true && map_picked == false)
    {
        ztimer->stop();
        ztimer->start(300000);
        if(maps.contains(map))
        {
            map_picked = true;
            cmap = map;
            return true;
        }
        return false;
    }
    return false;

}

bool statshandler::kick(QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 2)
    {
        return true;
    }
    return false;
}

bool statshandler::demote(QString name, QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 4 && isUser(name) == true)
    {
        rank = pr_rank;
        nrank = rank - 1;
        demote_q.bindValue(":newrnk",nrank);
        demote_q.bindValue(":name",name);
        demote_q.exec();
        return true;
    }
    return false;
}

bool statshandler::promote(QString name, QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 4 && isUser(name) == true)
    {

        rank = pr_rank;
        nrank = rank + 1;
        promote_q.bindValue(":newrnk",nrank);
        promote_q.bindValue(":name",name);
        promote_q.exec();
        return true;
    }
    return false;
}
bool statshandler::vouch(QString name, QString auth,QString vauth, QString vname)
{
    if (lookup(vname, vauth) && lookup_rank > 2 && lookup(name, auth) == false)
    {
        vouch_q.bindValue(":user",name);
        vouch_q.bindValue(":auth",auth);
        vouch_q.bindValue(":rnk", 1);
        vouch_q.exec();
        return true;
    }
    return false;

}
bool statshandler::unvouchq(QString name, QString vauth, QString vname)
{
    if(lookup(vname,vauth) && lookup_rank > 3 && isUser(name) == true)
    {
        unvouch_q.bindValue(":name",name);
        unvouch_q.exec();
        return true;
    }
    return false;

}
bool statshandler::lookup(QString name, QString auth)
{
    lookup_q.bindValue(":name", name);
    lookup_q.bindValue(":auth", auth);
    lookup_q.exec();
    if(lookup_q.next())
    {

        int fieldNo = lookup_q.record().indexOf("rank");
        lookup_rank = lookup_q.value(fieldNo).toInt();

        return true;
    }
    return false;

}
bool statshandler::isUser(QString name)
{
    lookup_qq.bindValue(":name", name);
    lookup_qq.exec();
    if(lookup_qq.next())
    {

        int fieldNo = lookup_qq.record().indexOf("rank");
        lookup_rank = lookup_qq.value(fieldNo).toInt();
        pr_rank = lookup_rank;

        return true;
    }
    return false;

}
bool statshandler::showStats(QString name, QString vname, QString vauth)
{
    if(name == NULL) {
        name = vname;
    }
    if(lookup(vname, vauth) && lookup_rank > 0)
    {

        stats_q.bindValue(":name", name);
        stats_q.exec();
        if(stats_q.next())
        {

            int fieldNo = stats_q.record().indexOf("rank");
            int fieldSec = stats_q.record().indexOf("rating");
            int fieldTre = stats_q.record().indexOf("wins");
            int fieldQua = stats_q.record().indexOf("loses");
            user_rank = stats_q.value(fieldNo).toInt();
            elo = stats_q.value(fieldSec).toInt();
            wins = stats_q.value(fieldTre).toInt();
            loses = stats_q.value(fieldQua).toInt();

            return true;
        }
    }
    return false;
}
void statshandler::CloseCl()
{
    if((game_started == true && capt2_ac == false) || (game_started == true && capt2_ac == true && map_picked == false)
            || (game_started == true && capt2_ac == true && map_picked == true && players_picked == false) )
    {
        capt1.clear();
        capt2.clear();
        capt2_ac = false;
        game_started = false;
        map_picked = false;
        players_picked = 0;
        playerList.clear();
        CT.clear();
        T.clear();
        timed_out = false;
        if(ztimer->isActive())
        {
            ztimer->stop();
        }
    }

}
bool statshandler::closeGame(QString vauth, QString vname, int gnumber)
{
    if(lookup(vname, vauth) && lookup_rank > 2 && checkGame(gnumber))
    {
        closegame_q.bindValue(":abc","closed");
        closegame_q.bindValue(":idg",gnumber);
        closegame_q.exec();
        return true;
    }
    return false;
}
bool statshandler::showInfo(QString vauth, QString vname, int gnumber)
{
    if(lookup(vname, vauth) && lookup_rank > 2 && checkGame(gnumber))
    {
        return true;
    }
    return false;
}
QString statshandler::checkCT(int gnumber)
{
    checkTeams_q.bindValue(":idn", gnumber);
    checkTeams_q.exec();
    checkTeams_q.next();

    int na = checkTeams_q.record().indexOf("CT");
    qDebug() << "Index of CT" << na;
    QString ct = checkTeams_q.value(na).toString();

    return ct;

}
QString statshandler::checkT(int gnumber)
{
    checkTeams_q.bindValue(":idn", gnumber);
    checkTeams_q.exec();
    checkTeams_q.next();

    int no = checkTeams_q.record().indexOf("T");
    qDebug() << "Index of T" << no;
    QString t = checkTeams_q.value(no).toString();

    return t;

}

QString statshandler::checkStatus(int gnumber)
{
        checkTeams_q.bindValue(":idn", gnumber);
        checkTeams_q.exec();
        checkTeams_q.next();

        int no = checkTeams_q.record().indexOf("gstatus");
        qDebug() << "Index of gstatus" << no;
        QString g = checkTeams_q.value(no).toString();

        if(g == "NULL")
        {
            g.clear();
            g.append("No Result");
        }

        return g;
}
bool statshandler::hasActive(QString vname)
{
    stats_q.bindValue(":name",vname);
    stats_q.exec();
    stats_q.next();

    int no = stats_q.record().indexOf("lastgame");
    int g = stats_q.value(no).toInt();

    if(checkStatus(g) != "NULL")
    {
        return false;
    }
    return true;
}

void statshandler::addLastGame(QString tempct, QString tempt)
{
    tempct.replace(","," ");
    tempt.replace(","," ");

    QStringList ulist = tempct.split(" ");
    QStringList ulist2 = tempt.split(" ");

    for(int i = 0; i < tempct.size(); i++ )
    {
        setLast_q.bindValue(":name",ulist[i]);
        setLast_q.bindValue(":dd",gamecounter);
        setLast_q.exec();
    }
    for(int i = 0; i < tempt.size(); i++ )
    {
        setLast_q.bindValue(":name",ulist2[i]);
        setLast_q.bindValue(":dd",gamecounter);
        setLast_q.exec();
    }

}
void statshandler::giveServ()
{
    checkTeams_q.bindValue(":idn",gamecounter);
    checkTeams_q.exec();

    int fieldNo = checkTeams_q.record().indexOf("server_ID");
    int adc = checkTeams_q.value(fieldNo).toInt();

    serverList_q.bindValue(":idzz",adc);
    serverList_q.exec();

    int fieldSec = serverList_q.record().indexOf("ip");
    ip = serverList_q.value(fieldSec).toString();

    int fieldTre = serverList_q.record().indexOf("port");
    port = serverList_q.value(fieldTre).toInt();

    updateServer_q.bindValue(":no","no");
    updateServer_q.bindValue(":idz",gamecounter);
    updateServer_q.exec();

}
