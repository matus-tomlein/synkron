#ifndef SYNCACTION_H
#define SYNCACTION_H

class Folders;
class FolderActionGroup;
class SyncFile;
class SyncExceptionBundle;
class FileCompare;
class SyncOutMessage;
class MTFile;

class QFileInfo;

#include <QThread>

class SyncAction : public QThread
{
    Q_OBJECT

public:
    SyncAction(Folders *, SyncExceptionBundle *);
    ~SyncAction();

    void run();

public slots:
    void start(Priority = InheritPriority);

private:
    Folders * base_folders;
    FileCompare * file_compare;
    SyncExceptionBundle * exception_bundle;

    int skipped_count;

    void createSyncFileFromFolders(SyncFile *, FolderActionGroup *&);
    void sync(SyncFile *, FolderActionGroup *&);
    void copyFile(QFileInfo *, const QString &);
    void updateFile(QFileInfo *, const QString &);
    bool backupFile(MTFile *);

signals:
    void messageBox(const QString);
    void filesCounted(int);
    void finished();

    void fileCopied(const QString, const QString);
    void fileUpdated(const QString, const QString);

    void syncOutMessage(SyncOutMessage *);
};

#endif // SYNCACTION_H
