#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwindow.h"
#include <QDateTime>
#include <QHeaderView>
#include <QInputDialog>

MainWindow::MainWindow(const QString& username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentUsername(username)
    , dbManager(DatabaseManager::instance())
    , currentAccountId()
{
    ui->setupUi(this);
    setupUI();

    // 如果是管理员，初始化管理员UI
    if (isAdmin()) {
        setupAdminUI();
    } else {
        // 普通用户隐藏管理员标签页
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->adminTab));
    }

    loadAccountInfo();
}

bool MainWindow::isAdmin() const
{
    return currentUsername == "admin";
}

void MainWindow::setupAdminUI()
{
    // 设置用户管理表格
    ui->tableUsers->setColumnCount(7);
    ui->tableUsers->setHorizontalHeaderLabels(
        QStringList() << "用户ID" << "用户名" << "姓名" << "身份证" << "电话" << "邮箱" << "注册时间");

    // 设置账户管理表格
    ui->tableAllAccounts->setColumnCount(6);
    ui->tableAllAccounts->setHorizontalHeaderLabels(
        QStringList() << "账户号" << "用户名" << "类型" << "余额" << "状态" << "开户时间");

    // 设置表格样式
    ui->tableUsers->setAlternatingRowColors(true);
    ui->tableUsers->horizontalHeader()->setStretchLastSection(true);
    ui->tableUsers->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableAllAccounts->setAlternatingRowColors(true);
    ui->tableAllAccounts->horizontalHeader()->setStretchLastSection(true);
    ui->tableAllAccounts->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 连接管理员功能信号槽
    connect(ui->btnRefreshUsers, &QPushButton::clicked, this, &MainWindow::onRefreshUsers);
    connect(ui->btnDeleteUser, &QPushButton::clicked, this, &MainWindow::onDeleteUser);
    connect(ui->btnRefreshAccounts, &QPushButton::clicked, this, &MainWindow::onRefreshAllAccounts);
    connect(ui->btnFreezeAccount, &QPushButton::clicked, this, &MainWindow::onFreezeAccount);
    connect(ui->btnUnfreezeAccount, &QPushButton::clicked, this, &MainWindow::onUnfreezeAccount);
    connect(ui->btnDeleteAccount, &QPushButton::clicked, this, &MainWindow::onDeleteAdminAccount);

    // 初始化加载数据
    if (isAdmin()) {
        loadAllUsers();
        loadAllAccounts();
    }
}

void MainWindow::onRefreshUsers()
{
    loadAllUsers();
}

void MainWindow::loadAllUsers()
{
    if (!isAdmin()) return;

    ui->tableUsers->setRowCount(0);

    QList<QVariantMap> users = dbManager.getAllUsers();
    for (const auto& user : users) {
        int row = ui->tableUsers->rowCount();
        ui->tableUsers->insertRow(row);

        ui->tableUsers->setItem(row, 0, new QTableWidgetItem(user["user_id"].toString()));
        ui->tableUsers->setItem(row, 1, new QTableWidgetItem(user["username"].toString()));
        ui->tableUsers->setItem(row, 2, new QTableWidgetItem(user["full_name"].toString()));
        ui->tableUsers->setItem(row, 3, new QTableWidgetItem(user["id_card"].toString()));
        ui->tableUsers->setItem(row, 4, new QTableWidgetItem(user["phone"].toString()));
        ui->tableUsers->setItem(row, 5, new QTableWidgetItem(user["email"].toString()));
        ui->tableUsers->setItem(row, 6, new QTableWidgetItem(user["created_at"].toDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    }
}

void MainWindow::onChangePasswordClicked()
{
    bool ok;
    QString newPassword = QInputDialog::getText(this, "修改密码",
                                                "请输入新密码:", QLineEdit::Password, "", &ok);

    if (!ok || newPassword.isEmpty()) {
        return;
    }

    // 验证密码长度
    if (newPassword.length() < 6) {
        showMessage("错误", "密码长度不能少于6位！");
        return;
    }

    // 确认密码
    QString confirmPassword = QInputDialog::getText(this, "确认密码",
                                                    "请再次输入新密码:", QLineEdit::Password, "", &ok);

    if (!ok) {
        return;
    }

    if (newPassword != confirmPassword) {
        showMessage("错误", "两次输入的密码不一致！");
        return;
    }

    // 如果是管理员修改其他用户密码
    if (isAdmin() && currentUsername != "admin") {
        QString targetUser = QInputDialog::getText(this, "修改密码",
                                                   "请输入要修改密码的用户名（留空则修改当前用户）:", QLineEdit::Normal, currentUsername, &ok);

        if (ok && !targetUser.isEmpty()) {
            if (dbManager.updateUserPassword(targetUser, newPassword)) {
                showMessage("成功", QString("用户 %1 的密码修改成功！").arg(targetUser));
            } else {
                showMessage("错误", "密码修改失败！");
            }
            return;
        }
    }

    // 修改当前用户密码
    if (dbManager.updateUserPassword(currentUsername, newPassword)) {
        showMessage("成功", "密码修改成功！");
    } else {
        showMessage("错误", "密码修改失败！");
    }
}

void MainWindow::onDeleteUser()
{
    if (!isAdmin()) return;

    int row = ui->tableUsers->currentRow();
    if (row < 0) {
        showMessage("提示", "请先选择要删除的用户！");
        return;
    }

    QString username = ui->tableUsers->item(row, 1)->text();
    QString userIdStr = ui->tableUsers->item(row, 0)->text();
    int userId = userIdStr.toInt();

    if (username == "admin") {
        showMessage("错误", "不能删除管理员账户！");
        return;
    }

    if (QMessageBox::question(this, "确认删除",
                              QString("确定要删除用户 %1 吗？此操作不可撤销！").arg(username),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

        if (dbManager.deleteUser(userId)) {
            showMessage("成功", "用户删除成功！");
            loadAllUsers();
        } else {
            showMessage("错误", "删除失败！该用户可能有账户未删除。");
        }
    }
}

void MainWindow::onRefreshAllAccounts()
{
    loadAllAccounts();
}

void MainWindow::loadAllAccounts()
{
    if (!isAdmin()) return;

    ui->tableAllAccounts->setRowCount(0);

    QList<QVariantMap> accounts = dbManager.getAllAccounts();
    for (const auto& account : accounts) {
        int row = ui->tableAllAccounts->rowCount();
        ui->tableAllAccounts->insertRow(row);

        ui->tableAllAccounts->setItem(row, 0, new QTableWidgetItem(account["account_id"].toString()));
        ui->tableAllAccounts->setItem(row, 1, new QTableWidgetItem(account["username"].toString()));
        ui->tableAllAccounts->setItem(row, 2, new QTableWidgetItem(account["account_type"].toString()));
        ui->tableAllAccounts->setItem(row, 3, new QTableWidgetItem(QString("¥%1").arg(account["balance"].toDouble(), 0, 'f', 2)));
        ui->tableAllAccounts->setItem(row, 4, new QTableWidgetItem(account["status"].toString()));
        ui->tableAllAccounts->setItem(row, 5, new QTableWidgetItem(account["created_at"].toDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    }
}

void MainWindow::onFreezeAccount()
{
    if (!isAdmin()) return;

    int row = ui->tableAllAccounts->currentRow();
    if (row < 0) {
        showMessage("提示", "请先选择要冻结的账户！");
        return;
    }

    QString accountId = ui->tableAllAccounts->item(row, 0)->text();
    QString username = ui->tableAllAccounts->item(row, 1)->text();
    QString status = ui->tableAllAccounts->item(row, 4)->text();

    // 检查账户状态
    if (status == "冻结") {
        showMessage("提示", QString("账户 %1 已经是冻结状态！").arg(accountId));
        return;
    }

    if (QMessageBox::question(this, "确认冻结",
                              QString("确定要冻结账户 %1 (用户: %2) 吗？\n冻结后该账户将无法进行任何交易。").arg(accountId, username),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

        if (dbManager.freezeAccount(accountId)) {
            showMessage("成功", "账户冻结成功！");
            loadAllAccounts();
        } else {
            showMessage("错误", "冻结失败！");
        }
    }
}

void MainWindow::onUnfreezeAccount()
{
    if (!isAdmin()) return;

    int row = ui->tableAllAccounts->currentRow();
    if (row < 0) {
        showMessage("提示", "请先选择要解冻的账户！");
        return;
    }

    QString accountId = ui->tableAllAccounts->item(row, 0)->text();
    QString username = ui->tableAllAccounts->item(row, 1)->text();
    QString status = ui->tableAllAccounts->item(row, 4)->text();

    // 检查账户状态
    if (status != "冻结") {
        showMessage("提示", QString("账户 %1 当前状态为 '%2'，无需解冻！").arg(accountId).arg(status));
        return;
    }

    if (QMessageBox::question(this, "确认解冻",
                              QString("确定要解冻账户 %1 (用户: %2) 吗？").arg(accountId, username),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

        if (dbManager.unfreezeAccount(accountId)) {
            showMessage("成功", "账户解冻成功！");
            loadAllAccounts();  // 刷新账户列表
        } else {
            showMessage("错误", "解冻失败！");
        }
    }
}

void MainWindow::onDeleteAdminAccount()
{
    if (!isAdmin()) return;

    int row = ui->tableAllAccounts->currentRow();
    if (row < 0) {
        showMessage("提示", "请先选择要删除的账户！");
        return;
    }

    QString accountId = ui->tableAllAccounts->item(row, 0)->text();
    QString username = ui->tableAllAccounts->item(row, 1)->text();
    double balance = 0.0;

    // 获取余额（从表格中解析）
    QString balanceText = ui->tableAllAccounts->item(row, 3)->text();
    balanceText = balanceText.replace("¥", "").replace(",", "");
    bool ok;
    balance = balanceText.toDouble(&ok);
    if (!ok) balance = 0.0;

    // 检查余额是否为0
    if (balance > 0) {
        showMessage("错误", QString("账户 %1 还有余额 ¥%2，不能删除！\n请先将余额转出或取款。")
                                  .arg(accountId).arg(balance, 0, 'f', 2));
        return;
    }

    // 特别检查是否是管理员自己的账户
    if (username == "admin") {
        showMessage("警告", "这是管理员账户，删除可能导致系统异常！");
        if (QMessageBox::question(this, "确认删除",
                                  "这是管理员账户，确定要删除吗？此操作可能导致系统功能异常！",
                                  QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
            return;
        }
    }

    if (QMessageBox::question(this, "确认删除",
                              QString("确定要删除账户 %1 (用户: %2) 吗？\n此操作将永久删除该账户的所有交易记录！")
                                  .arg(accountId, username),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

        // 二次确认
        if (QMessageBox::warning(this, "最后确认",
                                 "这是不可逆操作！确定要删除吗？",
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

            if (dbManager.deleteAccount(accountId)) {
                showMessage("成功", "账户删除成功！");
                loadAllAccounts();  // 刷新账户列表

                // 如果删除的是当前用户的账户，刷新用户界面
                if (username == currentUsername) {
                    loadAccountInfo();
                }
            } else {
                showMessage("错误", "删除失败！账户可能不存在或有其他关联数据。");
            }
        }
    }
}

void MainWindow::onChangePassword()
{
    // 添加修改密码对话框
    bool ok;
    QString newPassword = QInputDialog::getText(this, "修改密码",
                                                "请输入新密码:", QLineEdit::Password, "", &ok);

    if (ok && !newPassword.isEmpty()) {
        if (dbManager.updateUserPassword(currentUsername, newPassword)) {
            showMessage("成功", "密码修改成功！");
        } else {
            showMessage("错误", "密码修改失败！");
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle(QString("银行账户管理系统 - 欢迎 %1").arg(currentUsername));

    // 设置表格
    ui->tableHistory->setColumnCount(6);
    ui->tableHistory->setHorizontalHeaderLabels(
        QStringList() << "交易ID" << "类型" << "金额" << "对方账户" << "描述" << "时间");

    // 设置表格样式
    ui->tableHistory->setAlternatingRowColors(true);
    ui->tableHistory->horizontalHeader()->setStretchLastSection(true);
    ui->tableHistory->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 设置按钮样式
    ui->btnDeposit->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    ui->btnWithdraw->setStyleSheet("background-color: #F44336; color: white; font-weight: bold;");
    ui->btnTransfer->setStyleSheet("background-color: #2196F3; color: white; font-weight: bold;");
    ui->btnCreateAccount_2->setStyleSheet("background-color: #9C27B0; color: white; font-weight: bold;");

    // 设置余额标签样式
    ui->labelBalance->setStyleSheet("color: #2E8B57;");

    // 设置账户类型下拉框
    ui->comboAccountType->clear();
    ui->comboAccountType->addItem("储蓄账户");
    ui->comboAccountType->addItem("活期账户");
    ui->comboAccountType->addItem("定期账户");

    // 连接信号槽
    connect(ui->btnDeposit, &QPushButton::clicked, this, &MainWindow::onDepositClicked);
    connect(ui->btnWithdraw, &QPushButton::clicked, this, &MainWindow::onWithdrawClicked);
    connect(ui->btnTransfer, &QPushButton::clicked, this, &MainWindow::onTransferClicked);
    connect(ui->btnCheckBalance, &QPushButton::clicked, this, &MainWindow::onCheckBalanceClicked);
    connect(ui->btnRefreshHistory, &QPushButton::clicked, this, &MainWindow::onRefreshHistory);
    connect(ui->btnCreateAccount, &QPushButton::clicked, this, &MainWindow::onCreateAccountClicked);
    connect(ui->btnCreateAccount_2, &QPushButton::clicked, this, &MainWindow::onCreateAccountClicked);
    connect(ui->btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(ui->btnChangePassword, &QPushButton::clicked, this, &MainWindow::onChangePasswordClicked);  // 新增
    connect(ui->comboAccounts, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAccountSelected);

    // 设置标签页
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::loadAccountInfo()
{
    ui->comboAccounts->clear();

    QList<QVariantMap> accounts = dbManager.getUserAccounts(currentUsername);

    if (accounts.isEmpty()) {
        // 新用户没有账户
        showMessage("提示", "欢迎新用户！您还没有账户，请先开户。");
        return;
    }

    for (const auto& account : accounts) {
        QString accountId = account["account_id"].toString();
        QString accountType = account["account_type"].toString();
        double balance = account["balance"].toDouble();

        QString displayText = QString("%1 (%2) - 余额: ¥%3")
                                  .arg(accountId)
                                  .arg(accountType)
                                  .arg(balance, 0, 'f', 2);

        ui->comboAccounts->addItem(displayText, accountId);
    }

    if (ui->comboAccounts->count() > 0) {
        currentAccountId = ui->comboAccounts->currentData().toString();
        updateBalanceDisplay();
        loadTransactionHistory();
    }
}

void MainWindow::updateBalanceDisplay()
{
    if (currentAccountId.isEmpty()) {
        ui->labelBalance->setText("¥0.00");
        return;
    }

    double balance = dbManager.getBalance(currentAccountId);
    ui->labelBalance->setText(QString("¥%1").arg(balance, 0, 'f', 2));
}

void MainWindow::loadTransactionHistory()
{
    if (currentAccountId.isEmpty()) return;

    ui->tableHistory->setRowCount(0);

    QList<QVariantMap> history;
    if (isAdmin()) {
        // 管理员查看所有记录
        history = dbManager.getTransactionHistory("", currentUsername);
    } else {
        // 普通用户查看自己账户记录
        history = dbManager.getTransactionHistory(currentAccountId, currentUsername);
    }

    // 根据是否是管理员设置表格列数
    if (isAdmin()) {
        ui->tableHistory->setColumnCount(8);  // 管理员看到更多信息
        ui->tableHistory->setHorizontalHeaderLabels(
            QStringList() << "交易ID" << "账户号" << "用户名" << "类型" << "金额" << "对方账户" << "描述" << "时间");
    } else {
        ui->tableHistory->setColumnCount(6);  // 普通用户
        ui->tableHistory->setHorizontalHeaderLabels(
            QStringList() << "交易ID" << "类型" << "金额" << "对方账户" << "描述" << "时间");
    }

    for (const auto& record : history) {
        int row = ui->tableHistory->rowCount();
        ui->tableHistory->insertRow(row);

        if (isAdmin()) {
            // 管理员视图
            ui->tableHistory->setItem(row, 0, new QTableWidgetItem(record["id"].toString()));
            ui->tableHistory->setItem(row, 1, new QTableWidgetItem(record["account_id"].toString()));
            ui->tableHistory->setItem(row, 2, new QTableWidgetItem(record["username"].toString()));
            ui->tableHistory->setItem(row, 3, new QTableWidgetItem(record["type"].toString()));

            double amount = record["amount"].toDouble();
            QString amountText = QString("¥%1").arg(amount, 0, 'f', 2);
            ui->tableHistory->setItem(row, 4, new QTableWidgetItem(amountText));

            ui->tableHistory->setItem(row, 5, new QTableWidgetItem(record["target"].toString()));
            ui->tableHistory->setItem(row, 6, new QTableWidgetItem(record["description"].toString()));

            QDateTime time = record["time"].toDateTime();
            ui->tableHistory->setItem(row, 7, new QTableWidgetItem(time.toString("yyyy-MM-dd HH:mm:ss")));
        } else {
            // 普通用户视图
            ui->tableHistory->setItem(row, 0, new QTableWidgetItem(record["id"].toString()));
            ui->tableHistory->setItem(row, 1, new QTableWidgetItem(record["type"].toString()));

            double amount = record["amount"].toDouble();
            QString amountText = QString("¥%1").arg(amount, 0, 'f', 2);
            ui->tableHistory->setItem(row, 2, new QTableWidgetItem(amountText));

            ui->tableHistory->setItem(row, 3, new QTableWidgetItem(record["target"].toString()));
            ui->tableHistory->setItem(row, 4, new QTableWidgetItem(record["description"].toString()));

            QDateTime time = record["time"].toDateTime();
            ui->tableHistory->setItem(row, 5, new QTableWidgetItem(time.toString("yyyy-MM-dd HH:mm:ss")));
        }
    }
}

void MainWindow::onDepositClicked()
{
    if (currentAccountId.isEmpty()) {
        showMessage("错误", "请先选择账户！");
        return;
    }

    bool ok;
    double amount = ui->txtDepositAmount->text().toDouble(&ok);

    if (!ok || amount <= 0) {
        showMessage("错误", "请输入有效的存款金额！");
        return;
    }

    if (dbManager.deposit(currentAccountId, amount)) {
        showMessage("成功", QString("存款成功！存入金额: ¥%1").arg(amount, 0, 'f', 2));
        updateBalanceDisplay();
        loadTransactionHistory();
        ui->txtDepositAmount->clear();
    } else {
        showMessage("错误", "存款失败！");
    }
}

void MainWindow::onWithdrawClicked()
{
    if (currentAccountId.isEmpty()) {
        showMessage("错误", "请先选择账户！");
        return;
    }

    bool ok;
    double amount = ui->txtWithdrawAmount->text().toDouble(&ok);

    if (!ok || amount <= 0) {
        showMessage("错误", "请输入有效的取款金额！");
        return;
    }

    if (dbManager.withdraw(currentAccountId, amount)) {
        showMessage("成功", QString("取款成功！取出金额: ¥%1").arg(amount, 0, 'f', 2));
        updateBalanceDisplay();
        loadTransactionHistory();
        ui->txtWithdrawAmount->clear();
    } else {
        showMessage("错误", "取款失败！余额不足或账户异常！");
    }
}

void MainWindow::onTransferClicked()
{
    if (currentAccountId.isEmpty()) {
        showMessage("错误", "请先选择账户！");
        return;
    }

    QString targetAccount = ui->txtTargetAccount->text();
    bool ok;
    double amount = ui->txtTransferAmount->text().toDouble(&ok);

    if (targetAccount.isEmpty()) {
        showMessage("错误", "请输入目标账户！");
        return;
    }

    if (!ok || amount <= 0) {
        showMessage("错误", "请输入有效的转账金额！");
        return;
    }

    if (targetAccount == currentAccountId) {
        showMessage("错误", "不能转账给自己！");
        return;
    }

    if (dbManager.transfer(currentAccountId, targetAccount, amount)) {
        showMessage("成功", QString("转账成功！转账金额: ¥%1").arg(amount, 0, 'f', 2));
        updateBalanceDisplay();
        loadTransactionHistory();
        ui->txtTargetAccount->clear();
        ui->txtTransferAmount->clear();
    } else {
        showMessage("错误", "转账失败！余额不足或目标账户不存在！");
    }
}

void MainWindow::onCheckBalanceClicked()
{
    updateBalanceDisplay();
    showMessage("余额", QString("当前账户余额: %1").arg(ui->labelBalance->text()));
}

void MainWindow::onRefreshHistory()
{
    loadTransactionHistory();
    showMessage("提示", "交易记录已刷新！");
}

void MainWindow::onCreateAccountClicked()
{
    QString accountType = ui->comboAccountType->currentText();

    qDebug() << "尝试开户，账户类型:" << accountType;

    // 获取用户ID
    int userId = dbManager.getUserId(currentUsername);
    qDebug() << "当前用户ID:" << userId;

    if (userId == -1) {
        showMessage("错误", "无法获取用户信息！");
        return;
    }

    // 检查是否已有该类型账户
    QList<QVariantMap> accounts = dbManager.getUserAccounts(currentUsername);
    for (const auto& account : accounts) {
        if (account["account_type"].toString() == accountType) {
            showMessage("提示", QString("您已经有一个%1，不能重复开户！").arg(accountType));
            return;
        }
    }

    QString accountId = dbManager.createAccount(userId, accountType);
    if (!accountId.isEmpty()) {
        showMessage("成功", QString("开户成功！\n账户号: %1\n账户类型: %2")
                                  .arg(accountId).arg(accountType));

        // 刷新账户列表
        loadAccountInfo();

        // 切换到仪表板
        ui->tabWidget->setCurrentIndex(0);
    } else {
        showMessage("错误", "开户失败！可能是数据库错误。");
    }
}

void MainWindow::onAccountSelected(int index)
{
    if (index >= 0) {
        currentAccountId = ui->comboAccounts->itemData(index).toString();
        updateBalanceDisplay();
        loadTransactionHistory();
    }
}

void MainWindow::onLogoutClicked()
{
    this->close();
    emit loggedOut();
}

void MainWindow::showMessage(const QString& title, const QString& message)
{
    QMessageBox::information(this, title, message);
}
