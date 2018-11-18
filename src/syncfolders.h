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

#include <QList>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QFileDialog>
#include <QIcon>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QTimer>

#ifndef SYNCFOLDERS_H
#define SYNCFOLDERS_H

class SyncFolder;

class SyncFolders : public QWidget
{
    Q_OBJECT

public:
    SyncFolders(QWidget * parent = 0);

    QList <SyncFolder *> folders_list;
    //QStringList paths_list;
    QVBoxLayout * folders_vlayout;
    //QToolButton * add_folder_btn;

    SyncFolder * syncFolder(int i) { return folders_list.at(i); }
    SyncFolder * at(int i) { return folders_list.at(i); }
    int count() { return folders_list.count(); }
    QStringList pathsList();
    QStringList pathsAndLabelsList();
    void loadPathsAndLabelsFromList(QStringList);

private:
    SyncFolder * current_folder;

public slots:
    SyncFolder * addFolder();
    void addToFolders(int);
    void removeFolder(int);
    void removeFolderTimer(SyncFolder * sfolder) { current_folder = sfolder; QTimer::singleShot(200, this, SLOT(removeFolder())); }
    void removeFolder();
    void removeAllFolders();
    void foldersChanged();
    void foldersEdited();

signals:
    void sigfolderschanged();
    void sigfoldersedited();
};

class SyncFolder : public QWidget
{
    Q_OBJECT

public:
    SyncFolder(QWidget * parent = 0);
    ~SyncFolder() {
        /*delete update_only_act;
        delete dont_update_act;
        delete backup_folder_act;
        delete remove_act;
        delete master_act;
        delete slave_act;
        delete config_menu;
        delete folder_name_lne;
        delete label_lne;
        delete browse_btn;
        delete config_btn;*/
    }

    QLineEdit * folder_name_lne;
    QLineEdit * label_lne;
    QPushButton * browse_btn;

    QPushButton * config_btn;
    QMenu * config_menu;
    QAction * update_only_act;
    QAction * dont_update_act;
    QAction * backup_folder_act;
    QAction * no_empty_folders_act;
    QAction * remove_act;
    QAction * master_act;
    QAction * slave_act;

public slots:
    void browse();
    void setPath(QString path) { folder_name_lne->setText(path); }
    QString path() { return folder_name_lne->text(); }
    QString label() { return label_lne->text(); }
    void setLabel(QString text) { label_lne->setText(text); }
    void removeFolder() { emit sigremove(this); }
    void lneEdited();

signals:
    void sigremove(SyncFolder *);
    void sigedited();
};

#endif
