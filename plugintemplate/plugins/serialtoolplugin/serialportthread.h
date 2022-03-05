#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H

#include <QThread>

#include "serialparam.h"

class SerialPortThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialPortThread(const SerialParam &, QObject *parent = nullptr);
    ~SerialPortThread() override;

signals:
    void serialOnLine(bool);
    void errorMessage(const QString&);
    void serialMessage(const QByteArray&);
    void sendMessage(const QByteArray&);

    void paramChanged(const SerialParam&);

protected:
    void run() override;

private:
    struct SerialPortThreadPrivate;
    QScopedPointer<SerialPortThreadPrivate> d_ptr;
};

#endif // SERIALPORTTHREAD_H
