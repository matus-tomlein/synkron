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

// +++ Options menu +++

void MainWindow::changeTemp()
{
    //QString text = getPathDialogue(true, temp_path);
    MTPathDialogue * dialogue = new MTPathDialogue(this);
    QString text = dialogue->getPath(true, raw_temp_path);
    if (text.isEmpty()) return;
    raw_temp_path = text;
    QDir dir (raw_temp_path);
    temp_path = dir.absolutePath();
    loadTempSettings();
}

void AbstractSyncPage::changeAllowedDifference()
{
    bool ok = false;
    int a = QInputDialog::getInt(this, tr("Enter allowed time difference"),
                                 tr("Allowed time difference (in seconds):"), allowed_difference, 0, 86400, 1, &ok);
    if (ok) {
        allowed_difference = a;
    }
}

void MainWindow::optionClicked(QAction * clckd_action)
{
    if (clckd_action->isChecked()) {
#ifdef Q_WS_WIN
        if (clckd_action == actionShut_down_after_sync) {
            actionQuit_after_sync->setChecked(false);
            actionSync_at_launch->setChecked(false);
        }
#endif
        if (clckd_action == actionQuit_after_sync) {
            actionShut_down_after_sync->setChecked(false);
            actionSync_at_launch->setChecked(false);
        } else if (clckd_action == actionSync_at_launch) {
            actionShut_down_after_sync->setChecked(false);
            actionQuit_after_sync->setChecked(false);
        }
    }
}

// --- Options menu ---
// +++ Search sync and multisync +++

void MainWindow::searchTw(const QString text)
{
    QTableWidget * tw = NULL;
    if (mainStackedWidget->currentIndex()==0) {
        SyncPage * page = tabs.value(tabWidget->currentWidget());
        if (page->logs_stw->currentIndex()==1) return;
        tw = page->tw;
    } else if (mainStackedWidget->currentIndex()==3) {
        MultisyncPage * page = (MultisyncPage *) multi_tabWidget->currentWidget();
        if (page->logs_stw->currentIndex()==1) return;
        tw = page->tw_multi;
    } else return;
    if (tw == NULL) return;
    for (int i = 0; i < tw->rowCount(); ++i) {
        if (tw->item(i, 0) && tw->item(i, 0)->text().contains(text, Qt::CaseInsensitive)) tw->showRow(i);
        else if (tw->item(i, 1) && tw->item(i, 1)->text().contains(text, Qt::CaseInsensitive)) tw->showRow(i);
        else tw->hideRow(i);
    }
}

void MainWindow::searchLw(const QString text)
{
    for (int i = 0; i < restore_list->count(); ++i) {
        if (!restore_list->item(i)->text().contains(text, Qt::CaseInsensitive)) restore_list->item(i)->setHidden(true);
        else restore_list->item(i)->setHidden(false);
    }
}

// --- Search sync and multisync ---
// +++ Global remove and rename +++

bool MainWindow::removeDir(QString path)
{
    QDir dir (path);
    if (!dir.exists()) return false;
    QFileInfoList info_list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden, (QDir::Name | QDir::DirsFirst | QDir::IgnoreCase));
    for (int i = 0; i < info_list.count(); ++i) {
        if (info_list.at(i).isSymLink() || !info_list.at(i).isDir()) {
            QFile file (info_list.at(i).absoluteFilePath());
            if (!file.remove()) return false;
            continue;
        }
        if (!removeDir(info_list.at(i).absoluteFilePath())) return false;
    }
    QString dirname = dir.dirName();
    dir.cdUp();
    if (!dir.rmdir(dirname)) return false;
    return true;
}

bool MainWindow::removeFile(QString path, bool messages)
{
    QFileInfo file_info (path);
    if (!file_info.exists()) return false;
    else if (file_info.isSymLink() || !file_info.isDir()) {
        if (!QFile(file_info.absoluteFilePath()).remove()) {
            if (messages) QMessageBox::critical(this, tr("Synkron"), tr("Error removing file %1").arg(file_info.path()));
            return false;
        }
    } else {
        QDir dir (path);
        if (!removeDir(dir.path())) {
            if (messages) QMessageBox::critical(this, tr("Synkron"), tr("Error removing directory %1").arg(dir.path()));
            return false;
        }
    }
    return true;
}

void MainWindow::globalDelete(QString path)
{
    if (path == "") return;
    QMessageBox msgBox; msgBox.setText(tr("Are you sure you want to remove \"%1\" from every synced location?").arg(path));
    msgBox.setWindowTitle(QString("Synkron")); msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QMapIterator<QWidget *, SyncPage *> i(tabs);
    MultisyncPage * multi_page;
    QProgressDialog progress (this);
    QFileInfo file_info (path);

    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        progress.setLabelText(tr("Removing files..."));
        progress.setMinimum(0);
        progress.setMaximum(tabWidget->count() /*+ multi_tabWidget->count()*/ + 1);
        progress.setMinimumDuration(0);
        progress.setWindowModality(Qt::WindowModal);

        path = file_info.absoluteFilePath();
        if (progress.wasCanceled()) return;
        removeFile(path);
        progress.setValue(progress.value()+1);
        qApp->processEvents();

        while (i.hasNext()) {
            i.next();
            if (progress.wasCanceled()) return;
            QString path2;
            for (int n = 0; n < i.value()->sync_folders->count(); ++n) {
                if (path.startsWith(i.value()->sync_folders->syncFolder(n)->path())) {
                    path2 = path;
                    path2.remove(i.value()->sync_folders->syncFolder(n)->path());
                    break;
                }
            }
            if (path2 == "" || path2 == ".") continue;
            for (int n = 0; n < i.value()->sync_folders->count(); ++n) {
                QString path3 = path2;
                path3.insert(0, i.value()->sync_folders->syncFolder(n)->path());
                if (path3 == path) continue;
                if (removeFile(path3)) {
                    i.value()->addTableItem(tr("File %1 deleted").arg(path3), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                }
            }
            progress.setValue(progress.value()+1);
            qApp->processEvents();
        }

        for (int m = 0; m < multi_tabWidget->count(); ++m) {
            multi_page = (MultisyncPage *)multi_tabWidget->widget(m);
            if (path.startsWith(multi_page->destination_multi->text())) {
                QString path2 = path;
                path2.remove(multi_page->destination_multi->text());
                if (path2.startsWith('/')) {
                    path2.remove(0, 1);
                }
                for (int s = 0; s < multi_page->list_multi->count(); ++s) {
                    if (path2.startsWith(multi_page->list_multi->item(s)->text())) {
                        /*if (path2.startsWith("HOMEPATH", Qt::CaseSensitive)) {
                            path2.replace("HOMEPATH", QDir::homePath());
                        } else if (path2.startsWith("ROOTPATH", Qt::CaseSensitive)) {
                            path2.replace("ROOTPATH", QDir::rootPath());
                        }*/
                        QMapIterator<QString, QString> iter(multi_page->vars_map);
                        while (iter.hasNext()) {
                            iter.next();
                            if (path2.startsWith(iter.key())) {
                                path2.replace(0, iter.key().length(), iter.value());
                            }
                        }
                        path2.replace("//", "/");
                        if (removeFile(path2)) {
                            multi_page->addTableItem(tr("File %1 deleted").arg(path2), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                        }
                        break;
                    }
                }
            } else {
                for (int s = 0; s < multi_page->list_multi->count(); ++s) {
                    QString path3 = multi_page->list_multi->item(s)->text();
                    /*if (path3.startsWith("HOMEPATH", Qt::CaseSensitive)) {
                        path3.replace("HOMEPATH", QDir::homePath());
                    } else if (path3.startsWith("ROOTPATH", Qt::CaseSensitive)) {
                        path3.replace("ROOTPATH", QDir::rootPath());
                    }*/
                    QMapIterator<QString, QString> iter(multi_page->vars_map);
                    while (iter.hasNext()) {
                        iter.next();
                        if (path3.startsWith(iter.key())) {
                            path3.replace(0, iter.key().length(), iter.value());
                        }
                    }
                    path3.replace("//", "/");
                    if (path.startsWith(path3)) {
                        QString path2 = QString("%1/%2%3").arg(multi_page->destination_multi->text())
                                .arg(multi_page->list_multi->item(s)->text())
                                .arg(QString(path).remove(0, path3.count()));
                        if (removeFile(path2)) {
                            multi_page->addTableItem(tr("File %1 deleted").arg(path2), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                        }
                        break;
                    }
                }
            }
            progress.setValue(progress.value()+1);
            qApp->processEvents();
        }
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
}

bool MainWindow::renameFile(QString path, QString name)
{
    QFileInfo file_info (path);
    path = file_info.absoluteFilePath();
    if (!file_info.exists()) return false;
    else if (file_info.isSymLink() || !file_info.isDir()) {
        QFile file (path);
        if (!file.rename(QString("%1/%2").arg(file_info.dir().path()).arg(name))) {
            QMessageBox::critical(this, tr("Synkron"), tr("Error renaming file %1").arg(path));
            return false;
        }
    } else {
        QDir dir (path);
        QString dirname = dir.dirName();
        dir.cdUp();
        if (!dir.rename(dirname, name)) {
            QMessageBox::critical(this, tr("Synkron"), tr("Error renaming directory %1").arg(path));
            return false;
        }
    }
    return true;
}

void MainWindow::globalRename(QString path, QString name)
{
    if (path == "" || name == "") return;
    QFileInfo file_info (path);
    path = file_info.absoluteFilePath();

    renameFile(path, name);

    QMapIterator<QWidget *, SyncPage *> i(tabs);
    while (i.hasNext()) {
        i.next();
        QString path2;
        for (int n = 0; n < i.value()->sync_folders->count(); ++n) {
            if (path.startsWith(i.value()->sync_folders->syncFolder(n)->path())) {
                path2 = path;
                path2.remove(i.value()->sync_folders->syncFolder(n)->path());
                break;
            }
        }
        if (path2 == "" || path2 == ".") continue;
        for (int n = 0; n < i.value()->sync_folders->count(); ++n) {
            QString path3 = path2;
            path3.insert(0, i.value()->sync_folders->syncFolder(n)->path());
            if (path3 == path) continue;
            if (renameFile(path3, name)) {
                i.value()->addTableItem(tr("File %1 renamed").arg(path3), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
            }
        }

        /*if (path.startsWith(i.value()->syncFolder1Text())) {
            path2 = path;
            path2.replace(i.value()->syncFolder1Text(), i.value()->syncFolder2Text());
        } else if (path.startsWith(i.value()->syncFolder2Text())) {
            path2 = path;
            path2.replace(i.value()->syncFolder2Text(), i.value()->syncFolder1Text());
        } else {
            continue;
        }
        if (path2 == "" || path2 == ".") continue;

        if (renameFile(path2, name)) {
            i.value()->addTableItem(tr("File %1 renamed").arg(path2), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
        }*/
    }

    MultisyncPage * multi_page;
    for (int m = 0; m < multi_tabWidget->count(); ++m) {
        multi_page = (MultisyncPage *)multi_tabWidget->widget(m);
        if (path.startsWith(multi_page->destination_multi->text())) {
            QString path2 = path;
            path2.remove(multi_page->destination_multi->text());
            if (path2.startsWith('/')) {
                path2.remove(0, 1);
            }
            for (int s = 0; s < multi_page->list_multi->count(); ++s) {
                if (path2.startsWith(multi_page->list_multi->item(s)->text())) {
                    /*if (path2.startsWith("HOMEPATH", Qt::CaseSensitive)) {
                        path2.replace("HOMEPATH", QDir::homePath());
                    } else if (path2.startsWith("ROOTPATH", Qt::CaseSensitive)) {
                        path2.replace("ROOTPATH", QDir::rootPath());
                    }*/
                    QMapIterator<QString, QString> iter(multi_page->vars_map);
                    while (iter.hasNext()) {
                        iter.next();
                        if (path2.startsWith(iter.key())) {
                            path2.replace(0, iter.key().length(), iter.value());
                        }
                    }
                    path2.replace("//", "/");
                    if (renameFile(path2, name)) {
                        multi_page->addTableItem(tr("File %1 renamed").arg(path2), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                    }
                }
            }
        } else {
            for (int s = 0; s < multi_page->list_multi->count(); ++s) {
                QString path3 = multi_page->list_multi->item(s)->text();
                /*if (path3.startsWith("HOMEPATH", Qt::CaseSensitive)) {
                    path3.replace("HOMEPATH", QDir::homePath());
                } else if (path3.startsWith("ROOTPATH", Qt::CaseSensitive)) {
                    path3.replace("ROOTPATH", QDir::rootPath());
                }*/
                QMapIterator<QString, QString> iter(multi_page->vars_map);
                while (iter.hasNext()) {
                    iter.next();
                    if (path3.startsWith(iter.key())) {
                        path3.replace(0, iter.key().length(), iter.value());
                    }
                }
                path3.replace("//", "/");
                if (path.startsWith(path3)) {
                    QString path2 = QString("%1/%2%3").arg(multi_page->destination_multi->text())
                            .arg(multi_page->list_multi->item(s)->text())
                            .arg(QString(path).remove(0, path3.count()));

                    if (renameFile(path2, name)) {
                        multi_page->addTableItem(tr("File %1 renamed").arg(path2), "", QString::fromUtf8(":/new/prefix1/images/file.png"), QBrush(Qt::darkMagenta), QBrush(Qt::white));
                    }
                }
            }
        }
    }
}

// --- Global remove and rename ---
// +++ Tray icon +++

void MainWindow::createTrayIcon()
{
    QMenu * trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(syncAction);
    trayIconMenu->addAction(syncAllAction);
    trayIconMenu->addMenu(menuOptions);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/images/Synkron16.png")));
    trayIcon->setToolTip("Synkron");
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::trayIconVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!visible);
    syncAction->setEnabled(true);
    syncAllAction->setEnabled(true);
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::MiddleClick:
    case QSystemTrayIcon::DoubleClick:
        if (this->isHidden()) {
            this->show();
        }
        else {
            this->hide();
        }
        break;
    case QSystemTrayIcon::Context:
        trayIcon->contextMenu()->show();
        break;
    default:
        break;
    }
}

bool MainWindow::showTrayMessage(QString title, QString message)
{
    if (!actionDisable_tray_messages->isChecked()) {
        trayIcon->showMessage(title, message);
        return true;
    } else return false;
}

void MainWindow::showEvent(QShowEvent *)
{
    shown_manually = true;
    trayIconVisible(true);
}

void MainWindow::hideEvent(QHideEvent *)
{
    saveSettings();
    trayIconVisible(false);
}

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("&Hide"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    //connect(minimizeAction, SIGNAL(triggered()), this, SLOT(minimizeTrayIcon()));

    maximizeAction = new QAction(tr("S&how"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(show()));
    //connect(maximizeAction, SIGNAL(triggered()), this, SLOT(maximizeTrayIcon()));

    syncAction = new QAction(tr("Sync &current tab"), this);
    connect(syncAction, SIGNAL(triggered()), this, SLOT(sync()));

    syncAllAction = new QAction(tr("Sync &all tabs"), this);
    connect(syncAllAction, SIGNAL(triggered()), this, SLOT(syncAll()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(closeApp()));

    checkRestoreItemAction = new QAction(tr("&Check/Uncheck"), this);
    connect(checkRestoreItemAction, SIGNAL(triggered()), this, SLOT(checkRestoreItem()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(restoreCurrentItem()));

    deleteRestoreItemAction = new QAction(tr("&Remove"), this);
    connect(deleteRestoreItemAction, SIGNAL(triggered()), this, SLOT(deleteRestoreItem()));

    blacklistRestoreItemAction = new QAction(tr("Add to &blacklist"), this);
    connect(blacklistRestoreItemAction, SIGNAL(triggered()), this, SLOT(blacklistRestoreItem()));
}

// --- Tray icon ---
// +++ Other +++

bool MainWindow::closeDialogue()
{
    QSettings settings ("Matus Tomlein", "Synkron");
    if (sync_settings->value("dont_ask_on_quit", settings.value("dont_ask_on_quit")).toBool()==true) {
        if (sync_settings->value("minimise_on_quit", settings.value("minimise_on_quit")).toBool()==true) {
            minimizeAction->trigger();
            return false;
        } else if (sync_settings->value("minimise_on_quit", settings.value("minimise_on_quit")).toBool()==false) {
            return true;
        }
    }
    QDialog * cl_dialogue = new QDialog (this, Qt::Dialog);
    cl_dialogue->setWindowModality(Qt::WindowModal);
    //cl_dialogue->setAttribute(Qt::WA_DeleteOnClose);
    cl_dialogue->setWindowTitle(tr("Quit Synkron"));
    QGridLayout * cl_glayout = new QGridLayout (cl_dialogue);
    cl_glayout->setMargin(4); cl_glayout->setSpacing(10);
    QLabel * cl_label = new QLabel (cl_dialogue);
    cl_label->setText(tr("Are you sure you want to quit?"));
    cl_label->setAlignment(Qt::AlignHCenter);
    cl_glayout->addWidget(cl_label, 0, 0);
    QCheckBox * rm_minimise = new QCheckBox(cl_dialogue);
    rm_minimise->setText(tr("Do not ask me again"));
    rm_minimise->setChecked(true);
    cl_glayout->addWidget(rm_minimise, 1, 0);
    QHBoxLayout * hlayout = new QHBoxLayout;
    hlayout->addStretch();
    QPushButton * close_btn = new QPushButton (cl_dialogue);
    close_btn->setText(tr("&Quit"));
    connect(close_btn, SIGNAL(released()), cl_dialogue, SLOT(accept()));
    hlayout->addWidget(close_btn);
    QPushButton * minimise_btn = new QPushButton (cl_dialogue);
    minimise_btn->setText(tr("&Minimise"));
    connect(minimise_btn, SIGNAL(released()), cl_dialogue, SLOT(reject()));
    hlayout->addWidget(minimise_btn);
    hlayout->addStretch();
    cl_glayout->addLayout(hlayout, 2, 0);
    switch (cl_dialogue->exec()) {
    case 0: // Minimise
        sync_settings->setValue("dont_ask_on_quit", rm_minimise->isChecked());
        sync_settings->setValue("minimise_on_quit", true);
        minimizeAction->trigger();
        return false;
        break;
    case 1: // Quit
        sync_settings->setValue("dont_ask_on_quit", rm_minimise->isChecked());
        sync_settings->setValue("minimise_on_quit", false);
        return true;
        break;
    }
    return true;
}

void MainWindow::saveSyncLog()
{
    AbstractSyncPage * page; QString tab_text;
    if (mainStackedWidget->currentIndex()==0) {
        page = tabs.value(tabWidget->currentWidget());
        tab_text = tabWidget->tabText(tabWidget->currentIndex());
    } else if (mainStackedWidget->currentIndex()==3) {
        page = (MultisyncPage *) multi_tabWidget->currentWidget();
        tab_text = multi_tabWidget->tabText(multi_tabWidget->currentIndex());
    } else return;
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Synkron - Save Multisync"),
                                                QString("%1/%2.html").arg(QDir::homePath()).arg(tab_text),
                                                tr("Sync logs (*.html)"));
    if (path.isNull() || path.isEmpty()) { return; }
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::critical(this, tr("Export sync log"), tr("Cannot write file %1:\n%2.").arg(path).arg(file.errorString()));
        return;
    }
    QTextStream sfile(&file);
    sfile.setCodec("UTF-8");
    sfile << "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><title>" << endl;
    sfile << tr("Synkron sync log") << " - " << tab_text << " - ";
    sfile << endl << "</title></head><body><table width=\"100%\" style=\"font-family:sans-serif;\">" << endl;
    for (int i = 0; i < page->tableWidget()->rowCount(); ++i) {
        sfile << "<tr>";
        if (page->tableWidget()->columnSpan(i, 0) == 2) {
            sfile << "<td colspan=\"2\" style=\"background-color: rgb(";
            sfile << page->tableWidget()->item(i, 0)->background().color().red() << ", ";
            sfile << page->tableWidget()->item(i, 0)->background().color().green() << ", ";
            sfile << page->tableWidget()->item(i, 0)->background().color().blue() << "); ";
            sfile << "color: rgb(";
            sfile << page->tableWidget()->item(i, 0)->foreground().color().red() << ", ";
            sfile << page->tableWidget()->item(i, 0)->foreground().color().green() << ", ";
            sfile << page->tableWidget()->item(i, 0)->foreground().color().blue() << ")\">";
            sfile << endl << page->tableWidget()->item(i, 0)->text() << "\n</td>" << endl;
        } else {
            sfile << "<td width=\"50%\">";
            sfile << endl << page->tableWidget()->item(i, 0)->text() << "\n</td>" << endl;

            sfile << "<td width=\"50%\">";
            sfile << endl << page->tableWidget()->item(i, 1)->text() << "\n</td>" << endl;
        }
        sfile << "</tr>";
    }
    sfile << "</table></body></html>" << endl;
}

void MainWindow::switchView(QAction * action)
{
    if (mainStackedWidget->currentIndex()==0) tabNameChanged();
    if (action == actionSynchronise) {
        mainStackedWidget->setCurrentIndex(0);
    }
    else if (action == actionRestore) {
        toRestorePage();
        mainStackedWidget->setCurrentIndex(1);
    }
    else if (action == actionBlacklist) {
        toBlacklist();
        mainStackedWidget->setCurrentIndex(2);
    }
    else if (action == actionMultisync) {
        mainStackedWidget->setCurrentIndex(3);
    }
    else if (action == actionScheduler) {
        if (tw_schedules->currentItem()!=0) activateSchedule();
        mainStackedWidget->setCurrentIndex(4);
    }
    else if (action == actionFilters) {
        mainStackedWidget->setCurrentIndex(5);
    }
    else if (action == actionSyncView) {
        toSyncView();
        mainStackedWidget->setCurrentIndex(6);
    }

    bool tabs_enable = false;
    if (action == actionSynchronise || action == actionMultisync) { tabs_enable = true; }
    actionNew_sync->setEnabled(tabs_enable);
    actionClose_sync->setEnabled(tabs_enable);
    menuTab->setEnabled(tabs_enable);
}

void MainWindow::shutDownComputer()
{
#ifdef Q_WS_WIN
    QProcess process;
    QStringList arguments;
    arguments << "-f" << "-s" << "-t" << "10";
    process.execute("shutdown", arguments);
#endif
}

/*void MainWindow::showToolbarContextMenu(const QPoint & point)
{
    QMessageBox::information(this, "jbjk", "ojfdsffsdfdsfdsfds");
    QMenu * toolbar_menu = new QMenu(toolBar);
    QAction * text_only = new QAction(tr("Show text only"), toolbar_menu);
    QAction * icons_only = new QAction(tr("Show icons only"), toolbar_menu);
    QAction * icons_and_text = new QAction(tr("Show icons and text"), toolbar_menu);
    QActionGroup * act_grp = new QActionGroup(toolbar_menu);
    act_grp->addAction(text_only);
    act_grp->addAction(icons_only);
    act_grp->addAction(icons_and_text);
    act_grp->setExclusive(true);
    QAction * triggered_action = toolbar_menu->exec(point);
    if (triggered_action) {
        QMessageBox::information(this, "jbjk", "oj");
    }
    delete text_only;
    delete icons_only;
    delete icons_and_text;
    delete toolbar_menu;
}*/

/*QMenu * MainWindow::createPopupMenu()
{
    QMessageBox::information(this, "jbjk", "ojfdsffsdfdsfdsfds");
    QMenu * popup_menu = this->QMainWindow::createPopupMenu();
    popup_menu->addSeparator();
    popup_menu->addAction(actionShow_icons_only);
    return popup_menu;
}*/

void MainWindow::showIconsOnly(bool show)
{
    if (show) {
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    } else {
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
}

// --- Other ---
