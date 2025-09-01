#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QRadioButton>
#include "client_globals.h"

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

private slots:
    void on_btnSubmit_clicked();
    void on_btnBack_clicked();
    void onRoleChanged();

private:
    void setupUI();
    void validateInput();

    Ui::Register *ui;
    UserRole m_selectedRole;
};

#endif // REGISTER_H