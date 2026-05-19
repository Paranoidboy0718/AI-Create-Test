#include "core/database_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), m_lastSnapshotId(0)
{
    m_flushTimer = new QTimer(this);
    m_flushTimer->setInterval(30000); // 30 秒批量写入
    connect(m_flushTimer, &QTimer::timeout, this, &DatabaseManager::flushBatch);

    m_cleanTimer = new QTimer(this);
    m_cleanTimer->setInterval(3600000); // 1 小时清理
    connect(m_cleanTimer, &QTimer::timeout, this, &DatabaseManager::cleanOldData);
}

DatabaseManager::~DatabaseManager()
{
    flushBatch(); // 关闭前写入剩余数据
    close();
}

bool DatabaseManager::initialize(const QString &dbPath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    createTables();
    m_flushTimer->start();
    m_cleanTimer->start();
    return true;
}

void DatabaseManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void DatabaseManager::createTables()
{
    QSqlQuery query(m_db);
    query.exec(
        "CREATE TABLE IF NOT EXISTS system_snapshots ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  cpu_percent REAL,"
        "  memory_percent REAL,"
        "  memory_used_mb INTEGER,"
        "  memory_total_mb INTEGER,"
        "  disk_percent REAL,"
        "  disk_read_mbps REAL,"
        "  disk_write_mbps REAL,"
        "  net_down_kbps INTEGER,"
        "  net_up_kbps INTEGER,"
        "  net_connections INTEGER"
        ")"
    );

    query.exec(
        "CREATE TABLE IF NOT EXISTS processes ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  snapshot_id INTEGER,"
        "  pid INTEGER,"
        "  name TEXT,"
        "  cpu_percent REAL,"
        "  memory_mb INTEGER,"
        "  window_title TEXT"
        ")"
    );

    query.exec(
        "CREATE TABLE IF NOT EXISTS file_events ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  path TEXT,"
        "  event_type TEXT,"
        "  file_size INTEGER"
        ")"
    );
}

void DatabaseManager::saveSnapshot(const MonitorData &data)
{
    m_snapshotBuffer.append(data);
    if (m_snapshotBuffer.size() >= 10) {
        flushBatch();
    }
}

void DatabaseManager::saveProcesses(const QList<ProcessInfo> &processes, qint64 snapshotId)
{
    Q_UNUSED(snapshotId);
    m_processBuffer.append(processes);
    // 进程数据每 30 秒批量写入
}

void DatabaseManager::saveFileEvent(const FileEvent &event)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO file_events (path, event_type, file_size, timestamp) "
        "VALUES (?, ?, ?, ?)"
    );
    query.addBindValue(event.path);
    query.addBindValue(event.eventType);
    query.addBindValue(static_cast<qint64>(event.fileSize));
    query.addBindValue(event.timestamp);
    query.exec();
}

void DatabaseManager::flushBatch()
{
    if (m_snapshotBuffer.isEmpty()) return;
    processBatchWrite();
    m_snapshotBuffer.clear();
    m_processBuffer.clear();
}

void DatabaseManager::processBatchWrite()
{
    QSqlQuery query(m_db);
    m_db.transaction();
    for (const auto &data : m_snapshotBuffer) {
        double cpu = data.values.value("cpuPercent").toDouble();
        double memPercent = data.values.value("memoryPercent").toDouble();
        int memUsed = data.values.value("memoryUsedMb").toInt();
        int memTotal = data.values.value("memoryTotalMb").toInt();
        double diskPct = data.values.value("diskPercent").toDouble();
        double diskR = data.values.value("diskReadMbps").toDouble();
        double diskW = data.values.value("diskWriteMbps").toDouble();
        int netDown = data.values.value("netDownKbps").toInt();
        int netUp = data.values.value("netUpKbps").toInt();
        int netConns = data.values.value("netConnections").toInt();

        query.prepare(
            "INSERT INTO system_snapshots "
            "(cpu_percent, memory_percent, memory_used_mb, memory_total_mb, "
            " disk_percent, disk_read_mbps, disk_write_mbps, "
            " net_down_kbps, net_up_kbps, net_connections) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );
        query.addBindValue(cpu);
        query.addBindValue(memPercent);
        query.addBindValue(memUsed);
        query.addBindValue(memTotal);
        query.addBindValue(diskPct);
        query.addBindValue(diskR);
        query.addBindValue(diskW);
        query.addBindValue(netDown);
        query.addBindValue(netUp);
        query.addBindValue(netConns);
        query.exec();
    }
    m_db.commit();
}

void DatabaseManager::cleanOldData()
{
    QSqlQuery query(m_db);
    query.exec("DELETE FROM system_snapshots WHERE timestamp < datetime('now', '-7 days')");
    query.exec("DELETE FROM processes WHERE snapshot_id NOT IN (SELECT id FROM system_snapshots)");
    query.exec("DELETE FROM file_events WHERE timestamp < datetime('now', '-7 days')");
    query.exec("VACUUM");
    qDebug() << "Cleaned old data older than 7 days";
}
