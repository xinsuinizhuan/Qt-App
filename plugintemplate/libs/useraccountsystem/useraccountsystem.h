#ifndef USERACCOUNTSYSTEM_H
#define USERACCOUNTSYSTEM_H

#include "useraccountsystem_global.h"

#include <controls/dialog.hpp>

using namespace Control;

namespace AccountSystem{

class AccountQuery;
class USERACCOUNTSYSTEM_EXPORT UserAccountSystem : public Dialog
{
    Q_OBJECT
public:
    explicit UserAccountSystem(QWidget *parent = nullptr);
    ~UserAccountSystem();

    bool loginState();

signals:
    void login(bool);

private slots:
    void onLogin();
    void onRegist();
    void onChangedPassword();
    void onCurrentLogin();
    void onComplete();
    void onDeleteAccount();
    void onLogout();
    void onModifyPassword();
    void onRegister();

private:
    void setupUI();
    void buildConnect();
    bool checkCurrentAccount();
    void onLoginState(bool state);

    void loadSetting();
    void saveSetting();

    class UserAccountSystemPrivate;
    QScopedPointer<UserAccountSystemPrivate> d_ptr;
};

}

#endif // USERACCOUNTSYSTEM_H
