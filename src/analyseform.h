#ifndef ANALYSEFORM_H
#define ANALYSEFORM_H

#include <QWidget>

class Folders;
class SyncExceptionBundle;
class SyncFile;
class AnalyseTreeWidgetItem;
class FolderActionGroup;
class AbstractSyncPage;

class QTreeWidgetItem;

namespace Ui {
    class AnalyseForm;
}

class AnalyseForm : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyseForm(AbstractSyncPage *, QWidget *parent = 0);
    ~AnalyseForm();

    void analyse();

private slots:
    void syncFileReceived(SyncFile *);
    void treeItemDoubleClicked(QTreeWidgetItem *, int);
    void treeItemSelectionChanged();
    void updateSelectedInfo(AnalyseTreeWidgetItem *);
    void openSelected(QAction *);
    void aboutToShowOpenMenu();
    void blacklistSelected(QAction *);
    void aboutToShowBlacklistMenu();
    void syncSelected();

private:
    void loadSyncFile(SyncFile *);
    void loadSyncFile(AnalyseTreeWidgetItem *);
    void removeItemChildren(AnalyseTreeWidgetItem *);
    AnalyseTreeWidgetItem * nextLevelItem(SyncFile *);
    QStringList relativePath(SyncFile *);

    Ui::AnalyseForm *ui;

    AbstractSyncPage * page;
    QList<SyncFile *> sf_queue;
    AnalyseTreeWidgetItem * current_level_item;
    SyncFile * current_sf;

signals:
    void syncSig(SyncFile *, FolderActionGroup *);
};

#endif // ANALYSEFORM_H
