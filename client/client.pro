QT += core gui widgets network multimedia concurrent
CONFIG += c++11
TEMPLATE = app
TARGET = client

# 源码/头文件（含 comm 子目录）
INCLUDEPATH += Headers \
               Headers/comm
SOURCES += \
    Sources/*.cpp \
    Sources/comm/*.cpp
HEADERS += \
    Headers/*.h \
    Headers/comm/*.h

# UI 表单与资源
FORMS += \
    Forms/*.ui
RESOURCES += \
    Resources/resources.qrc

# 更严格的编译告警（可按需调整）
QMAKE_CXXFLAGS += -Wall -Wextra