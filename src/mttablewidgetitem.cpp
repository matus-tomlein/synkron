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

#include "mttablewidgetitem.h"
#include "syncoutmessage.h"

MTTableWidgetItem::MTTableWidgetItem(SyncOutMessage *sync_out_message) :
    QTableWidgetItem()
{
    loadSyncOutMessage(sync_out_message);
}

void MTTableWidgetItem::loadSyncOutMessage(SyncOutMessage * message)
{
    switch (message->getType()) {
    case SyncOutMessage::FileCopied:
        this->setText(QString("File %1 copied to %2").arg(message->firstString()).arg(message->secondString()));
        break;

    case SyncOutMessage::FileUpdated:
        this->setText(QString("File %1 updated from %2").arg(message->secondString()).arg(message->firstString()));
        break;

    case SyncOutMessage::FolderCreated:
        this->setText(QString("Folder %1 created").arg(message->firstString()));
        break;

    case SyncOutMessage::FileDeleted:
        this->setText(QString("File %1 deleted").arg(message->firstString()));
        break;

    case SyncOutMessage::FolderDeleted:
        this->setText(QString("Folder %1 deleted").arg(message->firstString()));
        break;

    }

}
