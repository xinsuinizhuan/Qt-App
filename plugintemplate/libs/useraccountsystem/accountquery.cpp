#include "accountquery.h"

#include <utils/datbasesql.h>
#include <utils/utils.h>

#include <QDebug>
#include <QSqlError>
#include <QVariant>

using namespace Utils;

namespace AccountSystem {

class AccountQuery::AccountQueryPrivate
{
public:
    AccountQueryPrivate(QObject *parent)
        : owner(parent)
    {
        db = new DatabaseSQL(owner);
    }
    QObject *owner;
    DatabaseSQL *db;
    DatabaseParam dbParam;
    QStringList userNameList;
};

AccountQuery::AccountQuery(QObject *parent)
    : QObject(parent)
    , d_ptr(new AccountQueryPrivate(this))
{
    loadparam();
    if (!d_ptr->db->openSQL(d_ptr->dbParam)) {
        qWarning() << d_ptr->db->errorString();
        return;
    }
    init();
    allUsername();
}

AccountQuery::~AccountQuery() {}

QStringList AccountQuery::userNameList() const
{
    return d_ptr->userNameList;
}

bool AccountQuery::contains(const QString &username)
{
    return d_ptr->userNameList.contains(username);
}

bool AccountQuery::checkAccount(const QString &username, const QString &password)
{
    const QString sql = QLatin1String("SELECT password FROM account WHERE username = '%1'")
                            .arg(username);
    QSqlQuery query = d_ptr->db->query(sql);
    if (query.next() && query.value("password").toString() == password) {
        return true;
    }
    qWarning() << query.value("password").toString() << password;
    return false;
}

bool AccountQuery::addAccount(const QString &username, const QString &password)
{
    const QString sql
        = QLatin1String(
              "INSERT INTO account (username, password, timestamp) VALUES('%1', '%2', '%3')")
              .arg(username, password, QString::number(QDateTime::currentSecsSinceEpoch()));

    QSqlQuery query = d_ptr->db->query(sql);
    if (!query.lastError().isValid()) {
        d_ptr->userNameList.append(username);
        return true;
    }
    return false;
}

bool AccountQuery::updateAccount(const QString &username, const QString &password)
{
    const QString sql
        = QLatin1String(
              "UPDATE account SET password = '%1',  timestamp = '%2' WHERE username = '%3'")
              .arg(password, QString::number(QDateTime::currentSecsSinceEpoch()), username);

    QSqlQuery query = d_ptr->db->query(sql);
    if (!query.lastError().isValid())
        return true;
    qWarning() << query.lastError().text();
    return false;
}

bool AccountQuery::deleteAccount(const QString &username)
{
    const QString sql = QLatin1String("DELETE FROM account WHERE username = '%1'").arg(username);

    QSqlQuery query = d_ptr->db->query(sql);
    if (!query.lastError().isValid()) {
        return true;
    }
    return false;
}

void AccountQuery::init()
{
    if (d_ptr->db->tableContains("account")) {
        return;
    }
    const QString createTable(
        "CREATE TABLE [account]("
        "[id] INTEGER NOT NULL ON CONFLICT REPLACE UNIQUE ON CONFLICT REPLACE, "
        "[username] VARCHAR(50) NOT NULL ON CONFLICT ROLLBACK UNIQUE ON CONFLICT ROLLBACK, "
        "[password] VARCHAR(50) NOT NULL ON CONFLICT ROLLBACK, "
        "[timestamp] INTEGER NOT NULL ON CONFLICT ROLLBACK, "
        "PRIMARY KEY([id] COLLATE [BINARY] ASC) ON CONFLICT REPLACE);");

    QSqlQuery query = d_ptr->db->query(createTable);
    if (query.lastError().isValid()) {
        return;
    }
    query = d_ptr->db->query("CREATE UNIQUE INDEX [username_index] ON [account]([username]);");
}

void AccountQuery::loadparam()
{
    const QJsonObject json(
        Utils::jsonFromFile(qApp->applicationDirPath() + "/resource/config.json"));
    const QJsonObject database = json.value("database").toObject();
    d_ptr->dbParam.type = database.value("type").toString();
    d_ptr->dbParam.databaseName = Utils::getConfigPath() + "/config/apptools.db";
    d_ptr->dbParam.ip = database.value("host").toString();
    d_ptr->dbParam.port = database.value("port").toInt();
    d_ptr->dbParam.uesrname = database.value("username").toString();
    d_ptr->dbParam.password = database.value("password").toString();
}

void AccountQuery::allUsername()
{
    const QString sql("SELECT username FROM account");
    QSqlQuery query = d_ptr->db->query(sql);
    d_ptr->userNameList.clear();
    while (query.next()) {
        d_ptr->userNameList.append(query.value("username").toString());
    }
}

} // namespace AccountSystem
