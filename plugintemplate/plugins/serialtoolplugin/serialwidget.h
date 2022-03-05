#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include <QWidget>

class SerialWidget : public QWidget
{
    Q_OBJECT
public:   
    enum MessageType {
        Send,
        Recv,
        SuccessInfo,
        ErrorInfo
    };

    explicit SerialWidget(QWidget *parent = nullptr);
    ~SerialWidget();

private slots:
    void onSearchPort();
    void onSendData();
    void onParamChanged(const QString&);

    void onOpenOrCloseSerial(bool);

    void onSerialOnline(bool);
    void onAppendError(const QString&);
    void onSerialRecvMessage(const QByteArray &bytes);

    void onAutoSend(bool);
    void onSave();

private:
    void setupUI();
    void initWindow();
    void setWindowParam();
    void buildConnect();
    void setSerialParam();

    void appendDisplay(MessageType, const QString&);
    void setSendCount(int);
    void setRecvCount(int);

    void loadSetting();
    void saveSetting();

    class SerialWidgetPrivate;
    QScopedPointer<SerialWidgetPrivate> d_ptr;
};

#endif // SERIALWIDGET_H
