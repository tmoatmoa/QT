#include "login.h"
#include "ui_login.h"
#include "registe.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QtNetwork>
#include <synchapi.h>
#include <QtGlobal>

int num;
int judge=2;
QString temp;

login::login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
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
    num=qrand()%999;

    qDebug()<<num<<endl;

    sender = new QUdpSocket(this);
    receiver = new QUdpSocket(this);
    receiver->bind(45455,QUdpSocket::ShareAddress);
    connect(receiver, SIGNAL(readyRead()), this, SLOT(login1()));

}

login::~login()
{
    delete ui;
}


QHostAddress getHostIPV4Address()
{
    foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
        if ( hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address() )
            return hostAddress;
    return QHostAddress::LocalHost;
}

void login::on_pushButton_clicked()
{
    extern QString user ;

    QHostAddress address = getHostIPV4Address();

    int mes = 1;
    QString mesg = QString::number(mes);
    user = ui->lineEdit->text();
    if(ui->lineEdit->text()!=NULL&&ui->lineEdit_2->text()!=NULL){
        QString signal = QString::number(num);
        QByteArray datagram = mesg.toUtf8()+"><"+ui->lineEdit->text().toUtf8()+"><"                                      //传输的消息
                +ui->lineEdit_2->text().toUtf8()+"><"+signal.toUtf8()+"><"+address.toString().toUtf8()+"><"+" "+"><"+" ";
        sender->writeDatagram(datagram.data(), datagram.size(),QHostAddress("192.168.2.2"), 45454);
    }
    else{
        QMessageBox::information(this,"提示","用户名或密码不能为空");
    }
}

void login::on_pushButton_2_clicked()
{
    registe *reg = new registe;
    reg->setWindowTitle("注册页面");
    reg->show();
    this->close();
}

void login::login1()
{
    while(receiver->hasPendingDatagrams())                                 //收到返回消息后的动作
    {
        QByteArray datagram;
        datagram.resize(receiver->pendingDatagramSize());
        receiver->readDatagram(datagram.data(), datagram.size());
        temp = datagram;
    }
    QStringList list = temp.split("><");
    QString jud = list[0];
    QString sign = list[1];

    qDebug()<<sign.toInt();
    if(num==sign.toInt()){
        judge=jud.toInt();
        qDebug()<<num;
        if(judge==0){
            MainWindow *main =new MainWindow;
            main->setWindowTitle("聊天页面");
            main->show();
            judge=2;
            this->close();
        }
        else if(judge==1){
            QMessageBox::information(this,"提示","密码错误");
        }
        else if(judge==2){
            QMessageBox::information(this,"提示","没有该用户");
        }
    }


}
