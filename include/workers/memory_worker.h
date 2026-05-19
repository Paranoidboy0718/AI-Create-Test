#ifndef MEMORY_WORKER_H
#define MEMORY_WORKER_H

#include "workers/base_worker.h"

class MemoryWorker : public BaseWorker {
    Q_OBJECT
public:
    explicit MemoryWorker(QObject *parent = nullptr);

protected:
    void collect() override;
};

#endif // MEMORY_WORKER_H
