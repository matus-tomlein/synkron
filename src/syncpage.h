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

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QStackedWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QDateTimeEdit>
//NOTE: read how to replace QHttp with QtHttp https://qt-project.org/forums/viewthread/24466
//#include <QtHttp>
#if QT_VERSION >= 0x050000
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#else
#include <QHttp>
#endif

#include <QBuffer>
#include <QTextStream>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLocale>
#include <QTranslator>
#include <QDomDocument>
#include <QDomElement>
#include <QInputDialog>
#include <QSet>
#include <QTcpServer>
#include <QTcpSocket>
#include <QProgressDialog>
#include <QPalette>
#include <QTreeWidget>
#include <QDesktopServices>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include "ui_mainwindow.h"
#include "ui_multisyncpage.h"
#include "mtfile.h"
#include "mtadvancedgroupbox.h"
#include "mtstringset.h"
#include "syncfolders.h"
#include "extendedtreewidget.h"
#include "mtpathdialogue.h"
#include "mtdictionary.h"

class MainWindow;
class SyncViewItem;

class AbstractSyncPage : public QWidget
{
    Q_OBJECT

public:
    AbstractSyncPage() { allowed_difference = 2; syncing = false; text_database = true; loading = false; }
    AbstractSyncPage(MainWindow * parent) { mp_parent = parent; syncing = false; allowed_difference = 2; text_database = true; loading = false; }

    virtual QTableWidget * tableWidget() = 0;
    virtual QTreeWidget * analyseTree() = 0;
    virtual QStackedWidget * logsStw() = 0;
    virtual bool followSymlinks() = 0;
    virtual QString syncFolder1Text() = 0;
    virtual QString syncFolder2Text() = 0;
    virtual QString tabNameText() = 0;
    virtual int countSyncFolders() = 0;
    virtual QStringList syncFoldersList() = 0;
    virtual QStringList currentSyncFoldersList() = 0;
    virtual bool backupFolders() = 0;
    virtual void setSyncEnabled(bool) = 0;
    virtual QString searchLeText() = 0;
    virtual bool updateOnly(int) = 0;
    virtual bool cloneFolder(int) = 0;
    virtual bool backupFolder(int) = 0;
    virtual void setUpdateOnly(int, bool) = 0;
    virtual void setBackupFolder(int, bool) = 0;
    virtual bool dontModify(int) { return false; }
    virtual bool sortAnalysis() { return false; }
    virtual bool createEmptyFolders(int) = 0;

    void subSync(QDir&, QDir&, bool);
    bool subAnalyse(const MTStringSet &, QTreeWidgetItem */*, int i = 5*/);
    void moveContents(QDir&, QDir&);
    void addTableItem(QString, QString = "", QString = "", QBrush = Qt::white, QBrush = Qt::black);
    void errorCopyingFile(QString, QString, bool);
    void errorCopyingFolder(QString);
    void errorRemovingFile(QString, QString);
    void countExtsBl();
    void saveFolderDatabase(QString);
    MTDictionary getFolderDatabase(QString);
    MTDictionary getFolderDatabaseOfOtherTabs(QFile &);
    void deleteFolderDatabase(QString);
    MTDictionary getEntryList(QString, QString);

    void backupAndRemoveDir(QString, bool = true, bool = true);
    void backupAndRemoveFile(QFileInfo, bool = true, bool = true);
    void backupAndRemove(const QFileInfo *, const QFileInfo *, bool);

    bool isInDatabase(QString);
    bool isInGroupDatabase(QString);
    void changeTabNameInDatabase(QString, QString);
    void deleteAllFolderDatabases();
    void saveAllFolderDatabases();
    bool checkForCollision(QString, QString);
    int fileIsDifferentFromDB(QString);
    void displayCollisions();
    void copyFile(QString, QString, bool = false);
    void replaceFolderLabelInPath(QString &);
    QString getLabeledPath(QString path);
    void saveFolderEntries(QString, QString);
    QStringList getFolderEntries(QString);

    QSet<QString> extensions;
    bool syncing;
    bool text_database;
    QMap<QString, int> exts_bl_map;
    QMap<QString, MTDictionary> folder_prop_list_map;
    QDir::Filters dir_filters;
    MTDictionary collided;
    MTDictionary path_replacements;

    QWidget * blacklist;
    QAction * sync_hidden;
    QAction * sync_nosubdirs;
    QAction * ignore_blacklist;
    QAction * move;
    QAction * symlinks;
    QAction * propagate_deletions;
    QAction * fast_analyse;
    QAction * analyse_special_only;
    QAction * alert_collisions;
    QAction * no_empty_folders;
    QAction * allow_DST;
    QListWidget * blacklist_fileslist;
    QListWidget * blacklist_folderslist;
    QListWidget * blacklist_extslist;
    QPushButton * blacklist_addfile;
    QPushButton * blacklist_addfolder;
    QPushButton * blacklist_addext;
    QPushButton * blacklist_removefile;
    QPushButton * blacklist_removefolder;
    QPushButton * blacklist_removeext;
    QPushButton * blacklist_back;
    QTableWidgetItem * status_table_item;
    //MTAdvancedGroupBox * advanced;
    QMenu * advanced_menu;

    MainWindow * mp_parent;
    int synced_files;
    int allowed_difference;
    bool is_multisync;
    bool loading;
    QString update_time;
    QString last_sync;
    QString slist_path;
    QStringList files_blacklist;
    QStringList folders_blacklist;
    QStringList exts_blacklist;

public slots:
    virtual int sync() = 0;
    virtual void moveChecked(bool) = 0;
    virtual void blacklistStwChangeIndex(int) = 0;
    virtual void showThisPage() = 0;
    virtual void leaveAnalyse() = 0;
    virtual void syncCurrentAnalyseItem() = 0;
    void save();
    void saveAs();
    virtual void saveAs(QString file_name) = 0;
    //virtual void load(QDomElement &, QString) = 0;
    void sharedSave(QDomDocument &, QDomElement &);
    void sharedLoad(QDomElement &);
    void stopSync() { status_table_item->setText(tr("Stopping synchronisation, please wait...")); syncing = false; qApp->processEvents(); }
    void moveStateChanged(bool);
    void propagatedStateChanged(bool);
    void propagatedClicked(bool);
    void editBlacklist();
    void backFromBlacklist() { blacklistStwChangeIndex(0); }
    void addFileToBlacklist();
    void addFileToBlacklist(QString);
    void removeFileFromBlacklist();
    void removeFileFromBlacklist(QString);
    void addFolderToBlacklist();
    void addFolderToBlacklist(QString);
    void removeFolderFromBlacklist();
    void removeFolderFromBlacklist(QString);
    void addExtToBlacklist();
    void addExtToBlacklist(QString);
    void removeExtFromBlacklist();
    void removeExtFromBlacklist(QString);
    void setBlacklistWidget();
    void saveBackedUpFile(QFileInfo);
    void changeAllowedDifference();
    void orderPathReplacementsByLength();

    void analyseTreeItemExpanded(QTreeWidgetItem *);
    void analyseTreeItemClicked(QTreeWidgetItem *, int);
    void analyseTreeItemDoubleClicked(QTreeWidgetItem *, int);
    void checkAnalyseTree();
    void subCheckAnalyseTree(QTreeWidgetItem *);
    void analyseTreeConMenu(QPoint);
    void openAnalyseTreeItem();
    void deleteCurrentAnalyseItem();
    void renameCurrentAnalyseItem();
    void blacklistCurrentAnalyseItem();
    void blExtCurrentAnalyseItem();
    void searchAnalyseTree();
    bool subSearchAnalyseTree(QString, QTreeWidgetItem *);
};

class SyncPage : public AbstractSyncPage
{
    Q_OBJECT

signals:
    void sigsync(QWidget *);

public slots:
    void syncPage();
    void moveChecked(bool);
    void blacklistStwChangeIndex(int i) { tab_stw->setCurrentIndex(i); }
    int sync() { return sync(MTMap<QString, int>()); }
    int sync(MTMap<QString, int>);
    void subGroupSync(MTMap<QString, int>, MTStringSet = MTStringSet());
    void syncFoldersChanged();
    void showThisPage();
    void setSyncWidget();
    void goToAnalyse();
    void leaveAnalyse();
    void analyseFolders();
    void ignoreBlacklistClicked(bool) { if (logs_stw->currentIndex() == 1) checkAnalyseTree(); }
    void syncCurrentAnalyseItem();
    void syncFoldersEdited();
    void subSortedAnalyse(const QString &);
    void analyseSeparately();
    void createChildTreeItem(QTreeWidgetItem *, QString, bool, bool);
    void createChildItemsByFolder(QString, QStringList, QStringList, bool, bool, bool = false);
    void useTextDatabase(bool);

    void saveAs(QString file_name);
    void load(QDomDocument &, QString);

public:
    SyncPage(MainWindow *parent = 0) : AbstractSyncPage(parent) {
        is_multisync = false;
        files_to_be_copied.allowDuplicateKeys();
        files_to_be_overwritten.allowDuplicateKeys();
    };

    QTableWidget * tableWidget() { return tw; }
    QTreeWidget * analyseTree() { return analyse_tree; }
    QStackedWidget * logsStw() { return logs_stw; }
    bool followSymlinks() { return symlinks->isChecked(); }
    bool backupFolders() { return backup_folders->isChecked(); }
    QString syncFolder1Text() { return sync_folders->syncFolder(0)->path(); }
    QString syncFolder2Text() { return sync_folders->syncFolder(1)->path(); }
    QString tabNameText() { return tab_name->text(); }
    void setSyncEnabled(bool);
    int countSyncFolders() { return sync_folders->count(); }
    QStringList syncFoldersList() { return sync_folders->pathsList(); }
    QStringList currentSyncFoldersList() { return sync_folders->pathsList(); }
    QString searchLeText() { return log_search->text(); }
    bool isSlave(int i) { return sync_folders->at(i)->slave_act->isChecked(); }
    bool updateOnly(int i) { return update_only->isChecked() || sync_folders->at(i)->update_only_act->isChecked(); }
    bool cloneFolder(int i) {
        if (i == 0) return sync_folders->at(1)->slave_act->isChecked() && sync_folders->at(0)->master_act->isChecked();
        if (i == 1) return sync_folders->at(0)->slave_act->isChecked() && sync_folders->at(1)->master_act->isChecked();
        return false;
    }
    bool backupFolder(int i) { return backup_folders->isChecked() || sync_folders->at(i)->backup_folder_act->isChecked(); }
    void setUpdateOnly(int i, bool check) { sync_folders->at(i)->update_only_act->setChecked(check); }
    void setBackupFolder(int i, bool check) { sync_folders->at(i)->backup_folder_act->setChecked(check); }
    bool dontModify(int i) { return sync_folders->at(i)->dont_update_act->isChecked(); }
    bool createEmptyFolders(int i) {
        if (no_empty_folders->isChecked()) { return false; }
        return !sync_folders->at(i)->no_empty_folders_act->isChecked();
    }
    bool sortAnalysis() { return sort_analysis_by_action->isChecked(); }

    QWidget * tab;
    QWidget * analyse_widget;
    QLabel * icon_label;
    QLabel * sync_log_label;
    QLabel * separate_advanced;
    QLineEdit * tab_name;
    ExtendedLineEdit * log_search;
    QTableWidget * tw;
    QPushButton * sync_btn;
    QPushButton * stop_sync_btn;
    QPushButton * back;
    QPushButton * resync;
    QPushButton * go_to_analyse;
    QPushButton * analyse_back;
    QPushButton * show_advanced;
    QPushButton * add_folder_btn;
    QCheckBox * show_sync_folders;
    QAction * backup_folders;
    QAction * update_only;
    QStackedWidget * tab_stw;
    QStackedWidget * logs_stw;
    SyncFolders * sync_folders;
    ExtendedTreeWidget * analyse_tree;
    QMenu * filters_menu;
    QAction * sort_analysis_by_action;
    QAction * text_database_action;

    MTDictionary files_to_be_copied;
    MTDictionary files_to_be_overwritten;
    QStringList collided_files;
    QStringList skipped_files;
    QStringList skipped_folders;
    QStringList files_to_be_deleted;
    QStringList folders_to_be_created;
    QStringList folders_to_be_deleted;
};

class MultisyncPage : public AbstractSyncPage, private Ui::MultisyncForm
{
    Q_OBJECT

public:
    MultisyncPage(MainWindow *parent = 0);

    QTableWidget * tableWidget() { return tw_multi; }
    QTreeWidget * analyseTree() { return analyse_tree; }
    QStackedWidget * logsStw() { return logs_stw; }
    bool followSymlinks() { return symlinks->isChecked(); }
    bool backupFolders() { return backup_folder_1->isChecked() && backup_folder_2->isChecked(); }
    QString syncFolder1Text() { return sync_folder_1; }
    QString syncFolder2Text() { return sync_folder_2; }
    QString tabNameText() { return tab_name->text(); }
    void setSyncEnabled(bool);
    int countSyncFolders() { return 2; }
    QStringList syncFoldersList();
    QStringList currentSyncFoldersList() { QStringList l; l << sync_folder_1; l << sync_folder_2; return l; }
    QString searchLeText() { return search_multi->text(); }
    QString variablesToString();
    void variablesFromString(QString);
    bool updateOnly(int i) {
        return (i == 0 && update_only_1->isChecked()) || (i == 1 && update_only_2->isChecked());
    }
    bool cloneFolder(int i) {
        return i == 0 && clone_folder1->isChecked();
    }
    bool backupFolder(int i) {
        return (i == 0 && backup_folder_1->isChecked()) || (i == 1 && backup_folder_2->isChecked());
    }
    bool createEmptyFolders(int) { return !no_empty_folders->isChecked(); }
    void setUpdateOnly(int i, bool check) {
        if (i == 0) update_only_1->setChecked(check);
        if (i == 1) update_only_2->setChecked(check);
    }
    void setBackupFolder(int i, bool check) {
        if (i == 0) backup_folder_1->setChecked(check);
        if (i == 1) backup_folder_2->setChecked(check);
    }

public slots:
    void setAdvancedGB();
    void multitabNameChanged();
    //void showAdvancedGroupBox(bool show) { advanced->setChecked(show); }
    void saveAs(QString file_name);
    void load(QDomDocument &, QString);
    void moveChecked(bool);
    void destinationTextChanged() {
        if (destination_multi->text().isEmpty()) return;
        QDir dir(destination_multi->text());
        QString text = dir.path();
        if (text.at(0).isLower()) {
            text[0] = text.at(0).toUpper();
        }
        destination_multi->setText(text);
    }
    void blacklistStwChangeIndex(int i) { tab_stw->setCurrentIndex(i); }
    void showThisPage();
    int sync();
    void varsDialogue();
    void addVariable();
    void removeVariable();
    void analyse(QAction *);
    void goToAnalyse(QAction * action);
    void aboutToShowAnalyseMenu();
    void leaveAnalyse() { logs_stw->setCurrentIndex(0); }
    void syncCurrentAnalyseItem() {}
    void foldersChanged();
    void cloneStateChanged(bool);

private:
    QString sync_folder_1;
    QString sync_folder_2;
    QMap<QString, QString> vars_map;
    QTreeWidget * vars_tree;
    QListWidget * var_paths_list;
    QMenu * analyse_con_menu;
    QAction * update_only_1;
    QAction * update_only_2;
    QAction * backup_folder_1;
    QAction * backup_folder_2;
    QAction * clone_folder1;
    void resetSourcePaths(QMap<QString, QString>);

    friend class MainWindow;
};
