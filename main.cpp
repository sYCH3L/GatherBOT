#include <QCoreApplication>
#include <QSettings>
#include "handler.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSettings settings("config.ini", QSettings::IniFormat);
    QStringList info;
    settings.beginGroup("info");
    const QStringList childKeys = settings.childKeys();
    foreach (const QString &childKey, childKeys)
    {
        info.append(settings.value(childKey).toString());
    }
    settings.endGroup();
    handler han;
    han.Prepear(info[1],info[0],info[2]);
    return a.exec();
}
