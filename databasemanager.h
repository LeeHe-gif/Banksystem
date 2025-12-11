#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>

// 前向声明
class QSqlDatabase;
class QSqlQuery;

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // 单例模式
    static DatabaseManager& instance();

    // 数据库连接（MySQL版本）
    bool connectToDatabase(const QString& host,
                           const QString& database,
                           const QString& username,
                           const QString& password);

    // 账户管理
    bool freezeAccount(const QString& accountId);
    bool unfreezeAccount(const QString& accountId);
    bool deleteAccount(const QString& accountId);
    QList<QVariantMap> getAllUsers();  // 管理员获取所有用户
    bool deleteUser(int userId);
    bool updateUserPassword(const QString& username, const QString& newPassword);
    QList<QVariantMap> getAllAccounts();  // 获取所有账户
    QList<QVariantMap> getTransactionHistory(const QString& accountId, const QString& username);

    void disconnect();
    bool isConnected() const;

    // 用户操作
    bool createUser(const QString& username, const QString& password,
                    const QString& fullName, const QString& idCard,
                    const QString& phone, const QString& email);

    bool authenticateUser(const QString& username, const QString& password);

    // 账户操作
    QString createAccount(int userId, const QString& accountType = "储蓄账户");
    bool deposit(const QString& accountId, double amount);
    bool withdraw(const QString& accountId, double amount);
    bool transfer(const QString& fromAccount, const QString& toAccount, double amount);

    // 查询操作
    double getBalance(const QString& accountId);
    QList<QVariantMap> getTransactionHistory(const QString& accountId);
    QList<QVariantMap> getUserAccounts(const QString& username);
    int getUserId(const QString& username);

    // 测试数据库连接
    bool testConnection();

private:
    DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase* db;
    QString generateAccountId();

    QList<QVariantMap> getTransactionHistoryForAdmin();

    void createTables();
    void insertTestData();
};

#endif // DATABASEMANAGER_H
