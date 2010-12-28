#ifndef ANALYSETREEWIDGETITEM_H
#define ANALYSETREEWIDGETITEM_H

#include <QTreeWidgetItem>

class AnalyseFile;

class AnalyseTreeWidgetItem : public QTreeWidgetItem
{
public:
    AnalyseTreeWidgetItem(QTreeWidget *);
    AnalyseTreeWidgetItem(QTreeWidgetItem *);
    AnalyseTreeWidgetItem(AnalyseFile *, QTreeWidget *);
    AnalyseTreeWidgetItem(AnalyseFile *, QTreeWidgetItem *);

    AnalyseFile * syncFile();

private:
    void setTextFromAnalyseFile();

    AnalyseFile * sf;
};

#endif // ANALYSETREEWIDGETITEM_H
