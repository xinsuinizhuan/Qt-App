#include "tcpwidget.h"
#include "tcpclientthread.h"
#include "tcpserverthread.h"

#include <controls/messbox.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/utils.h>

#include <QHostAddress>
#include <QNetworkInterface>
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

struct TcpWidgetParam
{
    int model = 0;
    QString ip = "127.0.0.1";
    QString port = "65533";
    bool hex = false;
    int sendTime = 1000;
    int connectTime = 1000;
    QString sendData;
};

enum Model { Server, Client };

class TcpWidget::TcpWidgetPrivate
{
public:
    TcpWidgetPrivate(QWidget *parent)
        : owner(parent)
    {
        dataView = new QTextEdit(owner);
        dataView->document()->setMaximumBlockCount(1000);
        dataView->setReadOnly(true);

        sendData = new QTextEdit(owner);
        sendButton = new QPushButton(QObject::tr("Send"), owner);
        sendButton->setObjectName("SendButton");

        modelBox = new QComboBox(owner);
        modelBox->addItems(QStringList() << QObject::tr("TcpServer") << QObject::tr("TcpClient"));
        ipLabel = new QLabel(QObject::tr("Local IP List: "), owner);
        localIPBox = new QComboBox(owner);
        serverIPEdit = new QLineEdit(owner);
        serverIPEdit->setPlaceholderText(QObject::tr("Please enter the server IP address."));
        QRegularExpression regExp(
            "^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
        QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp,
                                                                                 serverIPEdit);
        serverIPEdit->setValidator(validator);
        portLabel = new QLabel(QObject::tr("Local Port: "), owner);
        portEdit = new QLineEdit(owner);
        portEdit->setPlaceholderText(QObject::tr("Please enter the port number."));
        portEdit->setValidator(new QIntValidator(0, 65536, portEdit));
        listenOrConnectButton = new QPushButton(owner);
        listenOrConnectButton->setCheckable(true);
        listenOrConnectButton->setObjectName("OpenButton");

        hexBox = new QCheckBox(QObject::tr("Hex"), owner);
        autoSendBox = new QCheckBox(QObject::tr("Auto Delivery"), owner);
        autoSendTimeBox = new QSpinBox(owner);
        autoSendTimeBox->setSuffix(QObject::tr(" ms"));
        autoSendTimeBox->setRange(0, 10000);
        autoSendTimeBox->setValue(1000);
        autoSendTimeBox->setSingleStep(50);

        allConnectBox = new QComboBox(owner);
        allConnectBox->addItem(QObject::tr("Connect All"));
        autoConnectBox = new QCheckBox(QObject::tr("Auto Reconnect"), owner);
        autoConnectTimeBox = new QSpinBox(owner);
        autoConnectTimeBox->setSuffix(QObject::tr(" ms"));
        autoConnectTimeBox->setRange(1000, 100000);
        autoConnectTimeBox->setValue(1000);
        autoConnectTimeBox->setSingleStep(50);

        sendConutButton = new QPushButton(owner);
        recvConutButton = new QPushButton(owner);
        saveButton = new QPushButton(QObject::tr("Save Data"), owner);
        clearButton = new QPushButton(QObject::tr("Clear Screen"), owner);

        setWidget = new QWidget(owner);
    }
    QWidget *owner;

    QTextEdit *dataView;
    QTextEdit *sendData;
    QPushButton *sendButton;

    QComboBox *modelBox;
    QLabel *ipLabel;
    QComboBox *localIPBox;
    QLineEdit *serverIPEdit;
    QLabel *portLabel;
    QLineEdit *portEdit;
    QPushButton *listenOrConnectButton;

    QCheckBox *hexBox;
    QCheckBox *autoSendBox;
    QSpinBox *autoSendTimeBox;
    QComboBox *allConnectBox;
    QCheckBox *autoConnectBox;
    QSpinBox *autoConnectTimeBox;

    QPushButton *sendConutButton;
    QPushButton *recvConutButton;
    QPushButton *saveButton;
    QPushButton *clearButton;

    QWidget *setWidget;

    TcpServerThread *serverThread = nullptr;
    TcpClientThread *clientThread = nullptr;

    QTimer sendTime;
    QTimer autoConnectTime;
    int sendCount = 0;
    int recvCount = 0;

    TcpWidgetParam tcpWidgetParam;
};

TcpWidget::TcpWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new TcpWidgetPrivate(this))
{
    setupUI();
    initWindow();
    loadSetting();
    buildConnect();
    setWindowParam();
}

TcpWidget::~TcpWidget()
{
    saveSetting();
}

void TcpWidget::onModelChange(const QString &text)
{
    if (text == tr("TcpServer")) {
        d_ptr->ipLabel->setText(tr("Local IP List: "));
        d_ptr->localIPBox->show();
        d_ptr->serverIPEdit->hide();
        d_ptr->portLabel->setText(tr("Local Port: "));
        d_ptr->listenOrConnectButton->setText(tr("Listen"));
        d_ptr->allConnectBox->show();
        d_ptr->autoConnectBox->hide();
        d_ptr->autoConnectTimeBox->hide();
        onServerOnline(false);
    } else if (text == tr("TcpClient")) {
        d_ptr->ipLabel->setText(tr("Server IP: "));
        d_ptr->localIPBox->hide();
        d_ptr->serverIPEdit->show();
        d_ptr->listenOrConnectButton->setText(tr("Connect"));
        d_ptr->portLabel->setText(tr("Server Port: "));
        d_ptr->allConnectBox->hide();
        d_ptr->autoConnectBox->show();
        d_ptr->autoConnectTimeBox->show();
        onClientOnLine(false);
    }
    clearCount();
    d_ptr->dataView->textCursor().removeSelectedText();
}

void TcpWidget::onListenOrConnect(bool state)
{
    d_ptr->listenOrConnectButton->setChecked(!state);
    QString port = d_ptr->portEdit->text();
    if (port.isEmpty()) {
        MessBox::Warning(this, tr("Please enter the port number!"), MessBox::CloseButton);
        d_ptr->portEdit->setFocus();
        return;
    }

    if (d_ptr->modelBox->currentText() == tr("TcpServer")) {
        if (state) {
            destoryServerOrClientThread();
            d_ptr->serverThread = new TcpServerThread(quint16(port.toUInt()),
                                                      d_ptr->localIPBox->currentText(),
                                                      this);
            connect(d_ptr->serverThread,
                    &TcpServerThread::serverOnLine,
                    this,
                    &TcpWidget::onServerOnline,
                    Qt::UniqueConnection);
            connect(d_ptr->serverThread,
                    &TcpServerThread::errorMessage,
                    this,
                    &TcpWidget::onAppendError,
                    Qt::UniqueConnection);
            connect(d_ptr->serverThread,
                    &TcpServerThread::newClientInfo,
                    this,
                    &TcpWidget::onServerNewClient,
                    Qt::UniqueConnection);
            connect(d_ptr->serverThread,
                    &TcpServerThread::disconnectClientInfo,
                    this,
                    &TcpWidget::onServerDisClient,
                    Qt::UniqueConnection);
            connect(d_ptr->serverThread,
                    &TcpServerThread::clientMessage,
                    this,
                    &TcpWidget::onServerRecvMessage,
                    Qt::UniqueConnection);
            d_ptr->serverThread->start();
        } else {
            destoryServerOrClientThread();
            for (int i = 1; i < d_ptr->allConnectBox->count(); i++) {
                onServerDisClient(d_ptr->allConnectBox->itemText(i));
                d_ptr->allConnectBox->removeItem(i);
            }
        }
    } else if (d_ptr->modelBox->currentText() == tr("TcpClient")) {
        if (state) {
            destoryServerOrClientThread();
            createTcpClientThread();
        } else if (d_ptr->clientThread)
            destoryServerOrClientThread();
    }
}

void TcpWidget::onSendData()
{
    QString str = d_ptr->sendData->toPlainText();
    if (str.isEmpty())
        return;

    QByteArray bytes;
    if (d_ptr->hexBox->isChecked()) {
        bytes = QByteArray::fromHex(str.toLocal8Bit()).toUpper();
        str = formatHex(bytes);
    } else
        bytes = str.toUtf8();

    if (d_ptr->serverThread) {
        if (d_ptr->allConnectBox->count() == 1) {
            QString error = tr("No client is currently online, "
                               "please stop sending invalid!");
            appendDisplay(ErrorInfo, error);
            return;
        }
        QString clientInfo = d_ptr->allConnectBox->currentText();
        if (clientInfo == tr("Connect All")) {
            appendDisplay(Send, QString(tr("Send To All Online Clients: %1.").arg(str)));
            clientInfo = "";
        } else
            appendDisplay(Send, QString(tr("Send To Clients [%1] : %2.").arg(clientInfo).arg(str)));
        emit d_ptr->serverThread->sendMessage(bytes, clientInfo);
        d_ptr->sendCount += str.size();
        setSendCount(d_ptr->sendCount);
    } else if (d_ptr->clientThread) {
        emit d_ptr->clientThread->sendMessage(bytes);
        appendDisplay(Send, str);
        d_ptr->sendCount += bytes.size();
        setSendCount(d_ptr->sendCount);
    }
}

void TcpWidget::onServerOnline(bool state)
{
    d_ptr->modelBox->setEnabled(!state);
    d_ptr->localIPBox->setEnabled(!state);
    d_ptr->portEdit->setEnabled(!state);
    d_ptr->listenOrConnectButton->setChecked(state);
    d_ptr->listenOrConnectButton->setText(state ? tr("Stop Listen") : tr("Listen"));
    if (!state) {
        d_ptr->autoSendBox->setChecked(state);
        d_ptr->sendTime.stop();
    }
    d_ptr->autoSendBox->setEnabled(state);
    d_ptr->sendButton->setEnabled(state);

    if (state)
        appendDisplay(SuccessInfo, tr("Server Online!"));
    else
        appendDisplay(ErrorInfo, tr("Server Offline!"));
}

void TcpWidget::onServerNewClient(const QString &clientInfo)
{
    d_ptr->allConnectBox->addItem(clientInfo);
    QString str = clientInfo + tr(" Online.");
    appendDisplay(SuccessInfo, str);
}

void TcpWidget::onServerDisClient(const QString &clientInfo)
{
    d_ptr->allConnectBox->removeItem(d_ptr->allConnectBox->findText(clientInfo));
    QString str = clientInfo + tr(" Offline.");
    appendDisplay(ErrorInfo, str);
}

void TcpWidget::onServerRecvMessage(const QString &clientInfo, const QByteArray &bytes)
{
    if (bytes.isEmpty())
        return;

    d_ptr->recvCount += bytes.size();
    setRecvCount(d_ptr->recvCount);
    QString str = clientInfo;
    if (d_ptr->hexBox->isChecked())
        str += formatHex(bytes);
    else
        str += QString::fromUtf8(bytes);
    appendDisplay(Recv, str);
}

void TcpWidget::onClientOnLine(bool state)
{
    d_ptr->modelBox->setEnabled(!state);
    d_ptr->serverIPEdit->setEnabled(!state);
    d_ptr->portEdit->setEnabled(!state);
    d_ptr->listenOrConnectButton->setChecked(state);
    d_ptr->listenOrConnectButton->setText(state ? tr("Disconnect") : tr("Connect"));
    if (!state) {
        d_ptr->autoSendBox->setChecked(state);
        d_ptr->sendTime.stop();
    }
    d_ptr->autoSendBox->setEnabled(state);
    d_ptr->sendButton->setEnabled(state);

    if (!state && !d_ptr->autoConnectBox->isChecked() && d_ptr->clientThread)
        destoryServerOrClientThread();

    if (state)
        appendDisplay(SuccessInfo, tr("Client Online!"));
    else
        appendDisplay(ErrorInfo, tr("Client Offline!"));
}

void TcpWidget::onClientRecvMessage(const QByteArray &bytes)
{
    if (bytes.isEmpty())
        return;
    d_ptr->recvCount += bytes.size();
    setRecvCount(d_ptr->recvCount);
    QString str;
    if (d_ptr->hexBox->isChecked())
        str = formatHex(bytes);
    else
        str = bytes;
    appendDisplay(Recv, str);
}

void TcpWidget::onAutoReconnectStartOrStop(bool state)
{
    d_ptr->setWidget->setEnabled(!state);
    if (state) {
        createTcpClientThread();
        d_ptr->autoConnectTime.start(d_ptr->autoConnectTimeBox->value());
    } else {
        d_ptr->autoConnectTime.stop();
        if (!d_ptr->listenOrConnectButton->isChecked())
            destoryServerOrClientThread();
    }
}

void TcpWidget::onAutoConnect()
{
    createTcpClientThread();
    emit d_ptr->clientThread->reconnect();
}

void TcpWidget::onAutoSend(bool state)
{
    d_ptr->autoSendTimeBox->setEnabled(!state);
    if (state)
        d_ptr->sendTime.start(d_ptr->autoSendTimeBox->value());
    else
        d_ptr->sendTime.stop();
}

void TcpWidget::onSave()
{
    QString data = d_ptr->dataView->toPlainText();
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

void TcpWidget::onAppendError(const QString &error)
{
    appendDisplay(ErrorInfo, error);
}

void TcpWidget::setupUI()
{
    QGroupBox *dataBox = new QGroupBox(tr("Data Display Window"), this);
    QHBoxLayout *dataLayout = new QHBoxLayout(dataBox);
    dataLayout->addWidget(d_ptr->dataView);

    QGroupBox *sendBox = new QGroupBox(tr("Data Sending Window"), this);
    sendBox->setObjectName("SendBox");
    QHBoxLayout *sendLayout = new QHBoxLayout(sendBox);
    sendLayout->addWidget(d_ptr->sendData);
    sendLayout->addWidget(d_ptr->sendButton);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(dataBox);
    splitter->addWidget(sendBox);
    splitter->setHandleWidth(0);
    splitter->setSizes(QList<int>() << 1000 << 1);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(splitter);

    QVBoxLayout *setLayout = new QVBoxLayout(d_ptr->setWidget);
    setLayout->setContentsMargins(0, 0, 0, 0);
    setLayout->addWidget(new QLabel(tr("Communication Mode: "), this));
    setLayout->addWidget(d_ptr->modelBox);
    setLayout->addWidget(d_ptr->ipLabel);
    setLayout->addWidget(d_ptr->localIPBox);
    setLayout->addWidget(d_ptr->serverIPEdit);
    setLayout->addWidget(d_ptr->portLabel);
    setLayout->addWidget(d_ptr->portEdit);
    setLayout->addWidget(d_ptr->listenOrConnectButton);

    QGroupBox *setBox = new QGroupBox(tr("Parameter Setting Window"), this);
    setBox->setObjectName("SetBox");
    QVBoxLayout *allSetLayout = new QVBoxLayout(setBox);
    allSetLayout->addWidget(d_ptr->setWidget);
    allSetLayout->addWidget(d_ptr->hexBox);
    allSetLayout->addWidget(d_ptr->autoSendBox);
    allSetLayout->addWidget(d_ptr->autoSendTimeBox);
    allSetLayout->addWidget(d_ptr->allConnectBox);
    allSetLayout->addWidget(d_ptr->autoConnectBox);
    allSetLayout->addWidget(d_ptr->autoConnectTimeBox);
    allSetLayout->addStretch();
    allSetLayout->addWidget(d_ptr->sendConutButton);
    allSetLayout->addWidget(d_ptr->recvConutButton);
    allSetLayout->addWidget(d_ptr->saveButton);
    allSetLayout->addWidget(d_ptr->clearButton);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout);
    layout->addWidget(setBox);
}

void TcpWidget::initWindow()
{
    d_ptr->localIPBox->clear();
    //  获得IP
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    for (QHostAddress address : ipList)
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            d_ptr->localIPBox->addItem(address.toString());
    d_ptr->localIPBox->setCurrentIndex(d_ptr->localIPBox->count() - 1);
}

void TcpWidget::setWindowParam()
{
    d_ptr->modelBox->setCurrentIndex(d_ptr->tcpWidgetParam.model);
    d_ptr->serverIPEdit->setText(d_ptr->tcpWidgetParam.ip);
    d_ptr->portEdit->setText(d_ptr->tcpWidgetParam.port);

    d_ptr->hexBox->setChecked(d_ptr->tcpWidgetParam.hex);
    d_ptr->autoSendTimeBox->setValue(d_ptr->tcpWidgetParam.sendTime);
    d_ptr->autoConnectTimeBox->setValue(d_ptr->tcpWidgetParam.connectTime);
    d_ptr->sendData->setText(d_ptr->tcpWidgetParam.sendData);

    onModelChange(d_ptr->modelBox->currentText());
}

void TcpWidget::buildConnect()
{
    connect(d_ptr->modelBox, &QComboBox::currentTextChanged, this, &TcpWidget::onModelChange);
    connect(d_ptr->listenOrConnectButton,
            &QPushButton::clicked,
            this,
            &TcpWidget::onListenOrConnect);

    QShortcut *sendShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
    connect(sendShortcut, &QShortcut::activated, this, &TcpWidget::onSendData);
    connect(d_ptr->sendButton, &QPushButton::clicked, this, &TcpWidget::onSendData);

    connect(d_ptr->autoSendBox, &QCheckBox::clicked, this, &TcpWidget::onAutoSend);
    connect(&d_ptr->sendTime, &QTimer::timeout, this, &TcpWidget::onSendData);

    connect(d_ptr->autoConnectBox,
            &QCheckBox::clicked,
            this,
            &TcpWidget::onAutoReconnectStartOrStop);
    connect(&d_ptr->autoConnectTime, &QTimer::timeout, this, &TcpWidget::onAutoConnect);

    connect(d_ptr->sendConutButton, &QPushButton::clicked, [this] {
        d_ptr->sendCount = 0;
        setSendCount(0);
    });
    connect(d_ptr->recvConutButton, &QPushButton::clicked, [this] {
        d_ptr->recvCount = 0;
        setRecvCount(0);
    });
    connect(d_ptr->saveButton, &QPushButton::clicked, this, &TcpWidget::onSave);
    connect(d_ptr->clearButton, &QPushButton::clicked, d_ptr->dataView, &QTextEdit::clear);
}

void TcpWidget::clearCount()
{
    d_ptr->sendCount = 0;
    d_ptr->recvCount = 0;
    setSendCount(0);
    setRecvCount(0);
}

void TcpWidget::appendDisplay(TcpWidget::MessageType type, const QString &message)
{
    if (message.isEmpty())
        return;

    QString display;
    switch (type) {
    case Send:
        display = tr(" >> Network Send: ");
        d_ptr->dataView->setTextColor(QColor("black"));
        break;
    case Recv:
        display = tr(" >> Network Recv: ");
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

void TcpWidget::setSendCount(int size)
{
    d_ptr->sendConutButton->setText(tr("Send: %1 Bytes").arg(size));
}

void TcpWidget::setRecvCount(int size)
{
    d_ptr->recvConutButton->setText(tr("Recv: %1 Bytes").arg(size));
}

void TcpWidget::createTcpClientThread()
{
    if (!d_ptr->clientThread) {
        QString port = d_ptr->portEdit->text();
        if (port.isEmpty()) {
            MessBox::Warning(this, tr("Please enter the port number!"), MessBox::CloseButton);
            d_ptr->portEdit->setFocus();
            d_ptr->autoConnectBox->setChecked(false);
            onAutoReconnectStartOrStop(false);
            return;
        }
        QString ip = d_ptr->serverIPEdit->text().trimmed();
        if (ip.isEmpty()) {
            MessBox::Warning(this, tr("Please enter the ip address!"), MessBox::CloseButton);
            d_ptr->serverIPEdit->setFocus();
            d_ptr->autoConnectBox->setChecked(false);
            onAutoReconnectStartOrStop(false);
            return;
        }
        d_ptr->clientThread = new TcpClientThread(ip, quint16(port.toUInt()), this);
        connect(d_ptr->clientThread,
                &TcpClientThread::clientOnLine,
                this,
                &TcpWidget::onClientOnLine,
                Qt::UniqueConnection);
        connect(d_ptr->clientThread,
                &TcpClientThread::errorMessage,
                this,
                &TcpWidget::onAppendError,
                Qt::UniqueConnection);
        connect(d_ptr->clientThread,
                &TcpClientThread::serverMessage,
                this,
                &TcpWidget::onClientRecvMessage,
                Qt::UniqueConnection);
        d_ptr->clientThread->start();
    }
}

void TcpWidget::destoryServerOrClientThread()
{
    if (d_ptr->serverThread) {
        delete d_ptr->serverThread;
        d_ptr->serverThread = nullptr;
    }
    if (d_ptr->clientThread) {
        d_ptr->clientThread->deleteLater();
        d_ptr->clientThread = nullptr;
    }
}

void TcpWidget::loadSetting()
{
    QSettings *setting = ExtensionSystem::PluginManager::settings();
    if (!setting)
        return;

    setting->beginGroup("tcp_config");
    d_ptr->tcpWidgetParam.model = setting->value("CommunicationMode", d_ptr->tcpWidgetParam.model)
                                      .toInt();
    d_ptr->tcpWidgetParam.ip = setting->value("ClientIP", d_ptr->tcpWidgetParam.ip).toString();
    d_ptr->tcpWidgetParam.port = setting->value("Port", d_ptr->tcpWidgetParam.port).toString();

    d_ptr->tcpWidgetParam.hex = setting->value("Hex", d_ptr->tcpWidgetParam.hex).toBool();
    d_ptr->tcpWidgetParam.sendTime = setting->value("SendTime", d_ptr->tcpWidgetParam.sendTime)
                                         .toInt();
    d_ptr->tcpWidgetParam.connectTime
        = setting->value("ConnectTime", d_ptr->tcpWidgetParam.connectTime).toInt();
    d_ptr->tcpWidgetParam.sendData = setting->value("SendData", d_ptr->tcpWidgetParam.sendData)
                                         .toString();
    setting->endGroup();
}

void TcpWidget::saveSetting()
{
    QSettings *setting = ExtensionSystem::PluginManager::settings();
    if (!setting) {
        return;
    }

    setting->beginGroup("tcp_config");
    setting->setValue("CommunicationMode", d_ptr->modelBox->currentIndex());
    setting->setValue("ClientIP", d_ptr->serverIPEdit->text());
    setting->setValue("Port", d_ptr->portEdit->text());

    setting->setValue("Hex", d_ptr->hexBox->isChecked());
    setting->setValue("SendTime", d_ptr->autoSendTimeBox->value());
    setting->setValue("ConnectTime", d_ptr->autoConnectTimeBox->value());
    setting->setValue("SendData", d_ptr->sendData->toPlainText().toUtf8());
    setting->endGroup();
}
