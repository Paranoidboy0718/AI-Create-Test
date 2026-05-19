#include "ui/file_log_widget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>

FileLogWidget::FileLogWidget(QWidget *parent)
    : QWidget(parent), m_maxEntries(200)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_listWidget = new QListWidget(this);
    m_listWidget->setStyleSheet(
        "QListWidget { background-color: #16213e; color: #ccc; border: none; }"
        "QListWidget::item { padding: 2px 6px; border-bottom: 1px solid #0f3460; }"
    );
    m_listWidget->setWordWrap(true);
    layout->addWidget(m_listWidget);
}

void FileLogWidget::addFileEvent(const FileEvent &event)
{
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(event.timestamp);
    QString timeStr = dt.toString("HH:mm:ss");

    QColor eventColor;
    QString eventIcon;
    if (event.eventType == "created") {
        eventColor = QColor("#53d8fb");
        eventIcon = "+";
    } else if (event.eventType == "deleted") {
        eventColor = QColor("#e94560");
        eventIcon = "-";
    } else {
        eventColor = QColor("#2ecc71");
        eventIcon = "~";
    }

    QString text = QString("%1 [%2] %3 %4")
        .arg(timeStr)
        .arg(eventIcon)
        .arg(event.path)
        .arg(event.eventType);

    auto *item = new QListWidgetItem(text);
    item->setForeground(eventColor);
    m_listWidget->insertItem(0, item);

    while (m_listWidget->count() > m_maxEntries) {
        delete m_listWidget->takeItem(m_listWidget->count() - 1);
    }
}

void FileLogWidget::clearLog()
{
    m_listWidget->clear();
}
