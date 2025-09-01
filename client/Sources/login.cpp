#include "login.h"
#include "ui_login.h"
#include "regist.h"
#include "client_factory.h"
#include "client_expert.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btnLogin_clicked()
{
    // 示例：判断用户类型，打开不同主界面
    QString user = ui->editUser->text();
    QString pwd  = ui->editPwd->text();

    // 这里应有真实鉴权逻辑。省略。
    if(user.startsWith("expert")) {
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
    Regist *r = new Regist;
    r->show();
    close();
}