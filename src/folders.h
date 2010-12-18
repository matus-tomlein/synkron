/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2010 Matus Tomlein (matus.tomlein@gmail.com)

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

#ifndef FOLDERS_H
#define FOLDERS_H

class Folder;

class Settings;
class FolderActionGroup;

#include <QStringList>

class Folders
{
public:
    Folders();

    QStringList allFoldersPaths();

    virtual Folder * addFolder(int) = 0;

    void save(Settings *);
    void load(Settings *);
    void closeFolder(int);
    int count();

    Folder * at(int);
    const QString pathAt(int);
    FolderActionGroup * folderActionGroup();

private:
    int folderListIndex(int);

protected:
    QList<Folder*> folders;
};

#endif // FOLDERS_H