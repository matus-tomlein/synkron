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

#include "abstractsyncadvancedview.h"
#include "abstractsyncpage.h"
#include "exceptionbundle.h"

#include <QTreeWidget>
#include <QMap>
#include <QMessageBox>

AbstractSyncAdvancedView::AbstractSyncAdvancedView(AbstractSyncPage * page, QTreeWidget * tree)
{
    this->page = page;
    this->tree = tree;

    items_by_name_map = new QMap<QString, QTreeWidgetItem *>;
    exception_items_map = new QMap<int, QTreeWidgetItem *>;

    populateAdvancedTree();

    QObject::connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemClicked(QTreeWidgetItem*, int)));

    QObject::connect(page, SIGNAL(exceptionBundleAdded(ExceptionBundle*)), this, SLOT(addBundle(ExceptionBundle*)));
    QObject::connect(page, SIGNAL(exceptionBundleChanged(ExceptionBundle*)), this, SLOT(changeBundle(ExceptionBundle*)));
    QObject::connect(page, SIGNAL(exceptionBundleRemoved(int)), this, SLOT(removeBundle(int)));

    loadExceptionBundles();
}

AbstractSyncAdvancedView::~AbstractSyncAdvancedView()
{
    for (int i = general_item->childCount() - 1; i >= 0; --i) {
        delete general_item->takeChild(i);
    }
    delete general_item;
    delete items_by_name_map;
}

/**
  * Creates the advanced option items and puts them in the tree. Also loads their check state from the page settings.
  */
void AbstractSyncAdvancedView::populateAdvancedTree()
{
    general_item = new QTreeWidgetItem(tree, QStringList("General"));
    general_item->setExpanded(true);

    initializeItem(sync_hidden_item, general_item, "sync_hidden", tr("Synchronise hidden files and folders"));
    initializeItem(no_empty_folders_item, general_item, "no_empty_folders", tr("Do not create empty folders"));
    initializeItem(sync_no_subdirs_item, general_item, "sync_no_subdirs", tr("Do not synchronise subdirectories"));
    initializeItem(no_backup_item, general_item, "no_backup", tr("Do not backup updated files"));
    initializeItem(update_only_item, general_item, "update_only", tr("Update existing files only"));
    initializeItem(propagate_deletions_item, general_item, "propagate_deletions", tr("Propagate deletions"));
    initializeItem(detect_collisions_item, general_item, "detect_collisions", tr("Detect collisions"));

    analyse_item = new QTreeWidgetItem(tree, QStringList("Analysis"));
    analyse_item->setExpanded(true);

    initializeItem(analyse_changed_only_item, analyse_item, "analyse_changed_only", tr("List files which need to be synchronised only"));

    exceptions_item = new QTreeWidgetItem(tree, QStringList(tr("Exceptions")));
    exceptions_item->setExpanded(true);
}

/**
  * Slot triggered when the user clicks on an item in the tree (clicks, not just changes it check state).
  * Saves the new value of the item into the page.
  */
void AbstractSyncAdvancedView::itemClicked(QTreeWidgetItem * item, int)
{
    if (item->parent() && item->parent() == exceptions_item) { // Exception clicked
        page->checkExceptionBundle(exception_items_map->key(item), item->checkState(0) == Qt::Checked);
    } else { // General setting clicked
        QString key = items_by_name_map->key(item);
        if (key.isNull()) return;
        page->setValue(key, item->checkState(0) == Qt::Checked);
    }
}

/**
  * Called when creating the items in the advances tree. Creates an option item with the given parent, name and text.
  */
void AbstractSyncAdvancedView::initializeItem(QTreeWidgetItem *& item, QTreeWidgetItem * parent, const QString & name, const QString & text)
{
    AbstractSyncAdvancedView::staticInitializeItem(item, parent, text, page->value(name).toBool());
    items_by_name_map->insert(name, item);
}

/**
  * Public static function. Creates a QTreeWidgetItem and names it.
  */
void AbstractSyncAdvancedView::staticInitializeItem(QTreeWidgetItem *& item, QTreeWidgetItem * parent, const QString & text, bool check)
{
    item = new QTreeWidgetItem(parent, QStringList(text));
    item->setToolTip(0, text);
    item->setCheckState(0, check ? Qt::Checked : Qt::Unchecked);
}

void AbstractSyncAdvancedView::addBundle(ExceptionBundle * bundle)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(exceptions_item, QStringList(bundle->value("name").toString()));
    item->setCheckState(0, page->exceptionBundleChecked(bundle->index()) ? Qt::Checked : Qt::Unchecked);
    exception_items_map->insert(bundle->index(), item);
}

void AbstractSyncAdvancedView::removeBundle(int b_id)
{
    delete exception_items_map->value(b_id);
}

void AbstractSyncAdvancedView::changeBundle(ExceptionBundle * bundle)
{
    exception_items_map->value(bundle->index())->setText(0, bundle->value("name").toString());
}

void AbstractSyncAdvancedView::loadExceptionBundles()
{
    QList<int> ids = page->exceptionBundleIds();
    for (int i = 0; i < ids.count(); ++i) {
        addBundle(page->exceptionBundle(ids.at(i)));
    }
}
