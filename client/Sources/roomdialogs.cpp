#include "roomdialogs.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>

// CreateRoomDialog implementation
CreateRoomDialog::CreateRoomDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("创建会议室");
    resize(400, 300);
}

void CreateRoomDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout;
    
    m_nameEdit = new QLineEdit;
    formLayout->addRow("会议室名称:", m_nameEdit);
    
    m_descriptionEdit = new QTextEdit;
    m_descriptionEdit->setMaximumHeight(80);
    formLayout->addRow("描述:", m_descriptionEdit);
    
    m_maxParticipantsCombo = new QComboBox;
    m_maxParticipantsCombo->addItems({"2", "4", "6", "8", "10", "15", "20"});
    m_maxParticipantsCombo->setCurrentText("6");
    formLayout->addRow("最大参与者:", m_maxParticipantsCombo);
    
    m_privacyCombo = new QComboBox;
    m_privacyCombo->addItems({"公开", "私有"});
    formLayout->addRow("会议室类型:", m_privacyCombo);
    
    mainLayout->addLayout(formLayout);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    m_createButton = new QPushButton("创建");
    m_cancelButton = new QPushButton("取消");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_createButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_createButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_nameEdit, &QLineEdit::textChanged, this, &CreateRoomDialog::validateInput);
    
    validateInput();
}

QString CreateRoomDialog::getRoomName() const
{
    return m_nameEdit->text();
}

QString CreateRoomDialog::getRoomDescription() const
{
    return m_descriptionEdit->toPlainText();
}

int CreateRoomDialog::getMaxParticipants() const
{
    return m_maxParticipantsCombo->currentText().toInt();
}

bool CreateRoomDialog::isPrivateRoom() const
{
    return m_privacyCombo->currentText() == "私有";
}

void CreateRoomDialog::validateInput()
{
    bool isValid = !m_nameEdit->text().trimmed().isEmpty();
    m_createButton->setEnabled(isValid);
}

// JoinRoomDialog implementation
JoinRoomDialog::JoinRoomDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("加入会议室");
    resize(500, 400);
}

void JoinRoomDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel("选择要加入的会议室:");
    mainLayout->addWidget(titleLabel);
    
    m_roomsList = new QListWidget;
    mainLayout->addWidget(m_roomsList);
    
    m_roomInfoLabel = new QLabel;
    m_roomInfoLabel->setWordWrap(true);
    m_roomInfoLabel->setMaximumHeight(60);
    mainLayout->addWidget(m_roomInfoLabel);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    m_refreshButton = new QPushButton("刷新");
    m_joinButton = new QPushButton("加入");
    m_cancelButton = new QPushButton("取消");
    
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_joinButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_roomsList, &QListWidget::currentItemChanged, this, &JoinRoomDialog::onRoomSelectionChanged);
    connect(m_refreshButton, &QPushButton::clicked, this, &JoinRoomDialog::onRefreshClicked);
    connect(m_joinButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    m_joinButton->setEnabled(false);
}

QString JoinRoomDialog::getSelectedRoom() const
{
    QListWidgetItem* item = m_roomsList->currentItem();
    return item ? item->text() : QString();
}

void JoinRoomDialog::setAvailableRooms(const QJsonObject& rooms)
{
    m_availableRooms = rooms;
    m_roomsList->clear();
    
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        QJsonObject roomInfo = it.value().toObject();
        QString roomName = it.key();
        int participants = roomInfo["participants"].toInt();
        int maxParticipants = roomInfo["max_participants"].toInt();
        bool isPrivate = roomInfo["private"].toBool();
        
        QString displayText = QString("%1 (%2/%3) %4")
                             .arg(roomName)
                             .arg(participants)
                             .arg(maxParticipants)
                             .arg(isPrivate ? "[私有]" : "[公开]");
        
        m_roomsList->addItem(displayText);
    }
}

void JoinRoomDialog::onRoomSelectionChanged()
{
    QListWidgetItem* item = m_roomsList->currentItem();
    m_joinButton->setEnabled(item != nullptr);
    
    if (item) {
        QString roomName = item->text().split(" ").first();
        if (m_availableRooms.contains(roomName)) {
            QJsonObject roomInfo = m_availableRooms[roomName].toObject();
            QString description = roomInfo["description"].toString();
            m_roomInfoLabel->setText(description);
        }
    } else {
        m_roomInfoLabel->clear();
    }
}

void JoinRoomDialog::onRefreshClicked()
{
    emit refreshRequested();
}

// RoomSettingsDialog implementation
RoomSettingsDialog::RoomSettingsDialog(const QJsonObject& roomInfo, QWidget *parent)
    : QDialog(parent)
    , m_originalSettings(roomInfo)
{
    setupUI();
    populateFields();
    setWindowTitle("会议室设置");
    resize(400, 300);
}

void RoomSettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout;
    
    m_nameEdit = new QLineEdit;
    formLayout->addRow("会议室名称:", m_nameEdit);
    
    m_descriptionEdit = new QTextEdit;
    m_descriptionEdit->setMaximumHeight(80);
    formLayout->addRow("描述:", m_descriptionEdit);
    
    m_maxParticipantsCombo = new QComboBox;
    m_maxParticipantsCombo->addItems({"2", "4", "6", "8", "10", "15", "20"});
    formLayout->addRow("最大参与者:", m_maxParticipantsCombo);
    
    m_privacyCombo = new QComboBox;
    m_privacyCombo->addItems({"公开", "私有"});
    formLayout->addRow("会议室类型:", m_privacyCombo);
    
    mainLayout->addLayout(formLayout);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    m_saveButton = new QPushButton("保存");
    m_cancelButton = new QPushButton("取消");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_saveButton, &QPushButton::clicked, this, &RoomSettingsDialog::onSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void RoomSettingsDialog::populateFields()
{
    m_nameEdit->setText(m_originalSettings["name"].toString());
    m_descriptionEdit->setPlainText(m_originalSettings["description"].toString());
    m_maxParticipantsCombo->setCurrentText(QString::number(m_originalSettings["max_participants"].toInt()));
    m_privacyCombo->setCurrentText(m_originalSettings["private"].toBool() ? "私有" : "公开");
}

QJsonObject RoomSettingsDialog::getRoomSettings() const
{
    QJsonObject settings;
    settings["name"] = m_nameEdit->text();
    settings["description"] = m_descriptionEdit->toPlainText();
    settings["max_participants"] = m_maxParticipantsCombo->currentText().toInt();
    settings["private"] = (m_privacyCombo->currentText() == "私有");
    return settings;
}

void RoomSettingsDialog::onSaveClicked()
{
    accept();
}