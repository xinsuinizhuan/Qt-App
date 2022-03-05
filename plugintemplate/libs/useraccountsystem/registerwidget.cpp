#include "registerwidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"

#include <controls/accountcontrols.h>

#include <QtWidgets>

using namespace Control;

namespace AccountSystem {

class RegisterWidget::RegisterWidgetPrivate
{
public:
    RegisterWidgetPrivate(QWidget *parent)
        : owner(parent){
        avatarWidget = new AvatarWidget(owner);
        usernameEdit = new QLineEdit(owner);
        usernameEdit->setObjectName("usernameEdit");
        usernameEdit->setPlaceholderText(QObject::tr("Please enter user name."));
        passwordEdit = new PasswordLineEdit(owner);
        passwordEdit->setPlaceholderText(QObject::tr("Please enter the password."));
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
        passwdAgainEdit = new PasswordLineEdit(owner);
        passwdAgainEdit->setPlaceholderText(QObject::tr("Please enter the password again."));
    }
    QWidget *owner;
    AvatarWidget *avatarWidget;
    QLineEdit *usernameEdit;
    PasswordLineEdit *passwordEdit;
    QLabel *promptLabel;
    PasswordLineEdit *passwdAgainEdit;
    AccountQuery *accountQuery = nullptr;
};

RegisterWidget::RegisterWidget(AccountQuery *accountQuery, QWidget *parent)
    : QWidget(parent)
    , d_ptr(new RegisterWidgetPrivate(this))
{
    d_ptr->accountQuery = accountQuery;
    setupUI();
}

RegisterWidget::~RegisterWidget()
{
}

QString RegisterWidget::username() const
{
    return d_ptr->usernameEdit->text();
}

QString RegisterWidget::password() const
{
    return d_ptr->passwordEdit->text();
}

void RegisterWidget::onRegister()
{
    d_ptr->promptLabel->clear();
    QString username = d_ptr->usernameEdit->text().trimmed();
    if(username.isEmpty()){
        d_ptr->promptLabel->setText(tr("Please enter username!"));
        d_ptr->usernameEdit->setFocus();
        return;
    }
    QString password = d_ptr->passwordEdit->text().trimmed();
    if(password.isEmpty()){
        d_ptr->promptLabel->setText(tr("Please enter password!"));
        d_ptr->passwordEdit->setFocus();
        return;
    }
    QString passwdAgain = d_ptr->passwdAgainEdit->text().trimmed();
    if(passwdAgain.isEmpty()){
        d_ptr->promptLabel->setText(tr("Please enter password again!"));
        d_ptr->passwdAgainEdit->setFocus();
        return;
    }
    if(password != passwdAgain){
        d_ptr->promptLabel->setText(tr("Two passwords are different, "
                                       "please enter password again!"));
        d_ptr->passwdAgainEdit->setFocus();
        return;
    }
    if (d_ptr->accountQuery->contains(username)) {
        d_ptr->promptLabel->setText(tr("An account with the same name already exists, "
                                       "please modify the account name!"));
        d_ptr->usernameEdit->setFocus();
        return;
    }
    if (d_ptr->accountQuery->addAccount(username, password)) {
        emit registered();
        return;
    }
    d_ptr->promptLabel->setText(tr("Registration failed, please try again later!"));
}

void RegisterWidget::setupUI()
{
    QPushButton *registerButton = new QPushButton(tr("Registered"), this);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    registerButton->setObjectName("BlueButton");
    cancelButton->setObjectName("GrayButton");
    connect(registerButton, &QPushButton::clicked, this, &RegisterWidget::onRegister);
    connect(cancelButton, &QPushButton::clicked, this, &RegisterWidget::cancel);
    connect(d_ptr->passwdAgainEdit,
            &PasswordLineEdit::returnPressed,
            this,
            &RegisterWidget::onRegister);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(cancelButton);
    btnLayout->addWidget(registerButton);

    setObjectName("WhiteWidget");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(d_ptr->avatarWidget);
    layout->addWidget(d_ptr->usernameEdit);
    layout->addWidget(d_ptr->passwordEdit);
    layout->addWidget(d_ptr->promptLabel);
    layout->addWidget(d_ptr->passwdAgainEdit);
    layout->addLayout(btnLayout);
}

}
