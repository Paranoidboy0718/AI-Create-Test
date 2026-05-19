#ifndef NETWORK_WORKER_H
#define NETWORK_WORKER_H

#include "workers/base_worker.h"

class NetworkWorker : public BaseWorker {
    Q_OBJECT
public:
    explicit NetworkWorker(QObject *parent = nullptr);

protected:
    void collect() override;

private:
    quint64 m_prevBytesIn;
    quint64 m_prevBytesOut;
    bool m_firstSample;
};

#endif // NETWORK_WORKER_H
