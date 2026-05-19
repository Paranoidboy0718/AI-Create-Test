#include "core/monitor_manager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QFileInfo>

MonitorManager::MonitorManager(QObject *parent)
    : QObject(parent), m_initialized(false), m_running(false)
{
    // 注册跨线程信号所需的元类型
    qRegisterMetaType<MonitorData>("MonitorData");
    qRegisterMetaType<QList<ProcessInfo>>("QList<ProcessInfo>");

    m_database = new DatabaseManager(this);

    m_cpuWorker = new CpuWorker();
    m_cpuWorker->setObjectName("cpuWorker");
    m_memWorker = new MemoryWorker();
    m_memWorker->setObjectName("memWorker");
    m_diskWorker = new DiskWorker();
    m_diskWorker->setObjectName("diskWorker");
    m_netWorker = new NetworkWorker();
    m_netWorker->setObjectName("netWorker");
    m_procWorker = new ProcessWorker();
    m_procWorker->setObjectName("procWorker");
    m_fileMonitor = new FileMonitor();
    m_fileMonitor->setObjectName("fileMonitor");

    m_cpuThread = new QThread(this);
    m_cpuThread->setObjectName("cpuThread");
    m_memThread = new QThread(this);
    m_memThread->setObjectName("memThread");
    m_diskThread = new QThread(this);
    m_diskThread->setObjectName("diskThread");
    m_netThread = new QThread(this);
    m_netThread->setObjectName("netThread");
    m_procThread = new QThread(this);
    m_procThread->setObjectName("procThread");

    // 连接 Worker 信号 → Manager 槽
    connect(m_cpuWorker, &CpuWorker::dataReady,
            this, &MonitorManager::onWorkerData);
    connect(m_memWorker, &MemoryWorker::dataReady,
            this, &MonitorManager::onWorkerData);
    connect(m_diskWorker, &DiskWorker::dataReady,
            this, &MonitorManager::onWorkerData);
    connect(m_netWorker, &NetworkWorker::dataReady,
            this, &MonitorManager::onWorkerData);
    connect(m_procWorker, &ProcessWorker::dataReady,
            this, &MonitorManager::onWorkerData);
    connect(m_procWorker, &ProcessWorker::processListReady,
            this, &MonitorManager::onProcessList);
    connect(m_fileMonitor, &FileMonitor::fileEventOccurred,
            this, &MonitorManager::onFileEvent);
}

MonitorManager::~MonitorManager()
{
    stop();
}

bool MonitorManager::initialize(const QString &dbPath)
{
    if (m_initialized) return true;

    QString actualPath = dbPath;
    if (actualPath.isEmpty()) {
        actualPath = QDir::homePath() + "/.trae/monitor_data.db";
    }
    QDir().mkpath(QFileInfo(actualPath).absolutePath());

    if (!m_database->initialize(actualPath)) {
        qWarning() << "MonitorManager: Failed to initialize database";
        return false;
    }

    qDebug() << "MonitorManager: Database initialized at" << actualPath;
    m_initialized = true;
    return true;
}

void MonitorManager::start()
{
    if (m_running || !m_initialized) {
        qWarning() << "MonitorManager: already running or not initialized";
        return;
    }

    // 将 Worker 移到工作线程
    m_cpuWorker->moveToThread(m_cpuThread);
    m_memWorker->moveToThread(m_memThread);
    m_diskWorker->moveToThread(m_diskThread);
    m_netWorker->moveToThread(m_netThread);
    m_procWorker->moveToThread(m_procThread);

    // 线程启动后自动调用 Worker::start()
    connect(m_cpuThread, &QThread::started, m_cpuWorker, &BaseWorker::start);
    connect(m_memThread, &QThread::started, m_memWorker, &BaseWorker::start);
    connect(m_diskThread, &QThread::started, m_diskWorker, &BaseWorker::start);
    connect(m_netThread, &QThread::started, m_netWorker, &BaseWorker::start);
    connect(m_procThread, &QThread::started, m_procWorker, &BaseWorker::start);

    // 线程结束时清理 Worker
    connect(m_cpuThread, &QThread::finished, m_cpuWorker, &QObject::deleteLater);
    connect(m_memThread, &QThread::finished, m_memWorker, &QObject::deleteLater);
    connect(m_diskThread, &QThread::finished, m_diskWorker, &QObject::deleteLater);
    connect(m_netThread, &QThread::finished, m_netWorker, &QObject::deleteLater);
    connect(m_procThread, &QThread::finished, m_procWorker, &QObject::deleteLater);

    qDebug() << "MonitorManager: Starting all threads...";
    m_cpuThread->start();
    m_memThread->start();
    m_diskThread->start();
    m_netThread->start();
    m_procThread->start();

    m_running = true;
}

void MonitorManager::stop()
{
    if (!m_running) return;

    qDebug() << "MonitorManager: Stopping all workers...";

    QMetaObject::invokeMethod(m_cpuWorker, &BaseWorker::stop, Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_memWorker, &BaseWorker::stop, Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_diskWorker, &BaseWorker::stop, Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_netWorker, &BaseWorker::stop, Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_procWorker, &BaseWorker::stop, Qt::QueuedConnection);

    m_cpuThread->quit();
    m_memThread->quit();
    m_diskThread->quit();
    m_netThread->quit();
    m_procThread->quit();

    m_cpuThread->wait(3000);
    m_memThread->wait(3000);
    m_diskThread->wait(3000);
    m_netThread->wait(3000);
    m_procThread->wait(3000);

    m_running = false;
    qDebug() << "MonitorManager: All workers stopped";
}

bool MonitorManager::isRunning() const
{
    return m_running;
}

void MonitorManager::addWatchPath(const QString &path)
{
    m_fileMonitor->addWatchPath(path);
}

void MonitorManager::onWorkerData(const MonitorData &data)
{
    qDebug() << "MonitorManager: got data type=" << data.type
              << "values=" << data.values;
    emit monitorDataReady(data);
    m_database->saveSnapshot(data);
}

void MonitorManager::onProcessList(const QList<ProcessInfo> &processes)
{
    qDebug() << "MonitorManager: got" << processes.size() << "processes";
    emit processListReady(processes);
}

void MonitorManager::onFileEvent(const FileEvent &event)
{
    emit fileEventOccurred(event);
    m_database->saveFileEvent(event);
}


