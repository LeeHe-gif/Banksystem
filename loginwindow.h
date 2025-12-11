#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginWindow;
}
QT_END_NAMESPACE

class MainWindow;

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    void showLoginWindow();

private slots:
    void onLoginClicked();
    void onConnectClicked();
    void onRegisterClicked();
    void onConnectDBClicked();  // 新增：连接数据库按钮点击

private:
    Ui::LoginWindow *ui;
    DatabaseManager& dbManager;
    MainWindow* mainWindow;

    void showMessage(const QString& title, const QString& message);
    void switchToLoginPage();
};

#endif // LOGINWINDOW_H
