#include <QCoreApplication>
#include "roomhub.h"
#include "udprelay.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    const quint16 tcpPort = 9000;
    RoomHub hub;
    if (!hub.start(tcpPort)) return 1;

    UdpRelay udp;
    if (!udp.start(tcpPort + 1)) return 1;

    return app.exec();
}