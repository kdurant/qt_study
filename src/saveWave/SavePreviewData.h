#ifndef SAVEPREVIEWDATA_H
#define SAVEPREVIEWDATA_H

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QObject>
#include <QFile>

class SavePreviewData : public QObject
{
    Q_OBJECT

public:
    SavePreviewData() = default;

signals:
     void sig_finish();  // 无参数信号

public slots:
    void writeToFile(QFile &file, const QByteArray &data)
    {
        file.write(data);
    }
};

#endif
