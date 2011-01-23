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

#include "restoreaction.h"
#include "backupdatabaserecord.h"
#include "mtfile.h"

RestoreAction::RestoreAction(QList<BackupDatabaseRecord *> * records, QString * temp_path, int action_type)
{
    this->records = records;
    this->action_type = action_type;
    this->temp_dir = new QDir(*temp_path);
}

RestoreAction::RestoreAction(BackupDatabaseRecord * record, QString * temp_path, int action_type)
{
    records = new QList<BackupDatabaseRecord *>;
    records->append(record);
    this->action_type = action_type;
    this->temp_dir = new QDir(*temp_path);
}

RestoreAction::~RestoreAction()
{
    delete records;
    delete temp_dir;
}

void RestoreAction::start()
{
    for (int i = 0; i < records->count(); ++i) {
        if (runRecordAction(records->at(i)))
            emit itemDone(records->at(i));
        else
            emit itemFailed(records->at(i));
    }

    emit finished();
}

bool RestoreAction::runRecordAction(BackupDatabaseRecord * record)
{
    switch (action_type) {
    case Restore:
        return restoreRecord(record);
        break;

    case Delete:
        return deleteRecord(record);
        break;
    }
}

bool RestoreAction::restoreRecord(BackupDatabaseRecord * record)
{
    if (!temp_dir->cd(record->time())) {
        return false;
    }

    MTFile file(record->path());
    QFileInfo original_fi(file);
    MTFile temp_file(temp_dir->absoluteFilePath(QString("%1.%2")
                                                .arg(original_fi.fileName())
                                                .arg(record->syncIndex())));
    temp_dir->cdUp();

    if (!temp_file.exists()) {
        return false;
    }

    if (original_fi.exists()) {
        if (!temp_file.copy(QString("%1.res").arg(original_fi.absoluteFilePath()))) {
            return false;
        }
        if (!file.remove()) {
            return false;
        }
        file.setFileName(QString("%1.res").arg(original_fi.absoluteFilePath()));
        if (!file.rename(original_fi.absoluteFilePath())) {
            return false;
        }
        file.touch(qApp);
    }
    else {
        if (!original_fi.dir().exists()) {
            if (!QDir().mkpath(original_fi.dir().absolutePath())) {
                return false;
            }
        }
        if (!temp_file.copy(original_fi.absoluteFilePath())) {
            return false;
        }
    }
    temp_file.remove();

    temp_dir->rmdir(record->time());
    return true;
}

bool RestoreAction::deleteRecord(BackupDatabaseRecord * record)
{
    if (!temp_dir->cd(record->time())) {
        return true;
    }

    QFileInfo original_fi(record->path());
    MTFile temp_file(temp_dir->absoluteFilePath(QString("%1.%2")
                                                .arg(original_fi.fileName())
                                                .arg(record->syncIndex())));
    temp_dir->cdUp();

    if (temp_file.exists()) {
        if (!temp_file.remove()) {
            return false;
        }
    }

    temp_dir->rmdir(record->time());
    return true;
}
