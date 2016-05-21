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

    if (!db.open() && !db.isValid())
    {
        qDebug() << db.lastError().text();
        exit(EXIT_FAILURE);
    }
    qDebug("MySQL Connection was successful");

    query = QSqlQuery(db);


    query.prepare("SELECT COUNT(*) AS g FROM games");

    query.exec();
    query.next();

    gamecounter = query.value(0).toInt();

    query.prepare("SELECT mapname FROM maplist");
    query.exec();
    while(query.next())
    {
        maps.append(query.value(query.record().indexOf("mapname")).toString());
    }

}
statshandler::~statshandler()
{
    delete ztimer;
    db.close();
}

bool statshandler::gamesList(QString vauth, QString vname)
{
    if(lookup(vname, vauth) && lookup_rank > 0)
    {
        query.prepare("SELECT id,map FROM games WHERE status = :stza");
        query.bindValue(":stza", "open");
        query.exec();
        gamelist.clear();
        while(query.next())
        {
            QString map = query.value(1).toString();
            QString gn = query.value(0).toString();
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
        return false;
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

    query.prepare("SELECT steamid FROM users WHERE name = :name and auth = :auth");
    query.bindValue(":name", vname);
    query.bindValue(":auth", vauth);
    query.exec();
    return false;
}

bool statshandler::selectServer(QString vauth, QString vname, int idz)
{
    if(lookup(vname,vauth) && lookup_rank > 3)
    {
        query.prepare("SELECT * FROM servers WHERE id = :idzz");
        query.bindValue(":idzz", idz);
        query.exec();
        query.next();


        QString ip = query.value(1).toString();
        int port = query.value(2).toInt();
        QString rcon = query.value(3).toString();

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
        query.prepare("SELECT * FROM games WHERE id = :idz AND status = :stz");
        query.bindValue(":idz", gamenumber);
        query.bindValue(":stz","open");
        query.exec();
        query.next();

        QString ct = query.value(2).toString();
        QString t = query.value(3).toString();

        if(ct.contains(ply1) && !ct.contains(ply2))
        {
            ct.replace(ply1,ply2);
            query.prepare("UPDATE games SET CT=:ct, T=:t WHERE id = :id");
            query.bindValue(":ct",ct);
            query.bindValue(":t",t);
            query.bindValue(":id",gamenumber);
            if(query.exec()) { return true; }
        }
        else if(t.contains(ply1) && !ct.contains(ply2))
        {
            t.replace(ply1,ply2);
            query.prepare("UPDATE games SET CT=:ct, T=:t WHERE id = :id");
            query.bindValue(":ct",ct);
            query.bindValue(":t",t);
            query.bindValue(":id", gamenumber);
            if(query.exec()) { return true; }
        }
    }
    return false;

}


bool statshandler::checkGame(int gamenumber)
{
    query.prepare("SELECT * FROM games WHERE id = :idz AND status = :stz");
    query.bindValue(":idz", gamenumber);
    query.bindValue(":stz","open");
    query.exec();
    if(query.next()){return true;}
    return false;
}
int statshandler::getServer()
{
    query.prepare("SELECT * FROM servers WHERE usable = :us");
    query.bindValue(":us","yes");

    if(!query.exec())
    {
        return -1;
    }

    int fieldNo = query.record().indexOf("id");
    int id = query.value(fieldNo).toInt();

    return id;
}


bool statshandler::AddGame(QString ct, QString t, QString map)
{
    gamecounter++;
    query.prepare("INSERT INTO games (id, map, CT, T, server_ID) "
                  "VALUES (:id, :map, :ct, :t, :sid)");
    query.bindValue(":id",gamecounter);
    query.bindValue(":map",map);
    query.bindValue(":ct",ct);
    query.bindValue(":t",t);
    int id = getServer();
    if(id == -1)
    {
        return false;
    }
    query.bindValue(":sid",id);
    if(query.exec())
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
        query.prepare("UPDATE users SET rank=:newrnk WHERE name = :name");
        query.bindValue(":newrnk",nrank);
        query.bindValue(":name",name);
        query.exec();
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
        query.prepare("UPDATE users SET rank=:newrnk WHERE name = :name");
        query.bindValue(":newrnk",nrank);
        query.bindValue(":name",name);
        query.exec();
        return true;
    }
    return false;
}
bool statshandler::vouch(QString name, QString auth,QString vauth, QString vname)
{
    if (lookup(vname, vauth) && lookup_rank > 2 && lookup(name, auth) == false)
    {
        query.prepare("INSERT INTO users (name, auth, rank) "
                      "VALUES (:user, :auth, :rnk)");
        query.bindValue(":user",name);
        query.bindValue(":auth",auth);
        query.bindValue(":rnk", 1);
        query.exec();
        return true;
    }
    return false;

}
bool statshandler::unvouchq(QString name, QString vauth, QString vname)
{
    if(lookup(vname,vauth) && lookup_rank > 3 && isUser(name) == true)
    {
        query.prepare("DELETE FROM users WHERE name = :name");
        query.bindValue(":name",name);
        query.exec();
        return true;
    }
    return false;

}
bool statshandler::lookup(QString name, QString auth)
{
    query.prepare("SELECT * FROM users WHERE name = :name AND auth = :auth");
    query.bindValue(":name", name);
    query.bindValue(":auth", auth);
    query.exec();
    if(query.next())
    {

        int fieldNo = query.record().indexOf("rank");
        lookup_rank = query.value(fieldNo).toInt();

        return true;
    }
    return false;

}
bool statshandler::isUser(QString name)
{

    query.prepare("SELECT * FROM users WHERE name = :name");
    query.bindValue(":name", name);
    query.exec();
    if(query.next())
    {

        int fieldNo = query.record().indexOf("rank");
        lookup_rank = query.value(fieldNo).toInt();
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

        query.prepare("SELECT * FROM users WHERE name = :name");
        query.bindValue(":name", name);
        query.exec();
        if(query.next())
        {

            int fieldNo = query.record().indexOf("rank");
            int fieldSec = query.record().indexOf("rating");
            int fieldTre = query.record().indexOf("wins");
            int fieldQua = query.record().indexOf("loses");
            user_rank = query.value(fieldNo).toInt();
            elo = query.value(fieldSec).toInt();
            wins = query.value(fieldTre).toInt();
            loses = query.value(fieldQua).toInt();

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
        query.prepare("UPDATE games SET status=:abc WHERE id=:idg");
        query.bindValue(":abc","closed");
        query.bindValue(":idg",gnumber);
        query.exec();
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
    query.prepare("SELECT * FROM games WHERE id = :idn");
    query.bindValue(":idn", gnumber);
    query.exec();
    query.next();

    int na = query.record().indexOf("CT");
    qDebug() << "Index of CT" << na;
    QString ct = query.value(na).toString();

    return ct;

}
QString statshandler::checkT(int gnumber)
{
    query.prepare("SELECT * FROM games WHERE id = :idn");
    query.bindValue(":idn", gnumber);
    query.exec();
    query.next();

    int no = query.record().indexOf("T");
    qDebug() << "Index of T" << no;
    QString t = query.value(no).toString();

    return t;

}

QString statshandler::checkStatus(int gnumber)
{
    query.prepare("SELECT * FROM games WHERE id = :idn");
    query.bindValue(":idn", gnumber);
    query.exec();
    query.next();

    int no = query.record().indexOf("gstatus");
    qDebug() << "Index of gstatus" << no;
    QString g = query.value(no).toString();

    if(g == "NULL")
    {
        g.clear();
        g.append("No Result");
    }

    return g;
}
bool statshandler::hasActive(QString vname)
{
    query.prepare("SELECT * FROM users WHERE name = :name");
    query.bindValue(":name",vname);
    query.exec();
    query.next();

    int no = query.record().indexOf("lastgame");
    int g = query.value(no).toInt();

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

    query.prepare("UPDATE users SET lastgame=:dd WHERE name = :name");

    for(int i = 0; i < tempct.size(); i++ )
    {
        query.bindValue(":name",ulist[i]);
        query.bindValue(":dd",gamecounter);
        query.exec();
    }
    for(int i = 0; i < tempt.size(); i++ )
    {
        query.bindValue(":name",ulist2[i]);
        query.bindValue(":dd",gamecounter);
        query.exec();
    }

}
QString statshandler::findAuth(QString vauth, QString vname, QString name)
{
    if(lookup(vname,vauth) && lookup_rank > 3)
    {
        query.prepare("SELECT auth FROM users WHERE name = :name");
        query.bindValue(":name",name);
        query.exec();

        return (query.value(0).toString());
    }
    return "null";

}

void statshandler::giveServ()
{
    query.prepare("SELECT * FROM games WHERE id = :idn");
    query.bindValue(":idn",gamecounter);
    query.exec();

    int fieldNo = query.record().indexOf("server_ID");
    int adc = query.value(fieldNo).toInt();

    query.prepare("SELECT * FROM servers WHERE id = :idzz");
    query.bindValue(":idzz",adc);
    query.exec();

    int fieldSec = query.record().indexOf("ip");
    ip = query.value(fieldSec).toString();

    int fieldTre = query.record().indexOf("port");
    port = query.value(fieldTre).toInt();


    query.prepare("UPDATE servers SET usable=:no WHERE id=:idz");
    query.bindValue(":no","no");
    query.bindValue(":idz",gamecounter);
    query.exec();

}
