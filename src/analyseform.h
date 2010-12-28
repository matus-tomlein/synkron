#ifndef ANALYSEFORM_H
#define ANALYSEFORM_H

#include <QWidget>

class Folders;
class SyncExceptionBundle;
class AnalyseFile;
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
    void syncFileReceived(AnalyseFile *);
    void treeItemDoubleClicked(QTreeWidgetItem *, int);
    void treeItemSelectionChanged();
    void updateSelectedInfo(AnalyseTreeWidgetItem *);
    void openSelected(QAction *);
    void aboutToShowOpenMenu();
    void blacklistSelected(QAction *);
    void aboutToShowBlacklistMenu();
    void syncSelected();

private:
    void loadSyncFile(AnalyseFile *);
    void loadSyncFile(AnalyseTreeWidgetItem *);
    void removeItemChildren(AnalyseTreeWidgetItem *);
    AnalyseTreeWidgetItem * nextLevelItem(AnalyseFile *);
    QStringList relativePath(AnalyseFile *);

    Ui::AnalyseForm *ui;

    AbstractSyncPage * page;
    QList<AnalyseFile *> sf_queue;
    AnalyseTreeWidgetItem * current_level_item;
    AnalyseFile * current_sf;

signals:
    void syncSig(SyncFile *, FolderActionGroup *);
};

#endif // ANALYSEFORM_H
