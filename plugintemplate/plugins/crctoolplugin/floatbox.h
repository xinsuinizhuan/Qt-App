#ifndef FLOATBOX_H
#define FLOATBOX_H

#include <QGroupBox>

class FloatBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit FloatBox(QWidget *parent = nullptr);
    ~FloatBox();

private slots:
    void onFloat2Hex();
    void onHex2Float();

private:
    void setupUI();

    class FloatBoxPrivate;
    QScopedPointer<FloatBoxPrivate> d_ptr;
};

#endif // FLOATBOX_H
