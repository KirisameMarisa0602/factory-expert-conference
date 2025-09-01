#include "orderinfo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QJsonObject>
#include <QDebug>

QJsonObject WorkOrder::toJson() const
{
    QJsonObject json;
    json["id"] = id;
    json["title"] = title;
    json["description"] = description;
    json["priority"] = priority;
    json["status"] = static_cast<int>(status);
    json["assigned_expert"] = assignedExpert;
    json["factory_location"] = factoryLocation;
    json["created_at"] = createdAt.toString(Qt::ISODate);
    json["updated_at"] = updatedAt.toString(Qt::ISODate);
    json["due_date"] = dueDate.toString(Qt::ISODate);
    return json;
}

void WorkOrder::fromJson(const QJsonObject& json)
{
    id = json["id"].toInt();
    title = json["title"].toString();
    description = json["description"].toString();
    priority = json["priority"].toString();
    status = static_cast<WorkOrderStatus>(json["status"].toInt());
    assignedExpert = json["assigned_expert"].toString();
    factoryLocation = json["factory_location"].toString();
    createdAt = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    updatedAt = QDateTime::fromString(json["updated_at"].toString(), Qt::ISODate);
    dueDate = QDateTime::fromString(json["due_date"].toString(), Qt::ISODate);
}

OrderInfo::OrderInfo(QWidget *parent)
    : QWidget(parent)
    , m_isReadOnly(false)
{
    setupUI();
    connectSignals();
}

OrderInfo::~OrderInfo()
{
}

void OrderInfo::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Form layout for order details
    QFormLayout* formLayout = new QFormLayout;
    
    m_titleEdit = new QLineEdit;
    formLayout->addRow("工单标题:", m_titleEdit);
    
    m_descriptionEdit = new QTextEdit;
    m_descriptionEdit->setMaximumHeight(100);
    formLayout->addRow("问题描述:", m_descriptionEdit);
    
    m_priorityCombo = new QComboBox;
    m_priorityCombo->addItems({"低", "中", "高"});
    formLayout->addRow("优先级:", m_priorityCombo);
    
    m_statusCombo = new QComboBox;
    m_statusCombo->addItems({"已创建", "进行中", "已完成", "已取消"});
    formLayout->addRow("状态:", m_statusCombo);
    
    m_expertEdit = new QLineEdit;
    formLayout->addRow("指派专家:", m_expertEdit);
    
    m_locationEdit = new QLineEdit;
    formLayout->addRow("工厂位置:", m_locationEdit);
    
    m_createdLabel = new QLabel;
    formLayout->addRow("创建时间:", m_createdLabel);
    
    m_updatedLabel = new QLabel;
    formLayout->addRow("更新时间:", m_updatedLabel);
    
    mainLayout->addLayout(formLayout);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    m_saveButton = new QPushButton("保存");
    m_cancelButton = new QPushButton("取消");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void OrderInfo::connectSignals()
{
    connect(m_saveButton, &QPushButton::clicked, this, &OrderInfo::onSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &OrderInfo::onCancelClicked);
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OrderInfo::onStatusChanged);
}

void OrderInfo::setWorkOrder(const WorkOrder& order)
{
    m_currentOrder = order;
    
    m_titleEdit->setText(order.title);
    m_descriptionEdit->setPlainText(order.description);
    
    // Set priority combo
    int priorityIndex = 0;
    if (order.priority == "中") priorityIndex = 1;
    else if (order.priority == "高") priorityIndex = 2;
    m_priorityCombo->setCurrentIndex(priorityIndex);
    
    // Set status combo
    m_statusCombo->setCurrentIndex(static_cast<int>(order.status));
    
    m_expertEdit->setText(order.assignedExpert);
    m_locationEdit->setText(order.factoryLocation);
    m_createdLabel->setText(order.createdAt.toString("yyyy-MM-dd hh:mm:ss"));
    m_updatedLabel->setText(order.updatedAt.toString("yyyy-MM-dd hh:mm:ss"));
}

WorkOrder OrderInfo::getWorkOrder() const
{
    WorkOrder order = m_currentOrder;
    
    order.title = m_titleEdit->text();
    order.description = m_descriptionEdit->toPlainText();
    
    QStringList priorities = {"低", "中", "高"};
    order.priority = priorities[m_priorityCombo->currentIndex()];
    
    order.status = static_cast<WorkOrderStatus>(m_statusCombo->currentIndex());
    order.assignedExpert = m_expertEdit->text();
    order.factoryLocation = m_locationEdit->text();
    order.updatedAt = QDateTime::currentDateTime();
    
    return order;
}

void OrderInfo::setReadOnly(bool readOnly)
{
    m_isReadOnly = readOnly;
    
    m_titleEdit->setReadOnly(readOnly);
    m_descriptionEdit->setReadOnly(readOnly);
    m_priorityCombo->setEnabled(!readOnly);
    m_statusCombo->setEnabled(!readOnly);
    m_expertEdit->setReadOnly(readOnly);
    m_locationEdit->setReadOnly(readOnly);
    m_saveButton->setVisible(!readOnly);
}

void OrderInfo::clearForm()
{
    m_currentOrder = WorkOrder();
    m_titleEdit->clear();
    m_descriptionEdit->clear();
    m_priorityCombo->setCurrentIndex(0);
    m_statusCombo->setCurrentIndex(0);
    m_expertEdit->clear();
    m_locationEdit->clear();
    m_createdLabel->clear();
    m_updatedLabel->clear();
}

void OrderInfo::onSaveClicked()
{
    WorkOrder order = getWorkOrder();
    emit orderSaved(order);
}

void OrderInfo::onCancelClicked()
{
    emit orderCancelled();
}

void OrderInfo::onStatusChanged()
{
    updateStatusDisplay();
}

void OrderInfo::updateStatusDisplay()
{
    // Could add visual indicators for different statuses
    WorkOrderStatus status = static_cast<WorkOrderStatus>(m_statusCombo->currentIndex());
    
    QString styleSheet;
    switch (status) {
    case WorkOrderStatus::Created:
        styleSheet = "color: blue;";
        break;
    case WorkOrderStatus::InProgress:
        styleSheet = "color: orange;";
        break;
    case WorkOrderStatus::Completed:
        styleSheet = "color: green;";
        break;
    case WorkOrderStatus::Cancelled:
        styleSheet = "color: red;";
        break;
    }
    
    m_statusCombo->setStyleSheet(styleSheet);
}