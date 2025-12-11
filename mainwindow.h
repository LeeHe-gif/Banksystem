#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QVariantMap>
#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& username, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDepositClicked();
    void onWithdrawClicked();
    void onTransferClicked();
    void onCheckBalanceClicked();
    void onRefreshHistory();
    void onCreateAccountClicked();
    void onAccountSelected(int index);
    void onLogoutClicked();
    void onChangePassword();
    // 管理员功能
    void onRefreshUsers();
    void onDeleteUser();
    void onRefreshAllAccounts();
    void onFreezeAccount();
    void onUnfreezeAccount();
    void onDeleteAdminAccount();
    void onChangePasswordClicked();  // 修改密码按钮

private:
    Ui::MainWindow *ui;
    QString currentUsername;
    QString currentAccountId;
    DatabaseManager& dbManager;

    void setupUI();
    void loadAccountInfo();
    void updateBalanceDisplay();
    void loadTransactionHistory();
    void showMessage(const QString& title, const QString& message);

    // 管理员功能
    void setupAdminUI();
    void loadAllUsers();
    void loadAllAccounts();
    bool isAdmin() const;

signals:
    void loggedOut();  // 退出登录信号
};

#endif // MAINWINDOW_H
