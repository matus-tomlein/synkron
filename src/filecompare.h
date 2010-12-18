#ifndef FILECOMPARE_H
#define FILECOMPARE_H

class QFileInfo;
class QDateTime;

class FileCompare
{
public:
    FileCompare();
    FileCompare(int);

    int compareTwoFiles(QFileInfo *, QFileInfo *);
    int compareTwoDates(const QDateTime, const QDateTime);

private:
    int ignored_secs;
};

#endif // FILECOMPARE_H
