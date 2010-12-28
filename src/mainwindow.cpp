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

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "synctabform.h"
#include "mttreewidgetitem.h"
#include "module.h"
#include "syncpage.h"
#include "exceptionform.h"
#include "exceptionbundle.h"

#include <QActionGroup>
#include <QMessageBox>
#include <QMap>
#include <QProgressBar>

MainWindow::MainWindow(Module * module, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->main_stckw->setCurrentIndex(0);
    for (int i = ui->sync_stckw->count() - 1; i >= 0; --i) delete ui->sync_stckw->widget(i);

    sync_item_tab_map = new QMap<QTreeWidgetItem *, SyncTabForm *>;
    this->module = module;

    if (tr("LTR") == "RTL") { qApp->setLayoutDirection(Qt::RightToLeft); }
    setUnifiedTitleAndToolBarOnMac(true);
    createNavigationTreeActions();
    connectActions();

    exception_form = new ExceptionForm(nav_exceptions_item, module->getExceptions(), ui->main_stckw);
    ui->main_stckw->addWidget(exception_form);

    // Toolbar menus +++
    sync_act_menu = new QMenu;
    QObject::connect(sync_act_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowSyncActMenu()));
    QObject::connect(sync_act_menu, SIGNAL(triggered(QAction*)), this, SLOT(syncActMenuActionTriggered(QAction*)));
    ui->synchronise_act->setMenu(sync_act_menu);

    exception_act_menu = new QMenu;
    QObject::connect(exception_act_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowExceptionActMenu()));
    QObject::connect(exception_act_menu, SIGNAL(triggered(QAction*)), this, SLOT(exceptionActMenuActionTriggered(QAction*)));
    ui->exceptions_act->setMenu(exception_act_menu);

    QPushButton * view_toolbar_btn = new QPushButton(ui->toolbar);
    view_toolbar_btn->setToolTip(tr("View"));
    view_toolbar_btn->setIcon(QIcon(":/images/syncview_48.png"));
    view_toolbar_btn->setMenu(ui->view_menu);
    view_toolbar_btn->setFlat(true);
    view_toolbar_btn->setIconSize(QSize(32, 32));
    view_toolbar_btn->setMaximumSize(32, 32);

    ui->toolbar->addSeparator();
    ui->toolbar->addWidget(view_toolbar_btn);
    // Toolbar menus ---

    load();
    makeReady();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::createNavigationTreeActions()
{
    nav_sync_item = new QTreeWidgetItem(ui->navigation_tree, QStringList(tr("Syncs")));
    nav_sync_item->setExpanded(true);
    nav_sync_item->setIcon(0, QIcon(":/images/Synchronise.png"));

    nav_multisync_item = new QTreeWidgetItem(ui->navigation_tree, QStringList(tr("Multisyncs")));
    nav_multisync_item->setExpanded(true);
    nav_multisync_item->setIcon(0, QIcon(":/images/multisync_16.png"));

    nav_exceptions_item = new QTreeWidgetItem(ui->navigation_tree, QStringList(tr("Exceptions")));
    nav_exceptions_item->setExpanded(true);
    nav_exceptions_item->setIcon(0, QIcon(":/images/blacklist.png"));

    nav_scheduler_item = new QTreeWidgetItem(ui->navigation_tree, QStringList(tr("Scheduler")));
    nav_scheduler_item->setIcon(0, QIcon(":/images/scheduler16.png"));

    nav_restore_item = new QTreeWidgetItem(ui->navigation_tree, QStringList(tr("Restore")));
    nav_restore_item->setIcon(0, QIcon(":/images/Restore16.png"));
}

/**
  * Run at startup. Connects actions inside the UI.
  */
void MainWindow::connectActions()
{
    QObject::connect(ui->navigation_tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(switchView(QTreeWidgetItem *, int)));
    QObject::connect(ui->quit_act, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->new_tab_act, SIGNAL(triggered()), this, SLOT(addTab()));
    QObject::connect(ui->close_tab_act, SIGNAL(triggered()), this, SLOT(closeTab()));
    //QObject::connect(ui->navigation_add_btn, SIGNAL(clicked()), this, SLOT(addTab()));
    //QObject::connect(ui->navigation_remove_btn, SIGNAL(clicked()), this, SLOT(closeTab()));

    ui->synchronise_act->trigger();

#ifdef Q_WS_MAC
    ui->quit_act->setMenuRole(QAction::QuitRole);
    ui->about_act->setMenuRole(QAction::AboutRole);
#endif
}

/**
  * Changes the current index on the main stacked widget or any other sub stacked widgets according to the item received
  */
void MainWindow::switchView(QTreeWidgetItem * item, int)
{
    if (item == nav_sync_item) {
        ui->main_stckw->setCurrentWidget(ui->synchronise_page);
    }
    else if (item == nav_restore_item) {
        ui->main_stckw->setCurrentWidget(ui->restore_page);
    }
    else if (item == nav_exceptions_item) {
        ui->main_stckw->setCurrentWidget(exception_form);
    }
    else if (item == nav_multisync_item) {
        ui->main_stckw->setCurrentWidget(ui->multisync_page);
    }
    else if (item == nav_scheduler_item) {
        ui->main_stckw->setCurrentWidget(ui->scheduler_page);
    }
    else if (item->parent()) {
        if (item->parent() == nav_sync_item) {
            ui->sync_stckw->setCurrentWidget(sync_item_tab_map->value(item));

            ui->main_stckw->setCurrentWidget(ui->synchronise_page);
        }
        else if (item->parent() == nav_exceptions_item) {
            ui->main_stckw->setCurrentWidget(exception_form);

            exception_form->navigationItemClicked(item);
        }
    }
}

/**
  * Slot called when the new tab action (new_tab_act) is triggered. Decides whether to create a sync or multisync tab.
  */
void MainWindow::addTab()
{
    if (ui->main_stckw->currentWidget() == ui->synchronise_page) {
        addSyncTab();
    } else if (ui->main_stckw->currentWidget() == ui->multisync_page) {

    } else if (ui->main_stckw->currentWidget() == exception_form) {
        addExceptionBundle();
    }
}

/**
  * Slot called when the remove button is clicked. Removes all of the selected items in the navigation tree.
  */
void MainWindow::closeTab()
{
    QList<QTreeWidgetItem *> items = ui->navigation_tree->selectedItems();

    for (int n = items.count() - 1; n >= 0; --n) {
        if (items.at(n)->parent() == nav_sync_item) {
            SyncTabForm * form = sync_item_tab_map->value(items.at(n));
            int i = form->index();
            delete items.at(n);
            delete form;
            module->closeSync(i);
        } else if (items.at(n)->parent() == nav_exceptions_item) {
            exception_form->removeBundle(items.at(n));
        }
    }
}

/**
  * Creates a new SyncPage. Usually called when user clicks the add tab button.
  */
void MainWindow::addSyncTab()
{
    addSyncTab(module->addSync());
}

/**
  * Creates a new SyncTabForm from the SyncPage.
  */
void MainWindow::addSyncTab(AbstractSyncPage * sync_page)
{
    SyncTabForm * tab_form = new SyncTabForm(sync_page);

    QObject::connect(tab_form, SIGNAL(addFolderSig(int,int)), module, SLOT(addSyncFolder(int,int)));
    QObject::connect(tab_form, SIGNAL(closeFolderSig(int,int)), module, SLOT(closeSyncFolder(int,int)));

    ui->sync_stckw->addWidget(tab_form);
    ui->sync_stckw->setCurrentWidget(tab_form);

    MTTreeWidgetItem * item = new MTTreeWidgetItem(nav_sync_item, QStringList(tab_form->title()));
    selectTreeItemExclusive(ui->navigation_tree, item);

    tab_form->assignNavigationItem(item);
    sync_item_tab_map->insert(item, tab_form);

    ui->navigation_tree->setItemWidget(item, 0, tab_form->navigationItemWidget());
    ui->main_stckw->setCurrentWidget(ui->synchronise_page);
}

void MainWindow::addExceptionBundle()
{
    exception_form->addBundle();
}

/**
  * Selects the given item and deselects all other items in the given tree
  */
void MainWindow::selectTreeItemExclusive(QTreeWidget * tree, QTreeWidgetItem * item)
{
    QList<QTreeWidgetItem *> items = tree->selectedItems();

    for (int i = 0; i < items.count(); ++i) {
        items.at(i)->setSelected(false);
    }
    item->setSelected(true);
}

/**
  * Loads the settings from Module
  */
void MainWindow::load()
{
    QMapIterator<int, AbstractSyncPage *> * i = module->syncIterator();
    while (i->hasNext()) {
        i->next();
        addSyncTab(i->value());
    }
    delete i;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    module->save();
    QMainWindow::closeEvent(event);
}

/**
  * This is called after all settings were loaded to make sure everything is ready for the user to use the MainWindow
  * (like there is at least one sync tab...)
  */
void MainWindow::makeReady()
{
    if (!sync_item_tab_map->count()) addSyncTab();
}

void MainWindow::aboutToShowSyncActMenu()
{
    QAction * act;

    sync_act_menu->clear();

    QMapIterator<QTreeWidgetItem *, SyncTabForm *> i(*sync_item_tab_map);
    while (i.hasNext()) {
        i.next();

        act = new QAction(sync_act_menu);
        act->setText(i.value()->title());
        act->setData(i.value()->index());
        sync_act_menu->addAction(act);
    }

    sync_act_menu->addSeparator();

    QAction * new_sync_act = new QAction(sync_act_menu);
    new_sync_act->setText(tr("Add sync"));
    new_sync_act->setIcon(QIcon(":/images/add.png"));
    QObject::connect(new_sync_act, SIGNAL(triggered()), this, SLOT(addSyncTab()));
    sync_act_menu->addAction(new_sync_act);
}

void MainWindow::aboutToShowExceptionActMenu()
{
    QAction * act;

    exception_act_menu->clear();

    QMapIterator<QTreeWidgetItem *, ExceptionBundle *> i(*(exception_form->bundles()));
    while (i.hasNext()) {
        i.next();

        act = new QAction(exception_act_menu);
        act->setText(i.value()->name());
        act->setData(i.value()->index());
        exception_act_menu->addAction(act);
    }

    exception_act_menu->addSeparator();

    QAction * new_exc_act = new QAction(exception_act_menu);
    new_exc_act->setText(tr("Add exception bundle"));
    new_exc_act->setIcon(QIcon(":/images/add.png"));
    QObject::connect(new_exc_act, SIGNAL(triggered()), this, SLOT(addExceptionBundle()));
    exception_act_menu->addAction(new_exc_act);
}

void MainWindow::syncActMenuActionTriggered(QAction * act)
{
    if (act->data().isNull()) return;

    ui->main_stckw->setCurrentWidget(ui->synchronise_page);
    int id = act->data().toInt();
    QMapIterator<QTreeWidgetItem *, SyncTabForm *> i(*sync_item_tab_map);
    while (i.hasNext()) {
        i.next();

        if (i.value()->index() == id) {
            ui->navigation_tree->clearSelection();
            i.key()->setSelected(true);
            ui->sync_stckw->setCurrentWidget(i.value());
            return;
        }
    }
}

void MainWindow::exceptionActMenuActionTriggered(QAction * act)
{
    if (act->data().isNull()) return;

    ui->main_stckw->setCurrentWidget(exception_form);
    int id = act->data().toInt();
    QMapIterator<QTreeWidgetItem *, ExceptionBundle *> i(*exception_form->bundles());
    while (i.hasNext()) {
        i.next();

        if (i.value()->index() == id) {
            ui->navigation_tree->clearSelection();
            i.key()->setSelected(true);
            exception_form->navigationItemClicked(i.key());
            return;
        }
    }
}
