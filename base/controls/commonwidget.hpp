#ifndef COMMONWIDGET_HPP
#define COMMONWIDGET_HPP

#include <QWidget>

#include "controls_global.h"

namespace Control {

class CONTROLS_EXPORT CommonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommonWidget(QWidget *parent = nullptr);
    ~CommonWidget() override;

    void setRestoreMaxButtonVisible(bool);
    void setMinButtonVisible(bool);

    void setTitle(const QString &);
    void setIcon(const QImage &);

    void setCentralWidget(QWidget *);
    void setTitleBar(QWidget *);

    void setShadowPadding(int shadowPadding = 10);
    int shadowPadding();

    void setSizeGripVisible(bool visible);

signals:
    void aboutToclose();

private slots:
    void onShowMaximized();
    void onShowNormal();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *e) override;

private:
    void setupUI();
    QWidget *titleWidget();
    void buildConnnect();
    void setTr();

    class CommonWidgetPrivate;
    QScopedPointer<CommonWidgetPrivate> d_ptr;
};

} // namespace Control

#endif // COMMONWIDGET_HPP
