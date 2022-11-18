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
    void openFile(QString fileName)
    {
        binFile.setFileName("data/" + fileName + ".bin");
        binFile.open(QIODevice::WriteOnly);
    }
    void closeFile()
    {
        binFile.close();
    }

signals:
     void sig_finish();  // 无参数信号

public slots:
    void writeToFile(const QByteArray &data)
    {
        if(binFile.isOpen())
            binFile.write(data);
    }
private:
    QFile binFile;

};

#endif
