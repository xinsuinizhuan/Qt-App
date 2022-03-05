#ifndef TCPCLIENTTHREAD_H
#define TCPCLIENTTHREAD_H

#include <QThread>

class TcpClientThread : public QThread
{
    Q_OBJECT
public:
    explicit TcpClientThread(const QString &ip, quint16 port, QObject *parent = nullptr);
    ~TcpClientThread() override;

    void setAutoReconnect(bool);

signals:
    void clientOnLine(bool);
    void errorMessage(const QString&);
    void serverMessage(const QByteArray&);
    void sendMessage(const QByteArray&);
    void reconnect();

protected:
    void run() override;

private:
    struct TcpClientThreadPrivate;
    QScopedPointer<TcpClientThreadPrivate> d_ptr;
};

#endif // TCPCLIENTTHREAD_H
