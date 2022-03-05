#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

public slots:
    void onSendMessage(const QByteArray&, const QString &clientInfo = QString(""));

signals:
    void errorMessage(const QString&);
    void newClientInfo(const QString&);
    void disconnectClientInfo(const QString&);
    void clientMessage(const QString&, const QByteArray&);

private slots:
    void onError();
    void onNewConnect();
    void onClientError(QAbstractSocket::SocketError);
    void onClientDisconnect();
    void onClientReadyRead();

private:
    struct TcpServerPrivate;
    QScopedPointer<TcpServerPrivate> d_ptr;
};

#endif // TCPSERVER_H
