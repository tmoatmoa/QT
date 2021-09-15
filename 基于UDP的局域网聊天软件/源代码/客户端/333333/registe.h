#ifndef REGISTE_H

#define REGISTE_H

#include <QMainWindow>
#include <QtNetwork>

namespace Ui {
class registe;
}

class registe : public QMainWindow
{
    Q_OBJECT

public:
    explicit registe(QWidget *parent = nullptr);
    ~registe();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void registe1();

private:
    Ui::registe *ui;
    QUdpSocket *sender;
    QUdpSocket *receiver;
};

#endif // REGISTE_H
