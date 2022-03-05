#include "systemconversionbox.h"

#include <QtWidgets>

class SystemConversionBox::SystemConversionBoxPrivate
{
public:
    SystemConversionBoxPrivate(QWidget *parent)
        : owner(parent){
        decimalEdit = new QLineEdit(owner);
        binaryEdit = new QLineEdit(owner);
        octalEdit = new QLineEdit(owner);
        hexEdit = new QLineEdit(owner);

        binaryEdit->setReadOnly(true);
        octalEdit->setReadOnly(true);
    }
    QWidget *owner;
    QLineEdit *decimalEdit;
    QLineEdit *binaryEdit;
    QLineEdit *octalEdit;
    QLineEdit *hexEdit;
};

SystemConversionBox::SystemConversionBox(QWidget *parent)
    : QGroupBox(parent)
    , d_ptr(new SystemConversionBoxPrivate(this))
{
    setupUI();
    buildConnect();
}

SystemConversionBox::~SystemConversionBox()
{
}

void SystemConversionBox::onDecimalChanged(const QString &text)
{
    QString dec = text.trimmed();
    if(dec.isEmpty()) return;

    long long data = dec.toLongLong();
    d_ptr->binaryEdit->blockSignals(true);
    d_ptr->octalEdit->blockSignals(true);
    d_ptr->hexEdit->blockSignals(true);
    d_ptr->binaryEdit->setText(QString::number(data, 2).toUpper());
    d_ptr->octalEdit->setText(QString::number(data, 8).toUpper());
    d_ptr->hexEdit->setText(QString::number(data, 16).toUpper());
    d_ptr->binaryEdit->blockSignals(false);
    d_ptr->octalEdit->blockSignals(false);
    d_ptr->hexEdit->blockSignals(false);
}

void SystemConversionBox::onHexChanged(const QString &text)
{
    QString hex = text.trimmed();
    if(hex.isEmpty()) return;

    long long data = hex.toLocal8Bit().toHex().toLongLong();
    d_ptr->decimalEdit->blockSignals(true);
    d_ptr->binaryEdit->blockSignals(true);
    d_ptr->octalEdit->blockSignals(true);
    d_ptr->decimalEdit->setText(QString::number(data));
    d_ptr->binaryEdit->setText(QString::number(data, 2).toUpper());
    d_ptr->octalEdit->setText(QString::number(data, 8).toUpper());
    d_ptr->decimalEdit->blockSignals(false);
    d_ptr->binaryEdit->blockSignals(false);
    d_ptr->octalEdit->blockSignals(false);
}

void SystemConversionBox::setupUI()
{
    setTitle(tr("System Conversion"));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Decimal: "), this), 0, 0, 1, 1);
    layout->addWidget(d_ptr->decimalEdit, 0, 1, 1, 1);
    layout->addWidget(new QLabel(tr("Binary: "), this), 1, 0, 1, 1);
    layout->addWidget(d_ptr->binaryEdit, 1, 1, 1, 1);
    layout->addWidget(new QLabel(tr("Octal: "), this), 2, 0, 1, 1);
    layout->addWidget(d_ptr->octalEdit, 2, 1, 1, 1);
    layout->addWidget(new QLabel(tr("Octal: "), this),3, 0, 1, 1);
    layout->addWidget(d_ptr->hexEdit, 3, 1, 1, 1);

    //    QFormLayout *layout = new QFormLayout(this);
    //    layout->addRow(tr("Decimal: "), d->decimalEdit);
    //    layout->addRow(tr("Binary: "), d->binaryEdit);
    //    layout->addRow(tr("Octal: "), d->octalEdit);
    //    layout->addRow(tr("Hex: "), d->hexEdit);
}

void SystemConversionBox::buildConnect()
{
    connect(d_ptr->decimalEdit,
            &QLineEdit::textChanged,
            this,
            &SystemConversionBox::onDecimalChanged);
    connect(d_ptr->hexEdit, &QLineEdit::textChanged, this, &SystemConversionBox::onHexChanged);
}
