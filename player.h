#ifndef PLAYER_H
#define PLAYER_H

#include <QHash>
#include <QString>

class IRCHandler;

enum Premissions
{
    PREMISSION_NONE = 0x0,
    PREMISSION_PLAYER = 0x1,
    PREMISSION_INVITE = 0x2,
    PREMISSION_ALL = PREMISSION_PLAYER | PREMISSION_INVITE
};

class Player
{
    friend class IRCHandler;

public:
    Player(int prems);

protected:
    int HasPremissions(Premissions perms) { return _prems & perms; }
    void SetPremissions(Premissions prems) { _prems |= prems; }
    void SetCombinedHostname(QString &host) { _combinedHostname = host; }
    const QString & GetCombinedHostname() { return _combinedHostname; }

private:
    unsigned int _prems;
    QString _combinedHostname;
};

typedef QHash<QString, Player> PlayerMap;

#endif // PLAYER_H
