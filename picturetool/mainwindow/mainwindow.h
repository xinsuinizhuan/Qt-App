#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_global.h"

#include <controls/commonwidget.hpp>

using namespace Control;

class MAINWINDOW_EXPORT MainWindow : public CommonWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void initToolBar();

    class MainWindowPrivate;
    QScopedPointer<MainWindowPrivate> d_ptr;
};
#endif // MAINWINDOW_H
