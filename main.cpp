#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include "handler.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if(!(QFile("config.ini").exists()))
    {
        qDebug() << "Config file missing exiting";
        return -1;
    }
    QSettings settings("config.ini", QSettings::IniFormat);
    QStringList info;
    int port = 0;
    settings.beginGroup("info");
    const QStringList childKeys = settings.childKeys();
    foreach (const QString &childKey, childKeys)
    {
        if(childKey == "port")
        {
            port = settings.value(childKey).toInt();
        }
        info.append(settings.value(childKey).toString());
    }
    settings.endGroup();
    handler han;
    han.Prepear(info[1],info[0],info[2],info[3],port);
    return a.exec();
}
