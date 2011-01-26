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
    if (db) delete db;
}

bool SyncDatabase::createDatabase()
{
    QDir dir(*temp_path);
    if (!dir.cd("syncs")) {
        if (!dir.mkdir("syncs") || !dir.cd("syncs"))
            return false;
    }

    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    db->setDatabaseName(dir.absoluteFilePath(QString("sync%1.syncdb").arg(page_id)));
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
    setupSyncFile(sf);
}

void SyncDatabase::setupSyncFile(SyncFile * parent_sf)
{
    QSqlQuery query(*db);
    SyncFile * sf;

    if (!query.exec(QString("SELECT id, file_name, last_modified FROM records WHERE parent_id = '%1'").arg(parent_sf->index())))
        return;

    while (query.next()) {
        sf = parent_sf->addChild(query.value(2).toString());
        sf->setIndex(query.value(0).toInt());
        sf->setLastModified(query.value(3).toString());
        setupSyncFile(sf);
    }
}
