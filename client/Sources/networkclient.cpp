#include "networkclient.h"
#include <QDebug>
#include <QJsonArray>

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_connectionTimer(new QTimer(this))
    , m_host(ClientGlobals::DEFAULT_SERVER_HOST)
    , m_port(ClientGlobals::DEFAULT_SERVER_PORT)
    , m_isLoggedIn(false)
{
    m_connectionTimer->setSingleShot(true);
    m_connectionTimer->setInterval(10000); // 10 seconds timeout
    connect(m_connectionTimer, &QTimer::timeout, this, &NetworkClient::onConnectionTimeout);
}

NetworkClient::~NetworkClient()
{
    disconnectFromServer();
}

void NetworkClient::connectToServer(const QString& host, int port)
{
    if (m_socket && m_socket->state() != QAbstractSocket::UnconnectedState) {
        disconnectFromServer();
    }

    m_host = host;
    m_port = port;
    
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &NetworkClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &NetworkClient::onSocketError);

    m_connectionTimer->start();
    m_socket->connectToHost(host, port);
}

void NetworkClient::disconnectFromServer()
{
    m_connectionTimer->stop();
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
    m_isLoggedIn = false;
}

bool NetworkClient::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void NetworkClient::login(const QString& username, const QString& password, UserRole role)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to server");
        return;
    }

    QJsonObject message;
    message["action"] = "login";
    message["username"] = username;
    message["password"] = password;
    message["role"] = ClientGlobals::userRoleToString(role);
    
    sendMessage(message);
}

void NetworkClient::logout()
{
    if (!isConnected()) {
        return;
    }

    QJsonObject message;
    message["action"] = "logout";
    
    sendMessage(message);
    m_isLoggedIn = false;
}

void NetworkClient::createWorkOrder(const QJsonObject& orderData)
{
    if (!m_isLoggedIn) {
        emit errorOccurred("Not logged in");
        return;
    }

    QJsonObject message;
    message["action"] = "create_order";
    message["data"] = orderData;
    
    sendMessage(message);
}

void NetworkClient::updateWorkOrder(int orderId, const QJsonObject& orderData)
{
    if (!m_isLoggedIn) {
        emit errorOccurred("Not logged in");
        return;
    }

    QJsonObject message;
    message["action"] = "update_order";
    message["order_id"] = orderId;
    message["data"] = orderData;
    
    sendMessage(message);
}

void NetworkClient::deleteWorkOrder(int orderId)
{
    if (!m_isLoggedIn) {
        emit errorOccurred("Not logged in");
        return;
    }

    QJsonObject message;
    message["action"] = "delete_order";
    message["order_id"] = orderId;
    
    sendMessage(message);
}

void NetworkClient::listWorkOrders()
{
    if (!m_isLoggedIn) {
        emit errorOccurred("Not logged in");
        return;
    }

    QJsonObject message;
    message["action"] = "list_orders";
    
    sendMessage(message);
}

void NetworkClient::getWorkOrder(int orderId)
{
    if (!m_isLoggedIn) {
        emit errorOccurred("Not logged in");
        return;
    }

    QJsonObject message;
    message["action"] = "get_order";
    message["order_id"] = orderId;
    
    sendMessage(message);
}

void NetworkClient::onConnected()
{
    m_connectionTimer->stop();
    emit connected();
}

void NetworkClient::onDisconnected()
{
    m_isLoggedIn = false;
    emit disconnected();
}

void NetworkClient::onReadyRead()
{
    while (m_socket->canReadLine()) {
        QByteArray data = m_socket->readLine();
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        
        if (error.error != QJsonParseError::NoError) {
            emit errorOccurred("Invalid JSON received: " + error.errorString());
            continue;
        }
        
        processMessage(doc.object());
    }
}

void NetworkClient::onSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    m_connectionTimer->stop();
    emit errorOccurred(m_socket->errorString());
}

void NetworkClient::onConnectionTimeout()
{
    if (m_socket) {
        m_socket->abort();
    }
    emit errorOccurred("Connection timeout");
}

void NetworkClient::sendMessage(const QJsonObject& message)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to server");
        return;
    }

    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(data);
}

void NetworkClient::processMessage(const QJsonObject& message)
{
    QString action = message["action"].toString();
    
    if (action == "login_response") {
        bool success = message["success"].toBool();
        QString msg = message["message"].toString();
        if (success) {
            m_isLoggedIn = true;
            ClientGlobals::sessionToken = message["token"].toString();
        }
        emit loginResult(success, msg);
    }
    else if (action == "order_created") {
        int orderId = message["order_id"].toInt();
        emit workOrderCreated(orderId);
    }
    else if (action == "order_updated") {
        int orderId = message["order_id"].toInt();
        emit workOrderUpdated(orderId);
    }
    else if (action == "order_deleted") {
        int orderId = message["order_id"].toInt();
        emit workOrderDeleted(orderId);
    }
    else if (action == "orders_list") {
        QJsonObject orders = message["orders"].toObject();
        emit workOrdersList(orders);
    }
    else if (action == "order_details") {
        QJsonObject order = message["order"].toObject();
        emit workOrderDetails(order);
    }
    else if (action == "error") {
        QString error = message["message"].toString();
        emit errorOccurred(error);
    }
}