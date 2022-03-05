#ifndef LABELFRAME_H
#define LABELFRAME_H

#include <QFrame>

class LabelFrame : public QWidget
{
    Q_OBJECT
public:
    explicit LabelFrame(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // LABELFRAME_H
