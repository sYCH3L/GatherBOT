#include "commands.h"
#include <QStringList>

commands::commands(QObject *parent) :
    QObject(parent)
{
    _socket = new QTcpSocket(this);
    _string = new QString();
    _timer = new QTimer(this);
    _queueTimer = new QTimer(this);
    _delayTimer = new QTimer(this);
    _commandQueue = new Command();
    firstRun = 1;

    connect(ztimer, SIGNAL(timeout()), this, SLOT(randz()));


    _lCount = 0;
    _bProcess = false;
    sec = 0;

}
commands::~commands()
{
    delete _socket;
    delete _string;
    delete _timer;
    delete _delayTimer;
}
void commands::randz()
{
    this->CloseCl();
    _socket->write((QString("PRIVMSG #rps.ee :Challenge mode closed.\r\n")).toLatin1());
}

void commands::commandHandler()
{
    //List & Extraction Variables
    QStringList datalist = _string->split(QRegExp("\\s"));
    QString temps = datalist[0];

    temps.replace("@"," ");
    temps.replace("."," ");
    temps.replace("!"," ");
    temps.remove(":");

    QStringList userinfo = temps.split(QRegExp("\\s"));

    if(firstRun && datalist.contains(":End"))
    {
        _socket->write((QString("JOIN ") += channel += QString(" \r\n")).toLatin1());
        firstRun = 0;
    }

    if(datalist[1] == "JOIN")
    {
        if(!lookup(userinfo[0],userinfo[2]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Welcome to %1 Gather 3").arg(gname) += userinfo[0] += QString("1, to get a vouch contact an admin or a moderator!\r\n")).toLatin1());
        }
        else
        {
            if(lookup_rank == 0)
            {
                _socket->write((QString("KICK ") += datalist[2] += QString(" ") += userinfo[0] += QString(" :User is banned until futher notice! \r\n")).toLatin1());
            }
            else if(lookup_rank == 2 || lookup_rank == 3)
            {
                //MODE #Finnish +v Wiz
                _socket->write((QString("MODE ") += datalist[2] += QString(" +v ") += userinfo[0] += QString(" \r\n")).toLatin1());
            }
            else if(lookup_rank > 3)
            {
                //MODE #Finnish +v Wiz
                _socket->write((QString("MODE ") += datalist[2] += QString(" +o ") += userinfo[0] += QString(" \r\n")).toLatin1());
            }
            else
            {
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Welcome to %1 Gather3 ").arg(gname) += userinfo[0] += QString(" \r\n")).toLatin1());
            }
        }
    }
    if(datalist[3] == ":`join")
    {
            _socket->write((QString("JOIN ") += datalist[4] += QString(" \r\n")).toLatin1());
    }
    if(datalist[3] == ":.giveauth")
    {
        if(giveAuth(userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" : The authkey for you is %1 \r\n").arg(stringA)).toLatin1());

        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.connect")
    {
        int a = datalist[4].toInt();
        if(datalist[4] == "" || (datalist[4] != "" && datalist[5] == "") || (datalist[4] != "" && datalist[5] != "" && datalist[6] == ""))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :The syntax is .connect port ip rcon!\r\n")).toLatin1());
        }
        else if(checkConnect(userinfo[2],userinfo[0],a,datalist[5],datalist[6]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :GatherBOT connected to your server!\r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :ip/port/rcon is not valid or insufficient privileges!\r\n")).toLatin1());
        }

    }
    if(datalist[3] == ":.send")
    {
        QString cmd;
        if(datalist[4] != "" && datalist[5] != "")
        {
            cmd += datalist[4];
            cmd += " ";
            cmd += datalist[5];
        }
        else
        {
            cmd.append(datalist[4]);
        }
        if(datalist[4] == "")
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :The syntax is .send command!\r\n")).toLatin1());
        }
        else if(sendCommandCheck(userinfo[2],userinfo[0],cmd))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Custom command sent!\r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Insufficient privileges or not connected to server!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.listconnect")
    {
        int a = datalist[4].toInt();
        if(datalist[4] == "")
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :The syntax is .listconnect servernr!\r\n")).toLatin1());
        }
        else if(selectServer(userinfo[2],userinfo[0],a))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Connected to server %1!\r\n").arg(a)).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Insufficient privileges or servernumber false!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.games")
    {
        QString templist = gamelist;
        if(gamesList(userinfo[2],userinfo[0]))
        {
            if(gamelist.isEmpty())
            {
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :No games going on right now!\r\n")).toLatin1());
            }
            else
            {
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Games going on right now: %1 \r\n").arg(templist)).toLatin1());

            }
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :No games or insufficient privileges!\r\n")).toLatin1());

        }
    }
    if(datalist[3] == ":.captains")
    {
        if(checkCapt(userinfo[2],userinfo[0]))
        {
            QString a = capt1;
            QString b = capt2;
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" : Captain for CT is %1 and Captain for T is %2 \r\n").arg(a).arg(b)).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.pick")
    {
        if(pickPlayer(datalist[4],userinfo[2],userinfo[0]))
        {
            if(CT.count() < 4 && T.count() < 4)
            {
                QString a = userinfo[0];
                QString team;
                if(a == capt1)
                {
                    team = "CT";
                }
                else if(a == capt2)
                {
                    team = "T";
                }
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" : %1 has picked %2 for team %3 \r\n").arg(userinfo[0]).arg(datalist[4]).arg(team)).toLatin1());
            }
            else
            {
                QString m = cmap;
                QString tempct;
                for(int i=0;i<CT.count();i++)
                {
                    tempct += CT.at(i);
                    tempct += ",";
                }
                QString tempt;
                for(int i=0;i<T.count();i++)
                {
                    tempt += T.at(i);
                    tempt += ",";
                }
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :The teams have been chosen!\r\n CT:%1 \r\n T:%2 \r\n Map:%3").arg(tempct).arg(tempt).arg(m)).toLatin1());
                if(AddGame(tempct,tempt,m))
                {
                    this->CloseCl();
                    addLastGame(tempct,tempt);
                    giveServ();

                    tempct.replace(","," ");
                    tempt.replace(","," ");

                    QStringList ulist = tempct.split(" ");
                    QStringList ulist2 = tempt.split(" ");

                    for(int i = 0; i < tempct.size(); i++ )
                    {
                        _socket->write((QString("PRIVMSG ") += ulist[i] += QString(" :Server for the game: %1:%2").arg(ip).arg(port)).toLatin1());
                    }
                    for(int i = 0; i < tempt.size(); i++ )
                    {
                        _socket->write((QString("PRIVMSG ") += ulist2[i] += QString(" :Server for the game: %1:%2").arg(ip).arg(port)).toLatin1());
                    }

                }
                else
                {
                    _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" : Error, couldn't add game.!\r\n")).toLatin1());
                }

            }

        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.swap")
    {

        int a = datalist[4].toInt();
        if(datalist[4] == "" || (datalist[4] != "" && datalist[5] == "") || (datalist[4] != "" && datalist[5] != "" && datalist[6] == ""))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :The syntax is .swap gamenr player1 player2!\r\n")).toLatin1());
        }
        else if(swapPlayer(a,datalist[5],datalist[6],userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" : %1 has been swapped with %2 in game %3 !\r\n").arg(datalist[5]).arg(datalist[6]).arg(a)).toLatin1());

        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Game/Players not found or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.ready")
    {
        if(isReady(userinfo[2],userinfo[0]))
        {
            if(capt1_rdy && capt2_rdy)
            {
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Starting picking stage.Too see playerlist write .pl. To pick write .pick name! %1 has the first pick.\r\n").arg(capt1)).toLatin1());
            }
            else
            {
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :%1 is ready to pick. Other captain to start the picking stage write .ready! \r\n").arg(userinfo[0])).toLatin1());
            }
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or insufficient privileges!\r\n")).toLatin1());

        }
    }
    if(datalist[3] == ":.forceabort")
    {
        if(forceClose(userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(": %1 has aborted the game.").arg(userinfo[0])).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or insufficient privileges!\r\n")).toLatin1());
        }

    }
    if(datalist[3] == ":.sign")
    {
        if(signIn(userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(": %1 has signed into the game.").arg(userinfo[0])).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ".out")
    {
        if(signOut(userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(": %1 has signed out of the game.").arg(userinfo[0])).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.pl")
    {
        if(checkPool(userinfo[2],userinfo[0]))
        {
            QString temp;
            for(int i=0;i<playerList.count();i++)
            {
                temp += playerList.at(i);
                temp += ",";
            }
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :There are currently %2 players signed in. Players who have signed into challenge mode:4 %1 \r\n").arg(temp).arg(playerList.count())).toLatin1());


        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or insufficient privileges!\r\n")).toLatin1());
        }

    }
    if(datalist[3] == ":.maplist")
    {
        QString temp;
        for(int i = 0; i < maps.size(); i++)
        {
            temp += maps[i];
            if(!(maps.size() - 1 == i))
            {
            temp += ",";
            }
        }
        _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :3Maplist: %1\r\n").arg(temp)).toLatin1());
    }
    if(datalist[3] == ":.map")
    {
        if(MapVoting(userinfo[2],userinfo[0],datalist[4]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Map has been chosen. Map:4 %1 \r\n").arg(cmap)).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or you're not the Captain or insufficient privileges or map chosen isn't in the list!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.accept")
    {
        if(checkAccept(userinfo[2],userinfo[0]))
        {
            QString temp_capt = capt2;
            //(QString("PRIVMSG ") += datalist[2] += QString(" :4 ") += capt2 += QString(" 3 chooses the map. Use .maplist to see maps available! To pick a map use .map <mapname>! \r\n")
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Captain:4") += temp_capt += QString(" 3has accepted the challenge! ") += QString("3Pre-Game started. Initiating map choosing. ")+= QString(" \r\n\r\n\r\n\r\n\r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or you're not the Captain or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.decline")
    {
        if(checkDecline(userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Captain:4") += capt2 += QString(" 3has declined the challenge! ") += QString("3Pre-Game aborted. Returning to normal ")+= QString(" \r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Challenge mode not activated or you're not the Captain or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.cl")
    {
        if(startGame(datalist[4],userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :User %1 has challenged %2").arg(userinfo[0]).arg(datalist[4]) += QString("! %1 to accept write .accept or .decline! After 1-2 minutes challenge closes.\r\n").arg(datalist[4])).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :User doesn't exist or insufficient privileges or challenge mode activated!\r\n")).toLatin1());
        }

    }
    if(datalist[3] == ":.promote")
    {
        QString ranks[6] = {"Banned","Vouched", "VIP", "Moderator","Administrator",  "Owner" };
        if(promote(datalist[4],userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :User 3") += datalist[4] += QString(" 1has been promoted to 3") += ranks[nrank] += QString("1!\r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :User doesn't exist or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.demote")
    {
        QString ranks[6] = {"Banned","Vouched", "VIP", "Moderator","Administrator",  "Owner" };
        if(demote(datalist[4],userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :User 3") += datalist[4] += QString(" 1has been demoted to 3") += ranks[nrank] += QString("1!\r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :User doesn't exist or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.vouch")
    {
        if(!vouch(datalist[4],datalist[5],userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :You don't have rights do use this command or user is already vouched!\r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :3User 4") += datalist[5] += QString(" 3has been added with username 4") += datalist[4] += QString(" \r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.unvouch")
    {
        if(!unvouchq(datalist[4],userinfo[2],userinfo[0]))
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :You don't have rights do use this command or user doesn't exist!\r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :4User ") += datalist[4] += QString(" has been deleted!") += QString(" \r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.k")
    {
        if(kick(userinfo[2],userinfo[0]))
        {
            _socket->write((QString("KICK ") += datalist[2] += QString(" ") += datalist[4] += QString(" :Requested kick! \r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :User doesn't exist or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.lookup" || datalist[3] == ":.stats")
    {
        QString ranks[6] = {"Banned","Vouched", "VIP", "Moderator","Administrator",  "Owner" };
        QString tempname;
        if(datalist[4] == NULL){ tempname = userinfo[0]; } else { tempname = datalist[4]; }
        if(showStats(datalist[4],userinfo[0],userinfo[2]))
        {
            // Rank list - Banned,Vouched,VIP,Moderator,Administrator,Owner
            if(this->user_rank == 0)
            {
                //KICK #Finnish John :Speaking English
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :4User ") += datalist[4] += QString(" has been 4banned1!") += QString(" \r\n")).toLatin1());
            }
            else
            {
                _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :3User:4") += tempname += QString(" 3Rank:4") += ranks[user_rank] += QString(" 3Rating:4 %1 3Wins:4 %2 3Loses:4 %3").arg(elo).arg(wins).arg(loses) += QString(" \r\n")).toLatin1());
            }
        }

    }
    if(datalist[3] == ":.close" || datalist[3] == ":.cg")
    {
        int a = datalist[4].toInt();
        if(closeGame(userinfo[2],userinfo[0],a) && datalist[4] != "")
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :%1 has closed gamenumber ").arg(userinfo[0]) += datalist[4] += QString("!\r\n")).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Game not found or insufficient privileges!\r\n")).toLatin1());
        }
    }
    if(datalist[3] == ":.info")
    {
        int a = datalist[4].toInt();
        if(showInfo(userinfo[2],userinfo[0],a) && datalist[4] != "")
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Gamenumber:") +=datalist[4] += QString(" CT:%1 T:%2 Game Winner:%3 \r\n").arg(checkCT(a)).arg(checkT(a)).arg(checkStatus(a))).toLatin1());
        }
        else
        {
            _socket->write((QString("PRIVMSG ") += datalist[2] += QString(" :Game not found or insufficient privileges!\r\n")).toLatin1());
        }
    }
}
