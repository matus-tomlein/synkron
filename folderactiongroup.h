#ifndef FOLDERACTIONGROUP_H
#define FOLDERACTIONGROUP_H

#include <QString>

class FolderActionGroup
{
public:
    FolderActionGroup();

    void insert(int, const QString);
    const QString & at(int);
    const QString & byId(int);
    int idAt(int);
    int count();

private:
    QStringList * paths;
    QList<int> * ids;
};

#endif // FOLDERACTIONGROUP_H
