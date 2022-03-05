#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <controls/commonwidget.hpp>

using namespace Control;

class QStackedWidget;
class QAbstractButton;
class MainWindow : public CommonWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void extensionsInitialized();

public slots:
    void onAccount(bool);

private slots:
    void onShowGroupButton(QAbstractButton *);
    void onAboutPlugins();

private:
    void setupUI();
    void initToolBar();
    QWidget *menuWidget();
    void initPageWidget(const QString &);
    void initMenu();
    void createTray();

    class MainWindowPrivate;
    QScopedPointer<MainWindowPrivate> d_ptr;
};
#endif // MAINWINDOW_H
