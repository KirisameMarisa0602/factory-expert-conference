#include "login.h"
#include "ui_login.h"
#include "register.h"
#include "client_factory.h"
#include "client_expert.h"
#include "client_globals.h"
#include "networkclient.h"
#include <QMessageBox>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("工厂专家会议系统 - 登录");
}

Login::~Login()
{
    delete ui;
}

void Login::on_btnLogin_clicked()
{
    QString user = ui->editUser->text();
    QString pwd = ui->editPwd->text();

    if (user.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "登录错误", "请输入用户名和密码");
        return;
    }

    // Determine user role based on username prefix or use button selection
    UserRole role = UserRole::Unknown;
    if (user.startsWith("expert") || user.startsWith("专家")) {
        role = UserRole::Expert;
    } else if (user.startsWith("factory") || user.startsWith("工厂")) {
        role = UserRole::Factory;
    }

    // For demo purposes, if role is still unknown, check the UI buttons
    if (role == UserRole::Unknown) {
        // If we had radio buttons in login UI, we'd check them here
        // For now, default to factory if username doesn't indicate role
        role = UserRole::Factory;
    }

    // Store user info globally
    ClientGlobals::currentUser = user;
    ClientGlobals::currentUserRole = role;

    // Open the appropriate main window based on role
    if (role == UserRole::Expert) {
        auto *w = new ClientExpert;
        w->show();
    } else {
        auto *w = new ClientFactory;
        w->show();
    }
    close();
}

void Login::on_btnRegister_clicked()
{
    Register *r = new Register;
    r->show();
    close();
}