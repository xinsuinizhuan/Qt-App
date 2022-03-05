#include "useraccountsystem.h"
#include "accountquery.h"
#include "loginwidget.h"
#include "currentloginwidget.h"
#include "changepasswdwidget.h"
#include "registerwidget.h"

#include <extensionsystem/pluginmanager.h>

#include <QApplication>
#include <QSettings>
#include <QStackedWidget>

namespace AccountSystem {

struct Account{
    QString username;
    QString password;
};

class UserAccountSystem::UserAccountSystemPrivate
{
public:
    UserAccountSystemPrivate(QWidget *parent)
        : owner(parent){
        accountQuery = new AccountQuery(owner);

        changePasswdWidget = new ChangePasswdWidget(accountQuery, owner);
        currentLoginWidget = new CurrentLoginWidget(accountQuery, owner);
        loginWidget = new LoginWidget(accountQuery, owner);
        registerWidget = new RegisterWidget(accountQuery, owner);

        stackedWidget = new QStackedWidget(owner);
        stackedWidget->setObjectName("WhiteWidget");
        stackedWidget->addWidget(changePasswdWidget);
        stackedWidget->addWidget(currentLoginWidget);
        stackedWidget->addWidget(loginWidget);
        stackedWidget->addWidget(registerWidget);
    }
    QWidget* owner;
    Account currentAccount;
    bool autoLogin = false;
    AccountQuery *accountQuery;

    ChangePasswdWidget *changePasswdWidget;
    CurrentLoginWidget *currentLoginWidget;
    LoginWidget *loginWidget;
    RegisterWidget *registerWidget;
    QStackedWidget *stackedWidget;

    bool login = false;
};

UserAccountSystem::UserAccountSystem(QWidget *parent)
    : Dialog(parent)
    , d_ptr(new UserAccountSystemPrivate(this))
{
    setRestoreMaxButtonVisible(false);
    loadSetting();
    if(d_ptr->autoLogin && checkCurrentAccount())
        onLoginState(true);
    setupUI();
    buildConnect();
    resize(300, 480);
}

UserAccountSystem::~UserAccountSystem()
{
    saveSetting();
}

bool UserAccountSystem::loginState()
{
    return d_ptr->login;
}

void UserAccountSystem::onLogin()
{
    setTitle(tr("Login Widget"));
    onLoginState(false);
    d_ptr->loginWidget->clear();
    d_ptr->loginWidget->setNameList(d_ptr->accountQuery->userNameList());
    d_ptr->stackedWidget->setCurrentWidget(d_ptr->loginWidget);
}

void UserAccountSystem::onRegist()
{
    setTitle(tr("Register Widget"));
    d_ptr->stackedWidget->setCurrentWidget(d_ptr->registerWidget);
}

void UserAccountSystem::onChangedPassword()
{
    setTitle(tr("ChangePasswd Widget"));
    d_ptr->changePasswdWidget->clear();
    d_ptr->changePasswdWidget->setAccount(d_ptr->currentAccount.username,
                                      d_ptr->currentAccount.password);
    d_ptr->stackedWidget->setCurrentWidget(d_ptr->changePasswdWidget);
}

void UserAccountSystem::onCurrentLogin()
{
    setTitle(tr("Current Login Widget"));
    onLoginState(true);
    d_ptr->currentLoginWidget->setAccount(d_ptr->currentAccount.username,
                                      d_ptr->currentAccount.password);
    d_ptr->stackedWidget->setCurrentWidget(d_ptr->currentLoginWidget);
}

void UserAccountSystem::onComplete()
{
    d_ptr->currentAccount.username = d_ptr->loginWidget->username();
    d_ptr->currentAccount.password = d_ptr->loginWidget->password();
    d_ptr->autoLogin = d_ptr->loginWidget->autoLogin();
    onCurrentLogin();
}

void UserAccountSystem::onDeleteAccount()
{
    d_ptr->currentAccount.username.clear();
    d_ptr->currentAccount.password.clear();
    onLogin();
}

void UserAccountSystem::onLogout()
{
    d_ptr->currentAccount.username.clear();
    d_ptr->currentAccount.password.clear();
    onLogin();
}

void UserAccountSystem::onModifyPassword()
{
    d_ptr->currentAccount.password = d_ptr->changePasswdWidget->password();
    onCurrentLogin();
}

void UserAccountSystem::onRegister()
{
    d_ptr->currentAccount.username = d_ptr->registerWidget->username();
    d_ptr->currentAccount.password = d_ptr->registerWidget->password();
    onCurrentLogin();
}

void UserAccountSystem::onLoginState(bool state)
{
    d_ptr->login = state;
    emit login(state);
}

void UserAccountSystem::setupUI()
{
    setCentralWidget(d_ptr->stackedWidget);

    if(!checkCurrentAccount()){
        onLogin();
    }else{
        onCurrentLogin();
    }
}

void UserAccountSystem::buildConnect()
{
    connect(d_ptr->loginWidget, &LoginWidget::registered, this, &UserAccountSystem::onRegist);
    connect(d_ptr->loginWidget, &LoginWidget::complete, this, &UserAccountSystem::onComplete);

    connect(d_ptr->currentLoginWidget, &CurrentLoginWidget::changePassword, this, &UserAccountSystem::onChangedPassword);
    connect(d_ptr->currentLoginWidget, &CurrentLoginWidget::deleteAccount, this, &UserAccountSystem::onDeleteAccount);
    connect(d_ptr->currentLoginWidget, &CurrentLoginWidget::logout, this, &UserAccountSystem::onLogout);

    connect(d_ptr->changePasswdWidget, &ChangePasswdWidget::modifyPassword, this, &UserAccountSystem::onModifyPassword);
    connect(d_ptr->changePasswdWidget, &ChangePasswdWidget::cancel, this, &UserAccountSystem::onCurrentLogin);

    connect(d_ptr->registerWidget, &RegisterWidget::registered, this, &UserAccountSystem::onRegister);
    connect(d_ptr->registerWidget, &RegisterWidget::cancel, this, &UserAccountSystem::onCurrentLogin);
}

bool UserAccountSystem::checkCurrentAccount()
{
    if(d_ptr->currentAccount.username.isEmpty()
        || d_ptr->currentAccount.password.isEmpty())
        return false;
    if(d_ptr->accountQuery->checkAccount(d_ptr->currentAccount.username,
                                      d_ptr->currentAccount.password))
        return true;
    return false;
}

void UserAccountSystem::loadSetting()
{
    QSettings *setting = ExtensionSystem::PluginManager::settings();
    if(!setting)
        return;

    setting->beginGroup("accout_config");
    d_ptr->autoLogin = setting->value("AutoLogin", false).toBool();
    if(d_ptr->autoLogin){
        d_ptr->currentAccount.username = setting->value("AutoLoginUsername").toString();
        d_ptr->currentAccount.password = setting->value("AutoLoginPassword").toString();
    }
    setting->endGroup();
}

void UserAccountSystem::saveSetting()
{
    QSettings *setting = ExtensionSystem::PluginManager::settings();
    if(!setting)
        return;

    setting->beginGroup("accout_config");
    setting->setValue("AutoLogin", d_ptr->autoLogin);
    if(d_ptr->autoLogin){
        setting->setValue("AutoLoginUsername", d_ptr->currentAccount.username);
        setting->setValue("AutoLoginPassword", d_ptr->currentAccount.password);
    }
    setting->endGroup();
}

}
