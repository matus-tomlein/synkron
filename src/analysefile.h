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

#ifndef ANALYSEFILE_H
#define ANALYSEFILE_H

#include "syncfile.h"

class AnalyseFile : public SyncFile
{
public:
    AnalyseFile(const QString);

    void increaseNumObsolete();
    void increaseNumDeleted();
    void increaseNumNotFound();

    int numObsolete();
    int numDeleted();
    int numNotFound();
    int numNotSynced();

private:
    SyncFile * addNewChild(const QString &, int = -1);

    int num_obsolete;
    int num_deleted;
    int num_not_found;
};

#endif // ANALYSEFILE_H
