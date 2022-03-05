#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include "qmediaplaylist.h"

#include <QMediaPlayer>
#include <QWidget>

class PlayControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayControlWidget(QWidget *parent = nullptr);
    ~PlayControlWidget();

    void setPlayButtonEnable(bool enable);
    QMediaPlayer::PlaybackState state() const;
    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;

public slots:
    void onPlay();

    void setProcessValue(int offset);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

    void setState(QMediaPlayer::PlaybackState state);
    void setVolume(int volume);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void seek(int seconds);
    void changeVolume(int volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);
    void showList();

private slots:
    void onMuted(bool);
    void updateRate();
    void onVolumeSliderValueChanged();

private:
    void setupUI();
    void buildConnect();
    void updateDurationInfo(qint64 currentInfo);

    class PlayControlWidgetPrivate;
    QScopedPointer<PlayControlWidgetPrivate> d_ptr;
};

#endif // PLAYCONTROLWIDGET_H
