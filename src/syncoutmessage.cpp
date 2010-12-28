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

#include "syncoutmessage.h"
#include "folderactiongroup.h"

#include <QString>

SyncOutMessage::SyncOutMessage(SyncOutMessageAction message_type, FolderActionGroup * fag, bool error, const QString & error_str_arg)
{
    this->message_type = message_type;
    this->fag = fag;
    this->error = error;
    this->error_str = error_str_arg;
}

const QString & SyncOutMessage::firstString()
{
    return fag->first();
}

int SyncOutMessage::firstFolderId()
{
    return fag->firstId();
}

const QString & SyncOutMessage::secondString()
{
    return fag->last();
}

int SyncOutMessage::secondFolderId()
{
    return fag->lastId();
}

void SyncOutMessage::labelFolder(int f_i, int length, const QString & label)
{
    QString * str = &((*fag)[f_i]);
    str->remove(0, length);
    str->prepend(label);
}

int SyncOutMessage::folderCount()
{
    return fag->count();
}

int SyncOutMessage::folderIdAt(int i)
{
    return fag->idAt(i);
}

const QString & SyncOutMessage::stringAt(int i)
{
    return fag->at(i);
}
