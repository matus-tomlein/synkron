#ifndef SYNCACTIONOPTIONS_H
#define SYNCACTIONOPTIONS_H

#include <QVariant>

class SyncActionOptions
{
public:
    SyncActionOptions(QMap<QString, QVariant> *);
    ~SyncActionOptions();

    const QVariant value(const QString &);
    bool boolValue(const QString &);

    bool createEmptyFolders();
    bool updateOnly();

protected:
    QMap<QString, QVariant> * settings;
};

#endif // SYNCACTIONOPTIONS_H
