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

#include "messagehandler.h"

#include "mttablewidgetitem.h"
#include "syncoutmessage.h"
#include "folders.h"
#include "folder.h"

#include <QHeaderView>
#include <QTime>

#define ROW_HEIGHT 22

MessageHandler::MessageHandler(Folders * folders, QTableWidget * log_tbl)
{
    this->folders = folders;
    this->log_tbl = log_tbl;

    this->log_tbl->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

void MessageHandler::logMessage(SyncOutMessage * msg)
{
    Folder * folder;
    QString rel_path;
    QStringList msg_labels;
    for (int i = 0; i < msg->folderCount(); ++i) {
        folder = folders->byId(msg->folderIdAt(i));
        if (!folder)
            continue;
        msg_labels << folder->label();

        if (!i)
            rel_path = msg->stringAt(i).mid(folder->path().size());
        //msg->labelFolder(i, folder->path().size(), folder->label());
    }

    addLogItem(msg, rel_path, msg_labels);

    delete msg;
}

void MessageHandler::addLogItem(SyncOutMessage * msg, const QString & rel_path, const QStringList & msg_labels)
{
    QString action;
    QString folders_item_str;
    log_tbl->insertRow(log_tbl->rowCount());
    log_tbl->setItem(log_tbl->rowCount() - 1, 0, new QTableWidgetItem(rel_path));

    switch (msg->getType()) {
    case SyncOutMessage::FileCopied:
        action = tr("Copied");
        folders_item_str = trUtf8("%1 \xe2\x9e\x94 %2").arg(msg_labels.first()).arg(msg_labels.last());
        break;

    case SyncOutMessage::FileUpdated:
        action = tr("Updated");
        folders_item_str = trUtf8("%1 \xe2\x9e\x94 %2").arg(msg_labels.first()).arg(msg_labels.last());
        break;

    case SyncOutMessage::FolderCreated:
        action = tr("Created");
        folders_item_str = tr("%1").arg(msg_labels.first());
        break;

    case SyncOutMessage::FileDeleted:
    case SyncOutMessage::FolderDeleted:
        action = tr("Deleted");
        folders_item_str = tr("%1").arg(msg_labels.first());
        break;
    }

    log_tbl->setRowHeight(log_tbl->rowCount() - 1, ROW_HEIGHT);
    log_tbl->setItem(log_tbl->rowCount() - 1, 1, new QTableWidgetItem(action));
    log_tbl->setItem(log_tbl->rowCount() - 1, 2, new QTableWidgetItem(folders_item_str));
}

void MessageHandler::syncStarted()
{
    QTableWidgetItem * item = new QTableWidgetItem(tr("%1	Synchronisation started").arg(QTime().currentTime().toString("hh:mm:ss")));
    item->setBackground(QBrush(Qt::yellow));
    insertSpannedItem(item);
}

void MessageHandler::insertSpannedItem(QTableWidgetItem * item)
{
    log_tbl->insertRow(log_tbl->rowCount());
    log_tbl->setSpan(log_tbl->rowCount() - 1, 0, 1, 3);
    log_tbl->setRowHeight(log_tbl->rowCount() - 1, ROW_HEIGHT);
    log_tbl->setItem(log_tbl->rowCount() - 1, 0, item);
}

void MessageHandler::syncFinished(int changed_count)
{
    QTableWidgetItem * item = new QTableWidgetItem(tr("%1	Synchronisation complete: %2 file(s) synchronised").arg(QTime().currentTime().toString("hh:mm:ss")).arg(changed_count));
    item->setBackground(QBrush(Qt::green));
    insertSpannedItem(item);
}

void MessageHandler::showSkippedMessage(int skipped_count)
{
    if (skipped_count > 0) {
        QTableWidgetItem * item = new QTableWidgetItem(tr("%1 files skipped").arg(skipped_count));
        item->setBackground(QBrush(Qt::darkGray));
        item->setForeground(QBrush(Qt::white));
        item->setIcon(QIcon(":/new/prefix1/images/file.png"));
        insertSpannedItem(item);
    }
}
