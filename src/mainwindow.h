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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTreeWidget;
class QTreeWidgetItem;
template<class Key, class T>
class QMap;
class SyncTabForm;
class Module;
class AbstractSyncPage;
class ExceptionForm;
class QLabel;
class QProgressBar;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(Module *, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addTab();
    void closeTab();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    Module * module;
    ExceptionForm * exception_form;

    // Navigation tree items +++
    QTreeWidgetItem * nav_sync_item;
    QTreeWidgetItem * nav_multisync_item;
    QTreeWidgetItem * nav_restore_item;
    QTreeWidgetItem * nav_exceptions_item;
    QTreeWidgetItem * nav_scheduler_item;
    // Navigation tree items ---

    QMenu * sync_act_menu;
    QMenu * exception_act_menu;

    QMap<QTreeWidgetItem *, SyncTabForm *> * sync_item_tab_map;

    void createNavigationTreeActions();
    void connectActions();
    void addSyncTab(AbstractSyncPage *);
    void selectTreeItemExclusive(QTreeWidget *, QTreeWidgetItem *);
    void load();
    void makeReady();
    void closeEvent(QCloseEvent*);

private slots:
    void switchView(QTreeWidgetItem *, int);
    void aboutToShowSyncActMenu();
    void aboutToShowExceptionActMenu();
    void addSyncTab();
    void syncActMenuActionTriggered(QAction *);
    void exceptionActMenuActionTriggered(QAction *);
    void addExceptionBundle();

signals:
    void addSyncTabSig(int);
    void closeSyncTabSig(int);
    void addSyncFolderSig(int, int);
    void closeSyncFolderSig(int, int);
};

#endif // MAINWINDOW_H
