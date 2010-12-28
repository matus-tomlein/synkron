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

#include "exceptionfile.h"

#include <QStringList>

ExceptionFile::ExceptionFile(const QString name, bool is_dir)
{
    this->name = name;
    this->is_dir = is_dir;
    blacklisted = false;
    children = new QList<ExceptionFile *>;
    wildcards = NULL;
}

ExceptionFile * ExceptionFile::addChild(const QString & child_name, bool child_dir)
{
    for (int i = 0; i < children->count(); ++i) {
        if (children->at(i)->getName() == child_name) return children->at(i);
    }

    ExceptionFile * child = new ExceptionFile(child_name, child_dir);
    children->append(child);

    return child;
}

ExceptionFile * ExceptionFile::child(const QString & child_name)
{
    for (int i = 0; i < children->count(); ++i) {
        if (children->at(i)->getName() == child_name)
            return children->at(i);
    }

    return NULL;
}

int ExceptionFile::childCount()
{
    return children->count();
}

void ExceptionFile::addWildcard(const QString card)
{
    if (!wildcards) wildcards = new QStringList();

    wildcards->append(card);
}

const QString & ExceptionFile::wildcardAt(int i)
{
    return wildcards->at(i);
}

int ExceptionFile::wildcardCount()
{
    if (!wildcards) return 0;

    return wildcards->count();
}
