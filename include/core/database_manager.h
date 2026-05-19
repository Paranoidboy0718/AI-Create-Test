#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QTimer>
#include <QList>
#include "core/monitor_data.h"

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initialize(const QString &dbPath);
    void close();

public slots:
    void saveSnapshot(const MonitorData &data);
    void saveProcesses(const QList<ProcessInfo> &processes, qint64 snapshotId);
    void saveFileEvent(const FileEvent &event);

private slots:
    void flushBatch();
    void cleanOldData();

private:
    void createTables();
    void processBatchWrite();

    QSqlDatabase m_db;
    QTimer *m_flushTimer;
    QTimer *m_cleanTimer;

    // 批量写入缓冲区
    QList<MonitorData> m_snapshotBuffer;
    QList<ProcessInfo> m_processBuffer;
    QList<FileEvent> m_fileEventBuffer;
    qint64 m_lastSnapshotId;
};

#endif // DATABASE_MANAGER_H
