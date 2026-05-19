#include "workers/base_worker.h"
#include <QDebug>

BaseWorker::BaseWorker(int intervalMs, QObject *parent)
    : QObject(parent), m_timer(nullptr), m_interval(intervalMs)
{
}

BaseWorker::~BaseWorker()
{
    stop();
}

void BaseWorker::start()
{
    if (m_timer && m_timer->isActive()) return;

    // 在调用 start() 的线程中创建 QTimer
    if (!m_timer) {
        m_timer = new QTimer(this);
        m_timer->setInterval(m_interval);
        connect(m_timer, &QTimer::timeout, this, &BaseWorker::onTimeout);
    }

    // 首次启动时立即采集一次，不用等 3 秒
    collect();

    m_timer->start();
    qDebug() << objectName() << "started, interval:" << m_interval << "ms";
}

void BaseWorker::stop()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        qDebug() << objectName() << "stopped";
    }
}

bool BaseWorker::isRunning() const
{
    return m_timer && m_timer->isActive();
}

void BaseWorker::setInterval(int ms)
{
    m_interval = ms;
    if (m_timer) {
        m_timer->setInterval(m_interval);
    }
}

void BaseWorker::onTimeout()
{
    collect();
}
