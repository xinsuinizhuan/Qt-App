#include "mainwindow.h"
#include "drawwidget.h"
#include "imageviewer.h"

#include <utils/utils.h>

#include <QtWidgets>

class MainWindow::MainWindowPrivate
{
public:
    MainWindowPrivate(QWidget *parent)
        : owner(parent)
    {
        drawWidget = new DrawWidget(owner);
        imageViewer = new ImageViewer(owner);
        stackedWidget = new QStackedWidget(owner);
        stackedWidget->addWidget(imageViewer);
        stackedWidget->addWidget(drawWidget);
    }
    QWidget *owner;
    DrawWidget *drawWidget;
    ImageViewer *imageViewer;
    QStackedWidget *stackedWidget;
};

MainWindow::MainWindow(QWidget *parent)
    : CommonWidget(parent)
    , d_ptr(new MainWindowPrivate(this))
{
    connect(this, &MainWindow::aboutToclose, qApp, &QApplication::quit);
    setupUI();
    resize(1000, 600);
    Utils::windowCenter(this);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    // 图片读取限制大小
    qInfo() << tr("QImage memory allocation above this limit: %1MB.")
                   .arg(QImageReader::allocationLimit());
    //QImageReader::setAllocationLimit();
#endif
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setTitle(tr("PictureTool"));
    initToolBar();
    setCentralWidget(d_ptr->stackedWidget);
}

void MainWindow::initToolBar()
{
    QPushButton *qssButton = new QPushButton(tr("Reload QSS"), this);
    qssButton->setObjectName("QssButton");
    connect(qssButton, &QPushButton::clicked, this, [] { Utils::setQSS("picturetool_qss_files"); });

    QComboBox *menuBox = new QComboBox(this);
    menuBox->setObjectName("MenuBox");
    menuBox->addItems(QStringList() << tr("ImageViewer") << tr("DrawWidget"));
    connect(menuBox, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == tr("ImageViewer"))
            d_ptr->stackedWidget->setCurrentWidget(d_ptr->imageViewer);
        else if (text == tr("DrawWidget"))
            d_ptr->stackedWidget->setCurrentWidget(d_ptr->drawWidget);
    });

    QWidget *titleBar = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(menuBox);
    titleLayout->addWidget(qssButton);
    setTitleBar(titleBar);
}
