#include "folderactiongroup.h"

#include <QStringList>

FolderActionGroup::FolderActionGroup()
{
    init();
}

FolderActionGroup::FolderActionGroup(int i, const QString str)
{
    init();
    ids->append(i); paths->append(str);
}

FolderActionGroup::FolderActionGroup(int i1, const QString str1, int i2, const QString str2)
{
    init();
    ids->append(i1); paths->append(str1);
    ids->append(i2); paths->append(str2);
}

FolderActionGroup::~FolderActionGroup()
{
    delete ids;
    if (paths)
        delete paths;
}

void FolderActionGroup::init()
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

const QString & FolderActionGroup::first()
{
    return paths->first();
}

const QString & FolderActionGroup::last()
{
    return paths->last();
}

int FolderActionGroup::firstId()
{
    return ids->first();
}

int FolderActionGroup::lastId()
{
    return ids->last();
}

QString & FolderActionGroup::operator[] (int i)
{
    return (*paths)[i];
}
