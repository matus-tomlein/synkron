#ifndef FOLDERACTIONGROUP_H
#define FOLDERACTIONGROUP_H

#include <QString>

class FolderActionGroup
{
public:
    FolderActionGroup();
    FolderActionGroup(int, const QString);
    FolderActionGroup(int, const QString, int, const QString);

    void insert(int, const QString);
    const QString & at(int);
    const QString & byId(int);
    const QString & first();
    const QString & last();
    int idAt(int);
    int firstId();
    int lastId();
    int count();

private:
    QStringList * paths;
    QList<int> * ids;

    void init();
};

#endif // FOLDERACTIONGROUP_H
