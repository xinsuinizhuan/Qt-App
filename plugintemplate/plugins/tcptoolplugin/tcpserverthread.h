#ifndef TCPSERVERTHREAD_H
#define TCPSERVERTHREAD_H

#include <QThread>

class TcpServerThread : public QThread
{
    Q_OBJECT
public:
    explicit TcpServerThread(quint16, const QString &ip = "127.0.0.1", QObject *parent = nullptr);
    ~TcpServerThread();

signals:
    void serverOnLine(bool);
    void errorMessage(const QString&);
    void newClientInfo(const QString&);
    void disconnectClientInfo(const QString&);
    void clientMessage(const QString&, const QByteArray&);
    void sendMessage(const QByteArray&, const QString &clientInfo = QString(""));

protected:
    void run();

private:
    struct TcpServerThreadPrivate;
    QScopedPointer<TcpServerThreadPrivate> d_ptr;
};

#endif // TCPSERVERTHREAD_H
