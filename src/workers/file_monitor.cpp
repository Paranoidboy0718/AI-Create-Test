#include "workers/file_monitor.h"
#include <QFileInfo>
#include <QDateTime>

FileMonitor::FileMonitor(QObject *parent)
    : QObject(parent)
{
    m_watcher = new QFileSystemWatcher(this);
    connect(m_watcher, &QFileSystemWatcher::fileChanged,
            this, &FileMonitor::onFileChanged);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged,
            this, &FileMonitor::onDirectoryChanged);
}

void FileMonitor::addWatchPath(const QString &path)
{
    if (!m_watcher->files().contains(path) &&
        !m_watcher->directories().contains(path)) {
        m_watcher->addPath(path);
    }
}

void FileMonitor::removeWatchPath(const QString &path)
{
    m_watcher->removePath(path);
}

QStringList FileMonitor::watchedPaths() const
{
    QStringList all = m_watcher->files();
    all.append(m_watcher->directories());
    return all;
}

void FileMonitor::onFileChanged(const QString &path)
{
    FileEvent event;
    event.path = path;
    event.timestamp = QDateTime::currentMSecsSinceEpoch();

    QFileInfo fi(path);
    if (fi.exists()) {
        event.eventType = "modified";
        event.fileSize = fi.size();
    } else {
        event.eventType = "deleted";
        event.fileSize = 0;
    }

    emit fileEventOccurred(event);
}

void FileMonitor::onDirectoryChanged(const QString &path)
{
    FileEvent event;
    event.path = path;
    event.eventType = "modified";
    event.timestamp = QDateTime::currentMSecsSinceEpoch();
    event.fileSize = 0;
    emit fileEventOccurred(event);
}
