#ifndef ROOMDIALOGS_H
#define ROOMDIALOGS_H

#include <QDialog>
#include <QWidget>
#include <QJsonObject>

class QLineEdit;
class QTextEdit;
class QListWidget;
class QPushButton;
class QLabel;
class QComboBox;

class CreateRoomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateRoomDialog(QWidget *parent = nullptr);
    
    QString getRoomName() const;
    QString getRoomDescription() const;
    int getMaxParticipants() const;
    bool isPrivateRoom() const;

private slots:
    void validateInput();

private:
    void setupUI();
    
    QLineEdit* m_nameEdit;
    QTextEdit* m_descriptionEdit;
    QComboBox* m_maxParticipantsCombo;
    QComboBox* m_privacyCombo;
    QPushButton* m_createButton;
    QPushButton* m_cancelButton;
};

class JoinRoomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinRoomDialog(QWidget *parent = nullptr);
    
    QString getSelectedRoom() const;
    void setAvailableRooms(const QJsonObject& rooms);

private slots:
    void onRoomSelectionChanged();
    void onRefreshClicked();

signals:
    void refreshRequested();

private:
    void setupUI();
    
    QListWidget* m_roomsList;
    QLabel* m_roomInfoLabel;
    QPushButton* m_joinButton;
    QPushButton* m_refreshButton;
    QPushButton* m_cancelButton;
    
    QJsonObject m_availableRooms;
};

class RoomSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoomSettingsDialog(const QJsonObject& roomInfo, QWidget *parent = nullptr);
    
    QJsonObject getRoomSettings() const;

private slots:
    void onSaveClicked();

private:
    void setupUI();
    void populateFields();
    
    QLineEdit* m_nameEdit;
    QTextEdit* m_descriptionEdit;
    QComboBox* m_maxParticipantsCombo;
    QComboBox* m_privacyCombo;
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;
    
    QJsonObject m_originalSettings;
};

#endif // ROOMDIALOGS_H