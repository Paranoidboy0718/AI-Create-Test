#include "ui/process_table.h"
#include <QVBoxLayout>
#include <QHeaderView>

ProcessTable::ProcessTable(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({ "进程名", "PID", "CPU %", "内存 (MB)" });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(
        "QTableWidget { background-color: #16213e; color: #ccc; border: none; }"
        "QHeaderView::section { background-color: #0f3460; color: #888; padding: 4px; border: none; }"
        "QTableWidget::item:alternate { background-color: #1a1a2e; }"
    );
    m_table->verticalHeader()->setVisible(false);
    m_table->setSortingEnabled(true);
    layout->addWidget(m_table);
}

void ProcessTable::updateProcesses(const QList<ProcessInfo> &processes)
{
    m_table->setSortingEnabled(false);
    m_table->setRowCount(processes.size());

    for (int i = 0; i < processes.size(); ++i) {
        const auto &p = processes[i];
        m_table->setItem(i, 0, new QTableWidgetItem(p.name));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(p.pid)));

        auto *cpuItem = new QTableWidgetItem(
            QString::number(p.cpuPercent, 'f', 1));
        cpuItem->setForeground(p.cpuPercent > 50 ? QColor("#e94560") : QColor("#53d8fb"));
        m_table->setItem(i, 2, cpuItem);

        auto *memItem = new QTableWidgetItem(
            QString::number(p.memoryBytes / (1024 * 1024)));
        m_table->setItem(i, 3, memItem);
    }

    m_table->setSortingEnabled(true);
    m_table->resizeColumnsToContents();
}
