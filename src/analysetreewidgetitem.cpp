#include "analysetreewidgetitem.h"
#include "syncfile.h"

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(QTreeWidget * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = NULL;
}

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = NULL;
}

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(SyncFile * sf, QTreeWidget * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = sf;
    this->setText(0, sf->getName());
}

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(SyncFile * sf, QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = sf;
    this->setText(0, sf->getName());
}

SyncFile * AnalyseTreeWidgetItem::syncFile()
{
    return sf;
}
