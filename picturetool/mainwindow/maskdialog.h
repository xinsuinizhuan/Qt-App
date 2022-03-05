#ifndef MASKDIALOG_H
#define MASKDIALOG_H

#include <controls/dialog.hpp>

using namespace Control;

class MaskDialog : public Dialog
{
    Q_OBJECT
public:
    explicit MaskDialog(QWidget *parent = nullptr);
    ~MaskDialog();

    void setImageName(const QString &name);

    void setPixmap(const QPixmap& pixmap);
    QImage maskImage();
    void setMaskImage(const QImage& image);

private slots:
    void onButtonClicked(int id);
    void onPenSizeChanged(int value);
    void onOpacityChanged(double value);
    void onSave();

private:
    void setupUI();
    void buildConnect();

    class MaskDialogPrivate;
    QScopedPointer<MaskDialogPrivate> d_ptr;
};

#endif // MASKDIALOG_H
