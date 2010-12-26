#ifndef SYNCFILE_H
#define SYNCFILE_H

#include <QString>

class SyncFile
{
public:
    enum FileStatus {
        OK,
        Obsolete,
        NotFound,
        Deleted
    };
    struct FolderStatus {
        SyncFile::FileStatus file_stat;
        int folder_id;
    };

    SyncFile(const QString);
    ~SyncFile();

    SyncFile * addChild(const QString &);

    void setName(const QString);
    const QString & getName();

    int childCount();
    int count(bool = true);
    SyncFile * childAt(int);

    void addFolder(int);
    bool existsInFolder(int);
    void setFileStatusInFolder(int, FileStatus);
    SyncFile::FileStatus fileStatusInFolder(int);

    bool isDir() { return is_dir; }
    void setDir(bool d) { is_dir = d; }

    bool isBlacklisted() { return blacklisted; }
    void setBlacklisted(bool b) { blacklisted = b; }

private:
    QList<SyncFile *> * children;
    QList<SyncFile::FolderStatus> * folders;

    QString name;
    bool is_dir;
    bool blacklisted;

    int indexOfFolder(int);
};

#endif // SYNCFILE_H
