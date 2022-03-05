#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

namespace AccountSystem {

class AccountQuery;
class RegisterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterWidget(AccountQuery *, QWidget *parent = nullptr);
    ~RegisterWidget();

    QString username() const;
    QString password() const;

signals:
    void registered();
    void cancel();

private slots:
    void onRegister();

private:
    void setupUI();

    class RegisterWidgetPrivate;
    QScopedPointer<RegisterWidgetPrivate> d_ptr;
};

}

#endif // REGISTERWIDGET_H
