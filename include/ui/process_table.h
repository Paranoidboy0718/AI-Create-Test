#ifndef PROCESS_TABLE_H
#define PROCESS_TABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QList>
#include "core/monitor_data.h"

class ProcessTable : public QWidget {
    Q_OBJECT
public:
    explicit ProcessTable(QWidget *parent = nullptr);

public slots:
    void updateProcesses(const QList<ProcessInfo> &processes);

private:
    QTableWidget *m_table;
};

#endif // PROCESS_TABLE_H
