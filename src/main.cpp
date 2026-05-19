#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include <QDateTime>
#include <QtMessageHandler>
#include <QDebug>

#include "mainwindow.h"

static QFile logFile;
static QMutex logMutex;

void customMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    Q_UNUSED(ctx);
    QMutexLocker lock(&logMutex);
    if (!logFile.isOpen()) {
        logFile.setFileName(QApplication::applicationDirPath() + "/debug.log");
        logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    QTextStream out(&logFile);
    QString typeStr;
    switch (type) {
        case QtDebugMsg: typeStr = "D"; break;
        case QtWarningMsg: typeStr = "W"; break;
        case QtCriticalMsg: typeStr = "C"; break;
        case QtFatalMsg: typeStr = "F"; break;
        default: typeStr = "?"; break;
    }
    out << QDateTime::currentDateTime().toString("HH:mm:ss.zzz")
        << " [" << typeStr << "] " << msg << "\n";
    out.flush();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Desktop Monitor");
    a.setApplicationVersion("1.0");

    qInstallMessageHandler(customMessageHandler);
    qDebug() << "=== App Started ===";

    a.setStyle(QStyleFactory::create("Fusion"));

    MainWindow w;
    w.show();

    int ret = a.exec();
    qDebug() << "=== App Exited ===";
    logFile.close();
    return ret;
}
