#include "mainwindow.h"
#include "radarwidget.h"

#include <QApplication>

#include <QMessageBox>
#include <QHostInfo>
#include <QDebug>
#include <QFile>

#include <QtGlobal>
#include <QtDebug>
#include <QTextStream>
#include <QTextCodec>
#include <QLocale>
#include <QTime>
#include <QFile>

const QString logFilePath = "debug.log";
bool          logToFile   = false;

void customMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QHash<QtMsgType, QString> msgLevelHash({
        {QtDebugMsg,    "Debug"   },
        {QtInfoMsg,     "Info"    },
        {QtWarningMsg,  "Warning" },
        {QtCriticalMsg, "Critical"},
        {QtFatalMsg,    "Fatal"   }
    });
    QByteArray                localMsg         = msg.toLocal8Bit();
    QTime                     time             = QTime::currentTime();
    QString                   formattedTime    = time.toString("hh:mm:ss.zzz");
    QByteArray                formattedTimeMsg = formattedTime.toLocal8Bit();
    QString                   logLevelName     = msgLevelHash[type];
    QByteArray                logLevelMsg      = logLevelName.toLocal8Bit();

    if(logToFile)
    {
        QString txt = QString("%1 %2: %3 (%4)").arg(formattedTime, logLevelName, msg, context.file);
        QFile   outFile(logFilePath);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
        outFile.close();
    }
    fprintf(stderr, "%s %s: %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);
    fflush(stderr);

    if(type == QtFatalMsg)
        abort();
}

int main(int argc, char *argv[])
{
    //    qInstallMessageHandler(customMessageOutput);
    QApplication a(argc, argv);

    //    QFile qss("../Radar/qss/basic.qss");
    QFile qss(":/qss/basic.css");
    //    QFile qss("basic.qss");
    if(qss.open(QFile::ReadOnly))
        a.setStyleSheet(qss.readAll());
    else
        QMessageBox::warning(NULL, "warning", "没有主题文件");

    QFont font;
    font.setPixelSize(15);
    a.setFont(font);

//        RadarWidget w;
//        w.show();
    MainWindow w1;
    w1.show();

    //    Navigation n;
    //    n.show();

    return a.exec();
}
