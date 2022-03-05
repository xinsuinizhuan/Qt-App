#include "waitwidget.h"

#include <utils/utils.h>

#include <QtWidgets>

namespace Control {

class WaitWidget::WaitWidgetPrivate
{
public:
    WaitWidgetPrivate(QWidget *parent)
        : owner(parent){
        processBar = new QProgressBar(owner);
        processBar->setTextVisible(false);
        processBar->setRange(0, 100);
    }
    QWidget *owner;
    QProgressBar *processBar;
    QTimer timer;
};

WaitWidget::WaitWidget()
    : d_ptr(new WaitWidgetPrivate(this))
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint
                   | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    //setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI();
    connect(&d_ptr->timer, &QTimer::timeout, this, &WaitWidget::updateProgressBar);
    d_ptr->timer.start(100);
}

WaitWidget::~WaitWidget()
{

}

void WaitWidget::fullProgressBar()
{
    int value = d_ptr->processBar->value();
    if(value < 100)
        d_ptr->processBar->setValue(100);
}

void WaitWidget::updateProgressBar()
{
    int value = d_ptr->processBar->value() + 5;
    if(value >= 95) {
        d_ptr->timer.stop();
        return;
    }
    d_ptr->processBar->setValue(value);
}

void WaitWidget::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d_ptr->processBar);
    resize(600, 5);
    Utils::windowCenter(this);
}

}
