#ifndef TCPPAGE_H
#define TCPPAGE_H

#include <core/mpages.h>

using namespace Core;

class TcpPage : public MPages
{
    Q_OBJECT
public:
    explicit TcpPage(QObject *parent = nullptr);

private:
    void init();
};

#endif // TCPPAGE_H
