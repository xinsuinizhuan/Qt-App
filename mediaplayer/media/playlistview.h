#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QListView>
#include <QUrl>

class QMediaPlaylist;
class PlayListView : public QListView
{
    Q_OBJECT
public:
    explicit PlayListView(QWidget *parent = nullptr);
    ~PlayListView();

    QMediaPlaylist *mediaList();

signals:
    void play();
    void playListChanged();

public slots:
    void onOpenMedia();
    void onAddMedia(const QList<QUrl> &urls);
    void onRemoveMedia();
    void onRemoveAllMedia();
    void onPrevious();
    void onJump(const QModelIndex &index);
    void onNext();

private slots:
    void playlistPositionChanged(int);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void showError();
    void initMenu();

    class PlayListViewPrivate;
    QScopedPointer<PlayListViewPrivate> d_ptr;
};

#endif // PLAYLISTVIEW_H
