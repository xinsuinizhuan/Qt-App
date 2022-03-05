#include "accountcontrols.h"
#include "messbox.h"

#include <QtWidgets>

namespace Control {

class TableView : public QTableView
{
public:
    explicit TableView(QWidget *parent = nullptr)
        : QTableView(parent)
    {
        setShowGrid(false);
        setWordWrap(true);
        setAlternatingRowColors(true);
        verticalHeader()->hide();
        verticalHeader()->setDefaultSectionSize(50);
        horizontalHeader()->hide();
        horizontalHeader()->setStretchLastSection(true);
        horizontalHeader()->setDefaultSectionSize(30);
        horizontalHeader()->setMinimumSectionSize(30);
        horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setSelectionMode(QAbstractItemView::SingleSelection);
    }
};

class EditComboBox::EditComboBoxPrivate
{
public:
    EditComboBoxPrivate(QWidget *parent)
        : owner(parent)
        , tableView(new TableView(owner))
        , accountModel(new AccountModel(owner))
        , completer(new QCompleter(accountModel, owner))
        , completerTableView(new TableView(owner))
    {
        completer->setMaxVisibleItems(3);
        completer->setCompletionColumn(1);
        completer->setPopup(completerTableView);
        completerTableView->setItemDelegate(new QStyledItemDelegate(completerTableView));
    }

    ~EditComboBoxPrivate()
    {
        if (accountDatas.isEmpty())
            return;
        qDeleteAll(accountDatas);
        accountDatas.clear();
    }

    QWidget *owner;
    TableView *tableView;
    AccountModel *accountModel;
    QVector<AccountData *> accountDatas;
    QCompleter *completer;
    TableView *completerTableView;
    QString currentText;
};

EditComboBox::EditComboBox(QWidget *parent)
    : QComboBox(parent)
    , d_ptr(new EditComboBoxPrivate(this))
{
    setupUI();
}

EditComboBox::~EditComboBox() {}

void EditComboBox::addAccount(const QString &username)
{
    QImage image(qApp->applicationDirPath() + "/resource/image/plugintemplate/avatar.png");
    image = image.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    d_ptr->accountDatas.append(new AccountData{image, username});
    d_ptr->accountModel->setAccounts(d_ptr->accountDatas);
    showText(d_ptr->accountDatas.size() - 1);
}

QStringList EditComboBox::accountList() const
{
    QStringList list;
    for (const AccountData *account : qAsConst(d_ptr->accountDatas)) {
        list.append(account->name);
    }
    return list;
}

void EditComboBox::resizeEvent(QResizeEvent *resizeEvent)
{
    QComboBox::resizeEvent(resizeEvent);
    d_ptr->tableView->setColumnWidth(1, width() - 30 - 30 - 10);
    d_ptr->completerTableView->setColumnWidth(1, width() - 30 - 30 - 10);
}

void EditComboBox::showText(int row)
{
    if (row < 0 || row >= d_ptr->accountDatas.size())
        return;
    setEditText(d_ptr->accountDatas.at(row)->name);
}

void EditComboBox::onDeleteItem(int row)
{
    hidePopup();
    Dialog::ExecFlags flag = MessBox::Warning(this,
                                              tr("Confirm to delete the current account?"),
                                              MessBox::YesAndNoButton);
    showPopup();
    if (flag == Dialog::Rejected)
        return;
    if (row < 0 || row >= d_ptr->accountDatas.size())
        return;
    AccountData *accountData = d_ptr->accountDatas.takeAt(row);
    if (!accountData)
        return;
    delete accountData;
    d_ptr->accountModel->setAccounts(d_ptr->accountDatas);
}

void EditComboBox::setupUI()
{
    ButtonDelegate *buttonDelegate = new ButtonDelegate(this);
    connect(buttonDelegate,
            &ButtonDelegate::deleteMe,
            this,
            &EditComboBox::onDeleteItem,
            Qt::QueuedConnection);
    d_ptr->tableView->setItemDelegateForColumn(2, buttonDelegate);
    d_ptr->completerTableView->setItemDelegateForColumn(2, buttonDelegate);

    setMaxVisibleItems(3);
    setInsertPolicy(QComboBox::NoInsert);
    setModel(d_ptr->accountModel);
    setView(d_ptr->tableView);
    setEditable(true);
    setCompleter(d_ptr->completer);
}

PasswordLineEdit::PasswordLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setPlaceholderText(tr("Please enter the password."));
    setEchoMode(QLineEdit::Password);
    QToolButton *button = new QToolButton(this);
    button->setObjectName("ShowPasswordButton");
    button->setCursor(Qt::PointingHandCursor);
    button->setCheckable(true);
    connect(button, &QToolButton::clicked, this, &PasswordLineEdit::onShowPassword);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(1, 1, 10, 1);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(button);
}

void PasswordLineEdit::onShowPassword(bool state)
{
    setEchoMode(state ? QLineEdit::Normal : QLineEdit::Password);
}

class AvatarWidget::AvatarWidgetPrivate
{
public:
    AvatarWidgetPrivate(QWidget *parent)
        : owner(parent)
    {
        avatarLabel = new QLabel(owner);
        avatarLabel->setText(QObject::tr("Avatar"));
        avatarLabel->setObjectName("AvatarLabel");
    }
    QWidget *owner;
    QLabel *avatarLabel;
};

AvatarWidget::AvatarWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new AvatarWidgetPrivate(this))
{
    QGridLayout *avatarLayout = new QGridLayout(this);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 1);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 3);
    avatarLayout->addWidget(d_ptr->avatarLabel, 1, 2);
}

AvatarWidget::~AvatarWidget() {}

void AvatarWidget::setIcon(const QPixmap &icon)
{
    d_ptr->avatarLabel->setPixmap(icon);
}

AccountModel::AccountModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_deleteIcon = QIcon(qApp->applicationDirPath() + "/resource/image/base/controls.png");
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return false;

    int row = index.row();
    int col = index.column();

    AccountData *account = m_accounts.at(row);

    switch (role) {
    case Qt::DecorationRole:
        if (col == 0) {
            return account->avator;
        } else if (col == 2) {
            return m_deleteIcon;
        }
        break;
    case Qt::SizeHintRole:
        if (col == 0) {
            return QSize(30, 30);
        }
        break;
    case Qt::DisplayRole:
        if (col == 1) {
            return account->name;
        }
        break;
    case Qt::ToolTipRole:
        if (col == 0 || col == 1) {
            return account->name;
        } else {
            return tr("Delete");
        }
    case Qt::EditRole:
    case Qt::WhatsThisRole:
    case Qt::UserRole:
    case Qt::StatusTipRole: return account->name;
    case Qt::TextAlignmentRole: return Qt::AlignVCenter;
    default: break;
    }

    return QVariant();
}

void AccountModel::setAccounts(const QVector<AccountData *> &accounts)
{
    beginResetModel();
    m_accounts = accounts;
    endResetModel();
}

bool ButtonDelegate::editorEvent(QEvent *event,
                                 QAbstractItemModel *model,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: emit deleteMe(index.row()); break;
    default: break;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

} // namespace Control
