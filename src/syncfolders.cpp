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

#include "syncfolders.h"

//SyncFolders

SyncFolders::SyncFolders(QWidget * parent):
    QWidget(parent)
{
    folders_list.clear();
    
    connect(this, SIGNAL(sigfolderschanged()), this, SLOT(foldersChanged()));
    
    QVBoxLayout * main_vlayout = new QVBoxLayout(this);
    main_vlayout->setMargin(0);
#ifdef Q_WS_MAC
    main_vlayout->setSpacing(3);
#else
    main_vlayout->setSpacing(6);
#endif
    /*QLabel * label1 = new QLabel(this);
    label1->setText(tr("<b>Sync folders:</b>"));
    main_vlayout->addWidget(label1);*/
    
    folders_vlayout = new QVBoxLayout;
    folders_vlayout->setMargin(0);
#ifdef Q_WS_MAC
    folders_vlayout->setSpacing(0);
#else
    folders_vlayout->setSpacing(6);
#endif
    addFolder();
    addFolder();
    main_vlayout->addLayout(folders_vlayout);
}

SyncFolder * SyncFolders::addFolder()
{
    SyncFolder * folder = new SyncFolder(this);
    connect(folder, SIGNAL(sigremove(SyncFolder *)), this, SLOT(removeFolderTimer(SyncFolder *)));
    folders_vlayout->addWidget(folder);
    folders_list << folder;
    folder->setLabel(tr("Folder %1").arg(count()));
    connect(folder, SIGNAL(sigedited()), this, SLOT(foldersEdited()));
    emit sigfolderschanged();
    return folder;
}

QStringList SyncFolders::pathsList()
{
    QStringList paths;
    for (int i = 0; i < folders_list.count(); ++i) {
        paths << folders_list.at(i)->path();
    }
    return paths;
}

QStringList SyncFolders::pathsAndLabelsList()
{
    QStringList pl_list;
    for (int i = 0; i < folders_list.count(); ++i) {
        pl_list << folders_list.at(i)->path() + "<:>" + folders_list.at(i)->label();
    }
    return pl_list;
}

void SyncFolders::loadPathsAndLabelsFromList(QStringList pl_list)
{
    QStringList path_and_label;
    SyncFolder * folder;
    for (int i = 0; i < pl_list.count(); ++i) {
        if (pl_list.at(i).isEmpty()) continue;
        path_and_label = pl_list.at(i).split("<:>");
        folder = addFolder();
        folder->setPath(path_and_label.first());
        if (path_and_label.count() > 1) {
            folder->setLabel(path_and_label.last());
        }
    }
    addToFolders(2);
}

void SyncFolders::removeAllFolders()
{
    for (int i = 0; i < folders_list.count();) {
        delete folders_list.takeAt(i);
    }
    folders_list.clear();
}

void SyncFolders::removeFolder(int i)
{
    delete folders_list.takeAt(i);
    emit sigfolderschanged();
    emit sigfoldersedited();
}

void SyncFolders::removeFolder()
{
    qApp->processEvents(QEventLoop::AllEvents);
    int i = folders_list.indexOf(current_folder);
    if (i >= 0)
        delete folders_list.takeAt(i);
    else
        delete current_folder;
    emit sigfolderschanged();
    emit sigfoldersedited();
}

void SyncFolders::foldersChanged()
{
    bool enable = true;
    if (folders_list.count() == 2) enable = false;
    for (int i = 0; i < folders_list.count(); ++i) {
        folders_list.at(i)->remove_act->setEnabled(enable);
    }
}

void SyncFolders::addToFolders(int n)
{
    for (int i = folders_list.count(); i < n; ++i) {
        addFolder();
    }
}

void SyncFolders::foldersEdited()
{
    emit sigfoldersedited();
}

//SyncFolder

SyncFolder::SyncFolder(QWidget * parent):
    QWidget(parent)
{
    QHBoxLayout * hlayout = new QHBoxLayout(this);
    hlayout->setMargin(0); hlayout->setSpacing(6);

    config_btn = new QPushButton(this);
    config_btn->setStatusTip(tr("Configure folder"));
    config_btn->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/configure16.png")));
    config_btn->setFlat(true);
#ifdef Q_WS_MAC
    config_btn->setMinimumSize(22, 22);
    config_btn->setMaximumSize(22, 22);
#else
    config_btn->setMinimumHeight(22);
    config_btn->setMaximumHeight(22);
#endif
    config_menu = new QMenu;
    no_empty_folders_act = new QAction(tr("Do not create empty folders"), this);
    no_empty_folders_act->setStatusTip(tr("Do not create empty folders"));
    no_empty_folders_act->setCheckable(true);
    config_menu->addAction(no_empty_folders_act);
    dont_update_act = new QAction(tr("Do not modify the contents of this folder"), this);
    dont_update_act->setCheckable(true);
    dont_update_act->setStatusTip(tr("Do not make any changes to this folder"));
    config_menu->addAction(dont_update_act);
    update_only_act = new QAction(tr("Update existing files only"), this);
    update_only_act->setCheckable(true);
    update_only_act->setStatusTip(tr("Update existing files only"));
    config_menu->addAction(update_only_act);
    backup_folder_act = new QAction(tr("Do not backup updated files"), this);
    backup_folder_act->setStatusTip(tr("Do not backup updated files"));
    backup_folder_act->setCheckable(true);
    config_menu->addAction(backup_folder_act);
    config_menu->addSeparator();
    master_act = new QAction(tr("Master"), this);
    master_act->setStatusTip(tr("Use this folder as master"));
    master_act->setCheckable(true);
    master_act->setChecked(true);
    config_menu->addAction(master_act);
    slave_act = new QAction(tr("Slave"), this);
    slave_act->setStatusTip(tr("Use this folder as slave"));
    slave_act->setCheckable(true);
    config_menu->addAction(slave_act);
    QActionGroup * master_slave_actgrp = new QActionGroup(config_menu);
    master_slave_actgrp->addAction(master_act);
    master_slave_actgrp->addAction(slave_act);
    master_slave_actgrp->setExclusive(true);
    remove_act = new QAction(tr("Remove"), this);
    remove_act->setStatusTip(tr("Remove this folder"));
    remove_act->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/remove.png")));
    connect(remove_act, SIGNAL(triggered()), this, SLOT(removeFolder()));
    config_menu->addSeparator();
    config_menu->addAction(remove_act);
    config_btn->setMenu(config_menu);
    //config_menu->setTearOffEnabled(true);
    hlayout->addWidget(config_btn);

    label_lne = new QLineEdit (this);
    label_lne->setMaximumWidth(100);
    label_lne->setStatusTip(tr("Folder label"));
    label_lne->setToolTip(tr("Folder label"));
    connect(label_lne, SIGNAL(editingFinished()), this, SLOT(lneEdited()));
    hlayout->addWidget(label_lne);

    folder_name_lne = new QLineEdit (this);
    folder_name_lne->setStatusTip(tr("Folder path"));
    folder_name_lne->setToolTip(tr("Folder path"));
    connect(folder_name_lne, SIGNAL(editingFinished()), this, SLOT(lneEdited()));
    hlayout->addWidget(folder_name_lne);

    browse_btn = new QPushButton (this);
    browse_btn->setText(tr("Browse"));
    browse_btn->setStatusTip(tr("Browse"));
    browse_btn->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/folder_16.png")));
    connect(browse_btn, SIGNAL(released()), this, SLOT(browse()));
    hlayout->addWidget(browse_btn);
}

void SyncFolder::browse()
{
    QString text = QFileDialog::getExistingDirectory(
                this,
                "Choose a directory",
                QDir::homePath(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (text != "") folder_name_lne->setText(text);
    emit sigedited();
}

void SyncFolder::lneEdited()
{
    if (folder_name_lne->text()=="") return;
    QDir dir;
    dir.setPath(folder_name_lne->text());
    QString text = dir.path();
    if (text.at(0).isLower()) {
        text[0] = text.at(0).toUpper();
    }
    folder_name_lne->setText(text);
    emit sigedited();
}
