#ifndef MONITOR_MANAGER_H
#define MONITOR_MANAGER_H

#include <QObject>
#include <QThread>
#include <QList>
#include "core/monitor_data.h"
#include "core/database_manager.h"
#include "workers/cpu_worker.h"
#include "workers/memory_worker.h"
#include "workers/disk_worker.h"
#include "workers/network_worker.h"
#include "workers/process_worker.h"
#include "workers/file_monitor.h"

class MonitorManager : public QObject {
    Q_OBJECT
public:
    explicit MonitorManager(QObject *parent = nullptr);
    ~MonitorManager();

    bool initialize(const QString &dbPath);
    void start();
    void stop();
    bool isRunning() const;

    void addWatchPath(const QString &path);
    FileMonitor* fileMonitor() const { return m_fileMonitor; }

signals:
    void monitorDataReady(const MonitorData &data);
    void processListReady(const QList<ProcessInfo> &processes);
    void fileEventOccurred(const FileEvent &event);

private slots:
    void onWorkerData(const MonitorData &data);
    void onProcessList(const QList<ProcessInfo> &processes);
    void onFileEvent(const FileEvent &event);

private:
    DatabaseManager *m_database;

    CpuWorker *m_cpuWorker;
    MemoryWorker *m_memWorker;
    DiskWorker *m_diskWorker;
    NetworkWorker *m_netWorker;
    ProcessWorker *m_procWorker;
    FileMonitor *m_fileMonitor;

    QThread *m_cpuThread;
    QThread *m_memThread;
    QThread *m_diskThread;
    QThread *m_netThread;
    QThread *m_procThread;

    bool m_initialized;
    bool m_running;
};

#endif // MONITOR_MANAGER_H
