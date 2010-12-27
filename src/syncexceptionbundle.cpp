#include "syncexceptionbundle.h"
#include "exceptiongroup.h"
#include "folderactiongroup.h"
#include "exceptionfile.h"

#include <QMap>
#include <QStringList>

SyncExceptionBundle::SyncExceptionBundle() : AbstractExceptionBundle()
{
    filter_extensions = NULL;

    exception_files = new QList<ExceptionFile *>;

    reg_exp = new QRegExp;
    reg_exp->setCaseSensitivity(Qt::CaseInsensitive);
    reg_exp->setPatternSyntax(QRegExp::Wildcard);
}

SyncExceptionBundle::~SyncExceptionBundle()
{
    if (filter_extensions) delete filter_extensions;
}

void SyncExceptionBundle::importBundle(AbstractExceptionBundle * bundle)
{
    QList<ExceptionGroup *> * b_groups = bundle->groups();

    for (int i = 0; i < b_groups->count(); ++i) {
        importGroup(b_groups->at(i));
    }
}

void SyncExceptionBundle::importGroup(ExceptionGroup * i_group)
{
    groupByType(i_group->value("type").toInt())->importData(i_group->getData());
}

const QStringList & SyncExceptionBundle::filters()
{
    if (filter_extensions) return *filter_extensions;

    filter_extensions = new QStringList;
    ExceptionGroup * filters_group = groupByType(ExceptionGroup::Filters);

    for (int i = 0; i < filters_group->count(); ++i) {
        filter_extensions->append(QString("*%1").arg(filters_group->at(i)));
    }

    return *filter_extensions;
}

bool SyncExceptionBundle::isInBlacklist(const QString &, bool)
{
    /*ExceptionGroup * group = groupById(is_dir ? ExceptionGroup::FolderBlacklist : ExceptionGroup::FileBlacklist);

    for (int i = 0; i < group->count(); ++i) {
        reg_exp->setPattern(group->at(i));
        if (reg_exp->exactMatch(name)) {
            return true;
        }
    }
*/
    return false;
}

/**
  * Updates the root folder with all exceptions from blacklist bundles.
  */
ExceptionFile * SyncExceptionBundle::updateRootFolder(FolderActionGroup * fag)
{
    restartExceptionFiles();

    updateFolder(makeRelative(groupByType(ExceptionGroup::FileBlacklist)->getData(), fag));
    updateFolder(makeRelative(groupByType(ExceptionGroup::FolderBlacklist)->getData(), fag));

    return exception_files->first();
}

/**
  * Makes blacklist exception paths relative.
  * Filters out exceptions that don't apply to the folders in fag.
  * Wildcards with * sign that can apply to the fag are included.
  */
QStringList * SyncExceptionBundle::makeRelative(const QStringList * list, FolderActionGroup * fag)
{
    QStringList * new_list = new QStringList;
    QString root;
    bool is_wildcard = false;
    bool found = false;

    for (int i = 0; i < list->count(); ++i) {
        is_wildcard = false;
        found = false;
        if (list->at(i).contains('*')) {
            root = list->at(i).section('*', 0, 0);
            is_wildcard = true;
        } else root = list->at(i);

        for (int n = 0; n < fag->count(); ++n) {
            if ((is_wildcard && fag->at(n).startsWith(root)) || root.startsWith(fag->at(n))) {
                if (is_wildcard) root.remove(fag->at(n).left(root.count()));
                else root.remove(fag->at(n));
                found = true;
                break;
            }
        }

        if (found) {
            if (is_wildcard) {
                root.append('*').append(list->at(i).section('*', 1));
            }
            new_list->append(root);
        }
    }

    return new_list;
}

/**
  * Creates a folder structure of blacklist exceptions, each file, folder or subfolder having a separate ExceptionFile object.
  * Recognises wildcards (only * sign supported). Doesn't create subfolders for wilcard exceptions after the * sign.
  */
void SyncExceptionBundle::updateFolder(QStringList * list)
{
    QStringList split_list;
    ExceptionFile * file = NULL;

    for (int i = 0; i < list->count(); ++i) {
        file = exception_files->first();
        split_list = list->at(i).split('/');

        for (int n = 0; n < split_list.count(); ++n) {
            if (split_list.at(n).isEmpty())
                continue;
            if (split_list.at(n).contains("*")) {
                file->addWildcard(QString("*%1").arg(list->at(i).section('*', 1)));
                break;
            }

            file = file->addChild(split_list.at(n), true);
            if (n == split_list.count() - 1) file->setBlacklisted(true);
        }
    }
}

/**
  * Deletes all current exception files and creates a new root folder.
  */
void SyncExceptionBundle::restartExceptionFiles()
{
    for (int i = exception_files->count() - 1; i >= 0; --i) {
        delete exception_files->takeAt(i);
    }

    exception_files->append(new ExceptionFile("/"));
    num_files_out = 0;
}

/**
  * Returns the last used file in sync.
  */
ExceptionFile * SyncExceptionBundle::lastFile()
{
    return exception_files->last();
}

/**
  * Removes the last file from exception_files list.
  * If out of bounds, num_files_out is decreased.
  */
void SyncExceptionBundle::cdUp()
{
    if (num_files_out) {
        num_files_out--;
    } else {
        exception_files->takeLast();
    }
}

/**
  * Tries to find a matching file. On success adds the file to the exception_files list.
  * If it fails, num_files_out is increased.
  */
bool SyncExceptionBundle::cd(const QString & child_name)
{
    if (!num_files_out) {
        ExceptionFile * file = exception_files->last()->child(child_name);
        if (file) {
            exception_files->append(file);
            return true;
        }
    }

    num_files_out++;
    return false;
}

/**
  * Checks if file is in blacklist.
  */
bool SyncExceptionBundle::cdAndCheck(const QString & child_name)
{
    // Check for exact file name match.
    if (cd(child_name)) {
        if (exception_files->last()->isBlacklisted())
            return true;
    }

    // Check for wildcard match in all parent files.
    for (int i = 0; i < exception_files->count(); ++i) {
        for (int n = 0; n < exception_files->at(i)->wildcardCount(); ++n) {
            if (QRegExp(exception_files->at(i)->wildcardAt(n), Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(child_name))
                return true;
        }
    }

    return false; // means not in blacklist
}
