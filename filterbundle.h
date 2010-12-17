#ifndef FILTERBUNDLE_H
#define FILTERBUNDLE_H

#include "exceptionbundle.h"

class FilterBundle : public ExceptionBundle
{
public:
    FilterBundle(int);

    ExceptionItem * addItem(int);
    const QString iconPath() { return ":/images/filter16.png"; }

protected:
    void loadDefaultSettings();
};

#endif // FILTERBUNDLE_H
