#ifndef MPAGES_H
#define MPAGES_H

#include "core_global.h"

#include <QObject>

class QPushButton;

namespace Core {

class CORE_EXPORT MPages : public QObject
{
    Q_OBJECT
public:
    enum Group { Tool, About, Test };

    explicit MPages(QObject *parent = nullptr);
    ~MPages();

    QPushButton* button() const;
    QWidget* widget() const;

protected:
    void setWidget(QWidget*);
    void setButtonName(const QString&);
    void setButtonGroup(Group);
    void setButtonObejctName(const QString& objectname);

private:
    class MPagesPrivate;
    QScopedPointer<MPagesPrivate> d_ptr;
};

}

#endif // MPAGES_H
