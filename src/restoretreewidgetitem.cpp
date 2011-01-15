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

#include "restoretreewidgetitem.h"

#include "backupdatabaserecord.h"

RestoreTreeWidgetItem::RestoreTreeWidgetItem(QTreeWidget * tree)
    : QTreeWidgetItem(tree)
{
    db_record = NULL;
}

RestoreTreeWidgetItem::RestoreTreeWidgetItem(BackupDatabaseRecord * db_record, QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent)
{
    this->db_record = db_record;

    setupFromDatabaseRecord();
}

RestoreTreeWidgetItem::~RestoreTreeWidgetItem()
{
    if (db_record)
        delete db_record;
}

void RestoreTreeWidgetItem::setupFromDatabaseRecord()
{
    setText(0, db_record->path());
    setToolTip(0, db_record->path());
    setCheckState(0, Qt::Unchecked);
    setToolTip(1, QObject::tr("Restore"));
    setIcon(1, QIcon(":/images/Restore16.png"));
    setToolTip(2, QObject::tr("Delete"));
    setIcon(2, QIcon(":/images/clean_temp_16.png"));
}

BackupDatabaseRecord * RestoreTreeWidgetItem::databaseRecord()
{
    return db_record;
}
