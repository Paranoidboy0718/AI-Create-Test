#ifndef CPU_WORKER_H
#define CPU_WORKER_H

#include "workers/base_worker.h"

class CpuWorker : public BaseWorker {
    Q_OBJECT
public:
    explicit CpuWorker(QObject *parent = nullptr);
    ~CpuWorker();

protected:
    void collect() override;

private:
    qint64 m_prevIdleTime;
    qint64 m_prevKernelTime;
    qint64 m_prevUserTime;
    bool m_firstSample;
};

#endif // CPU_WORKER_H
