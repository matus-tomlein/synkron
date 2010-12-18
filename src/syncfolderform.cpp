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

#include "syncfolderform.h"
#include "ui_syncfolderform.h"
#include "folder.h"
#include "abstractsyncadvancedview.h"

#include <QTreeWidget>
#include <QFileDialog>
#include <QMessageBox>

/**
  * A Folder object is passed and stored inside the form.
  */
SyncFolderForm::SyncFolderForm(Folder * folder, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyncFolderForm)
{
    ui->setupUi(this);

    advanced_parent_item = new QTreeWidgetItem();
    items_by_name_map = new QMap<QString, QTreeWidgetItem *>;
    this->folder = folder;

    QObject::connect(ui->path_le, SIGNAL(textEdited(QString)), this, SLOT(pathChanged(QString)));
    QObject::connect(ui->label_le, SIGNAL(textEdited(QString)), this, SLOT(labelChanged(QString)));
    QObject::connect(ui->remove_btn, SIGNAL(clicked()), this, SLOT(removeFolder()));
    QObject::connect(ui->browse_btn, SIGNAL(clicked()), this, SLOT(browse()));

    load();
}

SyncFolderForm::~SyncFolderForm()
{
    for (int i = advanced_parent_item->childCount() - 1; i >= 0; --i) {
        delete advanced_parent_item->takeChild(i);
    }
    delete advanced_parent_item;
    delete items_by_name_map;

    delete ui;
}

void SyncFolderForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * Returns the id of the folder
  */
int SyncFolderForm::index()
{
    return folder->index();
}

/**
  * Slot triggered when the text inside path_le was edited
  */
void SyncFolderForm::pathChanged(QString p)
{
    folder->setPath(p);
}

/**
  * Slot triggered when the text inside label_le was edited
  */
void SyncFolderForm::labelChanged(QString l)
{
    folder->setLabel(l);
    advanced_parent_item->setText(0, l);
}

/**
  * Loads settings from the Folder object
  */
void SyncFolderForm::load()
{
    ui->path_le->setText(folder->path());
    ui->label_le->setText(folder->label());
}

/**
  * Creates advanced items in the tree
  */
void SyncFolderForm::makeYourselfAdvancedTreeItems(QTreeWidget * tree)
{
    advanced_tree = tree;

    tree->addTopLevelItem(advanced_parent_item);
    advanced_parent_item->setText(0, folder->label());
    advanced_parent_item->setExpanded(true);

    initializeItem(no_empty_folders_item, advanced_parent_item, "no_empty_folders", tr("Do not create empty folders"));
    initializeItem(dont_modify_item, advanced_parent_item, "dont_modify", tr("Do not modify the contents of this folder"));
    initializeItem(update_only_item, advanced_parent_item, "update_only", tr("Update existing files only"));
    initializeItem(dont_backup_item, advanced_parent_item, "dont_backup", tr("Do not backup updated files"));
    initializeItem(slave_item, advanced_parent_item, "slave", tr("Slave"));

    QObject::connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemClicked(QTreeWidgetItem*, int)));
}

void SyncFolderForm::initializeItem(QTreeWidgetItem *& item, QTreeWidgetItem * parent, const QString & name, const QString & text)
{
    AbstractSyncAdvancedView::staticInitializeItem(item, parent, text, folder->value(name).toBool());
    items_by_name_map->insert(name, item);
}

/**
  * Slot triggered when the user clicks on an item in the tree (clicks, not just changes it check state).
  * Saves the new value of the item into the page.
  */
void SyncFolderForm::itemClicked(QTreeWidgetItem * item, int)
{
    QString key = items_by_name_map->key(item);
    if (key.isNull()) return;
    folder->setValue(key, item->checkState(0) == Qt::Checked);
}

/**
  * Called when the browse button was clicked.
  */
void SyncFolderForm::browse()
{
    QString path = QFileDialog::getExistingDirectory(
            this,
            "Choose a directory",
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        ui->path_le->setText(path);
        pathChanged(path);
    }
}
