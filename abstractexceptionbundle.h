#ifndef ABSTRACTEXCEPTIONBUNDLE_H
#define ABSTRACTEXCEPTIONBUNDLE_H

class ExceptionGroup;
template<class T>
class QList;

class AbstractExceptionBundle
{
public:
    AbstractExceptionBundle();
    ~AbstractExceptionBundle();

    QList<ExceptionGroup *> * groups() { return exception_groups; }
    ExceptionGroup * groupByType(int);

protected:
    QList<ExceptionGroup *> * exception_groups;
};

#endif // ABSTRACTEXCEPTIONBUNDLE_H
