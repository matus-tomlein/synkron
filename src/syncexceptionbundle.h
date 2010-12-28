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

#ifndef SYNCEXCEPTIONBUNDLE_H
#define SYNCEXCEPTIONBUNDLE_H

#include "abstractexceptionbundle.h"

class QStringList;
class QString;
class QRegExp;
class FolderActionGroup;
class ExceptionFile;
class ExceptionGroup;

class SyncExceptionBundle : public AbstractExceptionBundle
{
public:
    SyncExceptionBundle();
    ~SyncExceptionBundle();

    void importBundle(AbstractExceptionBundle *);

    ExceptionFile * updateRootFolder(FolderActionGroup *);
    ExceptionFile * lastFile();
    bool cdAndCheck(const QString &);
    void cdUp();

    bool isInBlacklist(const QString &, bool = false);

    const QStringList & filters();

private:
    void importGroup(ExceptionGroup *);

    void updateFolder(QStringList *);
    QStringList * makeRelative(const QStringList *, FolderActionGroup *);
    void restartExceptionFiles();
    bool cd(const QString &);

    QStringList * filter_extensions;
    QRegExp * reg_exp;

    QList<ExceptionFile *> * exception_files;
    int num_files_out;
};

#endif // SYNCEXCEPTIONBUNDLE_H
