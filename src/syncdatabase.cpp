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

#include "syncdatabase.h"
#include "syncfile.h"

#include <QString>
#include <QVariant>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>

SyncDatabase::SyncDatabase(int page_id, QString * temp_path)
{
    this->page_id = page_id;
    this->temp_path = new QString(*temp_path);
    this->db = NULL;

    createDatabase();
}

SyncDatabase::~SyncDatabase()
{
    delete temp_path;

    if (db) {
        db->close();
        delete db;
    }
}

bool SyncDatabase::createDatabase()
{
    QDir dir(*temp_path);
    if (!dir.cd("syncs")) {
        if (!dir.mkdir("syncs") || !dir.cd("syncs"))
            return false;
    }

    QString sync_name = QString("sync%1.syncdb").arg(page_id);
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", sync_name));
    db->setDatabaseName(dir.absoluteFilePath(sync_name));
    if (!db->open()) {
        delete db;
        db = NULL;

        return false;
    }

    if (!db->tables().contains("content")) {
        QSqlQuery(*db).exec("CREATE TABLE content (id INTEGER, parent_id INTEGER, file_name TEXT, last_modified TEXT)");
    }
    db->commit();

    return true;
}

void SyncDatabase::setupRootSyncFile(SyncFile * sf)
{
    sf->setIndex(0);
    int num_records = 0;

    QSqlQuery query(*db);

    if (!query.exec(QString("SELECT COUNT(id) AS count FROM content")))
        return;

    if (query.next()) {
        num_records = query.value(0).toInt();
    }

    SyncFile ** sf_array = new SyncFile*[num_records];
    sf_array[0] = sf;

    if (!query.exec(QString("SELECT id, parent_id, file_name, last_modified FROM content ORDER BY id")))
        return;

    while (query.next()) {
        sf = sf_array[query.value(1).toInt()]->addChild(query.value(2).toString(), query.value(0).toInt());

        sf->setLastModified(query.value(3).toString());
        sf_array[sf->index()] = sf;
    }
    delete sf_array;
}

void SyncDatabase::saveSyncFile(SyncFile * sf)
{
    for (int i = 0; i < sf->childCount(); ++i) {
        saveSyncFile(sf->childAt(i), sf->index());
    }
    db->commit();
}

void SyncDatabase::saveSyncFile(SyncFile * sf, int parent_id)
{
    if (!sf->isInDatabase()) {
        QSqlQuery(*db).exec(QString("INSERT INTO content (id, parent_id, file_name, last_modified) VALUES (%1, %2, '%3', '%4')")
                            .arg(sf->index())
                            .arg(parent_id)
                            .arg(sf->getName())
                            .arg(sf->lastModifiedString()));
    } else if (sf->wasModified() && sf->wasDeleted()) {
        removeRecords(sf->index());
        return;
    } else if (sf->wasModified()) {
        QSqlQuery(*db).exec(QString("UPDATE content SET parent_id = %1, file_name = '%2', last_modified = '%3' WHERE id = %4")
                            .arg(parent_id)
                            .arg(sf->getName())
                            .arg(sf->lastModifiedString())
                            .arg(sf->index()));
    }
    for (int i = 0; i < sf->childCount(); ++i) {
        saveSyncFile(sf->childAt(i), sf->index());
    }
}

void SyncDatabase::removeRecords(int r_id)
{
    QSqlQuery query(*db);
    query.exec(QString("DELETE FROM content WHERE id = %1").arg(r_id));

    if (!query.exec(QString("SELECT id FROM content WHERE parent_id = %1").arg(r_id)))
        return;

    while (query.next()) {
        removeRecords(query.value(0).toInt());
    }
}
