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

#include "exceptionsview.h"
#include "exceptions.h"
#include "exceptionbundle.h"

#include <QTreeWidgetItem>

ExceptionsView::ExceptionsView(QTreeWidgetItem * nav_parent_item, Exceptions * exceptions)
{
    this->nav_parent_item = nav_parent_item;
    this->exceptions = exceptions;

    bundles_map = new QMap<QTreeWidgetItem *, ExceptionBundle *>;
}
/**
  * Creates a new ExceptionBundle object as well as a trewidgetitem and adds them to the bundles_map map.
  */
void ExceptionsView::addBundle()
{
    addBundle(exceptions->addBundle());
}

void ExceptionsView::addBundle(ExceptionBundle * bundle)
{
    QTreeWidgetItem * item = newBundleItem(bundle);
}

/**
  * Creates a new navigation item for the given bundle.
  */
QTreeWidgetItem * ExceptionsView::newBundleItem(ExceptionBundle * bundle)
{
    QTreeWidgetItem * item = new QTreeWidgetItem();
    nav_parent_item->addChild(item);

    item->setText(0, bundle->value("name").toString());
    item->setToolTip(0, bundle->value("name").toString());
    //item->setIcon(0, QIcon(bundle->iconPath()));

    return item;
}

void ExceptionsView::navigationItemClicked(QTreeWidgetItem * item)
{
    if (!bundles_map->contains(item)) return;

    ExceptionBundle * bundle = bundles_map->value(item);
}
