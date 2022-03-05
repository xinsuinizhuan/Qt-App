#ifndef CRASHDIALOG_H
#define CRASHDIALOG_H

#include <controls/dialog.hpp>

using namespace Control;

namespace Utils {

class CrashDialog : public Dialog
{
    Q_OBJECT
public:
    explicit CrashDialog(QWidget *parent = nullptr);
    ~CrashDialog();

private slots:
    void onOpenCrashPath();
    void onRestart();
    void onQuit();

private:
    void setupUI();
    void init();
};

}

#endif // CRASHDIALOG_H
