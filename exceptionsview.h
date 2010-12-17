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

#ifndef EXCEPTIONSVIEW_H
#define EXCEPTIONSVIEW_H

#include <QObject>

class QTreeWidgetItem;
class Exceptions;
class ExceptionBundle;
template<class Key, class T>
class QMap;

class ExceptionsView : public QObject
{
    Q_OBJECT
public:
    ExceptionsView(QTreeWidgetItem *, Exceptions *);

public slots:
    void addBundle();
    void addBundle(ExceptionBundle *);
    void navigationItemClicked(QTreeWidgetItem *);

private:
    QTreeWidgetItem * nav_parent_item;
    Exceptions * exceptions;
    QMap<QTreeWidgetItem *, ExceptionBundle *> * bundles_map;

    QTreeWidgetItem * newBundleItem(ExceptionBundle *);
};

#endif // EXCEPTIONSVIEW_H
