#ifndef ANALYSETREEWIDGETITEM_H
#define ANALYSETREEWIDGETITEM_H

#include <QTreeWidgetItem>

class SyncFile;

class AnalyseTreeWidgetItem : public QTreeWidgetItem
{
public:
    AnalyseTreeWidgetItem(QTreeWidget *);
    AnalyseTreeWidgetItem(QTreeWidgetItem *);
    AnalyseTreeWidgetItem(SyncFile *, QTreeWidget *);
    AnalyseTreeWidgetItem(SyncFile *, QTreeWidgetItem *);

    SyncFile * syncFile();

private:
    SyncFile * sf;
};

#endif // ANALYSETREEWIDGETITEM_H
