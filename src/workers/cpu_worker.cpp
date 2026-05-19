#include "workers/cpu_worker.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

CpuWorker::CpuWorker(QObject *parent)
    : BaseWorker(3000, parent), m_firstSample(true)
{
    m_prevIdleTime = 0;
    m_prevKernelTime = 0;
    m_prevUserTime = 0;
}

CpuWorker::~CpuWorker()
{
    stop();
}

void CpuWorker::collect()
{
#ifdef Q_OS_WIN
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        qWarning() << "CpuWorker: GetSystemTimes failed (" << GetLastError() << ")";
        // 发送默认值
        QVariantMap values;
        values["cpuPercent"] = 0.0;
        emit dataReady(MonitorData("cpu", values));
        return;
    }

    auto fileTimeToInt64 = [](const FILETIME &ft) -> qint64 {
        return (static_cast<qint64>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    };

    qint64 idle = fileTimeToInt64(idleTime);
    qint64 kernel = fileTimeToInt64(kernelTime);
    qint64 user = fileTimeToInt64(userTime);

    QVariantMap values;
    if (m_firstSample) {
        m_prevIdleTime = idle;
        m_prevKernelTime = kernel;
        m_prevUserTime = user;
        m_firstSample = false;
        values["cpuPercent"] = 0.0;
    } else {
        qint64 idleDiff = idle - m_prevIdleTime;
        qint64 kernelDiff = kernel - m_prevKernelTime;
        qint64 userDiff = user - m_prevUserTime;
        qint64 totalDiff = kernelDiff + userDiff;

        double cpuPercent = 0.0;
        if (totalDiff > 0) {
            cpuPercent = 100.0 * (1.0 - static_cast<double>(idleDiff) / totalDiff);
        }

        m_prevIdleTime = idle;
        m_prevKernelTime = kernel;
        m_prevUserTime = user;
        values["cpuPercent"] = qBound(0.0, cpuPercent, 100.0);
    }

    qDebug() << "CPU:" << values["cpuPercent"].toDouble();
    emit dataReady(MonitorData("cpu", values));
#else
    QVariantMap values;
    values["cpuPercent"] = 0.0;
    emit dataReady(MonitorData("cpu", values));
#endif
}
