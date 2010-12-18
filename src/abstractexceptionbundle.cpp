#include "abstractexceptionbundle.h"

#include "fileblacklistgroup.h"
#include "folderblacklistgroup.h"
#include "filtergroup.h"

AbstractExceptionBundle::AbstractExceptionBundle()
{
    exception_groups = new QList<ExceptionGroup *>;

    // ExceptionGroups +++
    ExceptionGroup * file_blacklist_group = new FileBlacklistGroup();
    ExceptionGroup * folder_blacklist_group = new FolderBlacklistGroup();
    ExceptionGroup * filter_group = new FilterGroup();
    exception_groups->append(file_blacklist_group);
    exception_groups->append(folder_blacklist_group);
    exception_groups->append(filter_group);
    // ExceptionGroups ---
}

AbstractExceptionBundle::~AbstractExceptionBundle()
{
    for (int i = 0; i < exception_groups->count(); ++i) {
        delete exception_groups->takeAt(i);
    }

    delete exception_groups;
}

ExceptionGroup * AbstractExceptionBundle::groupByType(int type)
{
    for (int i = 0; i < exception_groups->count(); ++i) {
        if (exception_groups->at(i)->value("type").toInt() == type) return exception_groups->at(i);
    }
    return NULL;
}
