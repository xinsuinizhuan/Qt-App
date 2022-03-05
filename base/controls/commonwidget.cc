#include "commonwidget.hpp"

#include <QtWidgets>

namespace Control {

class CommonWidget::CommonWidgetPrivate
{
public:
    CommonWidgetPrivate(QWidget *parent)
        : owner(parent)
    {
        iconLabel = new QLabel(owner);
        iconLabel->setObjectName("iconLabel");
        iconLabel->setFixedSize(25, 25);
        titleLabel = new QLabel(qAppName(), owner);
        titleLabel->setObjectName("titleLabel");
        minButton = new QToolButton(owner);
        minButton->setObjectName("minButton");
        maxButton = new QToolButton(owner);
        maxButton->setObjectName("maxButton");
        restoreButton = new QToolButton(owner);
        restoreButton->setObjectName("restoreButton");
        closeButton = new QToolButton(owner);
        closeButton->setObjectName("closeButton");
        centralWidget = new QWidget(owner);
        centralWidget->setObjectName("CommonCentralWidget");
        titleBar = new QWidget(owner);
        layout = new QHBoxLayout(owner);
        layout->setSpacing(0);
        layout->setContentsMargins(shadowPadding, shadowPadding, shadowPadding, shadowPadding);
        sizeGrip = new QSizeGrip(owner);
    }
    QWidget *owner;
    QLabel *iconLabel;
    QLabel *titleLabel;
    QToolButton *minButton;
    QToolButton *maxButton;
    QToolButton *restoreButton;
    QToolButton *closeButton;
    QWidget *titleWidget;
    QWidget *centralWidget;
    QWidget *titleBar;
    QHBoxLayout *layout;
    QSizeGrip *sizeGrip;

    int shadowPadding = 10;

    //记录鼠标位置
    QPoint lastPoint;
};

CommonWidget::CommonWidget(QWidget *parent)
    : QWidget{parent}
    , d_ptr(new CommonWidgetPrivate(this))
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowCloseButtonHint
                   | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI();
    buildConnnect();
    resize(1000, 618);
    setTr();
}

CommonWidget::~CommonWidget() {}

void CommonWidget::setRestoreMaxButtonVisible(bool visible)
{
    if (!visible) {
        d_ptr->maxButton->setVisible(visible);
        d_ptr->restoreButton->setVisible(visible);
        return;
    }
    if (isMaximized()) {
        d_ptr->restoreButton->setVisible(true);
        d_ptr->maxButton->setVisible(false);
    } else {
        d_ptr->restoreButton->setVisible(false);
        d_ptr->maxButton->setVisible(true);
    }
}

void CommonWidget::setMinButtonVisible(bool visible)
{
    d_ptr->minButton->setVisible(visible);
}

void CommonWidget::setTitle(const QString &title)
{
    d_ptr->titleLabel->setText(title);
    d_ptr->titleLabel->setToolTip(title);
    setWindowTitle(title);
}

void CommonWidget::setIcon(const QImage &icon)
{
    if (icon.isNull()) {
        d_ptr->iconLabel->hide();
        return;
    }

    QImage i = icon.scaled(d_ptr->iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    d_ptr->iconLabel->setPixmap(QPixmap::fromImage(i));
}

void CommonWidget::setCentralWidget(QWidget *widget)
{
    QHBoxLayout *layout = new QHBoxLayout(d_ptr->centralWidget);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(widget);
}

void CommonWidget::setTitleBar(QWidget *widget)
{
    QHBoxLayout *layout = new QHBoxLayout(d_ptr->titleBar);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(widget);
}

void CommonWidget::setShadowPadding(int shadowPadding)
{
    d_ptr->shadowPadding = shadowPadding;
}

int CommonWidget::shadowPadding()
{
    return d_ptr->shadowPadding;
}

void CommonWidget::setSizeGripVisible(bool visible)
{
    d_ptr->sizeGrip->setVisible(visible);
}

void CommonWidget::onShowMaximized()
{
    d_ptr->layout->setContentsMargins(QMargins());
    showMaximized();
    d_ptr->maxButton->hide();
    d_ptr->restoreButton->show();
}

void CommonWidget::onShowNormal()
{
    d_ptr->layout->setContentsMargins(d_ptr->shadowPadding,
                                      d_ptr->shadowPadding,
                                      d_ptr->shadowPadding,
                                      d_ptr->shadowPadding);
    showNormal();
    d_ptr->maxButton->show();
    d_ptr->restoreButton->hide();
}

void CommonWidget::mousePressEvent(QMouseEvent *event)
{
    const QMargins margins(d_ptr->layout->contentsMargins());
    const QRect rect(d_ptr->titleWidget->rect().adjusted(margins.left(),
                                                         margins.top(),
                                                         margins.right(),
                                                         margins.bottom()));
    if (rect.contains(event->pos())) {
        d_ptr->lastPoint = event->pos();
    }

    QWidget::mousePressEvent(event);
}

void CommonWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isMaximized() || isFullScreen()) {
    } else if (!d_ptr->lastPoint.isNull()) {
        move(QCursor::pos() - d_ptr->lastPoint);
    }

    QWidget::mouseMoveEvent(event);
}

void CommonWidget::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->lastPoint = QPoint();

    QWidget::mouseReleaseEvent(event);
}

void CommonWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!d_ptr->titleWidget->rect().contains(event->pos())) {
    } else if (d_ptr->maxButton->isVisible()) {
        d_ptr->maxButton->click();
    } else if (d_ptr->restoreButton->isVisible()) {
        d_ptr->restoreButton->click();
    }

    QWidget::mouseDoubleClickEvent(event);
}

void CommonWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange: setTr(); break;
    default: break;
    }
}

void CommonWidget::setupUI()
{
    d_ptr->titleWidget = titleWidget();
    d_ptr->titleWidget->setObjectName("titleWidget");

    QWidget *widget = new QWidget(this);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(0, 0);
    effect->setColor(Qt::gray);
    effect->setBlurRadius(d_ptr->shadowPadding);
    widget->setGraphicsEffect(effect);

    QGridLayout *layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d_ptr->titleWidget, 0, 0);
    layout->addWidget(d_ptr->centralWidget, 1, 0);
    layout->addWidget(d_ptr->sizeGrip, 1, 0, Qt::AlignRight | Qt::AlignBottom);

    d_ptr->layout->addWidget(widget);
}

QWidget *CommonWidget::titleWidget()
{
    if (isMaximized()) {
        d_ptr->maxButton->hide();
        d_ptr->restoreButton->show();
    } else {
        d_ptr->maxButton->show();
        d_ptr->restoreButton->hide();
    }

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(10);
    layout->addWidget(d_ptr->iconLabel);
    layout->addWidget(d_ptr->titleLabel);
    layout->addStretch();
    layout->addWidget(d_ptr->titleBar);
    layout->addWidget(d_ptr->minButton);
    layout->addWidget(d_ptr->maxButton);
    layout->addWidget(d_ptr->restoreButton);
    layout->addWidget(d_ptr->closeButton);

    return widget;
}

void CommonWidget::buildConnnect()
{
    connect(d_ptr->minButton, &QToolButton::clicked, this, &CommonWidget::showMinimized);
    connect(d_ptr->maxButton, &QToolButton::clicked, this, &CommonWidget::onShowMaximized);
    connect(d_ptr->restoreButton, &QToolButton::clicked, this, &CommonWidget::onShowNormal);
    connect(d_ptr->closeButton, &QToolButton::clicked, this, &CommonWidget::aboutToclose);
    connect(this, &CommonWidget::aboutToclose, this, &CommonWidget::close);
    connect(this, &CommonWidget::windowTitleChanged, d_ptr->titleLabel, &QLabel::setText);
    connect(this, &CommonWidget::windowIconChanged, d_ptr->iconLabel, &QLabel::setWindowIcon);
}

void CommonWidget::setTr()
{
    d_ptr->minButton->setToolTip(tr("Minimize"));
    d_ptr->maxButton->setToolTip(tr("Maximize"));
    d_ptr->restoreButton->setToolTip(tr("Restore"));
    d_ptr->closeButton->setToolTip(tr("Close"));
}

} // namespace Control
