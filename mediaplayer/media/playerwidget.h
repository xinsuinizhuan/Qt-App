#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "qmediaplaylist.h"

#include <QMediaPlayer>
#include <QWidget>

class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerWidget(QWidget *parent = nullptr);
    ~PlayerWidget();

private slots:
    void onPrevious();
    void onPlay(bool);
    void onSeek(int seconds);
    void metaDataChanged();

    void statusChanged(QMediaPlayer::MediaStatus status);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void bufferingProgress(int progress);

    void setPlayListVisiable();

    void displayErrorMessage();
    void setPlayButtonEnable();

    void onSetSource();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI();
    void buildConnect();
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    bool isPlayerAvailable() const;

    class PlayerWidgetPrivate;
    QScopedPointer<PlayerWidgetPrivate> d_ptr;
};

#endif // PLAYERWIDGET_H
