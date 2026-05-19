#ifndef FILE_LOG_WIDGET_H
#define FILE_LOG_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include "core/monitor_data.h"

class FileLogWidget : public QWidget {
    Q_OBJECT
public:
    explicit FileLogWidget(QWidget *parent = nullptr);

public slots:
    void addFileEvent(const FileEvent &event);
    void clearLog();

private:
    QListWidget *m_listWidget;
    int m_maxEntries;
};

#endif // FILE_LOG_WIDGET_H
