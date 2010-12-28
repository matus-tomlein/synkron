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

#include "folders.h"
#include "folder.h"
#include "settings.h"
#include "folderactiongroup.h"

#include <QList>

Folders::Folders()
{
}

QStringList Folders::allFoldersPaths()
{
    QStringList paths;
    for (int i = 0; i < folders.count(); ++i) {
        paths.append(folders.at(i)->path());
    }
    return paths;
}

/**
  * Calls each folder to save its settings in the folders group.
  */
void Folders::save(Settings * settings)
{
    settings->beginGroup("folders");
    settings->remove("");

    for (int i = 0; i < folders.count(); ++i) {
        folders.at(i)->save(settings);
    }

    settings->endGroup();
}

/**
  * Creates folders and calls them to load their settings from the folders group.
  */
void Folders::load(Settings * settings)
{
    settings->beginGroup("folders");

    QStringList folder_groups = settings->childGroups();
    for (int i = 0; i < folder_groups.count(); ++i) {
        int f_id = folder_groups.at(i).toInt();

        Folder * folder = addFolder(f_id);
        folder->load(settings);
    }

    settings->endGroup();
}

/**
  * Deletes folder with the given id.
  */
void Folders::closeFolder(int f_id)
{
    delete folders.takeAt(folderListIndex(f_id));
}

/**
  * Returns index of folder in the folders list based on the folder id
  */
int Folders::folderListIndex(int f_id)
{
    for (int i = 0; i < folders.count(); ++i) {
        if (folders.at(i)->index() == f_id) return i;
    }
    return -1;
}

/**
  * Returns number of folders in the folders list
  */
int Folders::count()
{
    return folders.count();
}

/**
  * Returns a folder at position i.
  */
Folder * Folders::at(int i)
{
    return folders.at(i);
}

Folder * Folders::first()
{
    return folders.first();
}

Folder * Folders::byId(int id)
{
    for (int i = 0; i < folders.count(); ++i) {
        if (folders.at(i)->index() == id)
            return folders.at(i);
    }

    return NULL;
}

/**
  * Returns path of the folder at position i.
  */
const QString Folders::pathAt(int i)
{
    return folders.at(i)->path();
}

/**
  * Creates a FolderActionGroup object which is mainly used in synchronisation.
  * This object contains folder paths and their indexes.
  */
FolderActionGroup * Folders::folderActionGroup()
{
    FolderActionGroup * fag = new FolderActionGroup;
    for (int i = 0; i < folders.count(); ++i) {
        fag->insert(folders.at(i)->index(), folders.at(i)->path());
    }
    return fag;
}
