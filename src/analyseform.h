#ifndef ANALYSEFORM_H
#define ANALYSEFORM_H

#include <QWidget>

class Folders;
class SyncExceptionBundle;
class SyncFile;
class AnalyseTreeWidgetItem;
class QTreeWidgetItem;

namespace Ui {
    class AnalyseForm;
}

class AnalyseForm : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyseForm(QWidget *parent = 0);
    ~AnalyseForm();

    void analyse(Folders *, SyncExceptionBundle *);

private slots:
    void syncFileReceived(SyncFile *);
    void treeItemDoubleClicked(QTreeWidgetItem *, int);
    void treeItemClicked(QTreeWidgetItem *, int);
    void updateSelectedInfo(AnalyseTreeWidgetItem *);

private:
    void loadSyncFile(SyncFile *);
    void loadSyncFile(AnalyseTreeWidgetItem *);
    void removeItemChildren(AnalyseTreeWidgetItem *);
    AnalyseTreeWidgetItem * nextLevelItem(SyncFile *);

    Ui::AnalyseForm *ui;

    Folders * folders;
    QList<SyncFile *> sf_queue;
    AnalyseTreeWidgetItem * current_level_item;
};

#endif // ANALYSEFORM_H
