QT += core gui widgets network sql multimedia
CONFIG += c++11
TEMPLATE = app
TARGET = client

INCLUDEPATH += Headers \
               Headers/comm

SOURCES += \
    Sources/*.cpp \
    Sources/comm/*.cpp

HEADERS += \
    Headers/*.h \
    Headers/comm/*.h

FORMS += \
    Forms/*.ui

# Conditionally include resources if the qrc exists to avoid build errors
exists(Resources/resources.qrc) {
    RESOURCES += Resources/resources.qrc
}

QMAKE_CXXFLAGS += -Wall -Wextra