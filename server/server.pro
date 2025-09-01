QT += core network
CONFIG += c++11 console
TEMPLATE = app
TARGET = server

SOURCES += \
    src/main.cpp \
    src/roomhub.cpp \
    src/udprelay.cpp

HEADERS += \
    src/roomhub.h \
    src/udprelay.h

# 引入共享协议（此 pri 内部已经把 protocol.h/.cpp 加入 HEADERS/SOURCES）
COMMON_DIR = $$PWD/../common
include($$COMMON_DIR/common.pri)

isEmpty(COMMON_DIR) {
    error("common.pri requires COMMON_DIR to be set by includer")
}

INCLUDEPATH += $$PWD/common
HEADERS += common/protocol.h
SOURCES += common/protocol.cpp
