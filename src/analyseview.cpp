#include "analyseview.h"

#include "syncfile.h"

#include <QTreeWidget>

AnalyseView::AnalyseView(QTreeWidget * tree, SyncFile * sf)
{
    this->tree = tree;
}
