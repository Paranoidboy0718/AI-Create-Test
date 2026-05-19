#ifndef PROCESS_WORKER_H
#define PROCESS_WORKER_H

#include "workers/base_worker.h"
#include <QList>
#include "core/monitor_data.h"

class ProcessWorker : public BaseWorker {
    Q_OBJECT
public:
    explicit ProcessWorker(QObject *parent = nullptr);

signals:
    void processListReady(const QList<ProcessInfo> &processes);

protected:
    void collect() override;
};

#endif // PROCESS_WORKER_H
