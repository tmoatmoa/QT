#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>

QString temp;
QStringList list;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sender = new QUdpSocket(this);
    receiver = new QUdpSocket(this);
    receiver->bind(45454, QUdpSocket::ShareAddress);
    connect(receiver, &QUdpSocket::readyRead, this, &MainWindow::allwork);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int addres(QString address){                                             //增加
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("jiwangkeshe");
    db.setUserName("root");
    db.setPassword("616709");
    db.open();
    if(!db.open())
    {
        qDebug()<<"connect failed"<<"connect to mysql error"<<db.lastError().text();
    }
    else
    {
        qDebug()<<"connect to mysql OK";
    }
    QSqlQuery query(db);
    query.exec("select * from address");
    int jud=0;
    while(query.next()){
    if(query.value("ip")==address)
        jud++;
    }
    if(jud==0){
        QString savesql = QString("INSERT INTO address(ip)");
        savesql += QString(" VALUES('%1')")
                .arg(address);
        bool ok=query.exec(savesql);
        if(ok){
            return 0;
        }
        else{
            return 2;
        }
    }
}

int confirm(QString aa,QString bb)                                //登录验证
{

    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("jiwangkeshe");
    db.setUserName("root");
    db.setPassword("616709");
    db.open();
    if(!db.open())
    {
        qDebug()<<"connect failed"<<"connect to mysql error"<<db.lastError().text();
    }
    else
    {
        qDebug()<<"connect to mysql OK";
    }
    QSqlQuery query(db);
    query.exec("select * from information");
    while(query.next()){                        //“0”为正确，“1”为密码错误，“2”为用户不存在
        if(query.value("username")==aa){
            qDebug()<<query.value("username");
            if(query.value("password")==bb){
                return 0;
            }
            else if(query.value("password").toString()!=bb){
                return 1;
            }
        }
    }
    return 2;
}

int registe1(QString aa,QString bb){                                 //注册
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("jiwangkeshe");
    db.setUserName("root");
    db.setPassword("616709");
    db.open();
    if(!db.open())
    {
        qDebug()<<"connect failed"<<"connect to mysql error"<<db.lastError().text();
    }
    else
    {
        qDebug()<<"connect to mysql OK";
    }
    QSqlQuery query(db);
    query.exec("select * from information");
    while(query.next()){
        if(query.value("username")==aa){
            return 1;                               //用户已存在
        }
    }

    QString savesql = QString("INSERT INTO information(username,password)");
    savesql += QString(" VALUES('%1','%2')")
            .arg(aa)
            .arg(bb);
    bool ok=query.exec(savesql);
    if(ok){
        return 0;
    }
    else{
        return 2;
    }
}

void MainWindow::allwork()
{
    while(receiver->hasPendingDatagrams())
    {QByteArray datagram;
        datagram.resize(receiver->pendingDatagramSize());
        receiver->readDatagram(datagram.data(), datagram.size());
        temp = datagram;
        list = temp.split("><");
        QString mes = list[0];
        QString mes1 = list[1];
        QString mes2 = list[2];
        QString mes3 = list[3];
        QString mes4 = list[4];
        QString mes5 = list[5];
        QString mes6 = list[6];
        qDebug()<<mes;
        if(mes.toInt() == 0){                                               //聊天
            receiver->readDatagram(datagram.data(), datagram.size());
            qDebug()<<datagram;

            QSqlDatabase db;
            if(QSqlDatabase::contains("qt_sql_default_connection"))
              db = QSqlDatabase::database("qt_sql_default_connection");
            else
              db = QSqlDatabase::addDatabase("QMYSQL");
            db.setHostName("127.0.0.1");
            db.setPort(3306);
            db.setDatabaseName("jiwangkeshe");
            db.setUserName("root");
            db.setPassword("616709");
            db.open();
            if(!db.open())
            {
                qDebug()<<"connect failed"<<"connect to mysql error"<<db.lastError().text();
            }
            else
            {
                qDebug()<<"connect to mysql OK";
            }
            QSqlQuery query(db);
            query.exec("select * from address");
            while(query.next()){
            sender->writeDatagram(datagram.data(), datagram.size(),QHostAddress(query.value("ip").toString()), 45457);
            }
        }
        else if(mes.toInt() == 1){                                 //登录
            QString username = list[1];
            QString password = list[2];
            QString sign = list[3];
            QString address = list[4];

            int add = addres(address);
            qDebug()<<username<<endl<<password<<endl<<sign;
            int judge = confirm(username,password);
            qDebug()<<judge;
            QString jud = QString::number(judge);
            QByteArray sendmsg =jud.toUtf8()+"><"+sign.toUtf8();

            QSqlDatabase db;
            if(QSqlDatabase::contains("qt_sql_default_connection"))
              db = QSqlDatabase::database("qt_sql_default_connection");
            else
              db = QSqlDatabase::addDatabase("QMYSQL");
            db.setHostName("127.0.0.1");
            db.setPort(3306);
            db.setDatabaseName("jiwangkeshe");
            db.setUserName("root");
            db.setPassword("616709");
            db.open();
            if(!db.open())
            {
                qDebug()<<"connect failed"<<"connect to mysql error"<<db.lastError().text();
            }
            else
            {
                qDebug()<<"connect to mysql OK";
            }
            QSqlQuery query(db);
            query.exec("select * from address");
            while(query.next()){
                sender->writeDatagram(sendmsg.data(), sendmsg.size(),QHostAddress(query.value("ip").toString()), 45455);
            }

        }
        else if(mes.toInt() == 2){                                          //注册
            QString username = list[1];
            QString password = list[2];
            QString sign = list[3];
            qDebug()<<username<<endl<<password<<endl<<sign;
            int judge = registe1(username,password);
            qDebug()<<judge;
            QString jud = QString::number(judge);
            QByteArray sendmsg =jud.toUtf8()+"><"+sign.toUtf8();

            QSqlDatabase db;
            if(QSqlDatabase::contains("qt_sql_default_connection"))
              db = QSqlDatabase::database("qt_sql_default_connection");
            else
              db = QSqlDatabase::addDatabase("QMYSQL");
            db.setHostName("127.0.0.1");
            db.setPort(3306);
            db.setDatabaseName("jiwangkeshe");
            db.setUserName("root");
            db.setPassword("616709");
            db.open();
            if(!db.open())
            {
                qDebug()<<"connect failed"<<"connect to mysql error"<<db.lastError().text();
            }
            else
            {
                qDebug()<<"connect to mysql OK";
            }
            QSqlQuery query(db);
            query.exec("select * from address");
            while(query.next()){
            sender->writeDatagram(sendmsg.data(), sendmsg.size(),QHostAddress(query.value("ip").toString()), 45456);
            }
        }

    }


}

void MainWindow::on_pushButton_clicked()
{
QString aa="alsdkjlaskd";
qDebug()<<aa;
}
