#include "filecompare.h"

#include <QFileInfo>
#include <QDateTime>

FileCompare::FileCompare()
{
    this->ignored_secs = 2;
}

FileCompare::FileCompare(int ignored_secs)
{
    this->ignored_secs = ignored_secs;
}

int FileCompare::compareTwoFiles(QFileInfo * fi1, QFileInfo * fi2)
{
    return compareTwoDates(fi1->lastModified(), fi2->lastModified());
}

int FileCompare::compareTwoDates(const QDateTime dt1, const QDateTime dt2)
{
    QString dt1_str = dt1.toString("yyyyMMddhhmmss");
    QString dt2_str = dt2.toString("yyyyMMddhhmmss");

    if (dt1_str == dt2_str) {
        return 0;
    }
    else if (dt1_str > dt2_str) {
        if (dt1.addSecs(0 - ignored_secs).toString("yyyyMMddhhmmss") > dt2_str) {
            return 1;
        }
    }
    else {
        if (dt1.addSecs(ignored_secs).toString("yyyyMMddhhmmss") < dt2_str) {
            return -1;
        }
    }
    return 0;
}
