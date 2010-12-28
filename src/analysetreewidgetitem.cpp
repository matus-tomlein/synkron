#include "analysetreewidgetitem.h"
#include "analysefile.h"

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

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(AnalyseFile * sf, QTreeWidget * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = sf;
    setTextFromAnalyseFile();
}

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(AnalyseFile * sf, QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = sf;
    setTextFromAnalyseFile();
}

AnalyseFile * AnalyseTreeWidgetItem::syncFile()
{
    return sf;
}

void AnalyseTreeWidgetItem::setTextFromAnalyseFile()
{
    this->setText(0, QString("%1 (%2)").arg(sf->getName()).arg(sf->numNotSynced()));
}
