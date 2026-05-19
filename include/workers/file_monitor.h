#ifndef FILE_MONITOR_H
#define FILE_MONITOR_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QStringList>
#include "core/monitor_data.h"

class FileMonitor : public QObject {
    Q_OBJECT
public:
    explicit FileMonitor(QObject *parent = nullptr);

    void addWatchPath(const QString &path);
    void removeWatchPath(const QString &path);
    QStringList watchedPaths() const;

signals:
    void fileEventOccurred(const FileEvent &event);

private slots:
    void onFileChanged(const QString &path);
    void onDirectoryChanged(const QString &path);

private:
    QFileSystemWatcher *m_watcher;
};

#endif // FILE_MONITOR_H
