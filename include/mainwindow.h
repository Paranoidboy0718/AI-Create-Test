#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>

namespace Ui {
class MainWindow;
}

class MonitorManager;
class DashboardWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void showWindow();

private:
    void setupSystemTray();

    Ui::MainWindow *ui;
    MonitorManager *m_monitorManager;
    DashboardWidget *m_dashboard;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
};

#endif // MAINWINDOW_H
