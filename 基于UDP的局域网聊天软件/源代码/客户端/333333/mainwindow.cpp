#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
#include <QtNetwork>
#include <QMessageBox>

QString user;
QString temp2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sender = new QUdpSocket(this);
    receiver = new QUdpSocket(this);
    receiver->bind(45457, QUdpSocket::ShareAddress);
    connect(receiver, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagram);
    ui->textEdit->setFont(QFont(tr("楷体"), 12));
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processPendingDatagram()
{
    while(receiver->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(receiver->pendingDatagramSize());
        receiver->readDatagram(datagram.data(), datagram.size());
        temp2 = datagram;
        qDebug()<<temp2;
    }
    QStringList list;
    list = temp2.split("><");
    QString username = list[1];
    QString text = list[2];
    QString font = list[3];
    int size = list[4].toInt();
    QString color = list[5];
    QString time =list[6];
    ui->textBrowser->append(tr("<font size = 2 color = gray>%1 &nbsp;&nbsp; %2 :</font>").arg(time).arg(username));             //富文本
    ui->textBrowser->append(tr("<font size = %4 face = %1 color = %3>%2</font>").arg(font).arg(text).arg(color).arg(size));
}

void MainWindow::on_pushButton_clicked()
{
    QTime current_time =QTime::currentTime();    //获取当前时间
    QString time = current_time.toString();

    if(ui->textEdit->toPlainText()!=NULL ){
        int mes =0;
        QString mesg=QString::number(mes);
    int ss=ui->comboBox_2->currentIndex()+1;
    QString sss=QString::number(ss);
    QByteArray datagram = mesg.toUtf8()+"><"                        //聊天信息
            +user.toUtf8()+"><"
            +ui->textEdit->toPlainText().toUtf8()+"><"
            +ui->fontComboBox->currentText().toUtf8()+"><"
            +sss.toUtf8()+"><"
            +ui->comboBox->currentText().toUtf8()+"><"
            +time.toUtf8();
    sender->writeDatagram(datagram.data(), datagram.size(),QHostAddress("192.168.2.2"), 45454);
    ui->textEdit->setText(NULL);
    }
    else{
        QMessageBox::information(this,"提示","发送消息不可为空");
    }
}

void MainWindow::on_fontComboBox_currentFontChanged(const QFont &f)               //发送文本框格式
{
    ui->textEdit->setText(NULL);
    ui->textEdit->setFontFamily(f.toString());
}

void MainWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    ui->textEdit->setText(NULL);
    ui->textEdit->setFontPointSize(arg1.toInt());
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->textEdit->setText(NULL);
    ui->textEdit->setTextColor(arg1);
}
