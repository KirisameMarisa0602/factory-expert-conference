#ifndef CLIENT_EXPERT_H
#define CLIENT_EXPERT_H

#include <QWidget>
#include <QJsonObject>
#include "comm/commwidget.h"

class QListWidget;
class QPushButton;
class OrderInfo;
class NetworkClient;
struct WorkOrder;

namespace Ui {
class ClientExpert;
}

class ClientExpert : public QWidget
{
    Q_OBJECT

public:
    explicit ClientExpert(QWidget *parent = nullptr);
    ~ClientExpert();

private slots:
    void loadWorkOrders();
    void onWorkOrderSelected();
    void onAcceptWorkOrder();
    void onOrderUpdated(const WorkOrder& order);
    void onWorkOrdersReceived(const QJsonObject& orders);
    void onWorkOrderDetailsReceived(const QJsonObject& orderJson);
    void onNetworkError(const QString& error);

private:
    void setupWorkOrderTab();

    Ui::ClientExpert *ui;
    CommWidget* commWidget_ = nullptr;
    
    // Work order management
    QListWidget* m_workOrderList = nullptr;
    QPushButton* m_refreshButton = nullptr;
    QPushButton* m_acceptButton = nullptr;
    OrderInfo* m_orderInfo = nullptr;
    NetworkClient* m_networkClient = nullptr;
};

#endif // CLIENT_EXPERT_H
