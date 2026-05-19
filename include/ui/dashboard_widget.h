#ifndef DASHBOARD_WIDGET_H
#define DASHBOARD_WIDGET_H

#include <QWidget>
#include <QList>
#include "core/monitor_data.h"

class GaugeCard;
class TrendChart;
class ProcessTable;
class FileLogWidget;

namespace Ui {
class DashboardWidget;
}

class DashboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget *parent = nullptr);
    ~DashboardWidget();

public slots:
    void onMonitorData(const MonitorData &data);
    void onProcessList(const QList<ProcessInfo> &processes);
    void onFileEvent(const FileEvent &event);

private:
    void initCards();
    void updateGauges(const MonitorData &data);
    void updateCharts(const MonitorData &data);

    Ui::DashboardWidget *ui;
    GaugeCard *m_cpuCard;
    GaugeCard *m_memCard;
    GaugeCard *m_diskCard;
    GaugeCard *m_netCard;
    TrendChart *m_cpuChart;
    TrendChart *m_netChart;
    ProcessTable *m_processTable;
    FileLogWidget *m_fileLog;
};

#endif
