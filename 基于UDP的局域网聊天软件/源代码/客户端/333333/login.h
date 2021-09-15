#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QtNetwork>


namespace Ui {
class login;
}

class login : public QMainWindow
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void login1();

private:
    Ui::login *ui;
    QUdpSocket *sender;
    QUdpSocket *receiver;
};

#endif // LOGIN_H
