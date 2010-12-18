#ifndef EXCEPTIONFOLDER_H
#define EXCEPTIONFOLDER_H

#include <QString>

class ExceptionFile
{
public:
    ExceptionFile(const QString, bool = false);

    bool isBlacklisted() { return blacklisted; }
    void setBlacklisted(bool b) { blacklisted = b; }

    ExceptionFile * addChild(const QString &, bool = false);
    ExceptionFile * child(const QString &);
    int childCount();

    const QString & getName() { return name; }
    void setName(const QString n) { name = n; }

    void addWildcard(const QString);
    int wildcardCount();
    const QString & wildcardAt(int);

private:
    QString name;
    bool is_dir;
    bool blacklisted;

    QList<ExceptionFile *> * children;
    QStringList * wildcards;
};

#endif // EXCEPTIONFOLDER_H
