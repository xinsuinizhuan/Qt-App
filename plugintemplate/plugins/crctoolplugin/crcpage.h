#ifndef CRCPAGE_H
#define CRCPAGE_H

#include <core/mpages.h>

using namespace Core;

class CRCPage : public MPages
{
    Q_OBJECT
public:
    explicit CRCPage(QObject *parent = nullptr);

private:
    void init();
};

#endif // CRCPAGE_H
