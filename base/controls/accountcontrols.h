#ifndef EIDTCOMBOBOX_H
#define EIDTCOMBOBOX_H

#include "controls_global.h"

#include <QComboBox>
#include <QLineEdit>
#include <QStyledItemDelegate>

namespace Control {

class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ButtonDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {}

signals:
    void deleteMe(int row); // Qt::QueuedConnection

protected:
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};

struct AccountData
{
    QImage avator;
    QString name;
};

class AccountModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AccountModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & = QModelIndex()) const { return m_accounts.size(); }
    int columnCount(const QModelIndex & = QModelIndex()) const { return 3; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setAccounts(const QVector<AccountData *> &accounts);

private:
    QVector<AccountData *> m_accounts;
    QIcon m_deleteIcon;
};

class CONTROLS_EXPORT EditComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit EditComboBox(QWidget *parent = nullptr);
    ~EditComboBox();

    void addAccount(const QString &);
    QStringList accountList() const;

private slots:
    void onDeleteItem(int row);

protected:
    void resizeEvent(QResizeEvent *resizeEvent) override;

private:
    void setupUI();
    void showText(int row);

    class EditComboBoxPrivate;
    QScopedPointer<EditComboBoxPrivate> d_ptr;
};

class CONTROLS_EXPORT PasswordLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit PasswordLineEdit(QWidget *parent = nullptr);

private slots:
    void onShowPassword(bool);
};

class CONTROLS_EXPORT AvatarWidget : public QWidget
{
public:
    explicit AvatarWidget(QWidget *parent = nullptr);
    ~AvatarWidget();

    void setIcon(const QPixmap &);

private:
    class AvatarWidgetPrivate;
    QScopedPointer<AvatarWidgetPrivate> d_ptr;
};

} // namespace Control

#endif // EIDTCOMBOBOX_H
