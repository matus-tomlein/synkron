/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2011 Matus Tomlein (matus.tomlein@gmail.com)

 Synkron is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public Licence
 as published by the Free Software Foundation; either version 2
 of the Licence, or (at your option) any later version.

 Synkron is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public Licence for more details.

 You should have received a copy of the GNU General Public Licence
 along with Synkron; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
********************************************************************/

#ifndef BACKUPHANDLER_H
#define BACKUPHANDLER_H

#include <QObject>

class Settings;
class BackupAction;
class RestoreAction;
class BackupDatabaseRecord;

class QString;
class QSqlDatabase;

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
    QList<BackupDatabaseRecord *> * recordsByTime(const QString &);

    void restoreRecord(BackupDatabaseRecord *);
    void removeRecord(BackupDatabaseRecord *);

public slots:
    void record(const QString &, int, const QString &);
    void commit();

private slots:
    void restoreActionDone(BackupDatabaseRecord *);

private:
    bool createDatabase();
    void runRestoreAction(RestoreAction *);

    QString * temp_path;
    QString * last_shown;
    QSqlDatabase * db;

signals:
    void restoreActionsFinished();
    void actionFinished(BackupDatabaseRecord *);
    void actionFailed(BackupDatabaseRecord *);
};

#endif // BACKUPHANDLER_H
