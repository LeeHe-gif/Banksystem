#include "databasemanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QRandomGenerator>
#include <QVariant>

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
    , db(nullptr)
{
}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connectToDatabase(const QString& host, // 数据库连接
                                        const QString& database,
                                        const QString& username,
                                        const QString& password)
{
    disconnect(); // 先断开现有连接

    qDebug() << "========== 开始连接数据库 ==========";
    qDebug() << "主机:" << host;
    qDebug() << "数据库:" << database;
    qDebug() << "用户名:" << username;

    // 检查可用驱动
    qDebug() << "可用数据库驱动:";
    QStringList drivers = QSqlDatabase::drivers();
    for (const QString& driver : drivers) {
        qDebug() << "  " << driver;
    }

    // 创建数据库连接
    QString connectionName = QString("BankSystemConnection_%1").arg(QDateTime::currentMSecsSinceEpoch());
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL", connectionName));

    // 设置连接参数
    db->setHostName(host);
    db->setDatabaseName(database);
    db->setUserName(username);
    db->setPassword(password);
    db->setConnectOptions("MYSQL_OPT_RECONNECT=1");

    qDebug() << "尝试打开数据库连接...";

    if (!db->open()) {
        QString error = db->lastError().text();
        qDebug() << "数据库连接错误:" << error;
        qDebug() << "错误类型:" << db->lastError().type();
        qDebug() << "数据库文本:" << db->lastError().databaseText();
        qDebug() << "驱动文本:" << db->lastError().driverText();

        delete db;
        db = nullptr;
        QSqlDatabase::removeDatabase(connectionName);

        return false;
    }

    qDebug() << "数据库连接成功！";
    qDebug() << "========== 数据库连接完成 ==========";

    // 测试连接
    return testConnection();
}

bool DatabaseManager::testConnection() //测试连接用，调试专用
{
    if (!isConnected()) {
        qDebug() << "测试连接失败：数据库未连接";
        return false;
    }

    QSqlQuery query(*db);
    if (query.exec("SELECT 1")) {
        qDebug() << "数据库连接测试成功";
        return true;
    } else {
        qDebug() << "数据库连接测试失败:" << query.lastError().text();
        return false;
    }
}

void DatabaseManager::disconnect()
{
    if (db && db->isOpen()) {
        QString connectionName = db->connectionName();
        db->close();
        delete db;
        db = nullptr;
        QSqlDatabase::removeDatabase(connectionName);
        qDebug() << "数据库已断开连接";
    }
}

bool DatabaseManager::isConnected() const
{
    return db && db->isOpen();
}

QString DatabaseManager::generateAccountId()
{
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString("yyyyMMddHHmm");
    int randomNum = QRandomGenerator::global()->bounded(100, 999);
    // 总长度：4(固定) + 12(时间) + 3(随机) = 19位
    return QString("6214%1%2").arg(timestamp).arg(randomNum, 3, 10, QChar('0'));
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password)
{
    if (!isConnected()) {
        qDebug() << "用户认证失败：数据库未连接";
        return false;
    }

    QSqlQuery query(*db);
    query.prepare("SELECT user_id FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "查询执行失败:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        qDebug() << "用户认证成功:" << username;
        return true;
    }

    qDebug() << "用户认证失败:" << username;
    return false;
}

int DatabaseManager::getUserId(const QString& username)
{
    if (!isConnected()) return -1;

    QSqlQuery query(*db);
    query.prepare("SELECT user_id FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return -1;
}

bool DatabaseManager::createUser(const QString& username, const QString& password,
                                 const QString& fullName, const QString& idCard,
                                 const QString& phone , const QString& email )
{
    if (!isConnected()) return false;

    QSqlQuery query(*db);
    query.prepare("INSERT INTO users (username, password, full_name, id_card, phone, email) "
                  "VALUES (:username, :password, :full_name, :id_card,:phone, :email)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":full_name", fullName);
    query.bindValue(":id_card", idCard);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);

    if (query.exec()) {
        qDebug() << "用户创建成功:" << username;
        return true;
    }

    qDebug() << "用户创建失败:" << query.lastError().text();
    return false;
}

QString DatabaseManager::createAccount(int userId, const QString& accountType)
{
    if (!isConnected()) return QString();

    QString accountId = generateAccountId();

    QSqlQuery query(*db);
    query.prepare("INSERT INTO accounts (account_id, user_id, account_type, balance) "
                  "VALUES (:account_id, :user_id, :account_type, 0.00)");
    query.bindValue(":account_id", accountId);
    query.bindValue(":user_id", userId);
    query.bindValue(":account_type", accountType);

    if (query.exec()) {
        qDebug() << "账户创建成功:" << accountId << "用户ID:" << userId;
        return accountId;
    }

    qDebug() << "账户创建失败:" << query.lastError().text();
    return QString();
}

double DatabaseManager::getBalance(const QString& accountId)
{
    if (!isConnected()) return 0.0;

    QSqlQuery query(*db);
    query.prepare("SELECT balance FROM accounts WHERE account_id = :account_id");
    query.bindValue(":account_id", accountId);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    qDebug() << "获取余额失败，账户:" << accountId << "错误:" << query.lastError().text();
    return 0.0;
}

bool DatabaseManager::deposit(const QString& accountId, double amount)
{
    if (!isConnected() || amount <= 0) return false;

    // 开始事务
    if (!db->transaction()) {
        qDebug() << "开始事务失败";
        return false;
    }

    QSqlQuery query(*db);

    // 更新余额
    query.prepare("UPDATE accounts SET balance = balance + :amount "
                  "WHERE account_id = :account_id");
    query.bindValue(":amount", amount);
    query.bindValue(":account_id", accountId);

    if (!query.exec()) {
        db->rollback();
        qDebug() << "存款更新失败:" << query.lastError().text();
        return false;
    }

    // 记录交易
    query.prepare("INSERT INTO transactions (account_id, transaction_type, amount, description) "
                  "VALUES (:account_id, '存款', :amount, '存款操作')");
    query.bindValue(":account_id", accountId);
    query.bindValue(":amount", amount);

    if (!query.exec()) {
        db->rollback();
        qDebug() << "存款交易记录失败:" << query.lastError().text();
        return false;
    }

    if (!db->commit()) {
        qDebug() << "提交事务失败";
        return false;
    }

    qDebug() << "存款成功，账户:" << accountId << "金额:" << amount;
    return true;
}

bool DatabaseManager::withdraw(const QString& accountId, double amount)
{
    if (!isConnected() || amount <= 0) return false;

    // 检查余额是否充足
    double balance = getBalance(accountId);
    if (balance < amount) {
        qDebug() << "余额不足，当前余额:" << balance << "需要:" << amount;
        return false;
    }

    // 开始事务
    if (!db->transaction()) {
        qDebug() << "开始事务失败";
        return false;
    }

    QSqlQuery query(*db);

    // 更新余额
    query.prepare("UPDATE accounts SET balance = balance - :amount "
                  "WHERE account_id = :account_id");
    query.bindValue(":amount", amount);
    query.bindValue(":account_id", accountId);

    if (!query.exec()) {
        db->rollback();
        qDebug() << "取款更新失败:" << query.lastError().text();
        return false;
    }

    // 记录交易
    query.prepare("INSERT INTO transactions (account_id, transaction_type, amount, description) "
                  "VALUES (:account_id, '取款', :amount, '取款操作')");
    query.bindValue(":account_id", accountId);
    query.bindValue(":amount", amount);

    if (!query.exec()) {
        db->rollback();
        qDebug() << "取款交易记录失败:" << query.lastError().text();
        return false;
    }

    if (!db->commit()) {
        qDebug() << "提交事务失败";
        return false;
    }

    qDebug() << "取款成功，账户:" << accountId << "金额:" << amount;
    return true;
}

bool DatabaseManager::transfer(const QString& fromAccount, const QString& toAccount, double amount)
{
    if (!isConnected() || amount <= 0 || fromAccount == toAccount) return false;

    // 检查转出账户余额
    double fromBalance = getBalance(fromAccount);
    if (fromBalance < amount) {
        qDebug() << "转账余额不足，当前余额:" << fromBalance << "需要:" << amount;
        return false;
    }

    // 检查转入账户是否存在
    QSqlQuery checkQuery(*db);
    checkQuery.prepare("SELECT COUNT(*) FROM accounts WHERE account_id = :to_account");
    checkQuery.bindValue(":to_account", toAccount);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        qDebug() << "目标账户不存在:" << toAccount;
        return false;
    }

    // 开始事务
    if (!db->transaction()) {
        qDebug() << "开始事务失败";
        return false;
    }

    QSqlQuery query(*db);

    // 减少转出账户余额
    query.prepare("UPDATE accounts SET balance = balance - :amount "
                  "WHERE account_id = :from_account");
    query.bindValue(":amount", amount);
    query.bindValue(":from_account", fromAccount);

    if (!query.exec()) {
        db->rollback();
        qDebug() << "转账支出更新失败:" << query.lastError().text();
        return false;
    }

    // 增加转入账户余额
    query.prepare("UPDATE accounts SET balance = balance + :amount "
                  "WHERE account_id = :to_account");
    query.bindValue(":amount", amount);
    query.bindValue(":to_account", toAccount);

    if (!query.exec()) {
        db->rollback();
        qDebug() << "转账收入更新失败:" << query.lastError().text();
        return false;
    }

    // 记录转出交易
    query.prepare("INSERT INTO transactions (account_id, transaction_type, amount, target_account, description) "
                  "VALUES (:from_account, '转账', :amount, :to_account, '转账支出')");
    query.bindValue(":from_account", fromAccount);
    query.bindValue(":amount", amount);
    query.bindValue(":to_account", toAccount);

    if (!query.exec()) {
        db->rollback();
        qDebug() << "转账支出记录失败:" << query.lastError().text();
        return false;
    }

    // 记录转入交易
    query.prepare("INSERT INTO transactions (account_id, transaction_type, amount, target_account, description) "
                  "VALUES (:to_account, '收款', :amount, :from_account, '转账收入')");
    query.bindValue(":to_account", toAccount);
    query.bindValue(":amount", amount);
    query.bindValue(":from_account", fromAccount);

    if (!query.exec()) {
        db->rollback();
        qDebug() << "转账收入记录失败:" << query.lastError().text();
        return false;
    }

    if (!db->commit()) {
        qDebug() << "提交事务失败";
        return false;
    }

    qDebug() << "转账成功:" << fromAccount << "->" << toAccount << "金额:" << amount;
    return true;
}

// 冻结账户
bool DatabaseManager::freezeAccount(const QString& accountId)
{
    if (!isConnected()) return false;

    QSqlQuery query(*db);
    query.prepare("UPDATE accounts SET status = '冻结' WHERE account_id = :account_id");
    query.bindValue(":account_id", accountId);

    if (query.exec()) {
        qDebug() << "账户冻结成功:" << accountId;
        return true;
    }

    qDebug() << "账户冻结失败:" << query.lastError().text();
    return false;
}

// 解冻账户
bool DatabaseManager::unfreezeAccount(const QString& accountId)
{
    if (!isConnected()) return false;

    QSqlQuery query(*db);
    query.prepare("UPDATE accounts SET status = '正常' WHERE account_id = :account_id");
    query.bindValue(":account_id", accountId);

    if (query.exec()) {
        qDebug() << "账户解冻成功:" << accountId;
        return true;
    }

    qDebug() << "账户解冻失败:" << query.lastError().text();
    return false;
}

// 删除账户
bool DatabaseManager::deleteAccount(const QString& accountId)
{
    if (!isConnected()) return false;

    QSqlQuery query(*db);
    query.prepare("DELETE FROM accounts WHERE account_id = :account_id");
    query.bindValue(":account_id", accountId);

    if (query.exec()) {
        qDebug() << "账户删除成功:" << accountId;
        return true;
    }

    qDebug() << "账户删除失败:" << query.lastError().text();
    return false;
}

// 获取所有用户（管理员用）
QList<QVariantMap> DatabaseManager::getAllUsers()
{
    QList<QVariantMap> users;

    if (!isConnected()) return users;

    QSqlQuery query(*db);
    query.prepare("SELECT user_id, username, full_name, id_card, phone, email, created_at "
                  "FROM users ORDER BY created_at DESC");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap user;
            user["user_id"] = query.value(0);
            user["username"] = query.value(1);
            user["full_name"] = query.value(2);
            user["id_card"] = query.value(3);
            user["phone"] = query.value(4);
            user["email"] = query.value(5);
            user["created_at"] = query.value(6);
            users.append(user);
        }
        qDebug() << "获取到" << users.size() << "个用户";
    } else {
        qDebug() << "获取用户列表失败:" << query.lastError().text();
    }

    return users;
}

// 删除用户
bool DatabaseManager::deleteUser(int userId)
{
    if (!isConnected()) return false;

    // 先检查是否有账户
    QSqlQuery checkQuery(*db);
    checkQuery.prepare("SELECT COUNT(*) FROM accounts WHERE user_id = :user_id");
    checkQuery.bindValue(":user_id", userId);

    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qDebug() << "用户有账户，不能删除";
        return false;
    }

    QSqlQuery query(*db);
    query.prepare("DELETE FROM users WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        qDebug() << "用户删除成功:" << userId;
        return true;
    }

    qDebug() << "用户删除失败:" << query.lastError().text();
    return false;
}

// 修改密码
bool DatabaseManager::updateUserPassword(const QString& username, const QString& newPassword)
{
    if (!isConnected()) return false;

    QSqlQuery query(*db);
    query.prepare("UPDATE users SET password = :password WHERE username = :username");
    query.bindValue(":password", newPassword);
    query.bindValue(":username", username);

    if (query.exec()) {
        qDebug() << "密码修改成功:" << username;
        return true;
    }

    qDebug() << "密码修改失败:" << query.lastError().text();
    return false;
}

// 获取所有账户（管理员用）
QList<QVariantMap> DatabaseManager::getAllAccounts()
{
    QList<QVariantMap> accounts;

    if (!isConnected()) return accounts;

    QSqlQuery query(*db);
    query.prepare("SELECT a.account_id, u.username, a.account_type, a.balance, a.status, a.created_at "
                  "FROM accounts a "
                  "JOIN users u ON a.user_id = u.user_id "
                  "ORDER BY a.created_at DESC");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap account;
            account["account_id"] = query.value(0);
            account["username"] = query.value(1);
            account["account_type"] = query.value(2);
            account["balance"] = query.value(3);
            account["status"] = query.value(4);
            account["created_at"] = query.value(5);
            accounts.append(account);
        }
        qDebug() << "获取到" << accounts.size() << "个账户";
    } else {
        qDebug() << "获取账户列表失败:" << query.lastError().text();
    }

    return accounts;
}

QList<QVariantMap> DatabaseManager::getTransactionHistory(const QString& accountId, const QString& username)
{
    // 如果用户是admin，调用管理员版本
    if (username == "admin") {
        return getTransactionHistoryForAdmin();
    }

    // 否则调用普通版本
    return getTransactionHistory(accountId);
}

// 普通用户版本（只查看自己账户的记录）
QList<QVariantMap> DatabaseManager::getTransactionHistory(const QString& accountId)
{
    QList<QVariantMap> history;

    if (!isConnected()) {
        qDebug() << "获取交易记录失败：数据库未连接";
        return history;
    }

    QSqlQuery query(*db);
    query.prepare("SELECT transaction_id, transaction_type, amount, "
                  "target_account, description, transaction_time "
                  "FROM transactions WHERE account_id = :account_id "
                  "ORDER BY transaction_time DESC");
    query.bindValue(":account_id", accountId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap record;
            record["id"] = query.value(0);
            record["type"] = query.value(1);
            record["amount"] = query.value(2);
            record["target"] = query.value(3);
            record["description"] = query.value(4);
            record["time"] = query.value(5);
            history.append(record);
        }
        qDebug() << "获取到" << history.size() << "条交易记录，账户:" << accountId;
    } else {
        qDebug() << "获取交易记录失败:" << query.lastError().text();
    }

    return history;
}

// 管理员版本：查看所有记录
QList<QVariantMap> DatabaseManager::getTransactionHistoryForAdmin()
{
    QList<QVariantMap> history;

    if (!isConnected()) {
        qDebug() << "获取交易记录失败：数据库未连接";
        return history;
    }

    QSqlQuery query(*db);
    query.prepare("SELECT t.transaction_id, t.transaction_type, t.amount, "
                  "t.target_account, t.description, t.transaction_time, "
                  "a.account_id, u.username "
                  "FROM transactions t "
                  "JOIN accounts a ON t.account_id = a.account_id "
                  "JOIN users u ON a.user_id = u.user_id "
                  "ORDER BY t.transaction_time DESC");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap record;
            record["id"] = query.value(0);
            record["type"] = query.value(1);
            record["amount"] = query.value(2);
            record["target"] = query.value(3);
            record["description"] = query.value(4);
            record["time"] = query.value(5);
            record["account_id"] = query.value(6);
            record["username"] = query.value(7);
            history.append(record);
        }
        qDebug() << "管理员获取到" << history.size() << "条交易记录";
    } else {
        qDebug() << "获取交易记录失败:" << query.lastError().text();
    }

    return history;
}

QList<QVariantMap> DatabaseManager::getUserAccounts(const QString& username)
{
    QList<QVariantMap> accounts;

    if (!isConnected()) {
        qDebug() << "获取用户账户失败：数据库未连接";
        return accounts;
    }

    QSqlQuery query(*db);
    query.prepare("SELECT a.account_id, a.account_type, a.balance, a.created_at "
                  "FROM accounts a "
                  "JOIN users u ON a.user_id = u.user_id "
                  "WHERE u.username = :username "
                  "ORDER BY a.created_at DESC");
    query.bindValue(":username", username);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap account;
            account["account_id"] = query.value(0).toString();
            account["account_type"] = query.value(1).toString();
            account["balance"] = query.value(2).toDouble();
            account["created_at"] = query.value(3);
            accounts.append(account);
        }
        qDebug() << "获取到" << accounts.size() << "个账户，用户:" << username;
    } else {
        qDebug() << "获取用户账户失败:" << query.lastError().text();
    }

    return accounts;
}
