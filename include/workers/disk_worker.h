#ifndef DISK_WORKER_H
#define DISK_WORKER_H

#include "workers/base_worker.h"

class DiskWorker : public BaseWorker {
    Q_OBJECT
public:
    explicit DiskWorker(QObject *parent = nullptr);

protected:
    void collect() override;
};

#endif // DISK_WORKER_H
