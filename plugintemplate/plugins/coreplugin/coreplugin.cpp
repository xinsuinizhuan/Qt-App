#include "coreplugin.h"
#include "mainwindow.h"

#include <QApplication>

CorePlugin::CorePlugin() {}

CorePlugin::~CorePlugin() {}

bool CorePlugin::initialize(const QStringList&, QString*)
{
    m_mainWindowPtr.reset(new MainWindow);
    return true;
}

void CorePlugin::extensionsInitialized()
{
    m_mainWindowPtr->extensionsInitialized();
}

QObject *CorePlugin::remoteCommand(const QStringList &,
                                   const QString &,
                                   const QStringList &)
{
    qApp->setApplicationDisplayName(QObject::tr("AppPlugin"));
    m_mainWindowPtr->show();
    return nullptr;
}
