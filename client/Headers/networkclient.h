#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include "client_globals.h"

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

    // Connection management
    void connectToServer(const QString& host = ClientGlobals::DEFAULT_SERVER_HOST, 
                        int port = ClientGlobals::DEFAULT_SERVER_PORT);
    void disconnectFromServer();
    bool isConnected() const;

    // Authentication
    void login(const QString& username, const QString& password, UserRole role);
    void logout();

    // Work order management
    void createWorkOrder(const QJsonObject& orderData);
    void updateWorkOrder(int orderId, const QJsonObject& orderData);
    void deleteWorkOrder(int orderId);
    void listWorkOrders();
    void getWorkOrder(int orderId);

signals:
    void connected();
    void disconnected();
    void loginResult(bool success, const QString& message);
    void workOrderCreated(int orderId);
    void workOrderUpdated(int orderId);
    void workOrderDeleted(int orderId);
    void workOrdersList(const QJsonObject& orders);
    void workOrderDetails(const QJsonObject& order);
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);
    void onConnectionTimeout();

private:
    void sendMessage(const QJsonObject& message);
    void processMessage(const QJsonObject& message);

    QTcpSocket* m_socket;
    QTimer* m_connectionTimer;
    QString m_host;
    int m_port;
    bool m_isLoggedIn;
};

#endif // NETWORKCLIENT_H