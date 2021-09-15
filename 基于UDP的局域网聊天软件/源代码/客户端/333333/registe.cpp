#include "registe.h"
#include "ui_registe.h"
#include "login.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QtNetwork>

int num1;
QString temp1;

registe::registe(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::registe)
{
    ui->setupUi(this);
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
    ui->lineEdit_3->setEchoMode(QLineEdit::Password);

    ui->pushButton->setStyleSheet("QPushButton{"
                                  "background-color:rgba(100,225,100,30);"//背景色（也可以设置图片）
                                  "border-style:outset;"                  //边框样式（inset/outset）
                                  "border-width:4px;"                     //边框宽度像素
                                  "border-radius:10px;"                   //边框圆角半径像素
                                  "border-color:rgba(255,255,255,30);"    //边框颜色
                                  "padding:6px;"                          //填衬
                                  "}");

    ui->pushButton_2->setStyleSheet("QPushButton{"
                                  "background-color:rgba(100,225,100,30);"//背景色（也可以设置图片）
                                  "border-style:outset;"                  //边框样式（inset/outset）
                                  "border-width:4px;"                     //边框宽度像素
                                  "border-radius:10px;"                   //边框圆角半径像素
                                  "border-color:rgba(255,255,255,30);"    //边框颜色
                                  "padding:6px;"                          //填衬
                                  "}");

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    num1=qrand()%999;

    qDebug()<<num1<<endl;
    sender = new QUdpSocket(this);
    receiver = new QUdpSocket(this);
    receiver->bind(45456, QUdpSocket::ShareAddress);

    connect(receiver, &QUdpSocket::readyRead, this, &registe::registe1);

}

registe::~registe()
{
    delete ui;
}

void registe::on_pushButton_clicked()
{
    if (ui->lineEdit->text()==nullptr){
        QMessageBox::information(this,"提示","用户名不能为空");
    }
    else if(ui->lineEdit_2->text()!=ui->lineEdit_3->text()){
        QMessageBox::information(this,"提示","两次输入密码不一致");
        ui->lineEdit_2->setText(NULL);
        ui->lineEdit_3->setText(NULL);
    }
    else if(ui->lineEdit_2->text()==NULL&&ui->lineEdit_3->text()==NULL){
         QMessageBox::information(this,"提示","密码不可以为空");
    }
    else{
        int mes = 2;
        QString mesg = QString::number(mes);
        QString signal = QString::number(num1);
        QByteArray datagram =mesg.toUtf8()+"><"+ ui->lineEdit->text().toUtf8()+"><"+ui->lineEdit_2->text().toUtf8()+"><"              //传输的消息
                +signal.toUtf8()+"><"+" "+"><"+" "+"><"+" ";
        sender->writeDatagram(datagram.data(), datagram.size(),QHostAddress("192.168.2.2"), 45454);
        qDebug()<<datagram<<endl;
        qDebug()<<num1<<endl;
    }
}

void registe::on_pushButton_2_clicked()
{

    login *log = new login;
    log->setWindowTitle("登录页面");
    log->show();
    this->close();
}

void registe::registe1()
{
    while(receiver->hasPendingDatagrams())                                     //收到消息
    {
        QByteArray datagram;
        datagram.resize(receiver->pendingDatagramSize());
        receiver->readDatagram(datagram.data(), datagram.size());
        temp1 = datagram;
    }
    QStringList list = temp1.split("><");
    QString jud = list[0];
    QString sign = list[1];

    qDebug()<<sign.toInt();
    if(num1==sign.toInt()){
        if(jud.toInt()==0){
            QMessageBox::about(NULL, "提示", "注册成功。");
        }
        else if(jud.toInt()==1){
            QMessageBox::about(NULL, "提示", "该用户名已被使用");
        }
        else
            QMessageBox::about(NULL, "提示", "出  错！");
    }


}

