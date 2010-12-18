#ifndef EXCEPTIONGROUP_H
#define EXCEPTIONGROUP_H

#include <QVariant>

class Settings;

class ExceptionGroup
{
public:
    ExceptionGroup();
    ~ExceptionGroup();

    enum GroupType {
        FileBlacklist,
        FolderBlacklist,
        Filters
    };

    void setValue(const QString &, const QVariant &);
    const QVariant value(const QString &);

    void save(Settings *);
    void load(Settings *);

    int count();
    const QString & at(int);

    void importData(const QStringList *);
    const QStringList * getData() { return data; }

    bool addItem(const QString &);
    void removeItem(const QString &);
    void removeAt(int);

private:
    QMap<QString, QVariant> * settings_map;
    QStringList * data;
};

#endif // EXCEPTIONGROUP_H
