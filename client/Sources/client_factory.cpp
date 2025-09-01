#include "client_factory.h"
#include "ui_client_factory.h"
#include "comm/commwidget.h"
#include "orderinfo.h"
#include "networkclient.h"
#include "client_globals.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QMessageBox>
#include <QLabel>

ClientFactory::ClientFactory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientFactory)
{
    ui->setupUi(this);
    setWindowTitle("工厂端主界面 - " + ClientGlobals::currentUser);

    // 实时通讯模块集成
    commWidget_ = new CommWidget(this);
    ui->verticalLayoutTabRealtime->addWidget(commWidget_);

    // Setup work order management tab
    setupWorkOrderTab();

    // Setup network client
    m_networkClient = new NetworkClient(this);
    connect(m_networkClient, &NetworkClient::workOrderCreated, this, &ClientFactory::onWorkOrderCreated);
    connect(m_networkClient, &NetworkClient::workOrdersList, this, &ClientFactory::onWorkOrdersReceived);
    connect(m_networkClient, &NetworkClient::workOrderDetails, this, &ClientFactory::onWorkOrderDetailsReceived);
    connect(m_networkClient, &NetworkClient::errorOccurred, this, &ClientFactory::onNetworkError);
    
    // Connect to server automatically
    m_networkClient->connectToServer();

    // 选中tab时可激活通讯界面
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int idx){
        if (ui->tabWidget->widget(idx) == ui->tabRealtime) {
            commWidget_->mainWindow()->setFocus();
        } else if (ui->tabWidget->widget(idx) == ui->tabWorkorders) {
            // Load work orders when switching to work order tab
            loadWorkOrders();
        }
    });
}

ClientFactory::~ClientFactory()
{
    delete ui;
}

void ClientFactory::setupWorkOrderTab()
{
    QHBoxLayout* mainLayout = new QHBoxLayout;
    
    // Left side - Work order list
    QVBoxLayout* leftLayout = new QVBoxLayout;
    
    QLabel* listLabel = new QLabel("工单列表:");
    leftLayout->addWidget(listLabel);
    
    m_workOrderList = new QListWidget;
    leftLayout->addWidget(m_workOrderList);
    
    // Buttons for work order actions
    QVBoxLayout* buttonLayout = new QVBoxLayout;
    m_createButton = new QPushButton("创建工单");
    m_editButton = new QPushButton("编辑工单");
    m_deleteButton = new QPushButton("删除工单");
    m_refreshButton = new QPushButton("刷新");
    
    m_editButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    
    buttonLayout->addWidget(m_createButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addStretch();
    
    leftLayout->addLayout(buttonLayout);
    
    // Right side - Work order details
    m_orderInfo = new OrderInfo;
    
    // Create splitter
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    
    QWidget* leftWidget = new QWidget;
    leftWidget->setLayout(leftLayout);
    leftWidget->setMaximumWidth(300);
    
    splitter->addWidget(leftWidget);
    splitter->addWidget(m_orderInfo);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
    ui->verticalLayoutTabWorkorders->addLayout(mainLayout);
    
    // Connect signals
    connect(m_workOrderList, &QListWidget::currentItemChanged, this, &ClientFactory::onWorkOrderSelected);
    connect(m_createButton, &QPushButton::clicked, this, &ClientFactory::onCreateWorkOrder);
    connect(m_editButton, &QPushButton::clicked, this, &ClientFactory::onEditWorkOrder);
    connect(m_deleteButton, &QPushButton::clicked, this, &ClientFactory::onDeleteWorkOrder);
    connect(m_refreshButton, &QPushButton::clicked, this, &ClientFactory::loadWorkOrders);
    connect(m_orderInfo, &OrderInfo::orderSaved, this, &ClientFactory::onOrderSaved);
    connect(m_orderInfo, &OrderInfo::orderCancelled, this, &ClientFactory::onOrderCancelled);
}

void ClientFactory::loadWorkOrders()
{
    if (m_networkClient && m_networkClient->isConnected()) {
        m_networkClient->listWorkOrders();
    } else {
        // Mock data for demonstration
        m_workOrderList->clear();
        m_workOrderList->addItem("工单#001 - 生产线故障");
        m_workOrderList->addItem("工单#002 - 设备异常响声");
        m_workOrderList->addItem("工单#003 - 温度控制异常");
    }
}

void ClientFactory::onWorkOrderSelected()
{
    QListWidgetItem* item = m_workOrderList->currentItem();
    bool hasSelection = (item != nullptr);
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
    
    if (item) {
        // Extract order ID from item text (assumes format "工单#XXX - Title")
        QString text = item->text();
        QStringList parts = text.split(" - ");
        if (parts.size() >= 2) {
            QString orderIdStr = parts[0].mid(3); // Remove "工单#" prefix
            bool ok;
            int orderId = orderIdStr.toInt(&ok);
            if (ok) {
                if (m_networkClient && m_networkClient->isConnected()) {
                    m_networkClient->getWorkOrder(orderId);
                } else {
                    // Mock data for demonstration
                    WorkOrder mockOrder;
                    mockOrder.id = orderId;
                    mockOrder.title = parts[1];
                    mockOrder.description = "这是一个模拟的工单描述。";
                    mockOrder.priority = "高";
                    mockOrder.status = WorkOrderStatus::Created;
                    mockOrder.factoryLocation = "生产车间A";
                    mockOrder.createdAt = QDateTime::currentDateTime().addDays(-1);
                    mockOrder.updatedAt = QDateTime::currentDateTime();
                    
                    m_orderInfo->setWorkOrder(mockOrder);
                    m_orderInfo->setReadOnly(true);
                }
            }
        }
    } else {
        m_orderInfo->clearForm();
    }
}

void ClientFactory::onCreateWorkOrder()
{
    m_orderInfo->clearForm();
    m_orderInfo->setReadOnly(false);
    m_isCreatingNew = true;
    
    // Pre-fill some factory information
    WorkOrder newOrder;
    newOrder.id = 0; // Will be assigned by server
    newOrder.factoryLocation = "默认车间";
    newOrder.createdAt = QDateTime::currentDateTime();
    newOrder.updatedAt = QDateTime::currentDateTime();
    newOrder.status = WorkOrderStatus::Created;
    
    m_orderInfo->setWorkOrder(newOrder);
}

void ClientFactory::onEditWorkOrder()
{
    if (m_workOrderList->currentItem()) {
        m_orderInfo->setReadOnly(false);
        m_isCreatingNew = false;
    }
}

void ClientFactory::onDeleteWorkOrder()
{
    QListWidgetItem* item = m_workOrderList->currentItem();
    if (!item) return;
    
    int ret = QMessageBox::question(this, "确认删除", "确定要删除这个工单吗？");
    if (ret == QMessageBox::Yes) {
        QString text = item->text();
        QStringList parts = text.split(" - ");
        if (parts.size() >= 2) {
            QString orderIdStr = parts[0].mid(3); // Remove "工单#" prefix
            bool ok;
            int orderId = orderIdStr.toInt(&ok);
            if (ok) {
                if (m_networkClient && m_networkClient->isConnected()) {
                    m_networkClient->deleteWorkOrder(orderId);
                } else {
                    QMessageBox::information(this, "成功", "工单已删除！");
                    delete item;
                    m_orderInfo->clearForm();
                }
            }
        }
    }
}

void ClientFactory::onOrderSaved(const WorkOrder& order)
{
    if (m_isCreatingNew) {
        if (m_networkClient && m_networkClient->isConnected()) {
            m_networkClient->createWorkOrder(order.toJson());
        } else {
            QMessageBox::information(this, "成功", "工单已创建！");
            loadWorkOrders(); // Refresh list
        }
    } else {
        if (m_networkClient && m_networkClient->isConnected()) {
            m_networkClient->updateWorkOrder(order.id, order.toJson());
        } else {
            QMessageBox::information(this, "成功", "工单已更新！");
        }
    }
    
    m_orderInfo->setReadOnly(true);
    m_isCreatingNew = false;
}

void ClientFactory::onOrderCancelled()
{
    m_orderInfo->setReadOnly(true);
    m_isCreatingNew = false;
}

void ClientFactory::onWorkOrderCreated(int orderId)
{
    QMessageBox::information(this, "成功", QString("工单 #%1 已创建！").arg(orderId));
    loadWorkOrders(); // Refresh list
    m_orderInfo->setReadOnly(true);
    m_isCreatingNew = false;
}

void ClientFactory::onWorkOrdersReceived(const QJsonObject& orders)
{
    m_workOrderList->clear();
    for (auto it = orders.begin(); it != orders.end(); ++it) {
        QJsonObject orderObj = it.value().toObject();
        QString title = orderObj["title"].toString();
        QString orderText = QString("工单#%1 - %2").arg(it.key()).arg(title);
        m_workOrderList->addItem(orderText);
    }
}

void ClientFactory::onWorkOrderDetailsReceived(const QJsonObject& orderJson)
{
    WorkOrder order;
    order.fromJson(orderJson);
    m_orderInfo->setWorkOrder(order);
}

void ClientFactory::onNetworkError(const QString& error)
{
    QMessageBox::warning(this, "网络错误", error);
}