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

#ifndef RESTOREACTION_H
#define RESTOREACTION_H

#include <QObject>
#include <QDir>

#include "syncthreadaction.h"

class BackupDatabaseRecord;

class RestoreAction : public QObject, public SyncThreadAction
{
    Q_OBJECT

public:
    RestoreAction(BackupDatabaseRecord *, QString *, int);
    RestoreAction(QList<BackupDatabaseRecord *> *, QString *, int);
    ~RestoreAction();

    enum RestoreActionType { Restore, Delete };

    void start();

private:
    bool restoreRecord(BackupDatabaseRecord *);
    bool deleteRecord(BackupDatabaseRecord *);
    bool runRecordAction(BackupDatabaseRecord *);

    int action_type;
    QList<BackupDatabaseRecord *> * records;
    QDir * temp_dir;

signals:
    void itemDone(BackupDatabaseRecord *);
    void itemFailed(BackupDatabaseRecord *);
    void finished();
};

#endif // RESTOREACTION_H
