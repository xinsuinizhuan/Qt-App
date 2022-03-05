#include "messbox.h"

#include <QtWidgets>

namespace Control {

class MessBox::MessBoxPrivate
{
public:
    MessBoxPrivate(CommonWidget *parent)
        : owner(parent)
    {
        iconLabel = new QLabel(owner);
        messageLabel = new QLabel(owner);
        messageLabel->setObjectName("MessageLabel");
        messageLabel->setWordWrap(true);
        closeButton = new QPushButton(QObject::tr("Close"), owner);
        closeButton->setObjectName("BlueButton");
        yesButton = new QPushButton(QObject::tr("Yes"), owner);
        yesButton->setObjectName("BlueButton");
        noButton = new QPushButton(QObject::tr("No"), owner);
        noButton->setObjectName("GrayButton");
    }
    CommonWidget *owner;
    QLabel *iconLabel;
    QLabel *messageLabel;
    QPushButton *yesButton;
    QPushButton *noButton;
    QPushButton *closeButton;
};

MessBox::MessBox(QWidget *parent)
    : Dialog(parent)
    , d_ptr(new MessBoxPrivate(this))
{
    setMinButtonVisible(false);
    setRestoreMaxButtonVisible(false);
    setupUI();
    buildConnect();
    resize(400, 250);
}

MessBox::~MessBox() {}

Dialog::ExecFlags MessBox::Info(QWidget *parent, const QString &msg, const MessButton button)
{
    MessBox messBox(parent);
    messBox.setIconLabelObjectName("InfoLabel");
    messBox.setMessage(msg);
    switch (button) {
    case YesAndNoButton: messBox.setYesAndNoButtonVisible(true); break;
    case CloseButton: messBox.setCloseButtonVisible(true); break;
    default: break;
    }
    return messBox.exec();
}

Dialog::ExecFlags MessBox::Warning(QWidget *parent, const QString &msg, const MessButton button)
{
    MessBox messBox(parent);
    messBox.setIconLabelObjectName("WarningLabel");
    messBox.setMessage(msg);
    switch (button) {
    case YesAndNoButton: messBox.setYesAndNoButtonVisible(true); break;
    case CloseButton: messBox.setCloseButtonVisible(true); break;
    default: break;
    }
    return messBox.exec();
}

void MessBox::setMessage(const QString &msg)
{
    d_ptr->messageLabel->setText(msg);
}

void MessBox::setIconLabelObjectName(const QString &objectName)
{
    d_ptr->iconLabel->setObjectName(objectName);
}

void MessBox::setYesAndNoButtonVisible(bool state)
{
    d_ptr->yesButton->setVisible(state);
    d_ptr->noButton->setVisible(state);
}

void MessBox::setCloseButtonVisible(bool state)
{
    d_ptr->closeButton->setVisible(state);
}

void MessBox::setupUI()
{
    QHBoxLayout *mLayout = new QHBoxLayout;
    mLayout->setSpacing(20);
    mLayout->addSpacerItem(new QSpacerItem(20, 10, QSizePolicy::Maximum, QSizePolicy::Expanding));
    mLayout->addWidget(d_ptr->iconLabel);
    mLayout->addWidget(d_ptr->messageLabel);
    mLayout->addSpacerItem(new QSpacerItem(20, 10, QSizePolicy::Maximum, QSizePolicy::Expanding));

    QWidget *btnWidget = new QWidget(this);
    btnWidget->setObjectName("MessBtnWidget");
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
    btnLayout->setContentsMargins(10, 5, 20, 5);
    btnLayout->setSpacing(5);
    btnLayout->addStretch();
    btnLayout->addWidget(d_ptr->yesButton);
    btnLayout->addWidget(d_ptr->noButton);
    btnLayout->addWidget(d_ptr->closeButton);

    d_ptr->yesButton->hide();
    d_ptr->noButton->hide();
    d_ptr->closeButton->hide();

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(mLayout);
    layout->addWidget(btnWidget);

    setCentralWidget(widget);
}

void MessBox::buildConnect()
{
    connect(d_ptr->closeButton, &QPushButton::clicked, this, &MessBox::rejected);
    connect(d_ptr->yesButton, &QPushButton::clicked, this, &MessBox::accept);
    connect(d_ptr->noButton, &QPushButton::clicked, this, &MessBox::rejected);
}

} // namespace Control
