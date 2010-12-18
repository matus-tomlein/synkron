#include "exceptionfile.h"

#include <QStringList>

ExceptionFile::ExceptionFile(const QString name, bool is_dir)
{
    this->name = name;
    this->is_dir = is_dir;
    blacklisted = false;
    children = new QList<ExceptionFile *>;
    wildcards = NULL;
}

ExceptionFile * ExceptionFile::addChild(const QString & child_name, bool child_dir)
{
    for (int i = 0; i < children->count(); ++i) {
        if (children->at(i)->getName() == child_name) return children->at(i);
    }

    ExceptionFile * child = new ExceptionFile(child_name, child_dir);
    children->append(child);

    return child;
}

ExceptionFile * ExceptionFile::child(const QString & child_name)
{
    for (int i = 0; i < children->count(); ++i) {
        if (children->at(i)->getName() == child_name)
            return children->at(i);
    }

    return NULL;
}

int ExceptionFile::childCount()
{
    return children->count();
}

void ExceptionFile::addWildcard(const QString card)
{
    if (!wildcards) wildcards = new QStringList();

    wildcards->append(card);
}

const QString & ExceptionFile::wildcardAt(int i)
{
    if (!wildcards) return "";

    return wildcards->at(i);
}

int ExceptionFile::wildcardCount()
{
    if (!wildcards) return 0;

    return wildcards->count();
}
