#include "workers/memory_worker.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

MemoryWorker::MemoryWorker(QObject *parent)
    : BaseWorker(3000, parent)
{
}

void MemoryWorker::collect()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (!GlobalMemoryStatusEx(&memStatus)) {
        qWarning() << "MemoryWorker: GlobalMemoryStatusEx failed (" << GetLastError() << ")";
        return;
    }

    QVariantMap values;
    values["memoryPercent"] = static_cast<double>(memStatus.dwMemoryLoad);
    values["memoryUsedMb"] = static_cast<qint64>(
        (memStatus.ullTotalPhys - memStatus.ullAvailPhys) / (1024 * 1024));
    values["memoryTotalMb"] = static_cast<qint64>(
        memStatus.ullTotalPhys / (1024 * 1024));
    values["memoryAvailMb"] = static_cast<qint64>(
        memStatus.ullAvailPhys / (1024 * 1024));

    qDebug() << "MEM:" << values["memoryPercent"].toDouble() << "%"
              << values["memoryUsedMb"].toInt() << "MB /"
              << values["memoryTotalMb"].toInt() << "MB";
    emit dataReady(MonitorData("memory", values));
#else
    QVariantMap values;
    values["memoryPercent"] = 0.0;
    values["memoryUsedMb"] = 0;
    values["memoryTotalMb"] = 0;
    emit dataReady(MonitorData("memory", values));
#endif
}
