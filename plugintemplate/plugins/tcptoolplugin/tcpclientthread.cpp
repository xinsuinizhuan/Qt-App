#include "tcpclientthread.h"
#include "tcpclient.h"

#include <QTcpSocket>

struct TcpClientThread::TcpClientThreadPrivate
{
    QString ip;
    quint16 port;
};

TcpClientThread::TcpClientThread(const QString &ip, quint16 port, QObject *parent)
    : QThread(parent)
    , d_ptr(new TcpClientThreadPrivate)
{
    d_ptr->ip = ip;
    d_ptr->port = port;
}

TcpClientThread::~TcpClientThread()
{
    if(isRunning()){
        quit();
        wait();
    }
    qDebug() << "~TcpClientThread";
}

void TcpClientThread::run()
{
    QScopedPointer<TcpClient> tcpClient(new TcpClient(d_ptr->ip, d_ptr->port));
    connect(tcpClient.data(), &TcpClient::clientOnLine, this, &TcpClientThread::clientOnLine);
    connect(tcpClient.data(), &TcpClient::errorMessage, this, &TcpClientThread::errorMessage);
    connect(tcpClient.data(), &TcpClient::serverMessage, this, &TcpClientThread::serverMessage);
    connect(this, &TcpClientThread::sendMessage, tcpClient.data(), &TcpClient::onWrite);
    connect(this, &TcpClientThread::reconnect, tcpClient.data(), &TcpClient::oConnectToServer);
    tcpClient->oConnectToServer();
    exec();

    emit clientOnLine(false);
}
