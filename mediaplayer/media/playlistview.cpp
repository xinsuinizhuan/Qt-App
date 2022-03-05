#include "playlistview.h"
#include "playlistmodel.h"
#include "qmediaplaylist.h"

#include <QAbstractItemView>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include <QStandardPaths>

class PlayListView::PlayListViewPrivate
{
public:
    PlayListViewPrivate(QWidget *parent)
        : owner(parent)
    {
        mediaList = new QMediaPlaylist;
        QFile file("./config/MediaList.txt");
        file.open(QIODevice::ReadOnly);
        mediaList->load(&file, "%s");
        playListModel = new PlaylistModel(owner);
        playListModel->setPlaylist(mediaList);
        menu = new QMenu(owner);
    }
    QWidget *owner;
    QMediaPlaylist *mediaList;
    PlaylistModel *playListModel;
    QMenu *menu;
    const QStringList filters = QStringList(
        {"All support files (*.avi *.mkv *.mp4 *.rmvb *.mp3 *.flac *.ape *.m4a)",
         "Video files (*.avi *.mkv *.mp4 *.rmvb)",
         "Music files (*.mp3 *.flac *.ape *.m4a)"});
};

PlayListView::PlayListView(QWidget *parent)
    : QListView(parent)
    , d_ptr(new PlayListViewPrivate(this))
{
    horizontalScrollBar()->setHidden(true);
    setModel(d_ptr->playListModel);
    setCurrentIndex(d_ptr->playListModel->index(d_ptr->mediaList->currentIndex(), 0));
    connect(this, &QAbstractItemView::activated, this, &PlayListView::onJump);
    connect(d_ptr->mediaList,
            &QMediaPlaylist::currentIndexChanged,
            this,
            &PlayListView::playlistPositionChanged);
    initMenu();
}

PlayListView::~PlayListView()
{
    QFile file("./config/MediaList.txt");
    file.open(QIODevice::WriteOnly);
    d_ptr->mediaList->save(&file, "%s");
}

QMediaPlaylist *PlayListView::mediaList()
{
    return d_ptr->mediaList;
}

void PlayListView::onOpenMedia()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));
    fileDialog.setNameFilters(d_ptr->filters);
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)
                                .value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted) {
        onAddMedia(fileDialog.selectedUrls());
    }
}

bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile()) {
        return false;
    }
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists()
           && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void PlayListView::onAddMedia(const QList<QUrl> &urls)
{
    for (auto &url : qAsConst(urls)) {
        if (isPlaylist(url)) {
            d_ptr->mediaList->load(url);
        } else {
            d_ptr->mediaList->addMedia(url);
        }
        showError();
    }
    emit playListChanged();
}

void PlayListView::onRemoveMedia()
{
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return;
    }
    int row = index.row();
    d_ptr->mediaList->removeMedia(row, row);
    emit playListChanged();
}

void PlayListView::onRemoveAllMedia()
{
    d_ptr->mediaList->clear();
    emit playListChanged();
}

void PlayListView::onPrevious()
{
    if (d_ptr->mediaList->isEmpty() || !d_ptr->mediaList->previousIndex()) {
        return;
    }
    d_ptr->mediaList->previous();
}

void PlayListView::onJump(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    d_ptr->mediaList->setCurrentIndex(index.row());
    emit play();
}

void PlayListView::onNext()
{
    if (d_ptr->mediaList->isEmpty() || !d_ptr->mediaList->nextIndex()) {
        return;
    }
    d_ptr->mediaList->next();
}

void PlayListView::playlistPositionChanged(int currentItem)
{
    //clearHistogram();
    setCurrentIndex(d_ptr->playListModel->index(currentItem, 0));
    emit play();
}

void PlayListView::contextMenuEvent(QContextMenuEvent *event)
{
    d_ptr->menu->exec(mapToGlobal(event->pos()));
}

void PlayListView::showError()
{
    if (d_ptr->mediaList->error() == QMediaPlaylist::NoError) {
        return;
    }
    qDebug() << d_ptr->mediaList->errorString();
    //QMessageBox::warning(this,
    //                     tr("PlayListError"),
    //                     d_ptr->mediaList->errorString());
}

void PlayListView::initMenu()
{
    d_ptr->menu->addAction(tr("Add Media File"), this, &PlayListView::onOpenMedia);
    d_ptr->menu->addAction(tr("Remove Media File"), this, &PlayListView::onRemoveMedia);
    d_ptr->menu->addAction(tr("Remove All Media File"), this, &PlayListView::onRemoveMedia);
}
