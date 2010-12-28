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
    SyncAction(FolderActionGroup *, SyncExceptionBundle *, SyncFile * = NULL);
    ~SyncAction();

    void run();

public slots:
    void start(Priority = InheritPriority);

private:
    FolderActionGroup * starting_fag;
    SyncFile * starting_sf;
    FileCompare * file_compare;
    SyncExceptionBundle * exception_bundle;

    int skipped_count;

    void createSyncFileFromFolders(SyncFile *, FolderActionGroup *);
    void sync(SyncFile *, FolderActionGroup *);
    virtual void copyFile(SyncFile *, FolderActionGroup *);
    virtual void updateFile(SyncFile *, FolderActionGroup *);
    virtual bool createFolder(SyncFile *, FolderActionGroup *);
    bool backupFile(MTFile *);

    virtual SyncFile * createRootSyncFile();
    virtual void finish(SyncFile *);

protected:
    QList<SyncFile *> sf_queue;

signals:
    void messageBox(const QString);
    void filesCounted(int);
    void finished();

    void fileCopied(const QString, const QString);
    void fileUpdated(const QString, const QString);
    void anotherItemChecked();

    void syncOutMessage(SyncOutMessage *);
};

#endif // SYNCACTION_H
