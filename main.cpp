#include "loginwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置应用程序样式
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // 设置应用程序信息
    QApplication::setApplicationName("BankSystem");
    QApplication::setOrganizationName("BankCorp");

    LoginWindow w;
    w.show();

    return a.exec();
}
