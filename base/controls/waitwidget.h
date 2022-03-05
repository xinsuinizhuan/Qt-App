#ifndef WAITWIDGET_H
#define WAITWIDGET_H

#include "controls_global.h"

#include <QSplashScreen>

namespace Control {

class CONTROLS_EXPORT WaitWidget : public QSplashScreen
{
    Q_OBJECT
public:
    explicit WaitWidget();
    ~WaitWidget();

    void fullProgressBar();

private slots:
    void updateProgressBar();

private:
    void setupUI();

    class WaitWidgetPrivate;
    QScopedPointer<WaitWidgetPrivate> d_ptr;
};

}

#endif // WAITWIDGET_H
