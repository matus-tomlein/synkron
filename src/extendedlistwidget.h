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

#ifndef EXTENDEDLISTWIDGET_H
#define EXTENDEDLISTWIDGET_H

#include <QContextMenuEvent>
#include <QListWidget>

class ExtendedListWidget : public QListWidget
{
    Q_OBJECT

public:
    ExtendedListWidget(QWidget * parent = 0):
    QListWidget(parent) {};

signals:
    void sigconmenu(QPoint);

protected:
    void contextMenuEvent(QContextMenuEvent * event) {
        emit sigconmenu(event->globalPos());
    };
};

#endif // EXTENDEDLISTWIDGET_H
