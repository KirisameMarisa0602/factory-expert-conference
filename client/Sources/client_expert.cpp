#include "client_expert.h"
#include "ui_client_expert.h"
#include "comm/commwidget.h"

ClientExpert::ClientExpert(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientExpert)
{
    ui->setupUi(this);

    // 实时通讯模块集成（同理，可以和ClientFactory一致）
    commWidget_ = new CommWidget(this);
    ui->verticalLayoutTabRealtime->addWidget(commWidget_);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int idx){
        if (ui->tabWidget->widget(idx) == ui->tabRealtime) {
            commWidget_->mainWindow()->setFocus();
        }
    });
}

ClientExpert::~ClientExpert()
{
    delete ui;
}
