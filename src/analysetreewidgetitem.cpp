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

#include "analysetreewidgetitem.h"
#include "analysefile.h"

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(QTreeWidget * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = NULL;
}

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = NULL;
}

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(AnalyseFile * sf, QTreeWidget * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = sf;
    setupFromAnalyseFile();
}

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(AnalyseFile * sf, QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = sf;
    setupFromAnalyseFile();
}

AnalyseFile * AnalyseTreeWidgetItem::syncFile()
{
    return sf;
}

void AnalyseTreeWidgetItem::setupFromAnalyseFile()
{
    if (sf->numNotSynced()) {
        setText(0, QString("%1 (%2)").arg(sf->getName()).arg(sf->numNotSynced()));
        setForeground(0, QBrush(Qt::red));
    } else {
        setText(0, sf->getName());
    }

    if (sf->isDir())
        setIcon(0, QIcon(":/images/folder_16.png"));
    else
        setIcon(0, QIcon(":/images/file.png"));
}
