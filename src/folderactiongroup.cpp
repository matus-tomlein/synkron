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

#include "folderactiongroup.h"

#include <QStringList>

FolderActionGroup::FolderActionGroup()
{
    init();
}

FolderActionGroup::FolderActionGroup(int i, const QString str)
{
    init();
    ids->append(i); paths->append(str);
}

FolderActionGroup::FolderActionGroup(int i1, const QString str1, int i2, const QString str2)
{
    init();
    ids->append(i1); paths->append(str1);
    ids->append(i2); paths->append(str2);
}

FolderActionGroup::~FolderActionGroup()
{
    delete ids;
    if (paths)
        delete paths;
}

void FolderActionGroup::init()
{
    paths = new QStringList;
    ids = new QList<int>;
}

void FolderActionGroup::insert(int id, const QString path)
{
    ids->append(id);
    paths->append(path);
}

const QString & FolderActionGroup::at(int i)
{
    return paths->at(i);
}

const QString & FolderActionGroup::byId(int i)
{
    return paths->at(ids->indexOf(i));
}

int FolderActionGroup::count()
{
    return ids->count();
}

/**
  * Returns id of the folder at index i in the list.
  */
int FolderActionGroup::idAt(int i)
{
    return ids->at(i);
}

const QString & FolderActionGroup::first()
{
    return paths->first();
}

const QString & FolderActionGroup::last()
{
    return paths->last();
}

int FolderActionGroup::firstId()
{
    return ids->first();
}

int FolderActionGroup::lastId()
{
    return ids->last();
}

QString & FolderActionGroup::operator[] (int i)
{
    return (*paths)[i];
}
