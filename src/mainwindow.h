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

#include "ui_about.h"
#include "ui_syncviewitem.h"
#include "syncpage.h"

//inline void release(QFileInfo *& f) { if (f) { delete f; f = NULL; } }
//inline void release(QFile *& f) { if (f) { delete f; f = NULL; } }
template<class T>
inline void release(T *& o) { if (o) { delete o; o = NULL; } }
template<class T>
inline void release(QList<T> *& l) { if (l) { delete l; l = NULL; } }

class SyncSchedule : public QObject
{
    Q_OBJECT

public:
    SyncSchedule(MainWindow *);
    QList<QTimer*> timers;

    QStringList sched_tab_list;
    QStringList sched_multitab_list;
    QStringList sched_time_list;
    QStringList sched_checked_time_list;
    QStringList sched_date_list;
    QStringList sched_checked_date_list;
    QStringList sched_unchecked_days_list;

    int periodical_interval;
    int timing_tab_index;
    bool scheduling;

signals:
    void sigsched(QWidget *);

public slots:
    void syncSchedule();
    void checkSchedStatus();
    void startSchedule();

private:
    MainWindow * sched_parent;
    QTableWidgetItem * parent_item;

    friend class MainWindow;
};

class Filter : public QListWidgetItem
{
public:
    Filter():
        QListWidgetItem(nullptr, QListWidgetItem::UserType) {}
    Filter(QString text):
        QListWidgetItem(nullptr, QListWidgetItem::UserType) { setText(text); }

    QStringList extensions;
};

class ClientConnection : public QObject
{
    Q_OBJECT

public:
    ClientConnection(MainWindow *, QTcpSocket *);

public slots:
    void read();

private:
    quint64 c_blocksize;
    QTcpSocket * c_socket;
    MainWindow * c_parent;
};

class SyncViewItem : public QWidget, private Ui::SyncViewItem
{
    Q_OBJECT

public:
    SyncViewItem(AbstractSyncPage *);
    AbstractSyncPage * parent_page;

    void setName(QString name) { sync_name_lbl->setText(name); }

public slots:
    void startSync();

protected:
    void mouseDoubleClickEvent(QMouseEvent *) { parent_page->showThisPage(); }
};

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QSettings *);

    //QStringList synchronised;
    QStringList files_blacklist;
    QStringList folders_blacklist;
    QStringList exts_blacklist;
    QString temp_path;
    QString raw_temp_path;

    bool syncingAll;
    bool skip_close_event;
    bool runHidden() { return run_hidden; }
    bool shownManually() { return shown_manually; }
    //void setShownManually(bool sm) { shown_manually = sm; }
    bool showTrayMessage(QString, QString);
    bool isSyncing();
    QSettings * sync_settings;
    QSettings * temp_settings;

public slots:
    void saveSettings();
    bool removeDir(QString);
    bool removeFile(QString, bool = true);
    void setShownManually() { shown_manually = true; }
    bool restoreItem(QListWidgetItem*);
    bool restoreFile(QString, QString, QString);
    void shutDownComputer();
    void syncCurrentTab();
    void analyseCurrentTab();
    void aboutToShowTabMenu();
    //void showToolbarContextMenu(const QPoint &);

private slots:

    // Synchronisation
    void sync() { sync(tabWidget->currentWidget()); }
    void sync(QWidget *);
    void syncAll();
    void tabNameChanged();
    SyncPage * addSyncTab();
    //void showAdvancedGroupBox(bool show, SyncPage * page) { page->advanced->setChecked(show); }

    // Restore
    void toRestorePage();
    void restoreItemChanged(QListWidgetItem *, QListWidgetItem *);
    void restoreFiles();
    void setCleanGB();
    void cleanTemporary();
    void selTmpAll();
    void deleteTempFile(QString);
    void restoreListConMenu(QPoint);
    void restoreCurrentItem();
    void deleteRestoreItem();
    void checkRestoreItem();
    void blacklistRestoreItem();
    void setSelectGB();
    void restoreSelLastSync();
    //void restoreSelOlderFiles();
    void restoreSelCommonDate();
    void loadTempSettings();
    void readTempSettings();
    void convertOldTempSettings(QStringList);
    int autoCleanTemp();

    // Blacklist
    void addToBlackList(int);
    void addFileToBlacklist();
    void removeFileFromBlacklist();
    void addFolderToBlacklist();
    void removeFolderFromBlacklist();
    void addExtToBlacklist();
    void removeExtFromBlacklist();

    // Multisync
    MultisyncPage * addMultiTab();
    void addSource();
    void removeSource();
    void browseMultiDestination();

    // Scheduler
    void addSchedule();
    SyncSchedule * addSchedule(QString);
    void removeSchedule();
    void reloadSchedule() { scheduleActivated(tw_schedules->currentRow(), tw_schedules->currentColumn(), tw_schedules->currentRow(), tw_schedules->currentColumn()); }
    void reloadSchedStatus();
    void scheduleActivated(int, int, int, int);
    void setSchedName(const QString text) { tw_schedules->item(tw_schedules->currentRow(), 0)->setText(text); }
    void addSchedTime();
    void removeSchedTime();
    void saveSchedSettings(int);
    void setScheduleStatusOn(bool, QTableWidgetItem*);
    void schedTabClicked(QListWidgetItem*);
    void schedMultitabClicked(QListWidgetItem*);
    void schedTimeClicked(QListWidgetItem*);
    void startSchedule() { startSchedule(tw_schedules->item(tw_schedules->currentRow(), 0)); }
    void startSchedule(QTableWidgetItem *);
    void stopSchedule() { stopSchedule(tw_schedules->item(tw_schedules->currentRow(), 0)); }
    void stopSchedule(QTableWidgetItem *);
    void searchTw(const QString);
    void searchLw(const QString);
    void startAllSchedules();
    void stopAllSchedules();
    void enableSchedule(int);
    void activateSchedule();
    void schedIntervalChanged(int);
    void timingTabIndexChanged(int);
    void setSchedDatesGB();
    void addSchedDate();
    void removeSchedDate();
    void schedDateClicked(QListWidgetItem *);
    void schedDayClicked(QListWidgetItem *);

    // Filters
    void addFilter();
    void addFilter(QString, QStringList);
    void removeFilter();
    void addFilterExtension();
    void removeFilterExtension();
    void filterChanged();
    void setFiltersEnabled(bool);

    //SyncView
    void refreshSyncs();
    void refreshMultisyncs();
    void toSyncView();

    // Other
    void changeLanguage(); void langChanged();
    void checkForUpdates(); void httpRequestFinished(bool);
    void about();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayIconVisible(bool);
    void switchView(QAction*);
    void addTab();
    void closeTab();
    bool closeDialogue();
    void closeApp();
    void saveSyncLog();
    void setRunHidden(bool b) { run_hidden = b; }
    void globalDelete(QString);
    void globalRename(QString, QString);
    bool renameFile(QString, QString);
    void sendMessageAndClose();
    void addConnection();
    void initServer(QAbstractSocket::SocketError);
    void optionClicked(QAction*);
    void changeTemp();
    void showIconsOnly(bool);

    // Tabs
    void saveTab();
    void saveTabAs();
    void loadTab(QString = QString());
    void updateActionsEnabling(int widgetIndex);

private:
    float f_ver;
    QString ver;
    bool run_hidden;
    bool sched_removed;
    bool no_closedialogue;
#if QT_VERSION >= 0x050000
    QNetworkAccessManager * http;
#else
    QHttp * http;
#endif

    QBuffer * http_buffer;
    QTcpServer * tcp_server; QTcpSocket * tcp_socket;

    void closeEvent(QCloseEvent*);
    void readSettings();
    void createTrayIcon();
    void createActions();
    void toBlacklist();

    //QMenu * createPopupMenu();

    QButtonGroup * btngrpBrowse;
    QActionGroup * actgrpView;
    QComboBox * langComboBox;
    QMap<QWidget *, SyncPage *> tabs;
    QMap<QTableWidgetItem*, SyncSchedule*> item_sched_map;
    QMap<QString, QString> synkron_i18n;

    QCheckBox * restore_clean_selected;
    QCheckBox * restore_clean_by_date;
    QCheckBox * restore_clean_repeated;
    QSpinBox * restore_clean_date;
    QSpinBox * restore_clean_files;
    QPushButton * restore_clean;
    QToolButton * restore_select_last_sync;
    /*QToolButton * restore_select_older;
    QSpinBox * restore_select_older_date;*/
    QToolButton * restore_select_common_date;
    QDateTimeEdit * restore_select_dt_edit;
    QListWidget * sched_sync_dates;
    QListWidget * sched_days_lw;
    QDateEdit * sched_date_edit;
    QPushButton * add_date_btn;
    QPushButton * rem_date_btn;

    QAction * minimizeAction;
    QAction * maximizeAction;
    QAction * syncAction;
    QAction * quitAction;
    QAction * syncAllAction;
    QAction * restoreAction;
    QAction * deleteRestoreItemAction;
    QAction * checkRestoreItemAction;
    QAction * blacklistRestoreItemAction;
    QAction * actionShow_icons_only;
    QSystemTrayIcon *trayIcon;
#ifdef Q_WS_MAC
    QAction * actionBrushedMetalStyle;
#endif
    bool shown_manually;

    friend class SyncSchedule;
    friend class SyncPage;
    friend class MultisyncPage;
    friend class SyncViewItem;
    friend class ClientConnection;

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
};

class About : public QDialog, private Ui::About
{
    Q_OBJECT

public:
    About(QString, QString);
};

class MTApplication : public QApplication
{
    Q_OBJECT
protected:
    void init() {
        app_main_window = nullptr;
    }
public:
    MTApplication(int & argc, char ** argv):
        QApplication(argc, argv) {
        init();
    }
    MTApplication(int & argc, char ** argv, bool GUIenabled):
        QApplication(argc, argv, GUIenabled) {
        init();
    }
    //BUG: Where is Type define?
    //    MTApplication(int & argc, char ** argv, Type type):QApplication(argc, argv, type) {
    //        init();
    //    }
#ifdef Q_WS_X11
    MTApplication(Display * display, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0):
        QApplication(display, visual, colormap) { init(); };
    MTApplication(Display * display, int & argc, char ** argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0):
        QApplication(display, argc, argv, visual, colormap) { init(); };
#endif
#ifdef Q_WS_MAC
    bool macEventFilter(EventHandlerCallRef, EventRef event) {
        switch (GetEventKind(event)) {
        case kEventAppActivated: if (app_main_window != NULL) {
                if (!app_main_window->runHidden() || app_main_window->shownManually())
                { app_main_window->show(); }
            } break;
        }
        return false;
    };
#endif
    void setAppMainWindow(MainWindow * w) { app_main_window = w; }
private:
    MainWindow * app_main_window;
};
