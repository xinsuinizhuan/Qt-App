#ifndef DIALOG_HPP
#define DIALOG_HPP

#include "commonwidget.hpp"
#include "controls_global.h"

namespace Control {

class CONTROLS_EXPORT Dialog : public CommonWidget
{
    Q_OBJECT
public:
    enum ExecFlags { Accepted, Rejected, Close };

    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    ExecFlags exec();

signals:
    void accepted();
    void rejected();

public slots:
    void accept();
    void reject();

private slots:
    void onClosed();

private:
    void buildConnect();

    struct DialogPrivate;
    QScopedPointer<DialogPrivate> d_ptr;
};

} // namespace Control

#endif // DIALOG_HPP
