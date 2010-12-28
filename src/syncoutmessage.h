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

#ifndef SYNCOUTMESSAGE_H
#define SYNCOUTMESSAGE_H

#include <QString>

class FolderActionGroup;

class SyncOutMessage
{
public:
    enum SyncOutMessageAction {
        FileCopied,
        FileUpdated,
        FolderCreated,
        FileDeleted,
        FolderDeleted
    };

    SyncOutMessage(SyncOutMessageAction, FolderActionGroup *, bool = false, const QString & = QString());

    SyncOutMessageAction getType() { return message_type; }
    bool isErrorMessage() { return error; }
    const QString & firstString();
    int firstFolderId();
    const QString & secondString();
    int secondFolderId();
    const QString & errorString() { return error_str; }

    void labelFolder(int, int, const QString &);
    int folderCount();
    const QString & stringAt(int);
    int folderIdAt(int);

    FolderActionGroup * folderActionGroup() { return fag; }

private:
    QString error_str;
    FolderActionGroup * fag;
    bool error;
    SyncOutMessageAction message_type;
};

#endif // SYNCOUTMESSAGE_H
