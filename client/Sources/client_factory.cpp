#include "client_factory.h"
#include "ui_client_factory.h"
#include "comm/commwidget.h"

ClientFactory::ClientFactory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientFactory)
{
    ui->setupUi(this);

    // 实时通讯模块集成
    commWidget_ = new CommWidget(this);
    ui->verticalLayoutTabRealtime->addWidget(commWidget_);

    // 选中tab时可激活通讯界面
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int idx){
        if (ui->tabWidget->widget(idx) == ui->tabRealtime) {
            commWidget_->mainWindow()->setFocus();
        }
    });
}

ClientFactory::~ClientFactory()
{
    delete ui;
}