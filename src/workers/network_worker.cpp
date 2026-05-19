#include "workers/network_worker.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <iphlpapi.h>
#endif

NetworkWorker::NetworkWorker(QObject *parent)
    : BaseWorker(3000, parent), m_firstSample(true)
{
    m_prevBytesIn = 0;
    m_prevBytesOut = 0;
}

void NetworkWorker::collect()
{
#ifdef Q_OS_WIN
    MIB_IFTABLE *ifTable = nullptr;
    ULONG bufSize = 0;

    if (GetIfTable(nullptr, &bufSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        ifTable = reinterpret_cast<MIB_IFTABLE*>(new char[bufSize]);
    } else {
        return;
    }

    quint64 totalIn = 0, totalOut = 0;
    if (GetIfTable(ifTable, &bufSize, FALSE) == NO_ERROR) {
        for (DWORD i = 0; i < ifTable->dwNumEntries; ++i) {
            MIB_IFROW &row = ifTable->table[i];
            if (row.dwType == IF_TYPE_ETHERNET_CSMACD ||
                row.dwType == IF_TYPE_PPP ||
                row.dwType == IF_TYPE_SOFTWARE_LOOPBACK) {
                totalIn += row.dwInOctets;
                totalOut += row.dwOutOctets;
            }
        }
    }

    delete[] reinterpret_cast<char*>(ifTable);

    QVariantMap values;
    if (m_firstSample) {
        m_prevBytesIn = totalIn;
        m_prevBytesOut = totalOut;
        m_firstSample = false;
        values["netDownKbps"] = 0;
        values["netUpKbps"] = 0;
    } else {
        quint64 downBytes = (totalIn >= m_prevBytesIn) ? (totalIn - m_prevBytesIn) : 0;
        quint64 upBytes = (totalOut >= m_prevBytesOut) ? (totalOut - m_prevBytesOut) : 0;
        values["netDownKbps"] = static_cast<qint64>(downBytes * 8 / 3000);
        values["netUpKbps"] = static_cast<qint64>(upBytes * 8 / 3000);
        m_prevBytesIn = totalIn;
        m_prevBytesOut = totalOut;
    }

    values["netConnections"] = 0;
    emit dataReady(MonitorData("network", values));
#else
    QVariantMap values;
    values["netDownKbps"] = 0;
    values["netUpKbps"] = 0;
    values["netConnections"] = 0;
    emit dataReady(MonitorData("network", values));
#endif
}
