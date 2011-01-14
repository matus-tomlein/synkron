#ifndef BACKUPHANDLER_H
#define BACKUPHANDLER_H

#include <QObject>

class Settings;
class BackupAction;
class QSqlDatabase;

class QString;

class BackupHandler : public QObject
{
    Q_OBJECT

public:
    BackupHandler();
    ~BackupHandler();

    bool load(Settings *);
    void save(Settings *);

    BackupAction * backupAction();

    QStringList * newDates();

public slots:
    void record(const QString &, int, const QString &);
    void commit();

private:
    bool createDatabase();

    QString * temp_path;
    QString * last_shown;
    QSqlDatabase * db;
};

#endif // BACKUPHANDLER_H
