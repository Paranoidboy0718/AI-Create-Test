# 桌面监控软件 — 设计文档

**日期**: 2026-05-17
**项目**: TraeAssistant — Desktop Monitor
**技术栈**: VS Code + Qt 5.12 + C++11 + SQLite

---

## 1. 项目概述

基于 Qt 5.12 和 C++11 开发的个人电脑桌面监控软件，提供六大维度的实时监控功能，以实时仪表盘为核心交互方式，支持 7 天历史数据回溯。

---

## 2. 功能范围

### 2.1 六大监控维度

- **CPU 监控**: 实时占用率、每个核心的负载、历史趋势
- **内存监控**: 已用/总量、占用百分比、进程级内存分布
- **磁盘监控**: 各分区使用率、读写速度（MBps）、I/O 状态
- **网络监控**: 上下行速度（Kbps）、TCP 连接数、活跃网卡流量
- **进程管理**: 进程列表、按 CPU/内存排序、结束进程
- **文件监控**: 指定目录的文件变更（新增/修改/删除），事件日志

### 2.2 核心特性

- 实时仪表盘展示（3 秒刷新）
- 7 天轻量历史数据持久化（SQLite）
- 自动清理过期数据
- 折线趋势图展示历史变化
- 可拖拽分隔的灵活布局

---

## 3. 架构设计

### 3.1 多 Worker 线程 + 信号槽架构


```
┌──────────────────────────────────────┐
│         主线程 (UI)                  │
│  仪表盘 | 图表 | 状态栏 | 系统托盘   │
└──────────┬───────────────────────────┘
           │ 信号槽跨线程通信
┌──────────▼───────────────────────────┐
│      Worker 线程池                   │
│  CPU  Mem  Disk  Net  Proc  FileMon  │
└──────────┬───────────────────────────┘
           │ 采集数据
┌──────────▼───────────────────────────┐
│      系统 API 层                     │
│  Windows PDH / WMI / Win32 API       │
└──────────────────────────────────────┘
           │
┌──────────▼───────────────────────────┐
│    SQLite 持久化层                    │
│    每 30 秒批量写入                   │
└──────────────────────────────────────┘
```

### 3.2 Worker 分工

| Worker | 采集方式 | 频率 |
|--------|---------|------|
| CpuWorker | PDH 性能计数器 | 3 秒 |
| MemoryWorker | GlobalMemoryStatusEx | 3 秒 |
| DiskWorker | GetDiskFreeSpaceEx / PDH | 3 秒 |
| NetworkWorker | GetIfTable / PDH | 3 秒 |
| ProcessWorker | CreateToolhelp32Snapshot | 3 秒 |
| FileMonitor | QFileSystemWatcher | 事件驱动 |

---

## 4. 模块详细设计

### 4.1 核心类

- **MonitorManager**: 总控制器，管理所有 Worker 的生命周期
  - 启动/停止所有 Worker
  - 汇总各 Worker 采集的数据，分发至 UI
  - 调度 DatabaseManager 进行批量写入
- **BaseWorker** (抽象基类, QObject):
  - QTimer m_timer (3 秒间隔)
  - virtual void collect() = 0
  - signal dataReady(const MonitorData&)
- **MonitorData** (struct):
  - QString type (cpu/mem/disk/net/proc/file)
  - QVariantMap values
  - qint64 timestamp
- **DatabaseManager**:
  - SQLite 封装
  - 每 30 秒批量 INSERT
  - 每小时清理 7 天前旧数据

---

## 5. UI 设计

### 5.1 布局结构

**顶部**: 4 个实时指标卡片（CPU / 内存 / 磁盘 / 网络）
**中部**: 折线趋势图（CPU/内存 7 天趋势 + 网络实时曲线）
**底部**: 进程列表（可排序）+ 文件监控日志

使用 QSplitter 实现可拖拽分隔布局。

---

## 6. 数据持久化

### 6.1 表结构

**system_snapshots**:
- id, timestamp, cpu_percent, memory_percent, memory_used_mb, memory_total_mb
- disk_percent, disk_read_mbps, disk_write_mbps, net_down_kbps, net_up_kbps, net_connections

**processes**:
- id, snapshot_id (FK), pid, name, cpu_percent, memory_mb, window_title

**file_events**:
- id, timestamp, path, event_type (modified|created|deleted), file_size

### 6.2 清理策略

- 每小时执行: DELETE FROM 表 WHERE timestamp < datetime('now', '-7 days')

---

## 7. 项目文件结构

```
TraeAssistant/
├── TraeAssistant.pro          # 扩展 QT += sql 和新增源文件
├── mainwindow.ui              # 重新设计为仪表盘布局
├── src/
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── core/
│   │   ├── monitor_manager.cpp
│   │   ├── database_manager.cpp
│   │   └── monitor_data.h
│   ├── workers/
│   │   ├── base_worker.cpp
│   │   ├── cpu_worker.cpp
│   │   ├── memory_worker.cpp
│   │   ├── disk_worker.cpp
│   │   ├── network_worker.cpp
│   │   ├── process_worker.cpp
│   │   └── file_monitor.cpp
│   └── ui/
│       ├── dashboard_widget.cpp
│       ├── gauge_card.cpp
│       ├── trend_chart.cpp
│       ├── process_table.cpp
│       └── file_log_widget.cpp
├── include/
│   ├── mainwindow.h
│   ├── core/
│   ├── workers/
│   └── ui/
└── resources/
    └── icons/
```

---

## 8. 技术依赖

- Qt 5.12: core, gui, widgets, network, sql
- C++11 标准
- Windows API: PDH, WMI, Kernel32
- SQLite (通过 Qt SQL 模块)

