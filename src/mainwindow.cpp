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

enum MainStackedWidgetIndex
{
    kMainStackedWidgetIndexSync = 0,
    kMainStackedWidgetIndexMultisync = 3
};

MainWindow::MainWindow(QSettings * s)
{
    setupUi(this);

    f_ver = 1.63;
    ver = "1.6.3";

    if (tr("LTR") == "RTL")
    {
        qApp->setLayoutDirection(Qt::RightToLeft);
    }

#ifdef Q_WS_MAC
    actionBrushedMetalStyle = new QAction(tr("Use the brushed metal style"), this);
    actionBrushedMetalStyle->setStatusTip(tr("Use the brushed metal style"));
    actionBrushedMetalStyle->setCheckable(true);
    menuOptions->addAction(actionBrushedMetalStyle);

    actionQuit->setMenuRole(QAction::QuitRole);
    actionAbout->setMenuRole(QAction::AboutRole);
#endif

#if QT_VERSION >= 0x050000
    http = new QNetworkAccessManager(this);
#else
    http = new QHttp(this);
#endif


    http_buffer = new QBuffer(this);

    createActions();
    createTrayIcon();
    trayIcon->show();
    trayIconVisible(true);
    syncingAll = false;
    sched_removed = false;
    run_hidden = false;
    no_closedialogue = false;
    skip_close_event = false;
    shown_manually = false;

    tw_schedules->setHorizontalHeaderLabels(QStringList() << tr("Schedule name") << tr("Status"));
    tw_schedules->verticalHeader()->hide();

    temp_path = QString("%1/.Synkron").arg(QDir::homePath());

#ifndef Q_WS_WIN
#if QT_VERSION >= 0x050000
    tw_schedules->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    tw_schedules->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

    actionShut_down_after_sync->setVisible(false);
#else
    tw_schedules->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
#endif

#if QT_VERSION >= 0x050000
    syncs_syncview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    syncs_syncview->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    syncs_syncview->horizontalHeader()->hide();
    syncs_syncview->verticalHeader()->hide();

#if QT_VERSION >= 0x050000
    multisyncs_syncview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    multisyncs_syncview->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    multisyncs_syncview->horizontalHeader()->hide();
    multisyncs_syncview->verticalHeader()->hide();

    actgrpView = new QActionGroup(this);
    actgrpView->addAction(actionSynchronise);
    actgrpView->addAction(actionRestore);
    actgrpView->addAction(actionBlacklist);
    actgrpView->addAction(actionMultisync);
    actgrpView->addAction(actionScheduler);
    actgrpView->addAction(actionFilters);
    actgrpView->addAction(actionSyncView);

    QActionGroup * options_actgrp = new QActionGroup(this);
    options_actgrp->addAction(actionShut_down_after_sync);
    options_actgrp->addAction(actionQuit_after_sync);
    options_actgrp->addAction(actionSync_at_launch);
    options_actgrp->setExclusive(false);

    actionShow_icons_only = new QAction(tr("Show icons only"), this);
    actionShow_icons_only->setCheckable(true);

    QTranslator translator; translator.load(":/i18n/Synkron-i18n.qm");
    synkron_i18n.insert("English", "English");
    synkron_i18n.insert(translator.translate("LanguageNames", "Slovak"), "Slovak");
    synkron_i18n.insert(translator.translate("LanguageNames", "German"), "German");
    synkron_i18n.insert(translator.translate("LanguageNames", "Japanese"), "Japanese");
    synkron_i18n.insert(translator.translate("LanguageNames", "Arabic"), "Arabic");
    synkron_i18n.insert(translator.translate("LanguageNames", "Russian"), "Russian");
    synkron_i18n.insert(translator.translate("LanguageNames", "Spanish"), "Spanish");
    synkron_i18n.insert(translator.translate("LanguageNames", "Brazilian Portuguese"), "Brazilian Portuguese");
    synkron_i18n.insert(translator.translate("LanguageNames", "Polish"), "Polish");
    synkron_i18n.insert(translator.translate("LanguageNames", "Italian"), "Italian");
    synkron_i18n.insert(translator.translate("LanguageNames", "French"), "French");
    synkron_i18n.insert(translator.translate("LanguageNames", "Valencian"), "Valencian");
    synkron_i18n.insert(translator.translate("LanguageNames", "Finnish"), "Finnish");
    synkron_i18n.insert(translator.translate("LanguageNames", "Czech"), "Czech");
    synkron_i18n.insert(translator.translate("LanguageNames", "Chinese"), "Chinese");
    synkron_i18n.insert(translator.translate("LanguageNames", "Dutch"), "Dutch");
    synkron_i18n.insert(translator.translate("LanguageNames", "Romanian"), "Romanian");
    synkron_i18n.insert(translator.translate("LanguageNames", "Turkish"), "Turkish");

    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionNew_sync, SIGNAL(triggered()), this, SLOT(addTab()));
    connect(actionClose_sync, SIGNAL(triggered()), this, SLOT(closeTab()));
    connect(actgrpView, SIGNAL(triggered(QAction*)), this, SLOT(switchView(QAction*)));
    connect(options_actgrp, SIGNAL(triggered(QAction*)), this, SLOT(optionClicked(QAction*)));
    connect(restore_list, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(restoreItemChanged(QListWidgetItem *, QListWidgetItem *)));
    connect(to_black_list, SIGNAL(stateChanged(int)), this, SLOT(addToBlackList(int)));
    connect(restore_files, SIGNAL(released()), this, SLOT(restoreFiles()));
    connect(blacklist_addfile, SIGNAL(released()), this, SLOT(addFileToBlacklist()));
    connect(blacklist_removefile, SIGNAL(released()), this, SLOT(removeFileFromBlacklist()));
    connect(blacklist_addfolder, SIGNAL(released()), this, SLOT(addFolderToBlacklist()));
    connect(blacklist_removefolder, SIGNAL(released()), this, SLOT(removeFolderFromBlacklist()));
    connect(blacklist_addext, SIGNAL(released()), this, SLOT(addExtToBlacklist()));
    connect(blacklist_removeext, SIGNAL(released()), this, SLOT(removeExtFromBlacklist()));
    connect(selTmpAllBtn, SIGNAL(released()), this, SLOT(selTmpAll()));
    connect(actionRun_hidden, SIGNAL(toggled(bool)), this, SLOT(setRunHidden(bool)));
    connect(actionSync_all, SIGNAL(triggered()), this, SLOT(syncAll()));
    connect(actionCheck_for_updates, SIGNAL(triggered()), this, SLOT(checkForUpdates()));
    connect(http, SIGNAL(done(bool)), this, SLOT(httpRequestFinished(bool)));
    connect(add_schedule, SIGNAL(released()), this, SLOT(addSchedule()));
    connect(remove_schedule, SIGNAL(released()), this, SLOT(removeSchedule()));
    connect(tw_schedules, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(scheduleActivated(int, int, int, int)));
    connect(sched_name, SIGNAL(textEdited(const QString)), this, SLOT(setSchedName(const QString)));
    connect(sched_add_time, SIGNAL(released()), this, SLOT(addSchedTime()));
    connect(sched_remove_time, SIGNAL(released()), this, SLOT(removeSchedTime()));
    connect(sched_start, SIGNAL(released()), this, SLOT(startSchedule()));
    connect(sched_stop, SIGNAL(released()), this, SLOT(stopSchedule()));
    connect(restore_search, SIGNAL(textEdited(const QString)), this, SLOT(searchLw(const QString)));
    connect(startall_schedules, SIGNAL(released()), this, SLOT(startAllSchedules()));
    connect(stopall_schedules, SIGNAL(released()), this, SLOT(stopAllSchedules()));
    connect(actionChange_language, SIGNAL(triggered()), this, SLOT(changeLanguage()));
    connect(filter_add, SIGNAL(released()), this, SLOT(addFilter()));
    connect(filter_remove, SIGNAL(released()), this, SLOT(removeFilter()));
    connect(filter_add_extension, SIGNAL(released()), this, SLOT(addFilterExtension()));
    connect(filter_remove_extension, SIGNAL(released()), this, SLOT(removeFilterExtension()));
    connect(filter_list, SIGNAL(itemSelectionChanged()), this, SLOT(filterChanged()));
    connect(sched_tab_lw, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(schedTabClicked(QListWidgetItem*)));
    connect(sched_multitab_lw, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(schedMultitabClicked(QListWidgetItem*)));
    connect(sched_time_lw, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(schedTimeClicked(QListWidgetItem*)));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(closeApp()));
    connect(actionSave_log, SIGNAL(triggered()), this, SLOT(saveSyncLog()));
    connect(restore_list, SIGNAL(sigconmenu(QPoint)), this, SLOT(restoreListConMenu(QPoint)));
    connect(sched_interval_spin, SIGNAL(valueChanged(int)), this, SLOT(schedIntervalChanged(int)));
    connect(timing_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(timingTabIndexChanged(int)));
    connect(actionSave_tab, SIGNAL(triggered()), this, SLOT(saveTab()));
    connect(actionSave_tab_as, SIGNAL(triggered()), this, SLOT(saveTabAs()));
    connect(actionLoad_tab, SIGNAL(triggered()), this, SLOT(loadTab()));
    connect(actionChange_temp, SIGNAL(triggered()), this, SLOT(changeTemp()));
    connect(actionShow_icons_only, SIGNAL(toggled(bool)), this, SLOT(showIconsOnly(bool)));
    connect(actionSync, SIGNAL(triggered()), this, SLOT(syncCurrentTab()));
    connect(actionAnalyse, SIGNAL(triggered()), this, SLOT(analyseCurrentTab()));
    connect(menuTab, SIGNAL(aboutToShow()), this, SLOT(aboutToShowTabMenu()));

    connect(mainStackedWidget,SIGNAL(currentChanged(int)),this,
            SLOT(updateActionsEnabling(int)));

    setCleanGB();
    setSelectGB();
    setSchedDatesGB();
    tabWidget->removeTab(0);
    multi_tabWidget->removeTab(0);

    sync_settings = s;
    readSettings();
    loadTempSettings();
    if (tabWidget->count()==0) addSyncTab();
    if (multi_tabWidget->count()==0) addMultiTab();

    QSettings settings ("Matus Tomlein", "Synkron");
    tcp_server = new QTcpServer(this);
    QObject::connect(tcp_server, SIGNAL(newConnection()), this, SLOT(addConnection()));
    tcp_socket = new QTcpSocket(this);
    //QObject::connect(tcp_socket, SIGNAL(connected()), this, SLOT(sendMessageAndClose()));
    //QObject::connect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(initServer(QAbstractSocket::SocketError)));
    tcp_socket->connectToHost("Localhost", settings.value("process_id", 1).toUInt());
    if (tcp_socket->waitForConnected(1000)) {
        sendMessageAndClose();
        skip_close_event = true;
        QTimer::singleShot(0, this, SLOT(close())); }
    else {
        initServer(QAbstractSocket::SocketTimeoutError);
        if (actionSync_at_launch->isChecked()) {
            QTimer::singleShot(0, this, SLOT(syncAll()));
        }
    }
    QTimer::singleShot(2000, this, SLOT(setShownManually()));
}

// +++ Connection +++

void MainWindow::initServer(QAbstractSocket::SocketError)
{
    if (tcp_server->isListening()) { return; }
    if (!tcp_server->listen() && !tcp_server->isListening()) {
        tcp_server->close();
    } else {
        QSettings settings ("Matus Tomlein", "Synkron");
        settings.setValue("process_id", tcp_server->serverPort());
        for (int i = 1; i < qApp->arguments().count(); ++i) {
            if (qApp->arguments().at(i) == "-delete" && i < qApp->arguments().count() - 1) {
                globalDelete(qApp->arguments().at(i + 1));
                i++;
            } else if (qApp->arguments().at(i) == "-rename" && i < qApp->arguments().count() - 1) {
                bool ok = false;
                QString file_name = QFileInfo(qApp->arguments().at(i + 1)).fileName();
                QString new_name = QInputDialog::getText(this, tr("Synkron - Rename file"), tr("Type a new name for \"%1\":").arg(qApp->arguments().at(i + 1)), QLineEdit::Normal, file_name, &ok);
                if (ok) globalRename(qApp->arguments().at(i + 1), new_name);
                i++;
            } else {
                QFileInfo file_info (qApp->arguments().at(i));
                //QMessageBox::information(this, tr("Synkron"), tr("%1").arg(qApp->arguments().at(i)));
                if (file_info.exists()) {
                    loadTab(file_info.absoluteFilePath());
                }
            }
        }
    }
}

void MainWindow::sendMessageAndClose()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << static_cast<quint64> (0);
    if (qApp->arguments().count() < 2) {
        out << QString("[Synkron raise];") << QString("\n");
    } else {
        for (int i = 1; i < qApp->arguments().count(); ++i) {
            if (qApp->arguments().at(i) == "-delete" && i < qApp->arguments().count() - 1) {
                out << QString("[Synkron globalDelete \"%1\"];").arg(qApp->arguments().at(i + 1)) << QString("\n");
                i++;
            } else if (qApp->arguments().at(i) == "-rename" && i < qApp->arguments().count() - 1) {
                out << QString("[Synkron globalRename \"%1\"];").arg(qApp->arguments().at(i + 1)) << QString("\n");
                i++;
            } else {
                QFileInfo file_info (qApp->arguments().at(i));
                if (file_info.exists()) {
                    out << QString("[Synkron loadMultisync \"%1\"];").arg(qApp->arguments().at(i)) << QString("\n");
                    //QMessageBox::information(this, tr("Synkron"), tr("%1").arg(qApp->arguments().at(i)));
                }
            }
        }
    }
    out.device()->seek(0);
    out << static_cast<quint64>(ba.size()) - sizeof(quint64);
    tcp_socket->write(ba);
    tcp_socket->disconnectFromHost();
    qApp->processEvents();
}

void MainWindow::addConnection()
{
    new ClientConnection(this, tcp_server->nextPendingConnection());
}

void MainWindow::closeApp()
{
    qApp->quit();
}

////////////////////////////////////////////////////////////////////////////////
// Tabs

void MainWindow::addTab()
{
    int widgetIndex = mainStackedWidget->currentIndex();
    if (kMainStackedWidgetIndexSync == widgetIndex)
    {
        addSyncTab();
    }
    else if (kMainStackedWidgetIndexMultisync == widgetIndex)
    {
        addMultiTab();
    }

    this->updateActionsEnabling(widgetIndex);
}

void MainWindow::closeTab()
{
    int widgetIndex = mainStackedWidget->currentIndex();
    if (((kMainStackedWidgetIndexSync == widgetIndex) && (0 == tabWidget->count())) ||
            ((kMainStackedWidgetIndexMultisync == widgetIndex) &&
             (0 == multi_tabWidget->count())))
    {
        return;
    }

    QMessageBox msgBox;
    msgBox.setText(tr("Are you sure you want to close this tab?"));
    msgBox.setWindowTitle(QString("Synkron"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (msgBox.exec())
    {
    case QMessageBox::Yes:
    {
        if (widgetIndex == kMainStackedWidgetIndexSync)
        {
            tabs.value(tabWidget->currentWidget())->deleteAllFolderDatabases();
            tabs.remove(tabWidget->currentWidget());
            tabWidget->removeTab(tabWidget->currentIndex());
        }
        else if (widgetIndex == kMainStackedWidgetIndexMultisync)
        {
            ( static_cast<MultisyncPage *> (multi_tabWidget->currentWidget())->deleteAllFolderDatabases());
            multi_tabWidget->removeTab(multi_tabWidget->currentIndex());
        }
        break;
    }
    case QMessageBox::No:
        break;
    default:
        break;
    }
    this->updateActionsEnabling(widgetIndex);
}

void MainWindow::saveTab()
{
    if (mainStackedWidget->currentIndex() == kMainStackedWidgetIndexSync)
    {
        tabs.value(tabWidget->currentWidget())->save();
    }
    else if (mainStackedWidget->currentIndex() == kMainStackedWidgetIndexMultisync)
    {
        ( static_cast<MultisyncPage *> ( multi_tabWidget->currentWidget())->save());
    }
}

void MainWindow::saveTabAs()
{
    if (mainStackedWidget->currentIndex() == kMainStackedWidgetIndexSync)
    {
        ( static_cast<AbstractSyncPage *> (tabs.value(
                    tabWidget->currentWidget()))->saveAs());
    }
    else if (mainStackedWidget->currentIndex() == kMainStackedWidgetIndexMultisync)
    {
        ( static_cast<AbstractSyncPage *> (multi_tabWidget->currentWidget())->saveAs());
    }
}

void MainWindow::loadTab(QString file_name)
{
    if (file_name.isEmpty())
    {
        file_name = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 QDir::homePath(),
                                                 tr("Synkron Tabs (*.slist)"));
        if (file_name.isEmpty())
        {
            return;
        }
    }
    QFile file(file_name);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, tr("Synkron"),
                              tr("Cannot read file %1:\n%2.").arg(file_name)
                              .arg(file.errorString()));
        return;
    }
    QTextStream in(&file);
    QDomDocument domdoc;
    domdoc.setContent(in.readAll());
    QDomElement el_sync = domdoc.firstChildElement("sync");
    if (el_sync.attribute("type") == "sync")
    {
        SyncPage * page = addSyncTab();
        page->load(domdoc, file_name);
        actionSynchronise->trigger();
    }
    else
    { //Multisync
        MultisyncPage * multi_page = addMultiTab();
        multi_page->load(domdoc, file_name);
        mainStackedWidget->setCurrentIndex(kMainStackedWidgetIndexMultisync);
        actionMultisync->trigger();
    }
}

void MainWindow::syncCurrentTab()
{
    if (mainStackedWidget->currentIndex() == kMainStackedWidgetIndexSync)
    {
        ( static_cast<AbstractSyncPage *> (tabs.value(tabWidget->currentWidget()))->sync());
    }
    else if (mainStackedWidget->currentIndex() == kMainStackedWidgetIndexMultisync)
    {
        ( static_cast<AbstractSyncPage *> (multi_tabWidget->currentWidget())->sync());
    }
}

void MainWindow::analyseCurrentTab()
{
    if (mainStackedWidget->currentIndex() == kMainStackedWidgetIndexSync)
    {
        tabs.value(tabWidget->currentWidget())->goToAnalyse();
    }
}

void MainWindow::aboutToShowTabMenu()
{
    int widgetIndex = mainStackedWidget->currentIndex();
    if (kMainStackedWidgetIndexSync == widgetIndex)
    {
        AbstractSyncPage *syncPage = static_cast<AbstractSyncPage *> (tabs.value(
                    tabWidget->currentWidget()));
        if (nullptr != syncPage)
        {
            actionAdvanced->setMenu(syncPage->advanced_menu);
        }
        actionAnalyse->setMenu(nullptr);
    }
    else if (kMainStackedWidgetIndexMultisync == widgetIndex)
    {
        MultisyncPage *multiSyncPage = static_cast<MultisyncPage *> (
                multi_tabWidget->currentWidget());
        if (nullptr != multiSyncPage)
        {
            actionAdvanced->setMenu(multiSyncPage->advanced_menu);
            actionAnalyse->setMenu(multiSyncPage->analyse_con_menu);
        }
    }
}

void MainWindow::updateActionsEnabling(int widgetIndex)
{
    bool tabActionsEnabled = false;
    switch(widgetIndex)
    {
    case kMainStackedWidgetIndexSync:
    {
        tabActionsEnabled = (0 != tabWidget->count());
        break;
    }
    case kMainStackedWidgetIndexMultisync:
    {
        tabActionsEnabled = (0 != multi_tabWidget->count());
        break;
    }
    default:
    {

    }
    }
    actionClose_sync->setEnabled(tabActionsEnabled);

    actionSave_tab->setEnabled(tabActionsEnabled);
    actionSave_tab_as->setEnabled(tabActionsEnabled);
    actionSave_log->setEnabled(tabActionsEnabled);
    actionSync->setEnabled(tabActionsEnabled);
    actionAnalyse->setEnabled(tabActionsEnabled);
}
