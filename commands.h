#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QString>
#include <QChar>
#include <QQueue>
#include <QtDebug>
#include <QThread>
#include "statshandler.h"

class commands : public QObject, statshandler
{
    Q_OBJECT
public:
    commands(QObject *parent = 0 );
    virtual ~commands();
    void Prepear(QString chl, QString bn, QString gn, QString ip, int port);
    int firstRun;

signals:

protected:
    void commandHandler();
    void socketHandler(QString data);
    QTcpSocket *_socket;
    QString *_string;
    QTimer* _timer;
    QTimer* _delayTimer;
private:
    int _lCount;
    bool _bProcess;

private slots:
    void NextStep();
    void SendInfo();
    void ProceedProcess();
    void ProcessContents();
    void randz();


};

#endif // COMMANDS_H
