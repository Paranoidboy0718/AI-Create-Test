#ifndef BASE_WORKER_H
#define BASE_WORKER_H

#include <QObject>
#include <QTimer>
#include "core/monitor_data.h"

class BaseWorker : public QObject {
    Q_OBJECT
public:
    explicit BaseWorker(int intervalMs, QObject *parent = nullptr);
    virtual ~BaseWorker();

    bool isRunning() const;
    void setInterval(int ms);

public slots:
    void start();
    void stop();

signals:
    void dataReady(const MonitorData &data);

protected:
    QTimer *m_timer;
    int m_interval;
    virtual void collect() = 0;

private slots:
    void onTimeout();
};

#endif // BASE_WORKER_H
