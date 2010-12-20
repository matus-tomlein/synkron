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

#ifndef SYNCTABFORM_H
#define SYNCTABFORM_H

#include <QWidget>

class SyncFolderForm;
template<class T>
class QList;
class QTreeWidgetItem;
class AbstractSyncPage;
class SyncAdvancedView;
class SyncOutMessage;
class Folder;
class MTProgressBar;

namespace Ui {
    class SyncTabForm;
}

class SyncTabForm : public QWidget {
    Q_OBJECT

public:
    SyncTabForm(AbstractSyncPage *, QWidget *parent = 0);
    ~SyncTabForm();

    int index();

    void assignNavigationItem(QTreeWidgetItem *);

    QString title();
    MTProgressBar * progressBar() { return progress_bar; }
    QWidget * navigationItemWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SyncTabForm *ui;
    AbstractSyncPage * page;
    SyncAdvancedView * advanced_view;

    int i;
    QList<SyncFolderForm *>* folder_forms;
    QTreeWidgetItem * nav_item;
    MTProgressBar * progress_bar;

    // Advanced options items +++
    QTreeWidgetItem * sync_hidden_item;
    // Advanced options items ---

    int getHighestFolderId();
    SyncFolderForm * getFolderById(int);
    void load();
    void makeReady();

private slots:
    void addFolder();
    void addFolder(Folder *);
    void removeFolder(int);
    void nameChanged(QString);
    void toggleAdvanced();
    void hideAdvanced();
    void sync();
    void showMessageBox(const QString = QString());
    void syncFinished();
    void syncOutMessage(SyncOutMessage*);

signals:
    void addFolderSig(int, int);
    void closeFolderSig(int, int);
};

#endif // SYNCTABFORM_H
