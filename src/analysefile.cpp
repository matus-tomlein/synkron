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

#include "analysefile.h"

AnalyseFile::AnalyseFile(const QString name)
    : SyncFile(name)
{
    this->num_deleted = 0;
    this->num_not_found = 0;
    this->num_obsolete = 0;
}

SyncFile * AnalyseFile::addNewChild(const QString & child_name, int child_id)
{
    SyncFile * child = new AnalyseFile(child_name);
    child->setIndex(child_id);
    addSyncFile(child);
    return child;
}

void AnalyseFile::increaseNumDeleted()
{
    this->num_deleted++;
}

void AnalyseFile::increaseNumNotFound()
{
    this->num_not_found++;
}

void AnalyseFile::increaseNumObsolete()
{
    this->num_obsolete++;
}

int AnalyseFile::numDeleted()
{
    return this->num_deleted;
}

int AnalyseFile::numNotFound()
{
    return this->num_not_found;
}

int AnalyseFile::numObsolete()
{
    return this->num_obsolete;
}

int AnalyseFile::numNotSynced()
{
    return this->num_deleted + this->num_not_found + this->num_obsolete;
}
