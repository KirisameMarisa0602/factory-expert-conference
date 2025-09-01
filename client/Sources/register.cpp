#include "register.h"
#include "ui_register.h"
#include "login.h"
#include <QMessageBox>

Register::Register(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Register)
    , m_selectedRole(UserRole::Unknown)
{
    ui->setupUi(this);
    setupUI();
}

Register::~Register()
{
    delete ui;
}

void Register::setupUI()
{
    // Connect radio button signals
    connect(ui->radioFactory, &QRadioButton::toggled, this, &Register::onRoleChanged);
    connect(ui->radioExpert, &QRadioButton::toggled, this, &Register::onRoleChanged);
    
    // Connect text change signals for validation
    connect(ui->editUser, &QLineEdit::textChanged, this, &Register::validateInput);
    connect(ui->editPwd, &QLineEdit::textChanged, this, &Register::validateInput);
    connect(ui->editPwdConfirm, &QLineEdit::textChanged, this, &Register::validateInput);
    
    validateInput();
}

void Register::on_btnSubmit_clicked()
{
    // Get form data
    QString username = ui->editUser->text();
    QString password = ui->editPwd->text();
    QString confirmPassword = ui->editPwdConfirm->text();
    
    // Basic validation
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "注册错误", "请输入用户名和密码");
        return;
    }
    
    if (password != confirmPassword) {
        QMessageBox::warning(this, "注册错误", "两次输入的密码不一致");
        return;
    }
    
    if (m_selectedRole == UserRole::Unknown) {
        QMessageBox::warning(this, "注册错误", "请选择用户角色");
        return;
    }
    
    // Here would be actual registration logic with NetworkClient
    // For now, just show success and go back to login
    QMessageBox::information(this, "注册成功", 
        QString("用户 %1 注册成功！\n角色: %2")
        .arg(username)
        .arg(ClientGlobals::userRoleToString(m_selectedRole)));
    
    Login *w = new Login();
    w->show();
    close();
}

void Register::on_btnBack_clicked()
{
    Login *w = new Login();
    w->show();
    close();
}

void Register::onRoleChanged()
{
    // Handle role selection change
    if (ui->radioFactory->isChecked()) {
        m_selectedRole = UserRole::Factory;
    } else if (ui->radioExpert->isChecked()) {
        m_selectedRole = UserRole::Expert;
    } else {
        m_selectedRole = UserRole::Unknown;
    }
    
    validateInput();
}

void Register::validateInput()
{
    bool isValid = !ui->editUser->text().isEmpty() && 
                   !ui->editPwd->text().isEmpty() &&
                   !ui->editPwdConfirm->text().isEmpty() &&
                   m_selectedRole != UserRole::Unknown;
    
    ui->btnSubmit->setEnabled(isValid);
}