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

# 引入共享协议（common.pri 内已将 protocol.h/.cpp 纳入）
# 注意：common 目录位于本工程下的 server/common
COMMON_DIR = $$PWD/common
include($$COMMON_DIR/common.pri)
