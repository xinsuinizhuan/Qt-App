#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QObject>

namespace AccountSystem {

class AccountQuery : public QObject
{
public:
    explicit AccountQuery(QObject *parent = nullptr);
    ~AccountQuery();

    QStringList userNameList() const;

    bool contains(const QString &);
    bool checkAccount(const QString&, const QString&);
    bool addAccount(const QString&, const QString&);
    bool updateAccount(const QString&, const QString&);
    bool deleteAccount(const QString &);

private:
    void init();
    void loadparam();
    void allUsername();

    class AccountQueryPrivate;
    QScopedPointer<AccountQueryPrivate> d_ptr;
};

}

#endif // SQLQUERY_H
