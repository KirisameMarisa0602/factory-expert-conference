#ifndef CLIENT_EXPERT_H
#define CLIENT_EXPERT_H

#include <QWidget>
#include "comm/commwidget.h"

namespace Ui {
class ClientExpert;
}

class ClientExpert : public QWidget
{
    Q_OBJECT

public:
    explicit ClientExpert(QWidget *parent = nullptr);
    ~ClientExpert();

private:
    Ui::ClientExpert *ui;
    CommWidget* commWidget_ = nullptr;
};

#endif // CLIENT_EXPERT_H
