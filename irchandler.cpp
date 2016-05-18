#include "irchandler.h"
#include <QRegExp>
#include <QStringList>

typedef struct Connector
{
    QString command;
    void (IRCHandler::*handler)(Slicer&);
} Connector;

const int CONNECTORS = 5; // how many handlers there are

const Connector mass[] =
{
    { "PING", &IRCHandler::PingHandler },
    { "INVITE", &IRCHandler::InviteHandler },
    { "JOIN", &IRCHandler::JoinHandler },
    { "352", &IRCHandler::WhoHandler },
    { "319", &IRCHandler::WhoisHandler }
};

IRCHandler::IRCHandler(QObject* parent) : QTcpSocket(parent)
{
    _authed = false;
    _initDatabaseLoading = true;
    connect(this, SIGNAL(readyRead()), this, SLOT(ReadSocket()));
    connectToHost("irc.fi.quakenet.org", 6667);

    _database = QSqlDatabase::addDatabase("QMYSQL");
    _database.setHostName("localhost");
    _database.setUserName("gather");
    _database.setPassword("Gather");
    _database.setDatabaseName("GatherBot");

    bool connected = _database.open();

    if (!connected)
        qDebug() << "Database failed to open up connection";

    Player temp(PREMISSION_ALL);
    QString host("sYCH3L!sYCH3L@Enari.users.quakenet.org");
    temp.SetCombinedHostname(host);
    _players.insert("sYCH3L", temp); // master user - shall not be removed
}

void IRCHandler::PerformMasterCleanup()
{
    qDebug() << "Nooo program was terminated";
}


void IRCHandler::PingHandler(Slicer &data)
{
    data.str->replace(0, 4, "PONG");
    write(data.str->toAscii());
}

// People with INVITE premission can access this
void IRCHandler::InviteHandler(Slicer &data)
{
    PlayerMap::Iterator itr = _players.find(data.list->at(0));
    if (itr != _players.end() && (itr.value().HasPremissions(PREMISSION_INVITE)))
    {
        QString join("JOIN ");
        join += data.list->at(3);
        join += "\r\n";
        _activeChannel = QString(data.list->at(3)); // makes sure that bot only works in one channel, also used in PRIVMSG handling
        write(join.toAscii());
    }
}

void IRCHandler::JoinHandler(Slicer &/*data*/)
{
    QString who("WHO ");
    who += _activeChannel;
    who += "\r\n";
    write(who.toAscii());
}

void IRCHandler::WhoHandler(Slicer &data)
{
    // load users from database
    if (_initDatabaseLoading)
    {
        QSqlQuery query = QSqlQuery(_database);

        if (query.exec("SELECT * FROM Users"))
            while (query.next())
                _players.insert(query.value(0).toString(), query.value(1).toInt());

        _initDatabaseLoading = false; // makes sure that it doesn't get executed twice
    }

    for (int i = 0; i < data.list->size(); ++i)
    {
        // ":Lighto!~Neo@vlan122-214-70.tartu.stv.ee"
        if (data.list->at(i) == ":3")
        {
            QString user (data.list->at(i - 5));
            QString combined(":");
            combined += data.list->at(i - 2); // nick
            combined += "!";
            combined += user; // user
            combined += "@";
            combined += data.list->at(i - 4); // host

            PlayerMap::Iterator itr = _players.find(user);

            if (itr != _players.end())
                itr.value().SetCombinedHostname(combined);
        }
    }
}

void IRCHandler::WhoisHandler(Slicer &data)
{
    data.list->at(17); // user
}

void IRCHandler::ReadSocket()
{
    QString* data = new QString(this->readAll());

    qDebug() << *(data);

    if (!_authed)
    {
        if (data->contains(QString("NOTICE AUTH :*** Checking Ident")))
        {
            write("NICK RPSGbot\r\nUSER RPSGbot RPSGbot irc.fi.quakenet.org :Written by sYCH3L - BETA ver\r\n");
            _authed = true;
            delete data;
        }
    }
    else
    {
        Slicer combined;
        combined.list = new QStringList(data->split(QRegExp("\\s")));
        combined.str = data;

        for (int i = 0; i < CONNECTORS; ++i)
        {
            if (mass[i].command == combined.list->at(0) || mass[i].command == combined.list->at(1))
            {
                (*this.*mass[i].handler)(combined);
                combined.Cleanup(); // should be removed IF multithreaded handlers are implemented
                break;
            }
        }
    }
}

