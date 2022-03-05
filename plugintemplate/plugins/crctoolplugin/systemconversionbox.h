#ifndef SYSTEMCONVERSIONBOX_H
#define SYSTEMCONVERSIONBOX_H

#include <QGroupBox>

class SystemConversionBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit SystemConversionBox(QWidget *parent = nullptr);
    ~SystemConversionBox();

private slots:
    void onDecimalChanged(const QString&);
    void onHexChanged(const QString&);

private:
    void setupUI();
    void buildConnect();

    class SystemConversionBoxPrivate;
    QScopedPointer<SystemConversionBoxPrivate> d_ptr;
};

#endif // SYSTEMCONVERSIONBOX_H
