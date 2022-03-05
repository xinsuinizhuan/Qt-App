#ifndef LOGOUTORDELETE_H
#define LOGOUTORDELETE_H

#include <QWidget>

namespace AccountSystem {

class AccountQuery;
class CurrentLoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CurrentLoginWidget(AccountQuery *, QWidget *parent = nullptr);
    ~CurrentLoginWidget();

    void setAccount(const QString &username, const QString& password);

    QString password() const;

signals:
    void changePassword();
    void deleteAccount();
    void logout();

private slots:
    void onDeleteAccount();

private:
    void setupUI();

    class CurrentLoginWidgetPrivate;
    QScopedPointer<CurrentLoginWidgetPrivate> d;
};

}

#endif // LOGOUTORDELETE_H
