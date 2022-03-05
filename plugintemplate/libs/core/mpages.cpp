#include "mpages.h"

#include <QtWidgets>

namespace Core {

class MPages::MPagesPrivate
{
public:
    MPagesPrivate(QObject *parent)
        : owner(parent){
        button = new QPushButton;
        button->setProperty("class", "GroupItemButton");
    }
    QObject *owner;
    QPushButton *button;
    QWidget *widget = nullptr;
};

MPages::MPages(QObject *parent)
    : QObject(parent)
    , d_ptr(new MPagesPrivate(this))
{
}

MPages::~MPages()
{

}

QPushButton *MPages::button() const
{
    return d_ptr->button;
}

QWidget *MPages::widget() const
{
    return d_ptr->widget;
}

void MPages::setWidget(QWidget *widget)
{
    d_ptr->widget = widget;
}

void MPages::setButtonName(const QString &text)
{
    d_ptr->button->setText(text);
}

void MPages::setButtonGroup(MPages::Group group)
{
    d_ptr->button->setProperty("Group", group);
}

void MPages::setButtonObejctName(const QString &objectname)
{
    d_ptr->button->setObjectName(objectname);
}

}
