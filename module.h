#ifndef MODULE_H
#define MODULE_H

#include <QString>
#include <QVariant>

class Settings;
class AbstractSyncPage;
class SyncPage;
class Folder;
class Exceptions;

template<class Key, class T>
class QMap;

class Module : public QObject
{
    Q_OBJECT

public:
    Module(const QString &);

    QVariant value(const QString &);
    void setValue(const QString &, const QVariant &);

    QMapIterator<int, AbstractSyncPage *> * syncIterator();

    Exceptions * getExceptions() { return exceptions; }

    void save();
    void load();

public slots:
    SyncPage * addSync();
    SyncPage * addSync(int);
    void closeSync(int);
    Folder * addSyncFolder(int, int);
    void closeSyncFolder(int, int);

private:
    void setupSyncSettings(const QString &);

    Settings * sync_settings;
    Exceptions * exceptions;

    QMap<QString, QVariant> * settings_map;
    QMap<int, AbstractSyncPage *> * syncs;
};

#endif // MODULE_H
