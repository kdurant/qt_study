#ifndef THREADPARSE_H
#define THREADPARSE_H

#include "mainwindow.h"
#include <QThread>

class threadparse : public QThread
{
    Q_OBJECT

    QByteArray data;
    qint8 index;
public:
    threadparse();
protected:
    void run();
};

#endif // THREADPARSE_H
