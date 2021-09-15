#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class QUdpSocket;


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

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void processPendingDatagram();

    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QUdpSocket *sender;
     QUdpSocket *receiver;
};



#endif // MAINWINDOW_H
