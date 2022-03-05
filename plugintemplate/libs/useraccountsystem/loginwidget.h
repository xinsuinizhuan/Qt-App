#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include "useraccountsystem_global.h"

#include <QWidget>

namespace AccountSystem {

class AccountQuery;
class USERACCOUNTSYSTEM_EXPORT LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(AccountQuery *, QWidget *parent = nullptr);
    ~LoginWidget();

    void setNameList(const QStringList &usernameList);

    QString username() const;
    QString password() const;
    QStringList usernameList() const;
    bool autoLogin();

    void clear();

signals:
    void registered();
    void complete();

private slots:
    void onLogin();

private:
    void setupUI();
    void buildConnect();

    class LoginWidgetPrivate;
    QScopedPointer<LoginWidgetPrivate> d;
};

}

#endif // LOGINWIDGET_H
