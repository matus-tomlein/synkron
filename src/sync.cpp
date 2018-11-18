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

#include "mainwindow.h"

SyncPage * MainWindow::addSyncTab()
{
    SyncPage * page = new SyncPage(this);
    page->tab_stw = new QStackedWidget(tabWidget);
    page->tab = new QWidget ();
    page->blacklist = new QWidget ();

    page->tab_stw->addWidget(page->tab);
    page->tab_stw->addWidget(page->blacklist);

    QString title;
    int n = 1; gen_title:
        title = tr("Sync #%1").arg(n); bool ok = true;
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == title) { ok = false; }
    }
    if (!ok) { n++; goto gen_title; }
    tabWidget->addTab(page->tab_stw, QIcon(QString::fromUtf8(":/new/prefix1/images/Synkron128.png")), title);

    page->setSyncWidget();
    page->setBlacklistWidget();

    tabs.insert(page->tab_stw, page);
    tabWidget->setCurrentIndex(tabWidget->indexOf(page->tab_stw));

    return page;
}

void SyncPage::setSyncWidget()
{
    QGridLayout * mainglayout = new QGridLayout (tab);
    mainglayout->setMargin(4); mainglayout->setSpacing(6);
    QGridLayout * hlayout0 = new QGridLayout;
    QSpacerItem * spacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    hlayout0->addItem(mp_parent->spacerItem, 0, 0);
    tab_name = new QLineEdit (tab);
    tab_name->setStatusTip(tr("Set sync name"));
    tab_name->setText(mp_parent->tabWidget->tabText(mp_parent->tabWidget->indexOf(tab_stw)));
    QObject::connect(tab_name, SIGNAL(editingFinished()), mp_parent, SLOT(tabNameChanged()));
    hlayout0->addWidget(tab_name, 0, 1);
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlayout0->addItem(spacerItem, 0, 2);
    mainglayout->addLayout(hlayout0, 0, 0);
    QHBoxLayout * folders_hlayout = new QHBoxLayout;
    add_folder_btn = new QPushButton (this);
    add_folder_btn->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/add.png")));
    add_folder_btn->setStatusTip(tr("Add"));
    add_folder_btn->setFlat(true);
    add_folder_btn->setMinimumSize(22, 22);
    add_folder_btn->setMaximumSize(22, 22);
    folders_hlayout->addWidget(add_folder_btn);
    show_sync_folders = new QCheckBox (tab);
    show_sync_folders->setStatusTip(tr("Show/hide sync folders"));
    show_sync_folders->setChecked(true);
    folders_hlayout->addWidget(show_sync_folders);
    QLabel * sync_text = new QLabel (tab);
    sync_text->setText(tr("<b>Sync folders:</b>"));
    folders_hlayout->addWidget(sync_text);
    folders_hlayout->addStretch();
    mainglayout->addLayout(folders_hlayout, 1, 0); // ###################################

    sync_folders = new SyncFolders (this);
    connect(sync_folders, SIGNAL(sigfolderschanged()), this, SLOT(syncFoldersChanged()));
    connect(sync_folders, SIGNAL(sigfoldersedited()), this, SLOT(syncFoldersEdited()));
    connect(show_sync_folders, SIGNAL(toggled(bool)), sync_folders, SLOT(setVisible(bool)));
    connect(show_sync_folders, SIGNAL(toggled(bool)), add_folder_btn, SLOT(setVisible(bool)));
    connect(add_folder_btn, SIGNAL(released()), sync_folders, SLOT(addFolder()));
    mainglayout->addWidget(sync_folders, 2, 0);

    QGridLayout * hlayout3 = new QGridLayout;
    sync_log_label = new QLabel (tab);
    sync_log_label->setText(tr("<b>Sync log:</b>"));
    hlayout3->addWidget(sync_log_label, 0, 0);
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlayout3->addItem(spacerItem, 0, 1);
    log_search = new ExtendedLineEdit(tab);
    log_search->setStatusTip(tr("Search sync log"));
    log_search->setText(tr("Search"));
    QObject::connect(log_search, SIGNAL(textEdited(const QString &)), mp_parent, SLOT(searchTw(const QString)));
    QObject::connect(log_search, SIGNAL(returnPressed()), this, SLOT(searchAnalyseTree()));
    hlayout3->addWidget(log_search, 0, 3);
    mainglayout->addLayout(hlayout3, 4, 0);// #################################
    tw = new QTableWidget (0, 2, tab);
    tw->setHorizontalHeaderLabels(QStringList() << tr("Source") << tr("Destination"));
    tw->verticalHeader()->hide();
    tw->setMinimumSize(0, 50);
    tw->setShowGrid(false);
    tw->setStatusTip(tr("List of synchronised files and folders"));
#if QT_VERSION >= 0x050000
    tw->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    tw->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

    tw->setLayoutDirection(Qt::LeftToRight);
    status_table_item = new QTableWidgetItem(tr("Press the \"Sync\" button to start synchronisation"));
    tw->insertRow(0);
    status_table_item->setBackground(Qt::blue);
    status_table_item->setForeground(Qt::white);
    tw->setSpan(0, 0, 1, 2);
    tw->setItem(0, 0, status_table_item);
    tw->setRowHeight(0, 16);
    tw->setTextElideMode(Qt::ElideMiddle);

    analyse_tree = new ExtendedTreeWidget (this);
    analyse_tree->setAlternatingRowColors(true);
    analyse_tree->setUniformRowHeights(true);
    connect(analyse_tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(analyseTreeItemClicked(QTreeWidgetItem *, int)));
    connect(analyse_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(analyseTreeItemDoubleClicked(QTreeWidgetItem *, int)));
    connect(analyse_tree, SIGNAL(sigconmenu(QPoint)), this, SLOT(analyseTreeConMenu(QPoint)));
    connect(analyse_tree, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(analyseTreeItemExpanded(QTreeWidgetItem *)));

    logs_stw = new QStackedWidget (tab);
    logs_stw->addWidget(tw);
    logs_stw->addWidget(analyse_tree);

    mainglayout->addWidget(logs_stw, 5, 0); // ###################################

    QHBoxLayout * hlayout4 = new QHBoxLayout;
    show_advanced = new QPushButton(tr("Advanced") + "  ", tab);
    show_advanced->setStatusTip(tr("Show advanced options of configuration"));
    show_advanced->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/configure16.png")));
    hlayout4->addWidget(show_advanced);
    hlayout4->addStretch(); // ###################################

    go_to_analyse = new QPushButton (tr("Analyse"), tab);
    go_to_analyse->setStatusTip(tr("Analyse"));
    go_to_analyse->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/analyse_16.png")));
    connect(go_to_analyse, SIGNAL(released()), this, SLOT(goToAnalyse()));
    hlayout4->addWidget(go_to_analyse);

    sync_btn = new QPushButton (tr("Sync"), tab);
    sync_btn->setStatusTip(tr("Synchronise"));
    sync_btn->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/sync22.png")));
    QObject::connect(sync_btn, SIGNAL(released()), this, SLOT(sync()));
    QObject::connect(this, SIGNAL(sigsync(QWidget *)), this, SLOT(sync()));
    hlayout4->addWidget(sync_btn);

    stop_sync_btn = new QPushButton (tr("Stop sync"), tab);
    stop_sync_btn->setStatusTip(tr("Stop synchronisation"));
    stop_sync_btn->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/stop_sync22.png")));
    stop_sync_btn->setVisible(false);
    hlayout4->addWidget(stop_sync_btn);
    QObject::connect(stop_sync_btn, SIGNAL(released()), this, SLOT(stopSync()));
    mainglayout->addLayout(hlayout4, 6, 0); // ##################################

    //Advanced -----------------------------------------------------------------
    advanced_menu = new QMenu;
    advanced_menu->setWindowTitle(tr("Advanced"));
    advanced_menu->setTearOffEnabled(true);

    sync_hidden = new QAction (tab);
    sync_hidden->setCheckable(true);
    sync_hidden->setStatusTip(tr("Synchronise hidden files and folders"));
    sync_hidden->setText(tr("Synchronise hidden files and folders"));
    advanced_menu->addAction(sync_hidden);

    no_empty_folders = new QAction (tab);
    no_empty_folders->setCheckable(true);
    no_empty_folders->setStatusTip(tr("Do not create empty folders"));
    no_empty_folders->setText(tr("Do not create empty folders"));
    advanced_menu->addAction(no_empty_folders);

    sync_nosubdirs = new QAction (tab);
    sync_nosubdirs->setCheckable(true);
    sync_nosubdirs->setStatusTip(tr("Do not synchronise subdirectories"));
    sync_nosubdirs->setText(tr("Do not synchronise subdirectories"));
    advanced_menu->addAction(sync_nosubdirs);

    backup_folders = new QAction (tr("Do not backup updated files"), tab);
    backup_folders->setStatusTip(tr("Do not backup updated files"));
    backup_folders->setCheckable(true);
    advanced_menu->addAction(backup_folders);

    update_only = new QAction (tr("Update existing files only"), tab);
    update_only->setStatusTip(tr("Update existing files only"));
    update_only->setCheckable(true);
    advanced_menu->addAction(update_only);

    ignore_blacklist = new QAction (tab);
    ignore_blacklist->setCheckable(true);
    ignore_blacklist->setStatusTip(tr("Ignore blacklist"));
    ignore_blacklist->setText(tr("Ignore blacklist"));
    connect(ignore_blacklist, SIGNAL(toggled(bool)), this, SLOT(ignoreBlacklistClicked(bool)));
    advanced_menu->addAction(ignore_blacklist);

    allow_DST = new QAction (tab);
    allow_DST->setCheckable(true);
    allow_DST->setText(tr("Ignore 1 hour difference"));
    allow_DST->setStatusTip(tr("Due to Daylight Saving Time (DST), files may have 1 hour delta"));
    advanced_menu->addAction(allow_DST);

    move = new QAction (tr("Move contents to folder 2, leaving folder 1 empty"), tab);
    move->setCheckable(true);
    move->setStatusTip(tr("Move contents to folder 2, leaving folder 1 empty"));
    connect(move, SIGNAL(toggled(bool)), this, SLOT(moveStateChanged(bool)));
    advanced_menu->addAction(move);
    advanced_menu->addSeparator();

    propagate_deletions = new QAction (tab);
    propagate_deletions->setCheckable(true);
    propagate_deletions->setStatusTip(tr("Propagate deletions"));
    propagate_deletions->setText(tr("Propagate deletions"));
    connect(propagate_deletions, SIGNAL(toggled(bool)), this, SLOT(propagatedStateChanged(bool)));
    connect(propagate_deletions, SIGNAL(toggled(bool)), this, SLOT(propagatedClicked(bool)));
    advanced_menu->addAction(propagate_deletions);

    alert_collisions = new QAction (tab);
    alert_collisions->setCheckable(true);
    alert_collisions->setText(tr("Detect collisions"));
    alert_collisions->setStatusTip(tr("Detect and alert collided files"));
    connect(alert_collisions, SIGNAL(toggled(bool)), this, SLOT(propagatedStateChanged(bool)));
    connect(alert_collisions, SIGNAL(toggled(bool)), this, SLOT(propagatedClicked(bool)));
    advanced_menu->addAction(alert_collisions);

    text_database_action = new QAction (tab);
    text_database_action->setCheckable(true);
    text_database_action->setChecked(true);
    text_database_action->setText(tr("Store database in a text file"));
    text_database_action->setStatusTip(tr("Store database in a text file"));
    connect(text_database_action, SIGNAL(toggled(bool)), this, SLOT(useTextDatabase(bool)));
    advanced_menu->addAction(text_database_action);

    symlinks = new QAction(tab);
    symlinks->setCheckable(true);
#ifdef Q_WS_WIN
    symlinks->setChecked(false);
#else
    symlinks->setChecked(false);
    symlinks->setStatusTip(tr("Follow symbolic links"));
    symlinks->setText(tr("Follow symbolic links"));
    advanced_menu->addAction(symlinks);
#endif

    advanced_menu->addSeparator();
    QMenu * adv_analysis_menu = new QMenu;
    adv_analysis_menu->setTitle(tr("Analysis"));
    adv_analysis_menu->setStatusTip(tr("Advanced options for the analysis"));
    adv_analysis_menu->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/analyse_16.png")));

    fast_analyse = new QAction (tr("Fast analysis"), tab);
    fast_analyse->setCheckable(true);
    fast_analyse->setStatusTip(tr("Fast analysis"));
    adv_analysis_menu->addAction(fast_analyse);

    analyse_special_only = new QAction (tr("List files which need to be synchronised only"), tab);
    analyse_special_only->setCheckable(true);
    analyse_special_only->setStatusTip(tr("List files which need to be synchronised only"));
    adv_analysis_menu->addAction(analyse_special_only);

    sort_analysis_by_action = new QAction (tr("Sort by action"), tab);
    sort_analysis_by_action->setCheckable(true);
    sort_analysis_by_action->setStatusTip(tr("Sort by action"));
    adv_analysis_menu->addAction(sort_analysis_by_action);

    advanced_menu->addMenu(adv_analysis_menu);

    filters_menu = new QMenu;
    filters_menu->setWindowTitle(tr("Filters"));
    filters_menu->setTitle(tr("Filters"));
    filters_menu->setStatusTip(tr("Choose filters to be used"));
    filters_menu->setTearOffEnabled(true);
    filters_menu->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/filter16.png")));
    QAction * action;
    for (int f = 0; f < mp_parent->filter_list->count(); ++f) {
        action = new QAction (mp_parent->filter_list->item(f)->text(), filters_menu);
        action->setCheckable(true);
        filters_menu->addAction(action);
    }

    advanced_menu->addMenu(filters_menu);
    advanced_menu->addSeparator();

    QAction * edit_blacklist = new QAction(tab);
    edit_blacklist->setText(tr("Edit blacklist"));
    edit_blacklist->setStatusTip(tr("Edit blacklist for this tab"));
    edit_blacklist->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/blacklist.png")));
    connect(edit_blacklist, SIGNAL(triggered(bool)), this, SLOT(editBlacklist()));
    advanced_menu->addAction(edit_blacklist);

    QAction * change_ignorance = new QAction(tab);
    change_ignorance->setText(tr("Change allowed time difference"));
    change_ignorance->setStatusTip(tr("Change the allowed time difference between synchronised files"));
    change_ignorance->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/sync.png")));
    connect(change_ignorance, SIGNAL(triggered(bool)), this, SLOT(changeAllowedDifference()));
    advanced_menu->addAction(change_ignorance);

    show_advanced->setMenu(advanced_menu);
}

void AbstractSyncPage::setBlacklistWidget()
{
    QVBoxLayout * bl_vlayout = new QVBoxLayout (blacklist);
    bl_vlayout->setMargin(4); bl_vlayout->setSpacing(6);
    QLabel * files = new QLabel (blacklist);
    files->setText(tr("<b>Files in the blacklist</b>"));
    bl_vlayout->addWidget(files);
    blacklist_fileslist = new QListWidget (blacklist);
    blacklist_fileslist->setStatusTip(tr("List of files in the blacklist"));
    bl_vlayout->addWidget(blacklist_fileslist);

    QHBoxLayout * files_btn_layout = new QHBoxLayout;
    blacklist_addfile = new QPushButton (blacklist);
    blacklist_addfile->setText(tr("Add"));
    blacklist_addfile->setStatusTip(tr("Add file to black list"));
    connect(blacklist_addfile, SIGNAL(released()), this, SLOT(addFileToBlacklist()));
    files_btn_layout->addWidget(blacklist_addfile);
    blacklist_removefile = new QPushButton (blacklist);
    blacklist_removefile->setText(tr("Remove"));
    blacklist_removefile->setStatusTip(tr("Remove file from black list"));
    connect(blacklist_removefile, SIGNAL(released()), this, SLOT(removeFileFromBlacklist()));
    files_btn_layout->addWidget(blacklist_removefile);
    files_btn_layout->addStretch();
    bl_vlayout->addLayout(files_btn_layout);

    QLabel * folders = new QLabel (blacklist);
    folders->setText(tr("<b>Folders in the blacklist</b>"));
    bl_vlayout->addWidget(folders);
    blacklist_folderslist = new QListWidget (blacklist);
    blacklist_folderslist->setStatusTip(tr("List of folders in the blacklist"));
    bl_vlayout->addWidget(blacklist_folderslist);

    QHBoxLayout * folders_btn_layout = new QHBoxLayout;
    blacklist_addfolder = new QPushButton (blacklist);
    blacklist_addfolder->setText(tr("Add"));
    blacklist_addfolder->setStatusTip(tr("Add folder to black list"));
    connect(blacklist_addfolder, SIGNAL(released()), this, SLOT(addFolderToBlacklist()));
    folders_btn_layout->addWidget(blacklist_addfolder);
    blacklist_removefolder = new QPushButton (blacklist);
    blacklist_removefolder->setText(tr("Remove"));
    blacklist_removefolder->setStatusTip(tr("Remove folder from black list"));
    connect(blacklist_removefolder, SIGNAL(released()), this, SLOT(removeFolderFromBlacklist()));
    folders_btn_layout->addWidget(blacklist_removefolder);
    folders_btn_layout->addStretch();
    bl_vlayout->addLayout(folders_btn_layout);

    QLabel * exts = new QLabel (blacklist);
    exts->setText(tr("<b>Extensions in the blacklist</b>"));
    bl_vlayout->addWidget(exts);
    blacklist_extslist = new QListWidget (blacklist);
    blacklist_extslist->setStatusTip(tr("List of extensions in the blacklist"));
    bl_vlayout->addWidget(blacklist_extslist);

    QHBoxLayout * exts_btn_layout = new QHBoxLayout;
    blacklist_addext = new QPushButton (blacklist);
    blacklist_addext->setText(tr("Add"));
    blacklist_addext->setStatusTip(tr("Add extension to black list"));
    connect(blacklist_addext, SIGNAL(released()), this, SLOT(addExtToBlacklist()));
    exts_btn_layout->addWidget(blacklist_addext);
    blacklist_removeext = new QPushButton (blacklist);
    blacklist_removeext->setText(tr("Remove"));
    blacklist_removeext->setStatusTip(tr("Remove extension from black list"));
    connect(blacklist_removeext, SIGNAL(released()), this, SLOT(removeExtFromBlacklist()));
    exts_btn_layout->addWidget(blacklist_removeext);
    exts_btn_layout->addStretch();
    bl_vlayout->addLayout(exts_btn_layout);

    QHBoxLayout * back_btn_layout = new QHBoxLayout;
    back_btn_layout->addStretch();
    blacklist_back = new QPushButton (blacklist);
    blacklist_back->setText(tr("Back"));
    blacklist_back->setStatusTip(tr("Go back"));
    connect(blacklist_back, SIGNAL(released()), this, SLOT(backFromBlacklist()));
    back_btn_layout->addWidget(blacklist_back);
    bl_vlayout->addLayout(back_btn_layout);
}

bool MainWindow::isSyncing()
{
    if (syncingAll) return true;
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabs.value(tabWidget->widget(i))->syncing) {
            return true;
        }
    }
    for (int i = 0; i < multi_tabWidget->count(); ++i) {
        if (( static_cast<MultisyncPage *> (multi_tabWidget->widget(i))->syncing)) {
            return true;
        }
    }
    return false;
}

void MainWindow::sync(QWidget* syncTab)
{
    tabs.value(syncTab)->sync();
}

int SyncPage::sync(MTMap<QString, int> sync_folders_set)
{
    if (syncing) return 0;
    leaveAnalyse();
    //MTStringSet sync_folders_set;
    if (sync_folders_set.count() == 0) {
        for (int i = 0; i < sync_folders->count(); ++i) {
            if (sync_folders->at(i)->path() == "") continue;
            QDir sync_dir (sync_folders->at(i)->path());
            if (sync_dir.exists()) {
                //sync_folders_set << sync_dir.path();
                sync_folders_set.setValue(sync_dir.path(), i);
            } else {
                if (!QDir().mkpath(sync_dir.path())) {
                    addTableItem(tr("%1	Failed to create directory %2").arg(QTime().currentTime().toString("hh:mm:ss")).arg(sync_dir.path()), "", "", QBrush(Qt::red), QBrush(Qt::white));
                } else {
                    addTableItem(tr("%1	Directory %2 created").arg(QTime().currentTime().toString("hh:mm:ss")).arg(sync_dir.path()), "", "", QBrush(Qt::darkBlue), QBrush(Qt::white));
                    //sync_folders_set << sync_dir.path();
                    sync_folders_set.setValue(sync_dir.path(), i);
                }
            }
        }
    }
    if (sync_folders_set.count() < 2) {
        addTableItem(tr("%1	Synchronisation failed: Not enough valid directories specified").arg(QTime().currentTime().toString("hh:mm:ss")), "", "", QBrush(Qt::red), QBrush(Qt::white));
        return 0;
    }
    setSyncEnabled(false);
    exts_bl_map.clear();
    extensions.clear();
    QList<QAction *> actions = filters_menu->actions();
    for (int f = 0; f < actions.count(); ++f) {
        if (actions.at(f)->isChecked()) {
            for (int l = 0; l < mp_parent->filter_list->count(); ++l) {
                if (mp_parent->filter_list->item(l)->text() == actions.at(f)->text()) {
                    for (int e = 0; e < ( static_cast<Filter *> (mp_parent->filter_list->item(l))->extensions.count()); ++e) {
                        extensions << QString("*%1").arg(( static_cast<Filter *> (mp_parent->filter_list->item(l))->extensions.at(e)));
                    } break;
                }
            }
        }
    }
    collided.clear();
    dir_filters = QDir::NoDotAndDotDot | QDir::Files;
    if (sync_hidden->isChecked()) { dir_filters |= QDir::Hidden; }
    if (!sync_nosubdirs->isChecked()) { dir_filters |= QDir::AllDirs; }
    if (text_database) {
        if (propagate_deletions->isChecked() || alert_collisions->isChecked()) {
            folder_prop_list_map.clear();
            MTDictionary prop_files_list;
            for (int i = 0; i < sync_folders_set.count(); ++i) {
                prop_files_list = getFolderDatabase(sync_folders_set.key(i));
                folder_prop_list_map.insert(sync_folders_set.key(i), prop_files_list);
            }
        }
    }
    addTableItem(tr("%1	Synchronisation started").arg(QTime().currentTime().toString("hh:mm:ss")), "", "", QBrush(Qt::yellow));
    if (sync_folders_set.count() == 2) {
        QDir d1 (sync_folders_set.key(0)); QDir d2 (sync_folders_set.key(1));
        if (d1.path() == d2.path()) {
            addTableItem(tr("%1	Synchronisation failed: Directories with the same path selected").arg(QTime().currentTime().toString("hh:mm:ss")), "", "", QBrush(Qt::red), QBrush(Qt::white));
            setSyncEnabled(true); return 0;
        }

        if (move->isChecked()) {
            status_table_item->setText(tr("Moving")); qApp->processEvents();
            moveContents(d1, d2);
        } else {
            status_table_item->setText(tr("Searching for changes")); qApp->processEvents();
            subSync(d1, d2, false);
        }
        countExtsBl();
    } else if (sync_folders_set.count() > 2) {
        status_table_item->setText(tr("Searching for changes")); qApp->processEvents();
        subGroupSync(sync_folders_set);
        countExtsBl();
    }
    if (alert_collisions->isChecked() && collided.count()) displayCollisions();
    if (propagate_deletions->isChecked() || alert_collisions->isChecked()) saveAllFolderDatabases();
    int deleted_temp = 0;
    if (mp_parent->restore_clean_auto_gb->isChecked()) deleted_temp = mp_parent->autoCleanTemp();
    if (deleted_temp > 0) {
        addTableItem(tr("%1 repeated temporary files deleted").arg(deleted_temp), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
    }
    bool manually_stopped = !syncing;
    extensions.clear();
    setSyncEnabled(true);
    mp_parent->saveSettings();
    last_sync = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
    addTableItem(tr("%1	Synchronisation complete: %2 file(s) %3").arg(QTime().currentTime().toString("hh:mm:ss")).arg(synced_files).arg(move->isChecked() ? tr("moved") : tr("synchronised")), "", "", QBrush(Qt::green));
    if (!mp_parent->syncingAll) {
        mp_parent->showTrayMessage(tr("Synchronisation complete"), tr("%1 files %2").arg(synced_files).arg(move->isChecked() ? tr("moved") : tr("synchronised")));
    }
    status_table_item->setText(tr("Last synced on %1").arg(last_sync)); qApp->processEvents();
    if (!manually_stopped) {
        if (mp_parent->actionShut_down_after_sync->isChecked()) {
            if (!mp_parent->isSyncing()) {
                mp_parent->shutDownComputer();
            }
        }
        if (mp_parent->actionQuit_after_sync->isChecked()) {
            if (!mp_parent->isSyncing()) {
                mp_parent->closeApp();
            }
        }
    }
    return synced_files;
}

void AbstractSyncPage::subSync(QDir& d1, QDir& d2, bool repeated)
{
    qApp->processEvents();
    if (!syncing) return;
    if (!ignore_blacklist->isChecked()) {
        /*if (folders_blacklist.contains(d1.path(), Qt::CaseInsensitive)) {
            addTableItem(tr("Folder %1 blacklisted, skipped").arg(d1.path()), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
            return;
        } if (folders_blacklist.contains(d2.path(), Qt::CaseInsensitive)) return;*/
        for (int b = 0; b < folders_blacklist.count(); ++b) {
            if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d1.path())) {
                addTableItem(tr("Folder %1 blacklisted, skipped").arg(getLabeledPath(d1.path())), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                return;
            }
            if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d2.path())) {
                return;
            }
        }
    }
    QFileInfoList d1_entries;
    if (extensions.count()==0) {
        d1_entries = d1.entryInfoList(dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
    } else {
        d1_entries = d1.entryInfoList(extensions.toList(), dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
    }
    bool d1_is_d1 = true;
    if (!d1.path().startsWith(syncFolder1Text(), Qt::CaseInsensitive)) d1_is_d1 = false;
    MTFile * file; QDir * temp = new QDir (mp_parent->temp_path);
    for (int i = 0; i < d1_entries.count(); ++i) {
        bool bl = false;
        if (!syncing) return;
        if (d1_entries.at(i).fileName().startsWith(".synkron.sync")) continue;
        if (!ignore_blacklist->isChecked()) {
            for (int e = 0; e < exts_blacklist.count(); ++e) {
                if (d1_entries.at(i).absoluteFilePath().endsWith(exts_blacklist.at(e))) {
                    //addTableItem(tr("File %1 blacklisted, skipped").arg(d1_entries.at(i).absoluteFilePath()), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                    exts_bl_map.insert(exts_blacklist.at(e), exts_bl_map.value(exts_blacklist.at(e), 0) + 1);
                    bl = true;
                    break;
                }
            }
            if (bl) continue;
            if (d1_entries.at(i).isDir()) {
                for (int b = 0; b < folders_blacklist.count(); ++b) {
                    if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d1_entries.at(i).absoluteFilePath())) {
                        addTableItem(tr("Folder %1 blacklisted, skipped").arg(getLabeledPath(d1_entries.at(i).absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                        bl = true;
                        break;
                    }
                } if (bl) continue;
            } else {
                for (int b = 0; b < files_blacklist.count(); ++b) {
                    if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d1_entries.at(i).absoluteFilePath())) {
                        addTableItem(tr("File %1 blacklisted, skipped").arg(getLabeledPath(d1_entries.at(i).absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                        bl = true;
                        break;
                    }
                } if (bl) continue;
            }
        }
        if (!syncing) return;

        QString d2_file_path = QString("%1/%2").arg(d2.path()).arg(d1_entries.at(i).fileName());
        MTFileInfo d2_info(d2_file_path);
        if (d2_info.exists()) { // *** Exists in D2 ***
            if (repeated) continue;
            bl = false;
            if (!ignore_blacklist->isChecked()) {
                if (d2_info.isDir()) {
                    for (int b = 0; b < folders_blacklist.count(); ++b) {
                        if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d2_info.absoluteFilePath())) {
                            bl = true;
                            break;
                        }
                    } if (bl) continue;
                } else {
                    for (int b = 0; b < files_blacklist.count(); ++b) {
                        if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d2_info.absoluteFilePath())) {
                            bl = true;
                            break;
                        }
                    } if (bl) continue;
                }
            }
            if (d1_entries.at(i).isDir() && d2_info.isDir()) {
                if (d2_info.isSymLink() || d1_entries.at(i).isSymLink()) {
                    if (followSymlinks()) {
                        QDir subDir1 (d1_entries.at(i).isSymLink() ? d1_entries.at(i).symLinkTarget() : d1_entries.at(i).absoluteFilePath());
                        QDir subDir2 (d2_info.isSymLink() ? d2_info.symLinkTarget() : d2_info.absoluteFilePath());
                        subSync(subDir1, subDir2, false);
                        continue;
                    } else if (!d1_entries.at(i).isSymLink() || !d2_info.isSymLink()) {
                        addTableItem(tr("A file or a folder and a symbolic link with the same name have been found. Unable to synchronise these files. (%1, %2)").arg(d1_entries.at(i).fileName()).arg(d2_info.fileName()), "",
                                     QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::red), QBrush(Qt::white));
                        continue;
                    }
                }
                else {
                    QDir subDir1 (d1_entries.at(i).absoluteFilePath());
                    QDir subDir2 (d2_info.absoluteFilePath());
                    subSync(subDir1, subDir2, false);
                    continue;
                }
            }
            if ((d1_entries.at(i).isDir() && !d2_info.isDir())) {
                addTableItem(tr("A folder (%1) and a file (%2) with the same name have been found. Unable to synchronise these files.").arg(d1_entries.at(i).fileName()).arg(d2_info.fileName()), "",
                             QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::red), QBrush(Qt::white));
                continue;
            }
            else if ((d2_info.isDir() && !d1_entries.at(i).isDir())) {
                addTableItem(tr("A file (%1) and a folder (%2) with the same name have been found. Unable to synchronise these files.").arg(d1_entries.at(i).fileName()).arg(d2_info.fileName()), "",
                             QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::red), QBrush(Qt::white));
                continue;
            }
            else {
                int compared_dates = MTFileInfo(d1_entries.at(i)).lastModified().compareWith(d2_info.lastModified(), allowed_difference, allow_DST->isChecked());
                if (compared_dates != 0) { // d1_entries.at(i) is different than d2_info
                    MTFileInfo old_fi; MTFileInfo new_fi;
                    int modified_folder = 0;
                    if (cloneFolder(0)) {
                        if (d1_is_d1) {
                            old_fi = d2_info;
                            new_fi = d1_entries.at(i);
                        } else {
                            new_fi = d2_info;
                            old_fi = d1_entries.at(i);
                        }
                        modified_folder = 1;
                    } else if (cloneFolder(1)) {
                        if (!d1_is_d1) {
                            old_fi = d2_info;
                            new_fi = d1_entries.at(i);
                        } else {
                            new_fi = d2_info;
                            old_fi = d1_entries.at(i);
                        }
                        modified_folder = 0;
                    } else {
                        if (alert_collisions->isChecked()) {
                            if (checkForCollision(d1_entries.at(i).absoluteFilePath(), d2_info.absoluteFilePath())) {
                                if (compared_dates > 0) collided.insertByValue(d1_entries.at(i).absoluteFilePath(), d2_info.absoluteFilePath());
                                else collided.insertByValue(d2_info.absoluteFilePath(), d1_entries.at(i).absoluteFilePath());
                                continue;
                            }
                        }
                        if (compared_dates < 0) { // d1_entries.at(i) is older than d2_info
                            new_fi = d2_info;
                            old_fi = d1_entries.at(i);
                            if (!d1_is_d1) modified_folder = 1;
                        } else {
                            old_fi = d2_info;
                            new_fi = d1_entries.at(i);
                            if (d1_is_d1) modified_folder = 1;
                        }
                    }
                    if (dontModify(modified_folder)) continue;
                    status_table_item->setText(tr("Updating file %1").arg(old_fi.absoluteFilePath())); qApp->processEvents();
                    file = new MTFile (old_fi.absoluteFilePath(), qApp);
                    bool skipped_temp = false;
                    bool old_fi_is_d1 = old_fi.absoluteFilePath().startsWith(syncFolder1Text(), Qt::CaseInsensitive);
                    if (old_fi_is_d1 && backupFolder(0)) { goto copying; skipped_temp = true; }
                    else if (!old_fi_is_d1 && backupFolder(1)) { goto copying; skipped_temp = true; }
                    temp->mkdir(QString(update_time));
                    if (!(file->copy(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(old_fi.fileName()).arg(synced_files)))) {
                        errorCopyingFile(old_fi.absoluteFilePath(), file->errorString(), true);
                        continue;
                    }
                    saveBackedUpFile(old_fi);
copying:
                    file->remove();
                    delete file;
                    file = new MTFile (new_fi.absoluteFilePath());
                    if (!file->copy(old_fi.absoluteFilePath())) {
                        errorCopyingFile(new_fi.absoluteFilePath(), file->errorString(), false);
                        delete file;
                        if (!skipped_temp) {
                            if (mp_parent->restoreFile(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(old_fi.fileName()).arg(synced_files),
                                                       old_fi.absoluteFilePath(), update_time)) {
                                addTableItem(tr("File %1 restored").arg(getLabeledPath(old_fi.absoluteFilePath())), "", "", QBrush(Qt::darkBlue), QBrush(Qt::white));
                            }
                        } continue;
                    }
                    addTableItem(new_fi.absoluteFilePath(), old_fi.absoluteFilePath(), QString::fromUtf8(":/new/prefix1/images/file.png"));
                    status_table_item->setText(tr("Searching for changes")); qApp->processEvents();
                    synced_files++; delete file;
                }
            }
        }
        else { // *** does not exist in D2 ***
            if (!d2_info.dir().exists())
                return;

            bool symfound = false;
            if (propagate_deletions->isChecked() && !((d1_is_d1 && dontModify(0)) || (!d1_is_d1 && dontModify(1)))) {
                if (isInDatabase(d2_file_path)) {
                    backupAndRemove(&(d1_entries.at(i)), &d2_info, (d1_is_d1 && backupFolder(0)) || (!d1_is_d1 && backupFolder(1)));
                    continue;
                }
            }
            if ((cloneFolder(0) && !d1_is_d1) || (cloneFolder(1) && d1_is_d1)) {
                //QString cloned_file = d1_entries.at(i).absoluteFilePath();
                if ((d1_is_d1 && dontModify(0)) || (!d1_is_d1 && dontModify(1))) continue;
                backupAndRemove(&(d1_entries.at(i)), &d2_info, (d1_is_d1 && backupFolder(0)) || (!d1_is_d1 && backupFolder(1)));
                continue;
            }
            if (!d1_is_d1 && (updateOnly(0) || dontModify(0))) goto end;
            if (d1_is_d1 && (updateOnly(1) || dontModify(1))) goto end;
            file = new MTFile (d1_entries.at(i).absoluteFilePath());
            if (d1_entries.at(i).isDir()) {
                if (!d1_entries.at(i).isSymLink() || (d1_entries.at(i).isSymLink() && followSymlinks())) {
                    if (createEmptyFolders(d1_is_d1 ? 1 : 0)) {
                        if (!(d2.mkdir(d1_entries.at(i).fileName()))) {
                            errorCopyingFolder(d1_entries.at(i).absoluteFilePath());
                            delete file; continue;
                        }
                        addTableItem(d1_entries.at(i).absoluteFilePath(), d2_file_path, QString::fromUtf8(":/new/prefix1/images/folder_16.png"));
                    }
                    QDir subDir1 (d1_entries.at(i).isSymLink() ? d1_entries.at(i).symLinkTarget() : d1_entries.at(i).absoluteFilePath());
                    QDir subDir2 (QString("%1/%2").arg(d2.absolutePath()).arg(d1_entries.at(i).fileName()));
                    subSync(subDir1, subDir2, true);
                } else if (d1_entries.at(i).isSymLink() && !followSymlinks()) symfound = true;
            }
            if (!d1_entries.at(i).isDir() || symfound)
            {
                status_table_item->setText(tr("Copying file %1").arg(d1_entries.at(i).absoluteFilePath())); qApp->processEvents();
                if (!file->copy(d2_file_path)) {
                    errorCopyingFile(d1_entries.at(i).absoluteFilePath(), file->errorString(), false);
                    delete file; continue;
                }
                else {
                    addTableItem(d1_entries.at(i).absoluteFilePath(), d2_file_path, QString::fromUtf8(":/new/prefix1/images/file.png"));
                }
                status_table_item->setText(tr("Searching for changes")); qApp->processEvents();
            }
            synced_files++;
            delete file;
end:
            synced_files = synced_files;
        }
    }

    if (!repeated && syncing) {
        subSync (d2, d1, true);
    }
    delete temp;
}

void AbstractSyncPage::moveContents(QDir& d1, QDir& d2)
{
    qApp->processEvents();
    if (!syncing) return;
    if (!ignore_blacklist->isChecked()) {
        for (int b = 0; b < folders_blacklist.count(); ++b) {
            if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d1.path())) {
                addTableItem(tr("Folder %1 blacklisted, skipped").arg(getLabeledPath(d1.path())), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                return;
            }
            if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d2.path())) {
                addTableItem(tr("Folder %1 blacklisted, skipped").arg(getLabeledPath(d2.path())), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                return;
            }
        }
    }
    QFileInfoList d1_entries; QFileInfoList d2_entries;
    /*QDir::Filters filters;
    filters = QDir::NoDotAndDotDot | QDir::Files;
    if (sync_hidden->isChecked()) { filters |= QDir::Hidden; }
    if (!sync_nosubdirs->isChecked()) { filters |= QDir::AllDirs; }*/
    if (extensions.count()==0) {
        d1_entries = d1.entryInfoList(dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
        d2_entries = d2.entryInfoList(dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
    } else {
        d1_entries = d1.entryInfoList(extensions.toList(), dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
        d2_entries = d2.entryInfoList(extensions.toList(), dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
    }
    QString buffer;
    bool found = false; MTFile * file; QDir *temp = new QDir (mp_parent->temp_path);
    for (int i = 0; i < d1_entries.count(); ++i) {
        if (!syncing) return;
        found = false;
        if (d1_entries.at(i).fileName().startsWith(".synkron.sync")) continue;
        if (!ignore_blacklist->isChecked()) {
            if (d1_entries.at(i).isDir()) {
                for (int b = 0; b < folders_blacklist.count(); ++b) {
                    if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d1_entries.at(i).absoluteFilePath())) {
                        addTableItem(tr("Folder %1 blacklisted, skipped").arg(getLabeledPath(d1_entries.at(i).absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                        found = true;
                        break;
                    }
                } if (found) continue;
            } else {
                for (int e = 0; e < exts_blacklist.count(); ++e) {
                    if (d1_entries.at(i).absoluteFilePath().endsWith(exts_blacklist.at(e))) {
                        exts_bl_map.insert(exts_blacklist.at(e), exts_bl_map.value(exts_blacklist.at(e), 0) + 1);
                        found = true;
                        break;
                    }
                }
                if (found) continue;
                for (int b = 0; b < files_blacklist.count(); ++b) {
                    if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d1_entries.at(i).absoluteFilePath())) {
                        addTableItem(tr("File %1 blacklisted, skipped").arg(getLabeledPath(d1_entries.at(i).absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                        found = true;
                        break;
                    }
                } if (found) continue;
            }
        }
        //if (found) continue;
        for (int n = 0; n < d2_entries.count(); ++n) {
            if (!syncing) return;
            if (d1_entries.at(i).fileName() == d2_entries.at(n).fileName()) {
                found = true;
                if (!ignore_blacklist->isChecked()) {
                    if (d2_entries.at(n).isDir()) {
                        for (int b = 0; b < folders_blacklist.count(); ++b) {
                            if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d2_entries.at(n).absoluteFilePath())) {
                                addTableItem(tr("Folder %1 blacklisted, skipped").arg(getLabeledPath(d2_entries.at(n).absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                                found = false;
                                break;
                            }
                        } if (!found) continue;
                    } else {
                        for (int b = 0; b < files_blacklist.count(); ++b) {
                            if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(d2_entries.at(n).absoluteFilePath())) {
                                addTableItem(tr("File %1 blacklisted, skipped").arg(getLabeledPath(d2_entries.at(n).absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                                found = false;
                                break;
                            }
                        } if (!found) continue;
                    }
                }
                if (d1_entries.at(i).isDir() && d2_entries.at(n).isDir()) {
                    if (d2_entries.at(n).isSymLink() || d1_entries.at(i).isSymLink()) {
                        if (followSymlinks()) {
                            QDir subDir1 (d1_entries.at(i).isSymLink() ? d1_entries.at(i).symLinkTarget() : d1_entries.at(i).absoluteFilePath());
                            QDir subDir2 (d2_entries.at(n).isSymLink() ? d2_entries.at(n).symLinkTarget() : d2_entries.at(n).absoluteFilePath());
                            moveContents(subDir1, subDir2);
                            if (d1_entries.at(i).isSymLink()) {
                                QString file_name = d1_entries.at(i).absoluteFilePath();
                                if (QFile(d1_entries.at(i).absoluteFilePath()).remove())
                                    addTableItem(tr("File %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                                else
                                    addTableItem(tr("Error removing file %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
                            }
                            else {
                                QString file_name = d1_entries.at(i).absoluteFilePath();
                                if (d1.rmdir(d1_entries.at(i).fileName()))
                                    addTableItem(tr("Folder %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                                else
                                    addTableItem(tr("Error removing directory %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/folder_16.png", QBrush(Qt::red), QBrush(Qt::white));
                            }
                            continue;
                        } else if (!d1_entries.at(i).isSymLink() || !d2_entries.at(n).isSymLink()) {
                            addTableItem(tr("A file or a folder and a symbolic link with the same name have been found. Unable to synchronise these files. (%1, %2)").arg(d1_entries.at(i).fileName()).arg(d2_entries.at(n).fileName()), "",
                                         QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::red), QBrush(Qt::white));
                            continue;
                        }
                    }
                    else {
                        QDir subDir1 (d1_entries.at(i).absoluteFilePath());
                        QDir subDir2 (d2_entries.at(n).absoluteFilePath());
                        moveContents(subDir1, subDir2);
                        QString file_name = d1_entries.at(i).absoluteFilePath();
                        if (d1.rmdir(d1_entries.at(i).fileName()))
                            addTableItem(tr("Folder %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                        else
                            addTableItem(tr("Error removing directory %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/folder_16.png", QBrush(Qt::red), QBrush(Qt::white));
                        continue;
                    }
                }
                if (d1_entries.at(i).isDir() && !d2_entries.at(n).isDir()) {
                    addTableItem(tr("A folder (%1) and a file (%2) with the same name have been found. Unable to synchronise these files.").arg(d1_entries.at(i).fileName()).arg(d2_entries.at(n).fileName()), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::red), QBrush(Qt::white));
                    continue;
                }
                else if (d2_entries.at(n).isDir() && !d1_entries.at(i).isDir()) {
                    addTableItem(tr("A file (%1) and a folder (%2) with the same name have been found. Unable to synchronise these files.").arg(d1_entries.at(i).fileName()).arg(d2_entries.at(n).fileName()), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::red), QBrush(Qt::white));
                    continue;
                } else {
                    int compared_dates = MTFileInfo(d1_entries.at(i)).lastModified().compareWith(MTFileInfo(d2_entries.at(n)).lastModified(), allowed_difference, allow_DST->isChecked());
                    if (compared_dates < 0) { // d1_entries.at(i) is older than d2_entries.at(n)
                        file = new MTFile (d1_entries.at(i).absoluteFilePath());
                        if (d1.path().startsWith(syncFolder1Text(), Qt::CaseInsensitive) && backupFolder(0)) { goto copying1; }
                        else if (d1.path().startsWith(syncFolder2Text(), Qt::CaseInsensitive) && backupFolder(1)) { goto copying1; }
                        temp->mkdir(QString(update_time));
                        if (!(file->copy(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(d1_entries.at(i).fileName()).arg(synced_files)))) {
                            errorCopyingFile(d1_entries.at(i).absoluteFilePath(), file->errorString(), true);
                            delete file; continue;
                        }
                        saveBackedUpFile(d1_entries.at(i));
                        //mp_parent->synchronised << d1_entries.at(i).fileName() << update_time << d1_entries.at(i).absoluteFilePath();
                        //mp_parent->synchronised << QString("%1/%2/%3.%4").arg(temp_path).arg(update_time).arg(d1_entries.at(i).fileName()).arg(synced_files);
                        addTableItem(tr("Older file %1 backed up and deleted").arg(getLabeledPath(d1_entries.at(i).absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/file.png"));
copying1:
                        QString file_name = d1_entries.at(i).absoluteFilePath();
                        if (file->remove())
                            addTableItem(tr("File %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                        else
                            addTableItem(tr("Error removing file %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
                        delete file;
                        synced_files++;
                    }
                    else if (compared_dates > 0) { // d1_entries.at(i) is newer than d2_entries.at(n)
                        file = new MTFile (d2_entries.at(n).absoluteFilePath());
                        bool skipped_temp = false;
                        if (d2.path().startsWith(syncFolder1Text(), Qt::CaseInsensitive) && backupFolder(0)) { goto copying2; skipped_temp = true; }
                        else if (d2.path().startsWith(syncFolder2Text(), Qt::CaseInsensitive) && backupFolder(1)) { goto copying2; skipped_temp = true; }
                        temp->mkdir(update_time);
                        if (!(file->copy(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(d2_entries.at(n).fileName()).arg(synced_files)))) {
                            errorCopyingFile(d2_entries.at(n).absoluteFilePath(), file->errorString(), true);
                            delete file; continue;
                        }
                        saveBackedUpFile(d2_entries.at(n));
                        //mp_parent->synchronised << d2_entries.at(n).fileName() << update_time << d2_entries.at(n).absoluteFilePath();
                        //mp_parent->synchronised << QString("%1/.Synkron/%2/%3.%4").arg(QDir::homePath()).arg(update_time).arg(d2_entries.at(n).fileName()).arg(synced_files);
copying2:
                        QString file_name = d2_entries.at(n).absoluteFilePath();
                        file->remove();
                        delete file;
                        file = new MTFile (d1_entries.at(i).absoluteFilePath());
                        buffer.clear(); buffer = d2_entries.at(n).absoluteFilePath();
                        if (!file->copy(buffer)) {
                            errorCopyingFile(d1_entries.at(i).absoluteFilePath(), file->errorString(), false);
                            delete file;
                            if (!skipped_temp) {
                                if (mp_parent->restoreFile(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(d2_entries.at(n).fileName()).arg(synced_files),
                                                           d2_entries.at(n).absoluteFilePath(), update_time)) {
                                    addTableItem(tr("File %1 restored").arg(getLabeledPath(d2_entries.at(n).absoluteFilePath())), "", "", QBrush(Qt::darkBlue), QBrush(Qt::white));
                                }
                            } continue;
                        }
                        file_name = d1_entries.at(i).absoluteFilePath();
                        if (file->remove())
                            addTableItem(tr("File %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                        else
                            addTableItem(tr("Error removing file %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
                        addTableItem(d1_entries.at(i).absoluteFilePath(), d2_entries.at(n).absoluteFilePath(), QString::fromUtf8(":/new/prefix1/images/file.png"));
                        synced_files++; delete file;
                    }
                    else if (compared_dates == 0) { // d1_entries.at(i) is the same as d2_entries.at(n)
                        file = new MTFile (d1_entries.at(i).absoluteFilePath());
                        QString file_name = d1_entries.at(i).absoluteFilePath();
                        if (file->remove())
                            addTableItem(tr("File %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                        else
                            addTableItem(tr("Error removing file %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
                        synced_files++; delete file;
                    }
                }
                break;
            }
        }

        if (found == false) {
            if (!syncing) return;
            bool symfound = false;
            QString file_name;
            //if (page->updateOnly1() && QString("%1/%2").arg(d2.path()).arg(d1_entries.at(i).fileName()).startsWith(page->syncFolder1Text())) goto end;
            if (updateOnly(1)/* && QString("%1/%2").arg(d2.path()).arg(d1_entries.at(i).fileName()).startsWith(page->syncFolder2Text())*/) goto end;
            file = new MTFile (d1_entries.at(i).absoluteFilePath());
            buffer.clear();
            buffer = QString("%1/%2").arg(d2.path()).arg(d1_entries.at(i).fileName());
            if (d1_entries.at(i).isDir()) {
                if (!d1_entries.at(i).isSymLink() || (d1_entries.at(i).isSymLink() && followSymlinks())) {
                    if (!(d2.mkdir(d1_entries.at(i).fileName()))) {
                        errorCopyingFolder(d1_entries.at(i).absoluteFilePath());
                        delete file; continue;
                    }
                    QDir subDir1 (d1_entries.at(i).isSymLink() ? d1_entries.at(i).symLinkTarget() : d1_entries.at(i).absoluteFilePath());
                    QDir subDir2 (QString("%1/%2").arg(d2.absolutePath()).arg(d1_entries.at(i).fileName()));
                    addTableItem(d1_entries.at(i).absoluteFilePath(), buffer, QString::fromUtf8(":/new/prefix1/images/folder_16.png"));
                    moveContents(subDir1, subDir2);
                    if (d1_entries.at(i).isSymLink()) {
                        file_name = d1_entries.at(i).absoluteFilePath();
                        if (file->remove())
                            addTableItem(tr("File %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                        else
                            addTableItem(tr("Error removing file %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
                    }
                    else {
                        file_name = d1_entries.at(i).absoluteFilePath();
                        if (d1.rmdir(d1_entries.at(i).fileName()))
                            addTableItem(tr("Folder %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                        else
                            addTableItem(tr("Error removing directory %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/folder_16.png", QBrush(Qt::red), QBrush(Qt::white));
                    }
                } else if (d1_entries.at(i).isSymLink() && !followSymlinks()) symfound = true;
            }
            if (!d1_entries.at(i).isDir() || symfound) {
                if (!file->copy(buffer)) {
                    errorCopyingFile(d1_entries.at(i).absoluteFilePath(), file->errorString(), false);
                    delete file; continue;
                }
                else {
                    addTableItem(d1_entries.at(i).absoluteFilePath(), buffer, QString::fromUtf8(":/new/prefix1/images/file.png"));
                }
            }
            file_name = d1_entries.at(i).absoluteFilePath();
            if (file->remove())
                addTableItem(tr("File %1 deleted").arg(getLabeledPath(file_name)), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
            else
                addTableItem(tr("Error removing file %1").arg(getLabeledPath(file_name)), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
            synced_files++;
            delete file;
end:
            synced_files = synced_files;
        }
    }
}

void SyncPage::subGroupSync(MTMap<QString, int> sync_folders_set, MTStringSet rel_paths)
{
    if (!syncing) return;
    if (rel_paths.count() == 0) {
        for (int i = 0; i < sync_folders_set.count(); ++i) {
            //Creating a set of all file names ---------------------------------
            QDir sync_dir = sync_folders_set.key(i);
            if (!sync_dir.exists()) continue;
            QStringList entries;
            if (extensions.count()==0) {
                entries = sync_dir.entryList(dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
            } else {
                entries = sync_dir.entryList(extensions.toList(), dir_filters, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
            }
            for (int n = 0; n < entries.count(); ++n) {
                rel_paths << entries.at(n);
            }
        }
    }

    MTMap<QString, int> sync_folders_set2;
    //MTMap<QString, int> slaves_set2;
    MTFileInfo * file_info = nullptr;
    MTFileInfo * file_info2 = nullptr;
    MTFile * file;

    for (int i = 0; i < rel_paths.count(); ++i) {
        if (!syncing) { return; }
        if (rel_paths.at(i).startsWith(".synkron.sync")) continue;
        if (!ignore_blacklist->isChecked()) {
            bool found_ext = false;
            for (int e = 0; e < exts_blacklist.count(); ++e) {
                if (rel_paths.at(i).endsWith(exts_blacklist.at(e))) {
                    exts_bl_map.insert(exts_blacklist.at(e), exts_bl_map.value(exts_blacklist.at(e), 0) + 1);
                    found_ext = true;
                    break;
                }
            }
            if (found_ext) continue;
        }
        sync_folders_set2.clear();
        release(file_info);
        for (int n = 0; n < sync_folders_set.count(); ++n) {
            // Obtaining absolute paths for the file names ---------------------
            release(file_info2);
            file_info2 = new MTFileInfo (QString("%1/%2").arg(sync_folders_set.key(n)).arg(rel_paths.at(i)));
            if (!file_info2->exists()) {
                if (!file_info2->dir().exists())
                    return;

                sync_folders_set2.setValue(file_info2->absoluteFilePath(), sync_folders_set.value(n));
                continue;
            }
            if (file_info2->isDir()) {
                if (file_info && !file_info->isDir()) {
                    addTableItem(tr("A file and a folder with the same name (%1) have been found. Unable to synchronise these files.").arg(file_info->fileName()), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::red), QBrush(Qt::white));
                    release(file_info);
                    break;
                }
                if (!ignore_blacklist->isChecked()) {
                    for (int b = 0; b < folders_blacklist.count(); ++b) {
                        if (QRegExp(folders_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info2->absoluteFilePath())) {
                            addTableItem(tr("Folder %1 blacklisted, skipped").arg(getLabeledPath(file_info2->absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/folder_16.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                            release(file_info); release(file_info2);
                            break;
                        }
                    }
                    if (file_info2 == nullptr) break;
                }
            } else {
                if (file_info && file_info->isDir()) {
                    addTableItem(tr("A file and a folder with the same name (%1) have been found. Unable to synchronise these files.").arg(file_info->fileName()), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::red), QBrush(Qt::white));
                    release(file_info);
                    break;
                }
                if (!ignore_blacklist->isChecked()) {
                    for (int b = 0; b < files_blacklist.count(); ++b) {
                        if (QRegExp(files_blacklist.at(b), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(file_info2->absoluteFilePath())) {
                            addTableItem(tr("File %1 blacklisted, skipped").arg(getLabeledPath(file_info2->absoluteFilePath())), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
                            release(file_info); release(file_info2);
                            break;
                        }
                    }
                    if (file_info2 == nullptr) break;
                }
            }
            if (!file_info && !isSlave(sync_folders_set.value(n))) { file_info = new MTFileInfo (file_info2->absoluteFilePath()); }
            sync_folders_set2.setValue(file_info2->absoluteFilePath(), sync_folders_set.value(n));
        }
        release(file_info2);

        if (!syncing) { return; }
        if (sync_folders_set2.count() > 1) {
            if (!file_info) { // No masters were found -----------------
                for (int n = 0; n < sync_folders_set2.count(); ++n) {
                    if (sync_folders->at(sync_folders_set2.value(n))->dont_update_act->isChecked()) continue;
                    if (isSlave(sync_folders_set2.value(n))) {
                        file_info2 = new MTFileInfo (sync_folders_set2.key(n));
                        if (file_info2->exists()) {
                            if (file_info2->isDir() && !file_info2->isSymLink()) {
                                backupAndRemoveDir(file_info2->absoluteFilePath(), !backupFolder(sync_folders_set2.value(n)));
                            } else {
                                backupAndRemoveFile(*file_info2, !backupFolder(sync_folders_set2.value(n)));
                            }
                        }
                        release(file_info2);
                    }
                }
            } else if (file_info->isDir() && !file_info->isSymLink()) { //Is dir ------
                MTMap<QString, int> sync_folders_set3;
                for (int n = 0; n < sync_folders_set2.count(); ++n) {
                    // Obtaining a set of available dirs -----------------------
                    file_info2 = new MTFileInfo (sync_folders_set2.key(n));
                    if (!file_info2->exists()) {
                        if (!file_info2->dir().exists())
                            return;
                        if (propagate_deletions->isChecked() && !isSlave(sync_folders_set2.value(n))) {
                            //Propagated deletions -----------------------------
                            if (isInGroupDatabase(file_info2->absoluteFilePath())) {
                                MTFileInfo * file_info3;
                                release(file_info2);
                                for (int m = 0; m < sync_folders_set2.count(); ++m) {
                                    if (!syncing) { return; }
                                    file_info3 = new MTFileInfo (sync_folders_set2.key(m));
                                    if (sync_folders->at(sync_folders_set2.value(m))->dont_update_act->isChecked()) continue;
                                    if (file_info3->exists()) backupAndRemoveDir(file_info3->absoluteFilePath(), !backupFolder(sync_folders_set2.value(m)));
                                    delete file_info3;
                                }
                                break;
                            }
                        }
                        //Creating folder --------------------------------------
                        if (!updateOnly(sync_folders_set2.value(n)) && !sync_folders->at(sync_folders_set2.value(n))->dont_update_act->isChecked()) {
                            if (!createEmptyFolders(sync_folders_set2.value(n))) {
                                sync_folders_set3.setValue(file_info2->absoluteFilePath(), sync_folders_set2.value(n));
                            } else if (QDir().mkpath(file_info2->absoluteFilePath())) {
                                addTableItem(file_info->absoluteFilePath(), file_info2->absoluteFilePath(), QString::fromUtf8(":/new/prefix1/images/folder_16.png"));
                                sync_folders_set3.setValue(file_info2->absoluteFilePath(), sync_folders_set2.value(n));
                                synced_files++;
                            } else {
                                errorCopyingFolder(file_info2->absoluteFilePath());
                            }
                        }
                    } else {
                        sync_folders_set3.setValue(file_info2->absoluteFilePath(), sync_folders_set2.value(n));
                    }
                    release(file_info2);
                }
                if (sync_folders_set3.count() > 1) {
                    subGroupSync(sync_folders_set3);
                }
            } else { //Is file or symlink --------------------------------------
                for (int n = 0; n < sync_folders_set2.count(); ++n) {
                    if (isSlave(sync_folders_set2.value(n))) continue;
                    file_info2 = new MTFileInfo (sync_folders_set2.key(n));
                    if (file_info2->exists() && (file_info2->lastModified() > file_info->lastModified())) {
                        release(file_info);
                        file_info = new MTFileInfo (file_info2->absoluteFilePath());
                    }
                    release(file_info2);
                }
                for (int n = 0; n < sync_folders_set2.count(); ++n) {
                    release(file_info2);
                    file_info2 = new MTFileInfo (sync_folders_set2.key(n));
                    if (!file_info2->exists()) {
                        if (!file_info2->dir().exists())
                            return;
                        if (propagate_deletions->isChecked() && !isSlave(sync_folders_set2.value(n))) {
                            //Propagated deletions -----------------------------
                            if (isInGroupDatabase(file_info2->absoluteFilePath())) {
                                MTFileInfo * file_info3;
                                for (int m = 0; m < sync_folders_set2.count(); ++m) {
                                    if (!syncing) { return; }
                                    file_info3 = new MTFileInfo (sync_folders_set2.key(m));
                                    if (sync_folders->at(sync_folders_set2.value(m))->dont_update_act->isChecked()) continue;
                                    if (file_info3->exists()) backupAndRemoveFile(*file_info3, !backupFolder(sync_folders_set2.value(m)));
                                    delete file_info3;
                                }
                                break;
                            }
                        }
                        //Copying file -----------------------------------------
                        if (!updateOnly(sync_folders_set2.value(n)) && !sync_folders->at(sync_folders_set2.value(n))->dont_update_act->isChecked()) {
                            status_table_item->setText(tr("Copying file %1").arg(file_info->absoluteFilePath())); qApp->processEvents();
                            file = new MTFile (file_info->absoluteFilePath());
                            if (!file->copy(file_info2->absoluteFilePath())) {
                                errorCopyingFile(file_info->absoluteFilePath(), file->errorString(), false);
                            } else {
                                addTableItem(file_info->absoluteFilePath(), file_info2->absoluteFilePath(), QString::fromUtf8(":/new/prefix1/images/file.png"));
                                synced_files++;
                            }
                            delete file;
                            status_table_item->setText(tr("Searching for changes")); qApp->processEvents();
                        }
                    } else {
                        if (sync_folders->at(sync_folders_set2.value(n))->dont_update_act->isChecked())
                            continue;
                        int compared_dates = file_info->lastModified().compareWith(file_info2->lastModified(), allowed_difference, allow_DST->isChecked());
                        if (compared_dates > 0 || (isSlave(sync_folders_set2.value(n)) && compared_dates != 0)) { // file_info is newer than file_info2
                            //Overwriting older file -------------------------------
                            if (alert_collisions->isChecked() && !isSlave(sync_folders_set2.value(n))) {
                                if (checkForCollision(file_info->absoluteFilePath(), file_info2->absoluteFilePath())) {
                                    collided.insertByValue(file_info->absoluteFilePath(), file_info2->absoluteFilePath());
                                    continue;
                                }
                            }
                            status_table_item->setText(tr("Updating file %1").arg(file_info2->absoluteFilePath())); qApp->processEvents();
                            file = new MTFile (file_info2->absoluteFilePath());
                            if (!backupFolder(sync_folders_set2.value(n))) {
                                QDir(mp_parent->temp_path).mkdir(update_time);
                                if (!file->copy(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(file_info2->fileName()).arg(synced_files))) {
                                    errorCopyingFile(file_info2->absoluteFilePath(), file->errorString(), true);
                                    delete file;
                                    continue;
                                }
                                saveBackedUpFile(*file_info2);
                            }
                            if (!file->remove()) {
                                errorRemovingFile(file_info2->absoluteFilePath(), file->errorString());
                                delete file; continue;
                            }
                            delete file;
                            file = new MTFile (file_info->absoluteFilePath());
                            if (!file->copy(file_info2->absoluteFilePath())) {
                                errorCopyingFile(file_info->absoluteFilePath(), file->errorString(), false);
                                if (mp_parent->restoreFile(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(file_info2->fileName()).arg(synced_files),
                                                           file_info2->absoluteFilePath(), update_time)) {
                                    addTableItem(tr("File %1 restored").arg(getLabeledPath(file_info2->absoluteFilePath())), "", "", QBrush(Qt::darkBlue), QBrush(Qt::white));
                                }
                            } else {
                                addTableItem(file_info->absoluteFilePath(), file_info2->absoluteFilePath(), QString::fromUtf8(":/new/prefix1/images/file.png"));
                                synced_files++;
                            }
                            delete file;
                            status_table_item->setText(tr("Searching for changes")); qApp->processEvents();
                        }
                    }
                }
                release(file_info2);
            }
        }
    }
    release(file_info);
}

void AbstractSyncPage::countExtsBl()
{
    QMapIterator<QString, int> i(exts_bl_map);
    while (i.hasNext()) {
        i.next();
        if (i.value() != 0) {
            addTableItem(tr("%1 %2 files skipped").arg(i.value()).arg(i.key()), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkGray), QBrush(Qt::white));
        }
    }
    exts_bl_map.clear();
}

void MainWindow::syncAll()
{
    syncingAll = true; int all_synced_files = 0;
    for (int i = 0; i<(tabWidget->count()); ++i) {
        all_synced_files += tabs.value(tabWidget->widget(i))->sync();
    }
    showTrayMessage(tr("Synchronisation complete"), tr("%1 files synchronised").arg(all_synced_files));
    syncingAll = false;
    if (actionShut_down_after_sync->isChecked()) {
        if (!isSyncing()) {
            shutDownComputer();
        }
    }
    if (actionQuit_after_sync->isChecked()) {
        if (!isSyncing()) {
            this->closeApp();
        }
    }
}

void AbstractSyncPage::addTableItem(QString source, QString destination, QString icon, QBrush background, QBrush foreground)
{
    tableWidget()->insertRow(tableWidget()->rowCount()-1);
    QTableWidgetItem *item = new QTableWidgetItem;
    if (icon != "") item->setIcon(QIcon(icon));
    if (destination == "") {
        item->setBackground(background);
        item->setForeground(foreground);
        tableWidget()->setSpan(tableWidget()->rowCount() - 2, 0, 1, 2);
    } else {
        replaceFolderLabelInPath(source);
        replaceFolderLabelInPath(destination);
    }
    item->setText(source);
    tableWidget()->setItem(tableWidget()->rowCount() - 2, 0, item);
    if (destination != "") {
        item = new QTableWidgetItem(destination);
        if (icon != "") item->setIcon(QIcon(icon));
        tableWidget()->setItem(tableWidget()->rowCount() - 2, 1, item);
        tableWidget()->setSpan(tableWidget()->rowCount() - 2, 0, 1, 1);
    }
    tableWidget()->setRowHeight(tableWidget()->rowCount() - 2, 16);
    tableWidget()->setSpan(tableWidget()->rowCount()-1, 0, 1, 2);
    tableWidget()->scrollToItem(status_table_item, QAbstractItemView::PositionAtBottom);
    qApp->processEvents();
}

void AbstractSyncPage::errorCopyingFile(QString file_name, QString error, bool to_temp)
{
    if (to_temp) {
        file_name.append(" " + tr("to temp"));
    }
    replaceFolderLabelInPath(file_name);
    addTableItem(tr("Error copying file %1: %2").arg(file_name).arg(error), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
}

void AbstractSyncPage::errorCopyingFolder(QString folder_name)
{
    replaceFolderLabelInPath(folder_name);
    addTableItem(tr("Error creating folder %1").arg(folder_name), "", ":/new/prefix1/images/folder_16.png", QBrush(Qt::red), QBrush(Qt::white));
}

void AbstractSyncPage::errorRemovingFile(QString file_name, QString error)
{
    replaceFolderLabelInPath(file_name);
    addTableItem(tr("Error removing file %1: %2").arg(file_name).arg(error), "", ":/new/prefix1/images/file.png", QBrush(Qt::red), QBrush(Qt::white));
}

void SyncPage::syncPage()
{
    emit sigsync(tab);
}

void MainWindow::tabNameChanged()
{
    if (tabWidget->count() == 0) return;
    SyncPage * page = tabs.value(tabWidget->currentWidget());
    if (page->tab_name->text() == tabWidget->tabText(tabWidget->indexOf(page->tab_stw))) return;
    QMapIterator<QTableWidgetItem*, SyncSchedule*>  i(item_sched_map);
    while (i.hasNext()) {
        i.next();
        for (int n = 0; n < i.value()->sched_tab_list.count(); ++n) {
            if (tabWidget->tabText(tabWidget->indexOf(page->tab_stw))==i.value()->sched_tab_list.at(n)) {
                i.value()->sched_tab_list[n] = page->tab_name->text();
            }
        }
    }
    if (page->propagate_deletions->isChecked()) {
        page->changeTabNameInDatabase(page->tab_name->text(), tabWidget->tabText(tabWidget->indexOf(page->tab_stw)));
    }
    tabWidget->setTabText(tabWidget->indexOf(page->tab_stw), page->tab_name->text());
}

void SyncPage::setSyncEnabled(bool enable)
{
    sync_folders->setEnabled(enable);
    log_search->setEnabled(enable);
    tab_name->setEnabled(enable);
    advanced_menu->setEnabled(enable);
    show_advanced->setEnabled(enable);
    syncing = !enable;
    stop_sync_btn->setVisible(!enable);
    sync_btn->setVisible(enable);
    go_to_analyse->setEnabled(enable);
    add_folder_btn->setEnabled(enable);
    mp_parent->actionClose_sync->setEnabled(enable);
    if (!enable) {
        synced_files = 0;
        update_time = (QDateTime::currentDateTime()).toString("yyyy.MM.dd-hh.mm.ss");
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    } else {
        QApplication::restoreOverrideCursor();
    }
    qApp->processEvents();
}

void AbstractSyncPage::moveStateChanged(bool checked)
{
    propagate_deletions->setEnabled(!checked);
    alert_collisions->setEnabled(!checked);
    moveChecked(checked);
}

void SyncPage::moveChecked(bool checked)
{
    if (checked){
        sync_btn->setText(tr("Move"));
        sync_btn->setStatusTip(tr("Move contents to folder 2, leaving folder 1 empty"));
    } else {
        sync_btn->setText(tr("Sync"));
        sync_btn->setStatusTip(tr("Synchronise"));
    }
}

void SyncPage::syncFoldersChanged()
{
    bool enable = true;
    if (sync_folders->count() > 2) enable = false;
    if (!enable) {
        symlinks->setChecked(false);
        move->setChecked(false);
    }
    move->setEnabled(enable);
}

void SyncPage::syncFoldersEdited()
{
    path_replacements.clear();
    //QStringList s_folders_list = currentSyncFoldersList();
    for (int i = 0; i < sync_folders->count(); ++i) {
        path_replacements.insert(sync_folders->at(i)->path(), sync_folders->at(i)->label().isEmpty() ? sync_folders->at(i)->path() : sync_folders->at(i)->label());
    }
    orderPathReplacementsByLength();
}

void SyncPage::showThisPage()
{
    mp_parent->tabWidget->setCurrentWidget(tab_stw);
    mp_parent->actionSynchronise->trigger();
}

void AbstractSyncPage::saveBackedUpFile(QFileInfo file_info)
{
    mp_parent->temp_settings->beginGroup(update_time);
    mp_parent->temp_settings->beginGroup(QString("%1.%2").arg(file_info.fileName()).arg(synced_files));
    mp_parent->temp_settings->setValue("file_name", file_info.fileName());
    mp_parent->temp_settings->setValue("old_file_path", file_info.absoluteFilePath());
    mp_parent->temp_settings->setValue("temp_file_path", QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(file_info.fileName()).arg(synced_files));
    mp_parent->temp_settings->endGroup();
    mp_parent->temp_settings->endGroup();
}

void AbstractSyncPage::copyFile(QString source_str, QString dest_str, bool touch)
{
    QString status_text = status_table_item->text();
    QFileInfo source_info(source_str);
    QFileInfo dest_info(dest_str);
    status_table_item->setText(tr("Updating file %1").arg(dest_str)); qApp->processEvents();
    MTFile source_file(source_str, qApp);
    MTFile dest_file(dest_str, qApp);
    if (touch) {
        source_file.touch(qApp);
    }
    bool skipped_temp = false;
    if (backupFolders()) { skipped_temp = true; }
    else {
        QDir(mp_parent->temp_path).mkdir(QString(update_time));
        if (!(dest_file.copy(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(dest_info.fileName()).arg(synced_files)))) {
            errorCopyingFile(dest_str, dest_file.errorString(), true);
            return;
        }
        saveBackedUpFile(dest_info);
    }
    dest_file.remove();
    if (!source_file.copy(dest_str)) {
        errorCopyingFile(source_str, source_file.errorString(), false);
        if (!skipped_temp) {
            if (mp_parent->restoreFile(QString("%1/%2/%3.%4").arg(mp_parent->temp_path).arg(update_time).arg(dest_str).arg(synced_files),
                                       dest_str, update_time)) {
                addTableItem(tr("File %1 restored").arg(dest_str), "", "", QBrush(Qt::darkBlue), QBrush(Qt::white));
            }
        } return;
    }
    addTableItem(source_str, dest_str, QString::fromUtf8(":/new/prefix1/images/file.png"));
    status_table_item->setText(status_text); qApp->processEvents();
    synced_files++;
}

void AbstractSyncPage::replaceFolderLabelInPath(QString & path)
{
    for (int i = 0; i < path_replacements.count(); ++i) {
        if (path.startsWith(path_replacements.key(i))) {
            path.replace(0, path_replacements.key(i).count(), path_replacements.value(i));
            break;
        }
    }
}

QString AbstractSyncPage::getLabeledPath(QString path) {
    for (int i = 0; i < path_replacements.count(); ++i) {
        if (path.startsWith(path_replacements.key(i))) {
            path.replace(0, path_replacements.key(i).count(), path_replacements.value(i));
            break;
        }
    }
    return path;
}

void AbstractSyncPage::orderPathReplacementsByLength()
{
    MTDictionary dict1;
    MTDictionary dict2 = path_replacements;
    QString dict_key; QString dict_value;
    for (int i = 0; i < path_replacements.count(); ++i) {
        dict_key = "";
        dict_value = "";
        int remove_at = -1;
        for (int n = 0; n < dict2.count(); ++n) {
            if (dict2.key(n).count() > dict_key.count()) {
                dict_key = dict2.key(n);
                dict_value = dict2.value(n);
                remove_at = n;
            }
        }
        if (remove_at >= 0) {
            dict2.removeAt(remove_at);
        }
        dict1.insert(dict_key, dict_value);
    }
    path_replacements = dict1;
}
