#include "ui/dashboard_widget.h"
#include "ui_dashboard.h"
#include "ui/gauge_card.h"
#include "ui/trend_chart.h"
#include "ui/process_table.h"
#include "ui/file_log_widget.h"

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DashboardWidget)
{
    ui->setupUi(this);

    // 从 ui 中获取自定义控件的指针
    m_cpuCard = ui->cpuCard;
    m_memCard = ui->memCard;
    m_diskCard = ui->diskCard;
    m_netCard = ui->netCard;
    m_cpuChart = ui->cpuChart;
    m_netChart = ui->netChart;
    m_processTable = ui->processTable;
    m_fileLog = ui->fileLog;

    initCards();
}

DashboardWidget::~DashboardWidget()
{
    delete ui;
}

void DashboardWidget::initCards()
{
    m_cpuCard->setTitle("CPU");
    m_cpuCard->setUnit("%");
    m_cpuCard->setAccentColor(QColor("#e94560"));

    m_memCard->setTitle("内存");
    m_memCard->setUnit("%");
    m_memCard->setAccentColor(QColor("#53d8fb"));

    m_diskCard->setTitle("磁盘");
    m_diskCard->setUnit("%");
    m_diskCard->setAccentColor(QColor("#2ecc71"));

    m_netCard->setTitle("网络");
    m_netCard->setUnit("Kbps");
    m_netCard->setAccentColor(QColor("#f39c12"));

    m_cpuChart->setTitle("CPU 趋势");
    m_cpuChart->setLineColor(QColor("#e94560"));

    m_netChart->setTitle("网络趋势");
    m_netChart->setLineColor(QColor("#f39c12"));
}

void DashboardWidget::onMonitorData(const MonitorData &data)
{
    updateGauges(data);
    updateCharts(data);
}

void DashboardWidget::updateGauges(const MonitorData &data)
{
    if (data.type == "cpu") {
        double val = data.values.value("cpuPercent").toDouble();
        m_cpuCard->updateValue(val, QString::number(val, 'f', 1));
    } else if (data.type == "memory") {
        double val = data.values.value("memoryPercent").toDouble();
        int used = data.values.value("memoryUsedMb").toInt();
        int total = data.values.value("memoryTotalMb").toInt();
        m_memCard->updateValue(val,
            QString::number(used) + "MB / " + QString::number(total) + "MB");
    } else if (data.type == "disk") {
        double val = data.values.value("diskPercent").toDouble();
        m_diskCard->updateValue(val, QString::number(val, 'f', 1));
    } else if (data.type == "network") {
        int down = data.values.value("netDownKbps").toInt();
        int up = data.values.value("netUpKbps").toInt();
        m_netCard->updateValue(
            QString("↓%1 / ↑%2 Kbps").arg(down).arg(up));
    }
}

void DashboardWidget::updateCharts(const MonitorData &data)
{
    if (data.type == "cpu") {
        m_cpuChart->addDataPoint(data.values.value("cpuPercent").toDouble());
    } else if (data.type == "network") {
        m_netChart->addDataPoint(
            data.values.value("netDownKbps").toDouble());
    }
}

void DashboardWidget::onProcessList(const QList<ProcessInfo> &processes)
{
    m_processTable->updateProcesses(processes);
}

void DashboardWidget::onFileEvent(const FileEvent &event)
{
    m_fileLog->addFileEvent(event);
}
