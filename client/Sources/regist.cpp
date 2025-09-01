#include "regist.h"
#include "ui_regist.h"
#include "login.h"

Regist::Regist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Regist)
{
    ui->setupUi(this);
}

Regist::~Regist()
{
    delete ui;
}

void Regist::on_btnSubmit_clicked()
{
    // 这里填写注册逻辑
    // ...
    Login *w = new Login();
    w->show();
    close();
}

void Regist::on_btnBack_clicked()
{
    Login *w = new Login();
    w->show();
    close();
}