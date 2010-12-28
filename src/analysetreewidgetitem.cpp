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
    setupFromAnalyseFile();
}

AnalyseTreeWidgetItem::AnalyseTreeWidgetItem(AnalyseFile * sf, QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent)
{
    this->sf = sf;
    setupFromAnalyseFile();
}

AnalyseFile * AnalyseTreeWidgetItem::syncFile()
{
    return sf;
}

void AnalyseTreeWidgetItem::setupFromAnalyseFile()
{
    if (sf->numNotSynced()) {
        setText(0, QString("%1 (%2)").arg(sf->getName()).arg(sf->numNotSynced()));
        setForeground(0, QBrush(Qt::red));
    } else {
        setText(0, sf->getName());
    }

    if (sf->isDir())
        setIcon(0, QIcon(":/images/folder_16.png"));
    else
        setIcon(0, QIcon(":/images/file.png"));
}
