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

#include "filecompare.h"

#include <QFileInfo>
#include <QDateTime>

FileCompare::FileCompare()
{
    this->ignored_secs = 2;
}

FileCompare::FileCompare(int ignored_secs)
{
    this->ignored_secs = ignored_secs;
}

int FileCompare::compareTwoFiles(QFileInfo * fi1, QFileInfo * fi2)
{
    return compareTwoDates(fi1->lastModified(), fi2->lastModified());
}

int FileCompare::compareTwoDates(const QDateTime dt1, const QDateTime dt2)
{
    QString dt1_str = dt1.toString("yyyyMMddhhmmss");
    QString dt2_str = dt2.toString("yyyyMMddhhmmss");

    if (dt1_str == dt2_str) {
        return 0;
    }
    else if (dt1_str > dt2_str) {
        if (dt1.addSecs(0 - ignored_secs).toString("yyyyMMddhhmmss") > dt2_str) {
            return 1;
        }
    }
    else {
        if (dt1.addSecs(ignored_secs).toString("yyyyMMddhhmmss") < dt2_str) {
            return -1;
        }
    }
    return 0;
}
