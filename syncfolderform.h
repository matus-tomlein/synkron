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

#ifndef SYNCFOLDERFORM_H
#define SYNCFOLDERFORM_H

#include <QWidget>

class Folder;
class QTreeWidget;
class QTreeWidgetItem;

namespace Ui {
    class SyncFolderForm;
}

class SyncFolderForm : public QWidget {
    Q_OBJECT
public:
    SyncFolderForm(Folder *, QWidget *parent = 0);
    ~SyncFolderForm();

    int index();
    void makeYourselfAdvancedTreeItems(QTreeWidget *);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SyncFolderForm *ui;
    Folder * folder;
    QMap<QString, QTreeWidgetItem *>* items_by_name_map;

    QTreeWidget * advanced_tree;
    QTreeWidgetItem * advanced_parent_item;
    QTreeWidgetItem * update_only_item;
    QTreeWidgetItem * dont_modify_item;
    QTreeWidgetItem * dont_backup_item;
    QTreeWidgetItem * no_empty_folders_item;
    QTreeWidgetItem * remove_item;
    QTreeWidgetItem * slave_item;

    void setAdvancedMenu();
    void load();
    void initializeItem(QTreeWidgetItem *&, QTreeWidgetItem *, const QString &, const QString &);

private slots:
    void removeFolder() { emit sigRemove(index()); }
    void pathChanged(QString);
    void labelChanged(QString);
    void itemClicked(QTreeWidgetItem *, int);
    void browse();

signals:
    void sigRemove(int);
};

#endif // SYNCFOLDERFORM_H
