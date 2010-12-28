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

#ifndef ABSTRACTSYNCADVANCEDVIEW_H
#define ABSTRACTSYNCADVANCEDVIEW_H

#include <QObject>
#include <QString>
template<class Key, class T>
class QMap;

class AbstractSyncPage;
class QTreeWidget;
class QTreeWidgetItem;
class ExceptionBundle;

class AbstractSyncAdvancedView : public QObject
{
    Q_OBJECT

public:
    AbstractSyncAdvancedView(AbstractSyncPage *, QTreeWidget *);
    ~AbstractSyncAdvancedView();

    static void staticInitializeItem(QTreeWidgetItem *&, QTreeWidgetItem *, const QString &, bool = false);

public slots:
    void addBundle(ExceptionBundle *);
    void removeBundle(int);
    void changeBundle(ExceptionBundle *);

private:
    AbstractSyncPage * page;
    QTreeWidget * tree;
    QMap<QString, QTreeWidgetItem *> * items_by_name_map;
    QMap<int, QTreeWidgetItem *> * exception_items_map;

    QTreeWidgetItem * general_item;
    QTreeWidgetItem * sync_hidden_item;
    QTreeWidgetItem * no_empty_folders_item;
    QTreeWidgetItem * sync_no_subdirs_item;
    QTreeWidgetItem * no_backup_item;
    QTreeWidgetItem * update_only_item;
    QTreeWidgetItem * propagate_deletions_item;
    QTreeWidgetItem * detect_collisions_item;
    QTreeWidgetItem * exceptions_item;

    void populateAdvancedTree();
    void initializeItem(QTreeWidgetItem *&, QTreeWidgetItem *, const QString &, const QString &);
    void loadExceptionBundles();

private slots:
    void itemClicked(QTreeWidgetItem *, int);
};

#endif // ABSTRACTSYNCADVANCEDVIEW_H
