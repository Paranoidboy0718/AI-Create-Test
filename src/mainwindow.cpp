#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/monitor_manager.h"
#include "ui/dashboard_widget.h"
#include <QStyle>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_monitorManager(nullptr)
    , m_dashboard(nullptr)
    , m_trayIcon(nullptr)
    , m_trayMenu(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Desktop Monitor v1.0");
    resize(1024, 680);
    setMinimumSize(800, 500);

    m_dashboard = new DashboardWidget(this);
    setCentralWidget(m_dashboard);

    m_monitorManager = new MonitorManager(this);

    QString dbPath = QCoreApplication::applicationDirPath() + "/data/monitor_data.db";
    if (m_monitorManager->initialize(dbPath)) {
        connect(m_monitorManager, &MonitorManager::monitorDataReady,
                m_dashboard, &DashboardWidget::onMonitorData);
        connect(m_monitorManager, &MonitorManager::processListReady,
                m_dashboard, &DashboardWidget::onProcessList);
        connect(m_monitorManager, &MonitorManager::fileEventOccurred,
                m_dashboard, &DashboardWidget::onFileEvent);

        m_monitorManager->addWatchPath(QDir::homePath() + "/Desktop");
        m_monitorManager->addWatchPath(QDir::homePath() + "/Downloads");
        m_monitorManager->start();
    } else {
        qWarning() << "Failed to initialize MonitorManager";
    }

    setupSystemTray();
    setStyleSheet(
        "QMainWindow { background-color: #0f0f23; }"
    );
}

MainWindow::~MainWindow()
{
    if (m_monitorManager) {
        m_monitorManager->stop();
    }
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("确认退出");
    msgBox.setText("是否退出 Desktop Monitor？");
    msgBox.setInformativeText("监控将在后台继续运行。");

    auto *quitBtn = msgBox.addButton("退出", QMessageBox::AcceptRole);
    auto *minBtn = msgBox.addButton("最小化到托盘", QMessageBox::ActionRole);
    msgBox.addButton("取消", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == quitBtn) {
        if (m_trayIcon) m_trayIcon->hide();
        event->accept();
        QApplication::quit();
    } else if (msgBox.clickedButton() == minBtn) {
        hide();
        event->ignore();
    } else {
        event->ignore();
    }
}

void MainWindow::setupSystemTray()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setToolTip("Desktop Monitor - 运行中");
    m_trayIcon->setIcon(qApp->style()->standardIcon(
        QStyle::SP_ComputerIcon));

    m_trayMenu = new QMenu(this);
    auto *showAction = m_trayMenu->addAction("显示窗口");
    connect(showAction, &QAction::triggered, this, &MainWindow::showWindow);

    auto *quitAction = m_trayMenu->addAction("退出");
    connect(quitAction, &QAction::triggered, this, [this]() {
        if (m_trayIcon) m_trayIcon->hide();
        QApplication::quit();
    });

    m_trayIcon->setContextMenu(m_trayMenu);
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayIconActivated);

    m_trayIcon->show();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        showWindow();
    }
}

void MainWindow::showWindow()
{
    showNormal();
    activateWindow();
    raise();
}
