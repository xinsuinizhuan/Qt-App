#ifndef CRCWIDGET_H
#define CRCWIDGET_H

#include <QWidget>

class CRCWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CRCWidget(QWidget *parent = nullptr);
    ~CRCWidget();

private slots:
    void onModelChanged(int);
    void onCalculate();

private:
    void setupUI();
    void init();
    void buildConnect();

    class CRCWidgetPrivate;
    QScopedPointer<CRCWidgetPrivate> d_ptr;
};

#endif // CRCWIDGET_H
