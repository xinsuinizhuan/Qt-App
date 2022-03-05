#include "playerwidget.h"
#include "mediawidget.h"
#include "playcontrolwidget.h"
#include "playlistview.h"

#include <QAudioOutput>
#include <QKeyEvent>
#include <QMediaFormat>
#include <QMediaMetaData>
#include <QtWidgets>

class PlayerWidget::PlayerWidgetPrivate
{
public:
    PlayerWidgetPrivate(QWidget *parent)
        : owner(parent)
        , mediaPlayer(new QMediaPlayer)
        , audioOutput(new QAudioOutput)
    {
        playControls = new PlayControlWidget(parent);
        playListWidget = new PlayListView(parent);
        mediaWidget = new MediaWidget(parent);

        mediaPlayer->setAudioOutput(audioOutput.data());
        mediaPlayer->setVideoOutput(mediaWidget);

        playControls->setProcessValue(mediaPlayer->duration());
        playControls->setVolume(audioOutput->volume());
        playControls->setState(mediaPlayer->playbackState());
        playControls->setMuted(audioOutput->isMuted());
    }
    QWidget *owner;
    QScopedPointer<QMediaPlayer> mediaPlayer;
    QScopedPointer<QAudioOutput> audioOutput;
    MediaWidget *mediaWidget;
    PlayControlWidget *playControls;
    PlayListView *playListWidget;

    QLabel *coverLabel = nullptr;

    QString trackInfo;
    QString statusInfo;
};

PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new PlayerWidgetPrivate(this))
{
    QMediaFormat mediaFormat;
    qInfo() << tr("Support Encode AudioCodec:")
            << mediaFormat.supportedAudioCodecs(QMediaFormat::Encode);
    qInfo() << tr("Support Decode AudioCodec:")
            << mediaFormat.supportedAudioCodecs(QMediaFormat::Decode);
    qInfo() << tr("Support Encode FileFormats:")
            << mediaFormat.supportedFileFormats(QMediaFormat::Encode);
    qInfo() << tr("Support Decode FileFormats:")
            << mediaFormat.supportedFileFormats(QMediaFormat::Decode);
    qInfo() << tr("Support Encode AudioCodec:")
            << mediaFormat.supportedVideoCodecs(QMediaFormat::Encode);
    qInfo() << tr("Support Decode AudioCodec:")
            << mediaFormat.supportedVideoCodecs(QMediaFormat::Decode);

    setupUI();
    buildConnect();

    if (!isPlayerAvailable()) {
        QMessageBox::warning(this,
                             tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"
                                "Please check the media service plugins are installed."));

        d_ptr->playControls->setEnabled(false);
        d_ptr->playListWidget->setEnabled(false);
    }

    metaDataChanged();
    setPlayButtonEnable();
}

PlayerWidget::~PlayerWidget() {}

void PlayerWidget::onPlay(bool play)
{
    if (play) {
        d_ptr->mediaPlayer->play();
    } else {
        d_ptr->mediaPlayer->pause();
    }
}

void PlayerWidget::metaDataChanged()
{
    auto metaData = d_ptr->mediaPlayer->metaData();
    setTrackInfo(QString("%1 - %2")
                     .arg(metaData.value(QMediaMetaData::AlbumArtist).toString())
                     .arg(metaData.value(QMediaMetaData::Title).toString()));

    if (!d_ptr->coverLabel) {
        return;
    }
    for (auto &key : metaData.keys()) {
        if (key == QMediaMetaData::CoverArtImage) {
            QVariant v = metaData.value(key);
            QImage coverImage = v.value<QImage>();
            d_ptr->coverLabel->setPixmap(QPixmap::fromImage(coverImage));
        } else if (key == QMediaMetaData::ThumbnailImage) {
            QVariant v = metaData.value(key);
            QImage thumbnailImage = v.value<QImage>();
            d_ptr->coverLabel->setPixmap(QPixmap::fromImage(thumbnailImage));
        }
    }
}

void PlayerWidget::onPrevious()
{
    if (d_ptr->playListWidget->mediaList()->isEmpty()
        || d_ptr->mediaPlayer->playbackState() == QMediaPlayer::StoppedState) {
        return;
    }
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if (d_ptr->mediaPlayer->position() <= 5000) {
        d_ptr->playListWidget->onPrevious();
    } else {
        d_ptr->mediaPlayer->setPosition(0);
    }
}

void PlayerWidget::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);
    // handle status message
    switch (status) {
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia: setStatusInfo(QString()); break;
    case QMediaPlayer::LoadingMedia: setStatusInfo(tr("Loading...")); break;
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(tr("Buffering %1%").arg(qRound(d_ptr->mediaPlayer->bufferProgress() * 100.)));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Stalled %1%").arg(qRound(d_ptr->mediaPlayer->bufferProgress() * 100.)));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        d_ptr->playListWidget->mediaList()->next();
        break;
    case QMediaPlayer::InvalidMedia: displayErrorMessage(); break;
    }
}

void PlayerWidget::playbackStateChanged(QMediaPlayer::PlaybackState state)
{
    d_ptr->playControls->setState(state);
    //if (state == QMediaPlayer::StoppedState)
    //   clearHistogram();
}

void PlayerWidget::bufferingProgress(int progress)
{
    if (d_ptr->mediaPlayer->mediaStatus() == QMediaPlayer::StalledMedia) {
        setStatusInfo(tr("Stalled %1%").arg(progress));
    } else {
        setStatusInfo(tr("Buffering %1%").arg(progress));
    }
}

void PlayerWidget::onSeek(int seconds)
{
    d_ptr->mediaPlayer->setPosition(seconds * 1000);
}

void PlayerWidget::displayErrorMessage()
{
    setStatusInfo(d_ptr->mediaPlayer->errorString());
}

void PlayerWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        d_ptr->playControls->onPlay();
    }
}

void PlayerWidget::setPlayListVisiable()
{
    d_ptr->playListWidget->setVisible(!d_ptr->playListWidget->isVisible());
}

void PlayerWidget::setupUI()
{
    // mediaPlayer->setVideoOutput(mediaWidget); 使下面代码效果不好
    //QVBoxLayout *mediaLayout = new QVBoxLayout(d_ptr->mediaWidget);
    //mediaLayout->setContentsMargins(0, 0, 0, 0);
    //mediaLayout->setSpacing(0);
    //mediaLayout->addStretch();
    //mediaLayout->addWidget(d_ptr->playControls);

    QWidget *leftWidget = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(d_ptr->mediaWidget);
    leftLayout->addWidget(d_ptr->playControls);

    QSplitter *splitter = new QSplitter(this);
    splitter->setHandleWidth(0);
    splitter->addWidget(leftWidget);
    splitter->addWidget(d_ptr->playListWidget);
    splitter->setSizes(QList<int>() << 1000 << 1);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(splitter);
}

void PlayerWidget::buildConnect()
{
    connect(d_ptr->playListWidget, &PlayListView::play, this, &PlayerWidget::onSetSource);

    connect(d_ptr->mediaPlayer.data(),
            QOverload<>::of(&QMediaPlayer::metaDataChanged),
            this,
            &PlayerWidget::metaDataChanged);
    connect(d_ptr->mediaPlayer.data(),
            &QMediaPlayer::mediaStatusChanged,
            this,
            &PlayerWidget::statusChanged);
    connect(d_ptr->mediaPlayer.data(),
            &QMediaPlayer::bufferProgressChanged,
            this,
            &PlayerWidget::bufferingProgress);
    connect(d_ptr->mediaPlayer.data(),
            &QMediaPlayer::hasVideoChanged,
            d_ptr->mediaWidget,
            &MediaWidget::setFullScreen);
    connect(d_ptr->mediaPlayer.data(),
            &QMediaPlayer::errorChanged,
            this,
            &PlayerWidget::displayErrorMessage);
    connect(d_ptr->mediaPlayer.data(),
            &QMediaPlayer::playbackStateChanged,
            this,
            &PlayerWidget::playbackStateChanged);
    connect(d_ptr->audioOutput.data(),
            &QAudioOutput::volumeChanged,
            d_ptr->playControls,
            &PlayControlWidget::setVolume);
    connect(d_ptr->audioOutput.data(),
            &QAudioOutput::mutedChanged,
            d_ptr->playControls,
            &PlayControlWidget::setMuted);
    connect(d_ptr->mediaPlayer.data(),
            &QMediaPlayer::durationChanged,
            d_ptr->playControls,
            &PlayControlWidget::durationChanged);
    connect(d_ptr->mediaPlayer.data(),
            &QMediaPlayer::positionChanged,
            d_ptr->playControls,
            &PlayControlWidget::positionChanged);

    connect(d_ptr->playControls,
            &PlayControlWidget::showList,
            this,
            &PlayerWidget::setPlayListVisiable);
    connect(d_ptr->playControls, &PlayControlWidget::previous, this, &PlayerWidget::onPrevious);
    connect(d_ptr->playControls,
            &PlayControlWidget::next,
            d_ptr->playListWidget,
            &PlayListView::onNext);
    connect(d_ptr->playControls,
            &PlayControlWidget::play,
            d_ptr->mediaPlayer.data(),
            &QMediaPlayer::play);
    connect(d_ptr->playControls,
            &PlayControlWidget::pause,
            d_ptr->mediaPlayer.data(),
            &QMediaPlayer::pause);
    connect(d_ptr->playControls,
            &PlayControlWidget::stop,
            d_ptr->mediaPlayer.data(),
            &QMediaPlayer::stop);
    connect(d_ptr->playControls, &PlayControlWidget::seek, this, &PlayerWidget::onSeek);
    connect(d_ptr->playControls,
            &PlayControlWidget::changeVolume,
            d_ptr->audioOutput.data(),
            &QAudioOutput::setVolume);
    connect(d_ptr->playControls,
            &PlayControlWidget::changeMuting,
            d_ptr->audioOutput.data(),
            &QAudioOutput::setMuted);
    connect(d_ptr->playControls,
            &PlayControlWidget::changeRate,
            d_ptr->mediaPlayer.data(),
            &QMediaPlayer::setPlaybackRate);
    connect(d_ptr->playControls,
            &PlayControlWidget::stop,
            d_ptr->mediaWidget,
            QOverload<>::of(&QVideoWidget::update));

    connect(d_ptr->mediaWidget, &MediaWidget::previous, this, &PlayerWidget::onPrevious);
    connect(d_ptr->mediaWidget, &MediaWidget::next, d_ptr->playListWidget, &PlayListView::onNext);
    connect(d_ptr->mediaWidget, &MediaWidget::play, d_ptr->playControls, &PlayControlWidget::onPlay);
    connect(d_ptr->mediaWidget,
            &MediaWidget::addMedia,
            d_ptr->playListWidget,
            &PlayListView::onOpenMedia);
    connect(d_ptr->mediaWidget,
            &MediaWidget::addMediaUrls,
            d_ptr->playListWidget,
            &PlayListView::onAddMedia);

    connect(d_ptr->playListWidget,
            &PlayListView::playListChanged,
            this,
            &PlayerWidget::setPlayButtonEnable);
}

void PlayerWidget::setPlayButtonEnable()
{
    if (d_ptr->playListWidget->mediaList()->isEmpty()) {
        d_ptr->playControls->setPlayButtonEnable(false);
    } else {
        d_ptr->playControls->setPlayButtonEnable(true);
    }
}

void PlayerWidget::onSetSource()
{
    d_ptr->mediaPlayer->setSource(d_ptr->playListWidget->mediaList()->currentMedia());
}

void PlayerWidget::setTrackInfo(const QString &info)
{
    d_ptr->trackInfo = info;
}

void PlayerWidget::setStatusInfo(const QString &info)
{
    d_ptr->statusInfo = info;
}

void PlayerWidget::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia || status == QMediaPlayer::BufferingMedia
        || status == QMediaPlayer::StalledMedia) {
        setCursor(QCursor(Qt::BusyCursor));
    } else {
        unsetCursor();
    }
#endif
}

bool PlayerWidget::isPlayerAvailable() const
{
    return d_ptr->mediaPlayer->isAvailable();
}
