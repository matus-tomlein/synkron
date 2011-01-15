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

#include "backupaction.h"
#include "mtfile.h"

#include <QString>
#include <QDateTime>

BackupAction::BackupAction(QString * tp)
{
    temp = new QDir(*tp);
    time_str = new QString(QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss"));
    num_backed_up = -1;
}

BackupAction::~BackupAction()
{
    emit commit();

    delete temp;
    delete time_str;
}

bool BackupAction::backupFile(MTFile * file)
{
    if (num_backed_up == -1) {
        if (!temp->mkdir(*time_str) || !temp->cd(*time_str)) {
            return false;
        }
        num_backed_up = 0;
    }

    if (!file->copy(temp->absoluteFilePath(QString("%3.%4")
                            .arg(QFileInfo(*file).fileName())
                            .arg(num_backed_up)))) {
        return false;
    }

    emit fileBackedUp(file->fileName(), num_backed_up, *time_str);

    num_backed_up++;
    return true;
}
