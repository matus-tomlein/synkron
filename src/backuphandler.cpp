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

#include "backuphandler.h"

#include "settings.h"
#include "backupaction.h"
#include "backupdatabaserecord.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

BackupHandler::BackupHandler()
{
    temp_path = NULL;
    db = NULL;
    last_shown = NULL;
}

BackupHandler::~BackupHandler()
{
    if (db) {
        db->commit();
        db->close();
        delete db;
    }

    if (temp_path)
        delete temp_path;
}

bool BackupHandler::load(Settings * settings)
{
    temp_path = new QString(settings->value("temp_path", QString("%1/.Synkron").arg(QDir::homePath())).toString());

    return createDatabase();
}

bool BackupHandler::createDatabase()
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    db->setDatabaseName(QDir(*temp_path).absoluteFilePath("backup.syncdb"));
    if (!db->open()) {
        delete db;
        db = NULL;

        return false;
    }

    if (!db->tables().contains("records")) {
        QSqlQuery(*db).exec("CREATE TABLE records (sync_index INTEGER, path TEXT, time TEXT)");
    }
    db->commit();

    return true;
}

void BackupHandler::save(Settings * settings)
{
    settings->setValue("temp_path", *temp_path);
}

BackupAction * BackupHandler::backupAction()
{
    BackupAction * action = new BackupAction(temp_path);
    QObject::connect(action, SIGNAL(fileBackedUp(QString,int,QString)), this, SLOT(record(QString,int,QString)), Qt::QueuedConnection);
    QObject::connect(action, SIGNAL(commit()), this, SLOT(commit()), Qt::QueuedConnection);
    return action;
}

void BackupHandler::record(const QString & path, int index, const QString & time)
{
    QSqlQuery(*db).exec(QString("INSERT INTO records (path, sync_index, time) VALUES ('%1', '%2', '%3')")
                            .arg(path).arg(index).arg(time));
}

void BackupHandler::commit()
{
    db->commit();
}

QStringList * BackupHandler::newDates()
{
    QStringList * dates = new QStringList;
    QSqlQuery query(*db);

    QString query_str = "SELECT DISTINCT time FROM records";
    if (last_shown) {
        query_str.append(" WHERE time > '");
        query_str.append(*last_shown);
        query_str.append("'");
    }
    query_str.append(" ORDER BY time");
    if (!query.exec(query_str)) return dates;

    while (query.next()) {
        dates->append(query.value(0).toString());
    }

    if (dates->count()) {
        if (last_shown) delete last_shown;
        last_shown = new QString(dates->last());
    }

    return dates;
}

QList<BackupDatabaseRecord *> * BackupHandler::recordsByTime(const QString & time)
{
    QSqlQuery query(*db);
    QList<BackupDatabaseRecord *> * list = new QList<BackupDatabaseRecord *>;

    if (!query.exec(QString("SELECT path, sync_index FROM records WHERE time = '%1'").arg(time)))
        return NULL;

    while (query.next()) {
        list->append(new BackupDatabaseRecord(query.value(0).toString(), query.value(1).toInt()));
    }

    return list;
}
