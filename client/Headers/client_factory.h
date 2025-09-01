#ifndef CLIENT_FACTORY_H
#define CLIENT_FACTORY_H

#include <QWidget>
#include <QJsonObject>
#include "comm/commwidget.h"

class QListWidget;
class QPushButton;
class OrderInfo;
class NetworkClient;
struct WorkOrder;

namespace Ui {
class ClientFactory;
}

class ClientFactory : public QWidget
{
    Q_OBJECT

public:
    explicit ClientFactory(QWidget *parent = nullptr);
    ~ClientFactory();

private slots:
    void loadWorkOrders();
    void onWorkOrderSelected();
    void onCreateWorkOrder();
    void onEditWorkOrder();
    void onDeleteWorkOrder();
    void onOrderSaved(const WorkOrder& order);
    void onOrderCancelled();
    void onWorkOrderCreated(int orderId);
    void onWorkOrdersReceived(const QJsonObject& orders);
    void onWorkOrderDetailsReceived(const QJsonObject& orderJson);
    void onNetworkError(const QString& error);

private:
    void setupWorkOrderTab();

    Ui::ClientFactory *ui;
    CommWidget* commWidget_ = nullptr;
    
    // Work order management
    QListWidget* m_workOrderList = nullptr;
    QPushButton* m_createButton = nullptr;
    QPushButton* m_editButton = nullptr;
    QPushButton* m_deleteButton = nullptr;
    QPushButton* m_refreshButton = nullptr;
    OrderInfo* m_orderInfo = nullptr;
    NetworkClient* m_networkClient = nullptr;
    bool m_isCreatingNew = false;
};

#endif // CLIENT_FACTORY_H
