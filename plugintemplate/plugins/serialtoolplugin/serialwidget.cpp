#include "serialwidget.h"
#include "serialparam.h"
#include "serialportthread.h"

#include <controls/messbox.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/utils.h>

#include <QSerialPortInfo>
#include <QtWidgets>

using namespace Control;

inline QString formatHex(const QByteArray &msg)
{
    QString temp;
    QString hex = QString::fromLocal8Bit(msg.toHex().toUpper());
    for (int i = 0; i < hex.length(); i = i + 2)
        temp += hex.mid(i, 2) + " "; //两个字符+空格（例子：7e ）
    return temp;
}

struct SerialExtraParam
{
    bool hex = false;
    int sendTime = 1000;
    QString sendData;
};

struct SerialWidgetParam
{
    SerialParam serialParam;
    SerialExtraParam serialExtraParam;
};

class SerialWidget::SerialWidgetPrivate
{
public:
    SerialWidgetPrivate(QWidget *parent)
        : owner(parent)
    {
        dataView = new QTextEdit(owner);
        dataView->document()->setMaximumBlockCount(1000);
        dataView->setReadOnly(true);

        sendEdit = new QTextEdit(owner);
        sendButton = new QPushButton(QObject::tr("Send"), owner);
        sendButton->setObjectName("SendButton");

        searchSerialButton = new QPushButton(QObject::tr("Search Available Serial"), owner);
        searchSerialButton->setObjectName("BlueButton");
        portNameBox = new QComboBox(owner);
        baudRateBox = new QComboBox(owner);
        dataBitsBox = new QComboBox(owner);
        stopBitsBox = new QComboBox(owner);
        parityBox = new QComboBox(owner);
        flowControlBox = new QComboBox(owner);
        openOrCloseButton = new QPushButton(QObject::tr("Open Serial"), owner);
        openOrCloseButton->setObjectName("OpenButton");
        openOrCloseButton->setCheckable(true);

        hexBox = new QCheckBox(QObject::tr("Hex"), owner);
        autoSendBox = new QCheckBox(QObject::tr("Auto Delivery"), owner);
        autoSendTimeBox = new QSpinBox(owner);
        autoSendTimeBox->setSuffix(QObject::tr(" ms"));
        autoSendTimeBox->setRange(0, 10000);
        autoSendTimeBox->setValue(1000);
        autoSendTimeBox->setSingleStep(50);

        sendConutButton = new QPushButton(QObject::tr("Send: 0 Bytes"), owner);
        recvConutButton = new QPushButton(QObject::tr("Receive: 0 Bytes"), owner);
        saveButton = new QPushButton(QObject::tr("Save Data"), owner);
        clearButton = new QPushButton(QObject::tr("Clear Screen"), owner);

        sendTime = new QTimer(owner);
    }
    QWidget *owner;

    QTextEdit *dataView;
    QTextEdit *sendEdit;
    QPushButton *sendButton;

    QPushButton *searchSerialButton;
    QComboBox *portNameBox;
    QComboBox *baudRateBox;
    QComboBox *dataBitsBox;
    QComboBox *stopBitsBox;
    QComboBox *parityBox;
    QComboBox *flowControlBox;
    QPushButton *openOrCloseButton;

    QCheckBox *hexBox;
    QCheckBox *autoSendBox;
    QSpinBox *autoSendTimeBox;
    QPushButton *sendConutButton;
    QPushButton *recvConutButton;
    QPushButton *saveButton;
    QPushButton *clearButton;

    QScopedPointer<SerialPortThread> serialThreadPtr;
    SerialWidgetParam serialWidgetParam;

    QTimer *sendTime = nullptr;
    int sendCount = 0;
    int recvCount = 0;
};

SerialWidget::SerialWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SerialWidgetPrivate(this))
{
    qRegisterMetaType<SerialParam>("SerialParam");
    setupUI();
    initWindow();
    loadSetting();
    setWindowParam();
    buildConnect();
    onSerialOnline(false);
    d_ptr->dataView->clear();
}

SerialWidget::~SerialWidget()
{
    saveSetting();
}

void SerialWidget::onSearchPort()
{
    d_ptr->portNameBox->clear();

    // 查找可用的串口
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        QSerialPort port;
        port.setPort(info);
        if (port.open(QIODevice::ReadWrite)) {
            d_ptr->portNameBox->addItem(port.portName()); //将串口号添加到portname
            port.close(); //关闭串口等待人为(打开串口按钮)打开
        }
    }
}

void SerialWidget::onSendData()
{
    QString str = d_ptr->sendEdit->toPlainText();
    if (str.isEmpty())
        return;

    QByteArray bytes;
    if (d_ptr->hexBox->isChecked()) {
        bytes = QByteArray::fromHex(str.toLocal8Bit()).toUpper();
        str = formatHex(bytes);
    } else
        bytes = str.toLatin1();

    if (d_ptr->serialThreadPtr) {
        emit d_ptr->serialThreadPtr->sendMessage(bytes);
        appendDisplay(Send, str);
        d_ptr->sendCount += bytes.size();
        setSendCount(d_ptr->sendCount);
    }
}

void SerialWidget::onParamChanged(const QString &)
{
    if (!d_ptr->serialThreadPtr)
        return;

    setSerialParam();
    emit d_ptr->serialThreadPtr->paramChanged(d_ptr->serialWidgetParam.serialParam);
}

void SerialWidget::onOpenOrCloseSerial(bool state)
{
    d_ptr->openOrCloseButton->setChecked(!state);
    if (state) {
        setSerialParam();
        d_ptr->serialThreadPtr.reset(new SerialPortThread(d_ptr->serialWidgetParam.serialParam));
        connect(d_ptr->serialThreadPtr.data(),
                &SerialPortThread::serialOnLine,
                this,
                &SerialWidget::onSerialOnline,
                Qt::UniqueConnection);
        connect(d_ptr->serialThreadPtr.data(),
                &SerialPortThread::errorMessage,
                this,
                &SerialWidget::onAppendError,
                Qt::UniqueConnection);
        connect(d_ptr->serialThreadPtr.data(),
                &SerialPortThread::serialMessage,
                this,
                &SerialWidget::onSerialRecvMessage,
                Qt::UniqueConnection);
        d_ptr->serialThreadPtr->start();
    } else {
        d_ptr->serialThreadPtr.reset();
    }
}

void SerialWidget::onSerialOnline(bool state)
{
    d_ptr->searchSerialButton->setEnabled(!state);
    d_ptr->openOrCloseButton->setChecked(state);
    d_ptr->openOrCloseButton->setText(state ? tr("Close Serial") : tr("Open Serial"));

    if (!state) {
        d_ptr->autoSendBox->setChecked(state);
        d_ptr->sendTime->stop();
    }
    d_ptr->autoSendBox->setEnabled(state);
    d_ptr->sendButton->setEnabled(state);

    if (state)
        appendDisplay(SuccessInfo, tr("Serial Open!"));
    else
        appendDisplay(ErrorInfo, tr("Serial Close!"));
}

void SerialWidget::onAppendError(const QString &error)
{
    appendDisplay(ErrorInfo, error);
}

void SerialWidget::onSerialRecvMessage(const QByteArray &bytes)
{
    //qDebug() << "onSerialRecvMessage: " << bytes;
    if (bytes.isEmpty())
        return;
    d_ptr->recvCount += bytes.size();
    setRecvCount(d_ptr->recvCount);
    QString str;
    if (d_ptr->hexBox->isChecked())
        str = formatHex(bytes);
    else
        str = QString::fromLatin1(bytes);
    appendDisplay(Recv, str);
}

void SerialWidget::onAutoSend(bool state)
{
    d_ptr->autoSendTimeBox->setEnabled(!state);
    if (state)
        d_ptr->sendTime->start(d_ptr->autoSendTimeBox->value());
    else
        d_ptr->sendTime->stop();
}

void SerialWidget::onSave()
{
    const QString data = d_ptr->dataView->toPlainText();
    if (data.isEmpty()) {
        return;
    }
    const QString openPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
                                 .value(0, QDir::homePath());
    const QString path
        = QFileDialog::getSaveFileName(this,
                                       tr("Open File"),
                                       QString("%1/%2").arg(openPath,
                                                            QDateTime::currentDateTime().toString(
                                                                "yyyy-MM-dd-HH-mm-ss")),
                                       tr("Text Files(*.txt)"));
    if (!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            MessBox::Warning(this,
                             tr("Write File: Can't open file:\n %1 !").arg(path),
                             MessBox::CloseButton);
            return;
        }
        QTextStream stream(&file);
        stream << data;
        file.close();
        appendDisplay(SuccessInfo, tr("The file was saved successfully."));
    } else {
        appendDisplay(ErrorInfo, tr("No file saved."));
    }
}

void SerialWidget::setupUI()
{
    QGroupBox *dataBox = new QGroupBox(tr("Data Display Window"), this);
    QHBoxLayout *dataLayout = new QHBoxLayout(dataBox);
    dataLayout->addWidget(d_ptr->dataView);

    QGroupBox *sendBox = new QGroupBox(tr("Data Sending Window"), this);
    sendBox->setObjectName("SendBox");
    QHBoxLayout *sendLayout = new QHBoxLayout(sendBox);
    sendLayout->addWidget(d_ptr->sendEdit);
    sendLayout->addWidget(d_ptr->sendButton);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(dataBox);
    splitter->addWidget(sendBox);
    splitter->setHandleWidth(0);
    splitter->setSizes(QList<int>() << 1000 << 1);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(splitter);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addRow(tr("Port: "), d_ptr->portNameBox);
    formLayout->addRow(tr("Baud Rate: "), d_ptr->baudRateBox);
    formLayout->addRow(tr("Data Bits: "), d_ptr->dataBitsBox);
    formLayout->addRow(tr("Stop Bits: "), d_ptr->stopBitsBox);
    formLayout->addRow(tr("Parity: "), d_ptr->parityBox);
    formLayout->addRow(tr("Flow Control: "), d_ptr->flowControlBox);

    QGroupBox *setBox = new QGroupBox(tr("Parameter Setting Window"), this);
    setBox->setObjectName("SetBox");
    QVBoxLayout *setLayout = new QVBoxLayout(setBox);
    setLayout->addWidget(d_ptr->searchSerialButton);
    setLayout->addLayout(formLayout);
    setLayout->addWidget(d_ptr->openOrCloseButton);
    setLayout->addWidget(d_ptr->hexBox);
    setLayout->addWidget(d_ptr->autoSendBox);
    setLayout->addWidget(d_ptr->autoSendTimeBox);
    setLayout->addStretch();
    setLayout->addWidget(d_ptr->sendConutButton);
    setLayout->addWidget(d_ptr->recvConutButton);
    setLayout->addWidget(d_ptr->saveButton);
    setLayout->addWidget(d_ptr->clearButton);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout);
    layout->addWidget(setBox);
}

void SerialWidget::initWindow()
{
    onSearchPort();
    QList<qint32> baudList = QSerialPortInfo::standardBaudRates();
    for (const qint32 baudrate : baudList)
        d_ptr->baudRateBox->addItem(QString::number(baudrate), baudrate);

    d_ptr->dataBitsBox->addItem("5", QSerialPort::Data5);
    d_ptr->dataBitsBox->addItem("6", QSerialPort::Data6);
    d_ptr->dataBitsBox->addItem("7", QSerialPort::Data7);
    d_ptr->dataBitsBox->addItem("8", QSerialPort::Data8);

    d_ptr->stopBitsBox->addItem("1", QSerialPort::OneStop);
#ifdef Q_OS_WIN
    d_ptr->stopBitsBox->addItem("1.5", QSerialPort::OneAndHalfStop);
#endif
    d_ptr->stopBitsBox->addItem("2", QSerialPort::TwoStop);

    d_ptr->parityBox->addItem(tr("No"), QSerialPort::NoParity);
    d_ptr->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    d_ptr->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);

    d_ptr->flowControlBox->addItem(tr("NoFlowControl"), QSerialPort::NoFlowControl);
    d_ptr->flowControlBox->addItem(tr("HardwareFlowControl"), QSerialPort::HardwareControl);
    d_ptr->flowControlBox->addItem(tr("SoftwareFlowControl"), QSerialPort::SoftwareControl);
}

inline void setComboxCurrentText(QComboBox *box, const QVariant &value)
{
    box->setCurrentIndex(box->findData(value));
}

void SerialWidget::setWindowParam()
{
    SerialParam *serialParam = &d_ptr->serialWidgetParam.serialParam;
    setComboxCurrentText(d_ptr->baudRateBox, serialParam->baudRate);
    setComboxCurrentText(d_ptr->dataBitsBox, serialParam->dataBits);
    setComboxCurrentText(d_ptr->stopBitsBox, serialParam->stopBits);
    setComboxCurrentText(d_ptr->parityBox, serialParam->parity);
    setComboxCurrentText(d_ptr->flowControlBox, serialParam->flowControl);

    SerialExtraParam *extraParam = &d_ptr->serialWidgetParam.serialExtraParam;
    d_ptr->hexBox->setChecked(extraParam->hex);
    d_ptr->autoSendTimeBox->setValue(extraParam->sendTime);
    d_ptr->sendEdit->setText(extraParam->sendData);
}

void SerialWidget::buildConnect()
{
    connect(d_ptr->searchSerialButton, &QPushButton::clicked, this, &SerialWidget::onSearchPort);

    connect(d_ptr->portNameBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d_ptr->baudRateBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d_ptr->dataBitsBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d_ptr->stopBitsBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d_ptr->parityBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d_ptr->flowControlBox,
            &QComboBox::currentTextChanged,
            this,
            &SerialWidget::onParamChanged);

    connect(d_ptr->openOrCloseButton,
            &QPushButton::clicked,
            this,
            &SerialWidget::onOpenOrCloseSerial);

    QShortcut *sendShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
    connect(sendShortcut, &QShortcut::activated, this, &SerialWidget::onSendData);
    connect(d_ptr->sendButton, &QPushButton::clicked, this, &SerialWidget::onSendData);

    connect(d_ptr->autoSendBox, &QCheckBox::clicked, this, &SerialWidget::onAutoSend);
    connect(d_ptr->sendTime, &QTimer::timeout, this, &SerialWidget::onSendData);

    connect(d_ptr->sendConutButton, &QPushButton::clicked, [this] {
        d_ptr->sendCount = 0;
        setSendCount(0);
    });
    connect(d_ptr->recvConutButton, &QPushButton::clicked, [this] {
        d_ptr->recvCount = 0;
        setRecvCount(0);
    });
    connect(d_ptr->saveButton, &QPushButton::clicked, this, &SerialWidget::onSave);
    connect(d_ptr->clearButton, &QPushButton::clicked, d_ptr->dataView, &QTextEdit::clear);
}

void SerialWidget::setSerialParam()
{
    SerialParam *serialParam = &d_ptr->serialWidgetParam.serialParam;
    serialParam->portName = d_ptr->portNameBox->currentText();
    serialParam->baudRate = QSerialPort::BaudRate(d_ptr->baudRateBox->currentData().toInt());
    serialParam->dataBits = QSerialPort::DataBits(d_ptr->dataBitsBox->currentData().toInt());
    serialParam->stopBits = QSerialPort::StopBits(d_ptr->stopBitsBox->currentData().toInt());
    serialParam->parity = QSerialPort::Parity(d_ptr->parityBox->currentData().toInt());
    serialParam->flowControl = QSerialPort::FlowControl(
        d_ptr->flowControlBox->currentData().toInt());
}

void SerialWidget::appendDisplay(SerialWidget::MessageType type, const QString &message)
{
    if (message.isEmpty())
        return;

    QString display;
    switch (type) {
    case Send:
        display = tr(" >> Serial Send: ");
        d_ptr->dataView->setTextColor(QColor("black"));
        break;
    case Recv:
        display = tr(" >> Serial Recv: ");
        d_ptr->dataView->setTextColor(QColor("dodgerblue"));
        break;
    case SuccessInfo:
        display = tr(" >> Prompt Message: ");
        d_ptr->dataView->setTextColor(QColor("green"));
        break;
    case ErrorInfo:
        display = tr(" >> Prompt Message: ");
        d_ptr->dataView->setTextColor(QColor("red"));
        break;
    default: return;
    }

    d_ptr->dataView->append(
        tr("Time [%1] %2 %3")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"),
                 display,
                 message));
}

void SerialWidget::setSendCount(int size)
{
    d_ptr->sendConutButton->setText(tr("Send: %1 Bytes").arg(size));
}

void SerialWidget::setRecvCount(int size)
{
    d_ptr->recvConutButton->setText(tr("Recv: %1 Bytes").arg(size));
}

void SerialWidget::loadSetting()
{
    QSettings *setting = ExtensionSystem::PluginManager::settings();
    if (!setting)
        return;

    setting->beginGroup("serial_config");

    SerialParam *serialParam = &d_ptr->serialWidgetParam.serialParam;
    serialParam->baudRate = QSerialPort::BaudRate(
        setting->value("BaudRate", serialParam->baudRate).toInt());
    serialParam->dataBits = QSerialPort::DataBits(
        setting->value("DataBits", serialParam->dataBits).toInt());
    serialParam->stopBits = QSerialPort::StopBits(
        setting->value("StopBits", serialParam->stopBits).toInt());
    serialParam->parity = QSerialPort::Parity(setting->value("Parity", serialParam->parity).toInt());
    serialParam->flowControl = QSerialPort::FlowControl(
        setting->value("FlowControl", serialParam->flowControl).toInt());

    SerialExtraParam *extraParam = &d_ptr->serialWidgetParam.serialExtraParam;
    extraParam->hex = setting->value("Hex", extraParam->hex).toBool();
    extraParam->sendTime = setting->value("SendTime", extraParam->sendTime).toInt();
    extraParam->sendData = setting->value("SendData", extraParam->sendData).toString();
    setting->endGroup();
}

void SerialWidget::saveSetting()
{
    QSettings *setting = ExtensionSystem::PluginManager::settings();
    if (!setting) {
        return;
    }

    SerialParam *serialParam = &d_ptr->serialWidgetParam.serialParam;
    setting->beginGroup("serial_config");
    setting->setValue("BaudRate", serialParam->baudRate);
    setting->setValue("DataBits", serialParam->dataBits);
    setting->setValue("StopBits", serialParam->stopBits);
    setting->setValue("Parity", serialParam->parity);
    setting->setValue("FlowControl", serialParam->flowControl);

    setting->setValue("Hex", d_ptr->hexBox->isChecked());
    setting->setValue("SendTime", d_ptr->autoSendTimeBox->value());
    setting->setValue("SendData", d_ptr->sendEdit->toPlainText().toUtf8());
    setting->endGroup();
}
