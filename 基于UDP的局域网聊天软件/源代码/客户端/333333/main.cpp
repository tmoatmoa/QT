#include "login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    login w;
    w.setWindowTitle("登录页面");
    w.show();

    return a.exec();
}
