#ifndef MONITOR_DATA_H
#define MONITOR_DATA_H

#include <QString>
#include <QVariantMap>
#include <QDateTime>
#include <QMetaType>

struct MonitorData {
    QString type;
    QVariantMap values;
    qint64 timestamp;

    MonitorData() : timestamp(QDateTime::currentMSecsSinceEpoch()) {}
    MonitorData(const QString &t, const QVariantMap &v)
        : type(t), values(v), timestamp(QDateTime::currentMSecsSinceEpoch()) {}
};

Q_DECLARE_METATYPE(MonitorData)

struct ProcessInfo {
    quint32 pid;
    QString name;
    double cpuPercent;
    quint64 memoryBytes;
    QString windowTitle;
};

Q_DECLARE_METATYPE(ProcessInfo)

struct FileEvent {
    QString path;
    QString eventType;
    qint64 fileSize;
    qint64 timestamp;
};

Q_DECLARE_METATYPE(FileEvent)

#endif // MONITOR_DATA_H
