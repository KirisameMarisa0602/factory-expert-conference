#include "client_expert.h"
#include "ui_client_expert.h"
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

ClientExpert::ClientExpert(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientExpert)
{
    ui->setupUi(this);
    setWindowTitle("专家端主界面 - " + ClientGlobals::currentUser);

    // 实时通讯模块集成
    commWidget_ = new CommWidget(this);
    ui->verticalLayoutTabRealtime->addWidget(commWidget_);

    // Setup work order management tab
    setupWorkOrderTab();

    // Setup network client
    m_networkClient = new NetworkClient(this);
    connect(m_networkClient, &NetworkClient::workOrdersList, this, &ClientExpert::onWorkOrdersReceived);
    connect(m_networkClient, &NetworkClient::workOrderDetails, this, &ClientExpert::onWorkOrderDetailsReceived);
    connect(m_networkClient, &NetworkClient::errorOccurred, this, &ClientExpert::onNetworkError);
    
    // Connect to server automatically
    m_networkClient->connectToServer();

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int idx){
        if (ui->tabWidget->widget(idx) == ui->tabRealtime) {
            commWidget_->mainWindow()->setFocus();
        } else if (ui->tabWidget->widget(idx) == ui->tabWorkorders) {
            // Load work orders when switching to work order tab
            loadWorkOrders();
        }
    });
}

ClientExpert::~ClientExpert()
{
    delete ui;
}

void ClientExpert::setupWorkOrderTab()
{
    QHBoxLayout* mainLayout = new QHBoxLayout;
    
    // Left side - Work order list
    QVBoxLayout* leftLayout = new QVBoxLayout;
    
    QLabel* listLabel = new QLabel("待处理工单:");
    leftLayout->addWidget(listLabel);
    
    m_workOrderList = new QListWidget;
    leftLayout->addWidget(m_workOrderList);
    
    // Buttons for work order actions
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    m_refreshButton = new QPushButton("刷新");
    m_acceptButton = new QPushButton("接受工单");
    m_acceptButton->setEnabled(false);
    
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addWidget(m_acceptButton);
    
    leftLayout->addLayout(buttonLayout);
    
    // Right side - Work order details
    m_orderInfo = new OrderInfo;
    m_orderInfo->setReadOnly(true); // Experts can view but may have limited editing
    
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
    connect(m_workOrderList, &QListWidget::currentItemChanged, this, &ClientExpert::onWorkOrderSelected);
    connect(m_refreshButton, &QPushButton::clicked, this, &ClientExpert::loadWorkOrders);
    connect(m_acceptButton, &QPushButton::clicked, this, &ClientExpert::onAcceptWorkOrder);
    connect(m_orderInfo, &OrderInfo::orderSaved, this, &ClientExpert::onOrderUpdated);
}

void ClientExpert::loadWorkOrders()
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

void ClientExpert::onWorkOrderSelected()
{
    QListWidgetItem* item = m_workOrderList->currentItem();
    m_acceptButton->setEnabled(item != nullptr);
    
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
                }
            }
        }
    } else {
        m_orderInfo->clearForm();
    }
}

void ClientExpert::onAcceptWorkOrder()
{
    QListWidgetItem* item = m_workOrderList->currentItem();
    if (!item) return;
    
    int ret = QMessageBox::question(this, "确认", "确定要接受这个工单吗？");
    if (ret == QMessageBox::Yes) {
        // Update work order to assign current expert
        WorkOrder order = m_orderInfo->getWorkOrder();
        order.assignedExpert = ClientGlobals::currentUser;
        order.status = WorkOrderStatus::InProgress;
        order.updatedAt = QDateTime::currentDateTime();
        
        if (m_networkClient && m_networkClient->isConnected()) {
            m_networkClient->updateWorkOrder(order.id, order.toJson());
        } else {
            QMessageBox::information(this, "成功", "工单已接受！");
            m_orderInfo->setWorkOrder(order);
        }
    }
}

void ClientExpert::onOrderUpdated(const WorkOrder& order)
{
    if (m_networkClient && m_networkClient->isConnected()) {
        m_networkClient->updateWorkOrder(order.id, order.toJson());
    } else {
        QMessageBox::information(this, "成功", "工单已更新！");
    }
}

void ClientExpert::onWorkOrdersReceived(const QJsonObject& orders)
{
    m_workOrderList->clear();
    for (auto it = orders.begin(); it != orders.end(); ++it) {
        QJsonObject orderObj = it.value().toObject();
        QString title = orderObj["title"].toString();
        QString orderText = QString("工单#%1 - %2").arg(it.key()).arg(title);
        m_workOrderList->addItem(orderText);
    }
}

void ClientExpert::onWorkOrderDetailsReceived(const QJsonObject& orderJson)
{
    WorkOrder order;
    order.fromJson(orderJson);
    m_orderInfo->setWorkOrder(order);
}

void ClientExpert::onNetworkError(const QString& error)
{
    QMessageBox::warning(this, "网络错误", error);
}
