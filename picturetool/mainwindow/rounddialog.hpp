#ifndef ROUNDDIALOG_HPP
#define ROUNDDIALOG_HPP

#include <controls/dialog.hpp>

using namespace Control;

class RoundDialog : public Dialog
{
    Q_OBJECT
public:
    explicit RoundDialog(QWidget *parent = nullptr);
    ~RoundDialog();

    void setImageName(const QString &name);

    void setPixmap(const QPixmap &pixmap);

private slots:
    void onStartRound(bool checked);
    void onSave();
    void onButtonClicked(int id);
    void onRectChanged(const QRectF &rectF);
    void onTopLeftXChanged(int value);
    void onTopLeftYChanged(int value);
    void onWidthChanged(int value);
    void onHeightChanged(int value);
    void onRadiusChanged(int value);

private:
    void setupUI();
    void buildConnect();
    void buildConnect2();
    QWidget *toolWidget();

    class RoundDialogPrivate;
    QScopedPointer<RoundDialogPrivate> d_ptr;
};

#endif // ROUNDDIALOG_HPP
