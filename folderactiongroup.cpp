#include "folderactiongroup.h"

#include <QStringList>

FolderActionGroup::FolderActionGroup()
{
    paths = new QStringList;
    ids = new QList<int>;
}

void FolderActionGroup::insert(int id, const QString path)
{
    ids->append(id);
    paths->append(path);
}

const QString & FolderActionGroup::at(int i)
{
    return paths->at(i);
}

const QString & FolderActionGroup::byId(int i)
{
    return paths->at(ids->indexOf(i));
}

int FolderActionGroup::count()
{
    return ids->count();
}

/**
  * Returns id of the folder at index i in the list.
  */
int FolderActionGroup::idAt(int i)
{
    return ids->at(i);
}
