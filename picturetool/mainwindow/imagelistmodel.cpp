#include "imagelistmodel.h"

#include <QScrollBar>

struct ImageListModel::ImageListModelPrivate
{
    ImageInfoList imageInfoList;
};

ImageListModel::ImageListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d_ptr(new ImageListModelPrivate)
{}

ImageListModel::~ImageListModel() {}

int ImageListModel::rowCount(const QModelIndex &) const
{
    return d_ptr->imageInfoList.size();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    ImageInfo imageInfo = d_ptr->imageInfoList.at(index.row());
    switch (role) {
    case Qt::DecorationRole: return imageInfo.image();
    case Qt::WhatsThisRole:
    case Qt::ToolTipRole: return imageInfo.fileInfo().fileName();
    case Qt::SizeHintRole: return QSize(WIDTH, WIDTH);
    case Qt::TextAlignmentRole: return Qt::AlignCenter;
    default: break;
    }
    return QVariant();
}

void ImageListModel::setImageInfoList(const ImageInfoList &imageInfoList)
{
    beginResetModel();
    d_ptr->imageInfoList = imageInfoList;
    endResetModel();
}

/*
 *
 * */

class ImageListView::ImageListViewPrivate
{
public:
    ImageListViewPrivate(QWidget *parent)
        : owner(parent)
    {
        imageListModel = new ImageListModel(owner);
    }
    QWidget *owner;
    ImageListModel *imageListModel;
};

ImageListView::ImageListView(QWidget *parent)
    : QListView(parent)
    , d_ptr(new ImageListViewPrivate(this))
{
    setupUI();
    connect(this, &QListView::doubleClicked, this, &ImageListView::onChangedItem);
}

ImageListView::~ImageListView() {}

void ImageListView::setImageInfoList(const ImageInfoList &imageVector)
{
    d_ptr->imageListModel->setImageInfoList(imageVector);
}

void ImageListView::onChangedItem(const QModelIndex &index)
{
    emit changeItem(index.row());
}

void ImageListView::setupUI()
{
    setFlow(LeftToRight);
    setResizeMode(Adjust);

    setModel(d_ptr->imageListModel);
}
