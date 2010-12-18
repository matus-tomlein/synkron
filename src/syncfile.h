#ifndef SYNCFILE_H
#define SYNCFILE_H

#include <QString>

class SyncFile
{
public:
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

    bool isDir() { return is_dir; }
    void setDir(bool d) { is_dir = d; }

    bool isBlacklisted() { return blacklisted; }
    void setBlacklisted(bool b) { blacklisted = b; }

private:
    QList<SyncFile *> * children;
    QList<int> * folders;

    QString name;
    bool is_dir;
    bool blacklisted;
};

#endif // SYNCFILE_H
