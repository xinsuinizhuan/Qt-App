#include "tcpserverthread.h"
#include "tcpserver.h"

struct TcpServerThread::TcpServerThreadPrivate
{
    QString ip;
    quint16 port;
};

TcpServerThread::TcpServerThread(quint16 port, const QString &ip, QObject *parent)
    : QThread(parent)
    , d_ptr(new TcpServerThreadPrivate)
{
    d_ptr->ip = ip;
    d_ptr->port = port;
}

TcpServerThread::~TcpServerThread()
{
    if(isRunning()){
        quit();
        wait();
    }
}

void TcpServerThread::run()
{
    QScopedPointer<TcpServer> tcpServer(new TcpServer);
    connect(tcpServer.data(), &TcpServer::errorMessage, this, &TcpServerThread::errorMessage);
    connect(tcpServer.data(), &TcpServer::newClientInfo, this, &TcpServerThread::newClientInfo);
    connect(tcpServer.data(), &TcpServer::disconnectClientInfo, this, &TcpServerThread::disconnectClientInfo);
    connect(tcpServer.data(), &TcpServer::clientMessage, this, &TcpServerThread::clientMessage);
    connect(this, &TcpServerThread::sendMessage, tcpServer.data(), &TcpServer::onSendMessage);

    bool ok = tcpServer->listen(QHostAddress(d_ptr->ip), d_ptr->port);
    emit serverOnLine(ok);
    if(!ok) return;

    exec();

    emit serverOnLine(false);
}
