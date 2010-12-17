#ifndef SYNCEXCEPTIONBUNDLE_H
#define SYNCEXCEPTIONBUNDLE_H

#include "abstractexceptionbundle.h"

class QStringList;
class QString;
class QRegExp;
class FolderActionGroup;
class ExceptionFile;
class ExceptionGroup;

class SyncExceptionBundle : public AbstractExceptionBundle
{
public:
    SyncExceptionBundle();
    ~SyncExceptionBundle();

    void importBundle(AbstractExceptionBundle *);

    ExceptionFile * updateRootFolder(FolderActionGroup *);
    ExceptionFile * lastFile();
    bool cdAndCheck(const QString &);
    void cdUp();

    bool isInBlacklist(const QString &, bool = false);

    const QStringList & filters();

private:
    void importGroup(ExceptionGroup *);

    void updateFolder(QStringList *);
    QStringList * makeRelative(const QStringList *, FolderActionGroup *);
    void restartExceptionFiles();
    bool cd(const QString &);

    QStringList * filter_extensions;
    QRegExp * reg_exp;

    QList<ExceptionFile *> * exception_files;
    int num_files_out;
};

#endif // SYNCEXCEPTIONBUNDLE_H
