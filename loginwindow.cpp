#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , dbManager(DatabaseManager::instance())
    , mainWindow(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("银行账户管理系统 - 登录");

    // 连接信号槽
    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(ui->btnConnect, &QPushButton::clicked, this, &LoginWindow::onConnectClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(ui->btnConnectDB, &QPushButton::clicked, this, &LoginWindow::onConnectDBClicked);

    // 连接页面切换按钮
    connect(ui->btnShowRegister, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(1); // 切换到注册页面
    });
    connect(ui->btnShowLogin, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(0); // 切换到登录页面
    });
    connect(ui->btnShowLoginForm, &QPushButton::clicked, this, &LoginWindow::switchToLoginPage);

    // 默认显示登录表单
    ui->stackedWidget->setCurrentIndex(0);

    // 设置默认数据库连接信息
    ui->txtServer->setText("localhost");
    ui->txtDatabase->setText("BankSystem");
    ui->txtDbUser->setText("root");
    ui->txtDbPassword->setText("password");
}

LoginWindow::~LoginWindow()
{
    delete ui;
    if (mainWindow) {
        delete mainWindow;
    }
}

void LoginWindow::onConnectDBClicked()
{
    // 切换到数据库连接页面
    ui->stackedWidget->setCurrentIndex(2);
}

void LoginWindow::onConnectClicked()
{
    QString server = ui->txtServer->text();
    QString database = ui->txtDatabase->text();
    QString username = ui->txtDbUser->text();
    QString password = ui->txtDbPassword->text();

    if (server.isEmpty() || database.isEmpty()) {
        showMessage("错误", "服务器和数据库名称不能为空！");
        return;
    }

    if (username.isEmpty() || password.isEmpty()) {
        showMessage("错误", "请输入数据库用户名和密码！");
        return;
    }

    qDebug() << "尝试连接数据库...";
    qDebug() << "服务器:" << server;
    qDebug() << "数据库:" << database;
    qDebug() << "用户名:" << username;

    if (dbManager.connectToDatabase(server, database, username, password)) {
        showMessage("成功", "数据库连接成功！");
        switchToLoginPage(); // 连接成功后切换到登录页面
    } else {
        showMessage("错误", "数据库连接失败！请检查连接信息。");
    }
}

void LoginWindow::onLoginClicked()
{
    if (!dbManager.isConnected()) {
        showMessage("错误", "请先连接数据库！");
        return;
    }

    QString username = ui->txtUsername->text();
    QString password = ui->txtPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        showMessage("错误", "用户名和密码不能为空！");
        return;
    }

    qDebug() << "尝试登录用户:" << username;

    if (dbManager.authenticateUser(username, password)) {
        // 隐藏登录窗口
        this->hide();

        // 每次都创建新的MainWindow实例
        if (mainWindow) {
            delete mainWindow;  // 删除旧的实例
        }

        mainWindow = new MainWindow(username);

        // 连接退出登录信号
        connect(mainWindow, &MainWindow::loggedOut, this, &LoginWindow::showLoginWindow);

        mainWindow->show();
    } else {
        showMessage("错误", "用户名或密码错误！");
    }
}

void LoginWindow::showLoginWindow()
{
    // 清理主窗口
    if (mainWindow) {
        disconnect(mainWindow, &MainWindow::loggedOut, this, &LoginWindow::showLoginWindow);
        mainWindow->deleteLater();
        mainWindow = nullptr;
    }

    // 清理用户输入
    ui->txtUsername->clear();
    ui->txtPassword->clear();

    // 显示登录窗口
    this->show();
    ui->stackedWidget->setCurrentIndex(0);
}

void LoginWindow::onRegisterClicked()
{
    if (!dbManager.isConnected()) {
        showMessage("错误", "请先连接数据库！");
        return;
    }

    QString username = ui->txtRegUsername->text();
    QString password = ui->txtRegPassword->text();
    QString confirmPassword = ui->txtRegConfirmPassword->text();
    QString fullName = ui->txtFullName->text();
    QString idCard = ui->txtIdCard->text();
    QString phone = ui->txtIdPhone->text();
    QString email = ui->txtIdEmail->text();

    // 验证输入
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty() ||
        fullName.isEmpty() || idCard.isEmpty()) {
        showMessage("错误", "所有字段都必须填写！");
        return;
    }

    if (password != confirmPassword) {
        showMessage("错误", "两次输入的密码不一致！");
        return;
    }

    qDebug() << "尝试注册用户:" << username;

    if (dbManager.createUser(username, password, fullName, idCard, phone, email)) {
        showMessage("成功", "用户注册成功！");
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        showMessage("错误", "用户注册失败，用户名或身份证可能已存在！");
    }
}

void LoginWindow::switchToLoginPage()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void LoginWindow::showMessage(const QString& title, const QString& message)
{
    QMessageBox::information(this, title, message);
}
