#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void allwork();


private:
    Ui::MainWindow *ui;
    QUdpSocket *sender;
    QUdpSocket *receiver;
};

#endif // MAINWINDOW_H
