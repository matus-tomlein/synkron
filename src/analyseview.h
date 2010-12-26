#ifndef ANALYSEVIEW_H
#define ANALYSEVIEW_H

class QTreeWidget;
class SyncFile;

class AnalyseView
{
public:
    AnalyseView(QTreeWidget *, SyncFile *);

private:
    QTreeWidget * tree;
};

#endif // ANALYSEVIEW_H
