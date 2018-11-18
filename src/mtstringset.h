/*******************************************************************
 This file is part of Synkron
 Copyright (C) 2005-2009 Matus Tomlein (matus.tomlein@gmail.com)

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

#ifndef MTSTRINGSET_H
#define MTSTRINGSET_H

#include <QStringList>

class MTStringSet : public QStringList
{
public:
    MTStringSet(): QStringList() {};
    MTStringSet(const QString & str): QStringList(str) {};
    MTStringSet(const QStringList & other): QStringList(other) {};
    MTStringSet(const QList<QString> & other): QStringList(other) {};
    MTStringSet(const MTStringSet & other): QStringList(*((QStringList *)&other)) {};
    //virtual ~MTStringSet() { this->QStringList::~QStringList(); };

    void append(const QString & str) {
        if (!contains(str)) { this->QStringList::append(str); }
    };

    inline QStringList & operator<<(const QString & str)
        { append(str); return *this; };
};

#endif // MTSTRINGSET_H
