#ifndef ORDERINFO_H
#define ORDERINFO_H

#include <QWidget>
#include <QDateTime>
#include <QJsonObject>
#include "client_globals.h"

class QLabel;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;

namespace Ui {
class OrderInfo;
}

struct WorkOrder {
    int id;
    QString title;
    QString description;
    QString priority;  // High, Medium, Low
    WorkOrderStatus status;
    QString assignedExpert;
    QString factoryLocation;
    QDateTime createdAt;
    QDateTime updatedAt;
    QDateTime dueDate;
    
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
};

class OrderInfo : public QWidget
{
    Q_OBJECT

public:
    explicit OrderInfo(QWidget *parent = nullptr);
    ~OrderInfo();

    void setWorkOrder(const WorkOrder& order);
    WorkOrder getWorkOrder() const;
    void setReadOnly(bool readOnly);
    void clearForm();

signals:
    void orderSaved(const WorkOrder& order);
    void orderCancelled();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onStatusChanged();

private:
    void setupUI();
    void connectSignals();
    void updateStatusDisplay();

    QLineEdit* m_titleEdit;
    QTextEdit* m_descriptionEdit;
    QComboBox* m_priorityCombo;
    QComboBox* m_statusCombo;
    QLineEdit* m_expertEdit;
    QLineEdit* m_locationEdit;
    QLabel* m_createdLabel;
    QLabel* m_updatedLabel;
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;
    
    WorkOrder m_currentOrder;
    bool m_isReadOnly;
};

#endif // ORDERINFO_H