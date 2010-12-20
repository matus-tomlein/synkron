#ifndef MTPROGRESSBAR_H
#define MTPROGRESSBAR_H

#include <QProgressBar>

class MTProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    MTProgressBar(QWidget * parent = 0);

public slots:
    void increaseValue();

};

#endif // MTPROGRESSBAR_H
