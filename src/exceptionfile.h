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

#ifndef EXCEPTIONFOLDER_H
#define EXCEPTIONFOLDER_H

#include <QString>

class ExceptionFile
{
public:
    ExceptionFile(const QString, bool = false);

    bool isBlacklisted() { return blacklisted; }
    void setBlacklisted(bool b) { blacklisted = b; }

    ExceptionFile * addChild(const QString &, bool = false);
    ExceptionFile * child(const QString &);
    int childCount();

    const QString & getName() { return name; }
    void setName(const QString n) { name = n; }

    void addWildcard(const QString);
    int wildcardCount();
    const QString & wildcardAt(int);

private:
    QString name;
    bool is_dir;
    bool blacklisted;

    QList<ExceptionFile *> * children;
    QStringList * wildcards;
};

#endif // EXCEPTIONFOLDER_H
