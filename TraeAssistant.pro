QT       += core gui widgets network sql
CONFIG   += c++11
TARGET   = TraeAssistant
TEMPLATE = app
DEFINES  += QT_DEPRECATED_WARNINGS

INCLUDEPATH += include
INCLUDEPATH += include/core
INCLUDEPATH += include/workers
INCLUDEPATH += include/ui

HEADERS += include/mainwindow.h
HEADERS += include/core/monitor_data.h
HEADERS += include/core/monitor_manager.h
HEADERS += include/core/database_manager.h
HEADERS += include/workers/base_worker.h
HEADERS += include/workers/cpu_worker.h
HEADERS += include/workers/memory_worker.h
HEADERS += include/workers/disk_worker.h
HEADERS += include/workers/network_worker.h
HEADERS += include/workers/process_worker.h
HEADERS += include/workers/file_monitor.h
HEADERS += include/ui/dashboard_widget.h
HEADERS += include/ui/gauge_card.h
HEADERS += include/ui/trend_chart.h
HEADERS += include/ui/process_table.h
HEADERS += include/ui/file_log_widget.h

SOURCES += src/mainwindow.cpp src/main.cpp
SOURCES += src/core/monitor_manager.cpp
SOURCES += src/core/database_manager.cpp
SOURCES += src/workers/base_worker.cpp
SOURCES += src/workers/cpu_worker.cpp
SOURCES += src/workers/memory_worker.cpp
SOURCES += src/workers/disk_worker.cpp
SOURCES += src/workers/network_worker.cpp
SOURCES += src/workers/process_worker.cpp
SOURCES += src/workers/file_monitor.cpp
SOURCES += src/ui/dashboard_widget.cpp
SOURCES += src/ui/gauge_card.cpp
SOURCES += src/ui/trend_chart.cpp
SOURCES += src/ui/process_table.cpp
SOURCES += src/ui/file_log_widget.cpp

FORMS += forms/mainwindow.ui forms/dashboard.ui

DESTDIR = build
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

LIBS += -liphlpapi
