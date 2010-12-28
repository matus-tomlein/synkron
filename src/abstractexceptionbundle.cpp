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

#include "abstractexceptionbundle.h"

#include "fileblacklistgroup.h"
#include "folderblacklistgroup.h"
#include "filtergroup.h"

AbstractExceptionBundle::AbstractExceptionBundle()
{
    exception_groups = new QList<ExceptionGroup *>;

    // ExceptionGroups +++
    ExceptionGroup * file_blacklist_group = new FileBlacklistGroup();
    ExceptionGroup * folder_blacklist_group = new FolderBlacklistGroup();
    ExceptionGroup * filter_group = new FilterGroup();
    exception_groups->append(file_blacklist_group);
    exception_groups->append(folder_blacklist_group);
    exception_groups->append(filter_group);
    // ExceptionGroups ---
}

AbstractExceptionBundle::~AbstractExceptionBundle()
{
    for (int i = 0; i < exception_groups->count(); ++i) {
        delete exception_groups->takeAt(i);
    }

    delete exception_groups;
}

ExceptionGroup * AbstractExceptionBundle::groupByType(int type)
{
    for (int i = 0; i < exception_groups->count(); ++i) {
        if (exception_groups->at(i)->value("type").toInt() == type) return exception_groups->at(i);
    }
    return NULL;
}
