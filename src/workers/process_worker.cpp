#include "workers/process_worker.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#endif

ProcessWorker::ProcessWorker(QObject *parent)
    : BaseWorker(30000, parent) // 每 30 秒一次
{
}

void ProcessWorker::collect()
{
#ifdef Q_OS_WIN
    QList<ProcessInfo> processes;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            ProcessInfo info;
            info.pid = pe32.th32ProcessID;
            info.name = QString::fromWCharArray(pe32.szExeFile);
            info.cpuPercent = 0.0; // CPU 百分比需要 PDH 更精确计算
            info.memoryBytes = static_cast<quint64>(pe32.cntUsage) * 1024;
            info.windowTitle = QString();
            processes.append(info);
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);

    QVariantMap values;
    values["processCount"] = static_cast<int>(processes.size());
    emit dataReady(MonitorData("process", values));
    emit processListReady(processes);
#else
    QVariantMap values;
    values["processCount"] = 0;
    emit dataReady(MonitorData("process", values));
    emit processListReady(QList<ProcessInfo>());
#endif
}
