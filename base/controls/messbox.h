#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "controls_global.h"
#include "dialog.hpp"

namespace Control {

class CONTROLS_EXPORT MessBox : public Dialog
{
    Q_OBJECT
public:
    enum MessButton { YESButton, NOButton, CloseButton, YesAndNoButton };

    explicit MessBox(QWidget *parent = nullptr);
    ~MessBox();

    static Dialog::ExecFlags Info(QWidget *parent,
                                  const QString &msg,
                                  const MessButton button = CloseButton);
    static Dialog::ExecFlags Warning(QWidget *parent, const QString &msg, const MessButton);

    void setMessage(const QString &);
    void setIconLabelObjectName(const QString &);

    void setYesAndNoButtonVisible(bool);
    void setCloseButtonVisible(bool);

private:
    void setupUI();
    void buildConnect();

    class MessBoxPrivate;
    QScopedPointer<MessBoxPrivate> d_ptr;
};

}

#endif // MESSAGEBOX_H
