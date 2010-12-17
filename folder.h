/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2010 Matus Tomlein (matus.tomlein@gmail.com)

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

#ifndef FOLDER_H
#define FOLDER_H

#include <QStringList>

template<class Key, class T>
class QMap;

class Settings;

class Folder
{
public:
    Folder(int);

    const QString path();
    void setPath(const QString &);

    const QString label();
    void setLabel(const QString &);

    void setValue(const QString &, const QVariant &);
    const QVariant value(const QString &);

    int index() { return id; }

    void save(Settings *);
    void load(Settings *);

private:
    int id;

    QMap<QString, QVariant> * settings_map;
};

#endif // FOLDER_H
