#include "imageviewer.h"
#include "imagelistmodel.h"
#include "maskdialog.h"
#include "rounddialog.hpp"

#include <controls/messbox.h>
#include <graphics/imageview.h>
#include <utils/utils.h>

#include <QtConcurrent>
#include <QtWidgets>

using namespace Graphics;
using namespace Control;

class ImageViewer::ImageViewerPrivate
{
public:
    ImageViewerPrivate(QWidget *parent)
        : owner(parent)
    {
        imageView = new ImageView(owner);
        imageViewFormat = new ImageView(owner);
        urlLabel = new QLabel("-", owner);
        urlLabel->setWordWrap(true);
        sizeLabel = new QLabel("-", owner);
        fileSizeLabel = new QLabel("-", owner);
        scaleLabel = new QLabel("-", owner);
        imageListView = new ImageListView(owner);

        formatBox = new QComboBox(owner);
        colorBox = new QComboBox(owner);

        const QMetaEnum imageFormat = QMetaEnum::fromType<QImage::Format>();
        for (int i = 0; i < imageFormat.keyCount(); i++) {
            formatBox->addItem(imageFormat.key(i), imageFormat.value(i));
        }
        const QMetaEnum Conversionflags = QMetaEnum::fromType<Qt::ImageConversionFlags>();
        for (int i = 0; i < Conversionflags.keyCount(); i++) {
            colorBox->addItem(Conversionflags.key(i), Conversionflags.value(i));
        }
    }
    QWidget *owner;
    ImageView *imageView;
    ImageView *imageViewFormat;
    QLabel *urlLabel;
    QLabel *fileSizeLabel;
    QLabel *sizeLabel;
    QLabel *scaleLabel;
    ImageInfoList imageInfoList;
    ImageListView *imageListView;

    QComboBox *formatBox;
    QComboBox *colorBox;
    bool runing = true;
    QFuture<void> watcher;
};

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ImageViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

ImageViewer::~ImageViewer()
{
    destroyImageLoadThread();
    clearThumbnail();
}

void ImageViewer::onOpenImage()
{
    QString imageFilters(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.svg *.tiff *.webp *.icns "
                            "*.bitmap *.graymap *.pixmap *.tga *.xbitmap *.xpixmap)"));
    qDebug() << imageFilters;
    const QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                             .value(0, QDir::homePath());
    const QString filename = QFileDialog::getOpenFileName(this,
                                                          tr("Open Image"),
                                                          path,
                                                          imageFilters);
    if (filename.isEmpty()) {
        return;
    }
    d_ptr->imageView->createScene(filename);
}

void ImageViewer::onMaskImage()
{
    QPixmap pixmap = d_ptr->imageView->pixmap();
    if (pixmap.isNull()) {
        return;
    }
    MaskDialog dialog(this);
    dialog.setPixmap(pixmap);
    dialog.setImageName(QFileInfo(d_ptr->urlLabel->text().trimmed()).fileName());
    dialog.exec();
}

void ImageViewer::onRoundImage()
{
    QPixmap pixmap = d_ptr->imageView->pixmap();
    if (pixmap.isNull()) {
        return;
    }
    RoundDialog dialog(this);
    dialog.setPixmap(pixmap);
    dialog.setImageName(QFileInfo(d_ptr->urlLabel->text().trimmed()).fileName());
    dialog.exec();
}

void ImageViewer::onScaleFactorChanged(qreal factor)
{
    const QString info = QString::number(factor * 100, 'f', 2) + QLatin1Char('%');
    d_ptr->scaleLabel->setText(info);
}

void ImageViewer::onImageSizeChanged(const QSize &size)
{
    QString imageSizeText;
    if (size.isValid()) {
        imageSizeText = QString::fromLatin1("%1x%2").arg(size.width()).arg(size.height());
    }
    d_ptr->sizeLabel->setText(imageSizeText);
}

void ImageViewer::onImageChanged(const QString &url)
{
    d_ptr->urlLabel->setText(url);
    d_ptr->fileSizeLabel->setText(Utils::bytesToString(QFile(url).size()));

    for (const ImageInfo &image : qAsConst(d_ptr->imageInfoList)) {
        if (image.fileInfo().absoluteFilePath() == url) {
            return;
        }
    }
    destroyImageLoadThread();
    clearThumbnail();
    startImageLoadThread(url);
}

void ImageViewer::onChangedImage(int index)
{
    d_ptr->imageView->createScene(d_ptr->imageInfoList.at(index).fileInfo().absoluteFilePath());
}

void ImageViewer::onImageLoaded(const QFileInfo &fileInfo, const QImage &image)
{
    if (image.isNull()) {
        return;
    }
    d_ptr->imageInfoList.append(ImageInfo(fileInfo, image));
    d_ptr->imageListView->setImageInfoList(d_ptr->imageInfoList);
}

void ImageViewer::onFormatChecked(bool state)
{
    d_ptr->formatBox->setVisible(state);
    d_ptr->imageViewFormat->setVisible(state);
    d_ptr->colorBox->setVisible(state);
}

void ImageViewer::onFormatChanged(const QString &)
{
    QImage::Format format = QImage::Format(d_ptr->formatBox->currentData().toInt());
    Qt::ImageConversionFlags flags = Qt::ImageConversionFlags(
        d_ptr->colorBox->currentData().toInt());

    QImage image = d_ptr->imageView->pixmap().toImage();
    QPixmap pixmap = QPixmap::fromImage(image.convertToFormat(format, flags));
    if (pixmap.isNull()) {
        MessBox::Warning(this, tr("Format Conversion Failed!"), MessBox::CloseButton);
        return;
    }
    d_ptr->imageViewFormat->setPixmap(pixmap);
}

void ImageViewer::startImageLoadThread(const QString &url)
{
    d_ptr->runing = true;
    d_ptr->watcher = QtConcurrent::run(&ImageViewer::imageLoad, this, url);
}

void ImageViewer::destroyImageLoadThread()
{
    if (d_ptr->watcher.isRunning()) {
        d_ptr->runing = false;
        d_ptr->watcher.waitForFinished();
    }
}

void ImageViewer::imageLoad(const QString &fileUrl)
{
    const QFileInfo file(fileUrl);
    const QFileInfoList list = file.absoluteDir().entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo &info : qAsConst(list)) {
        if (!d_ptr->runing) {
            break;
        }
        QImage image(info.absoluteFilePath());
        if (image.isNull()) {
            continue;
        }
        if (image.width() > WIDTH || image.height() > WIDTH) {
            image = image.scaled(WIDTH, WIDTH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        emit imageLoadReady(info, image);
    }
}

void ImageViewer::clearThumbnail()
{
    if (d_ptr->imageInfoList.isEmpty()) {
        return;
    }
    d_ptr->imageInfoList.clear();
    d_ptr->imageListView->setImageInfoList(d_ptr->imageInfoList);
}

void ImageViewer::setupUI()
{
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(d_ptr->imageView);
    splitter->addWidget(d_ptr->imageViewFormat);
    splitter->addWidget(toolWidget());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << INT_MAX << INT_MAX << 1);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(QMargins());
    layout->addWidget(splitter);
    layout->addWidget(d_ptr->imageListView);

    d_ptr->formatBox->hide();
    d_ptr->imageViewFormat->hide();
    d_ptr->colorBox->hide();
}

QWidget *ImageViewer::toolWidget()
{
    QPushButton *openImageButton = new QPushButton(tr("Open Picture"), this);
    openImageButton->setFlat(true);
    openImageButton->setObjectName("BlueButton");
    connect(openImageButton, &QPushButton::clicked, this, &ImageViewer::onOpenImage);

    QPushButton *maskImageButton = new QPushButton(tr("Mask Picture"), this);
    maskImageButton->setFlat(true);
    maskImageButton->setObjectName("BlueButton");
    connect(maskImageButton, &QPushButton::clicked, this, &ImageViewer::onMaskImage);

    QPushButton *roundImageButton = new QPushButton(tr("Round Picture"), this);
    roundImageButton->setFlat(true);
    roundImageButton->setObjectName("BlueButton");
    connect(roundImageButton, &QPushButton::clicked, this, &ImageViewer::onRoundImage);

    QGroupBox *infoBox = new QGroupBox(tr("Image Information"), this);
    QGridLayout *gridLayout = new QGridLayout(infoBox);
    gridLayout->addWidget(new QLabel(tr("Url: "), this), 0, 0, 1, 1);
    gridLayout->addWidget(d_ptr->urlLabel, 0, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("File Size: "), this), 1, 0, 1, 1);
    gridLayout->addWidget(d_ptr->fileSizeLabel, 1, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Image Size: "), this), 2, 0, 1, 1);
    gridLayout->addWidget(d_ptr->sizeLabel, 2, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Scaling Ratio:"), this), 3, 0, 1, 1);
    gridLayout->addWidget(d_ptr->scaleLabel, 3, 1, 1, 1);

    QCheckBox *formatBox = new QCheckBox(tr("Format"), this);
    connect(formatBox, &QCheckBox::clicked, this, &ImageViewer::onFormatChecked);
    QHBoxLayout *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(formatBox);
    formatLayout->addWidget(d_ptr->formatBox);
    formatLayout->addWidget(d_ptr->colorBox);

    QWidget *widget = new QWidget(this);
    widget->setObjectName("InfoWidget");
    QVBoxLayout *rightLayout = new QVBoxLayout(widget);
    rightLayout->addWidget(openImageButton);
    rightLayout->addWidget(infoBox);
    rightLayout->addLayout(formatLayout);
    rightLayout->addWidget(maskImageButton);
    rightLayout->addWidget(roundImageButton);
    rightLayout->addStretch();

    return widget;
}

void ImageViewer::buildConnect()
{
    connect(d_ptr->imageView,
            &ImageView::scaleFactorChanged,
            this,
            &ImageViewer::onScaleFactorChanged);
    connect(d_ptr->imageView, &ImageView::imageSizeChanged, this, &ImageViewer::onImageSizeChanged);
    connect(d_ptr->imageView, &ImageView::imageUrlChanged, this, &ImageViewer::onImageChanged);
    connect(d_ptr->imageListView, &ImageListView::changeItem, this, &ImageViewer::onChangedImage);
    connect(d_ptr->formatBox, &QComboBox::currentTextChanged, this, &ImageViewer::onFormatChanged);
    connect(d_ptr->colorBox, &QComboBox::currentTextChanged, this, &ImageViewer::onFormatChanged);
    connect(this, &ImageViewer::imageLoadReady, this, &ImageViewer::onImageLoaded);
}
