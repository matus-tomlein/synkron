#ifndef BACKUPACTION_H
#define BACKUPACTION_H

#include <QObject>

class QString;
class QDir;
class MTFile;

class BackupAction : public QObject
{
    Q_OBJECT

public:
    BackupAction(QString *);
    ~BackupAction();

    bool backupFile(MTFile *);

private:
    QDir * temp;
    QString * time_str;

    int num_backed_up;

signals:
    void fileBackedUp(const QString &, int, const QString &);
    void commit();
};

#endif // BACKUPACTION_H
