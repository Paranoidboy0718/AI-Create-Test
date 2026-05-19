#include "workers/disk_worker.h"
#include <QDebug>
#include <QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

DiskWorker::DiskWorker(QObject *parent)
    : BaseWorker(3000, parent)
{
}

void DiskWorker::collect()
{
#ifdef Q_OS_WIN
    QVariantMap values;

    ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
    if (GetDiskFreeSpaceExW(
            reinterpret_cast<LPCWSTR>(QDir::rootPath().utf16()),
            &freeBytes, &totalBytes, &totalFreeBytes)) {
        quint64 totalMB = totalBytes.QuadPart / (1024 * 1024);
        quint64 freeMB = freeBytes.QuadPart / (1024 * 1024);
        quint64 usedMB = totalMB - freeMB;
        double percent = (totalMB > 0) ? (100.0 * usedMB / totalMB) : 0.0;

        values["diskPercent"] = percent;
        values["diskUsedMb"] = static_cast<qint64>(usedMB);
        values["diskTotalMb"] = static_cast<qint64>(totalMB);
        values["diskFreeMb"] = static_cast<qint64>(freeMB);
        values["diskReadMbps"] = 0.0;
        values["diskWriteMbps"] = 0.0;
    }

    emit dataReady(MonitorData("disk", values));
#else
    QVariantMap values;
    values["diskPercent"] = 0.0;
    emit dataReady(MonitorData("disk", values));
#endif
}
