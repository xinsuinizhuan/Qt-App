#include "configwidget.h"

#include <utils/languageconfig.hpp>
#include <utils/utils.h>

#include <QtWidgets>

class ConfigWidget::ConfigWidgetPrivate
{
public:
    ConfigWidgetPrivate(QWidget *parent)
        : owner(parent)
    {
        languageBox = new QComboBox(owner);
    }
    QWidget *owner;
    QComboBox *languageBox;
};

ConfigWidget::ConfigWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ConfigWidgetPrivate(this))
{
    setupUI();
    buildConnect();
    initWindow();
    setWindowParam();
}

ConfigWidget::~ConfigWidget() {}

void ConfigWidget::onReloadLanguage(int)
{
    Utils::LanguageConfig::instance()->loadLanguage(
        Utils::LanguageConfig::Language(d_ptr->languageBox->currentData().toInt()));
}

void ConfigWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange: setTr(); break;
    default: break;
    }
}

void ConfigWidget::setupUI()
{
    QFormLayout *fromLayout = new QFormLayout(this);
    fromLayout->addRow(tr("Language(Requires Restart): "), d_ptr->languageBox);
}

void ConfigWidget::buildConnect()
{
    connect(d_ptr->languageBox,
            &QComboBox::currentIndexChanged,
            this,
            &ConfigWidget::onReloadLanguage);
}

void ConfigWidget::initWindow()
{
    d_ptr->languageBox->addItem(tr("Chinese"), Utils::LanguageConfig::Language::Chinese);
    d_ptr->languageBox->addItem(tr("English"), Utils::LanguageConfig::Language::English);
}

void ConfigWidget::setWindowParam()
{
    d_ptr->languageBox->setCurrentIndex(Utils::LanguageConfig::instance()->currentLanguage());
}

void ConfigWidget::setTr()
{
    d_ptr->languageBox->setItemText(0, tr("Chinese"));
    d_ptr->languageBox->setItemText(1, tr("English"));
}
