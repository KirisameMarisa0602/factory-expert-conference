#ifndef CLIENT_FACTORY_H
#define CLIENT_FACTORY_H

#include <QWidget>
#include "comm/commwidget.h"

namespace Ui {
class ClientFactory;
}

class ClientFactory : public QWidget
{
    Q_OBJECT

public:
    explicit ClientFactory(QWidget *parent = nullptr);
    ~ClientFactory();

private:
    Ui::ClientFactory *ui;
    CommWidget* commWidget_ = nullptr;
};

#endif // CLIENT_FACTORY_H
