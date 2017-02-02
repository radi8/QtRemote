#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void sendData(QString mySendString);

public slots:
    void readyRead();

private slots:
    void on_pushButton_Tune_clicked();

    void on_pushButton_Clear_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_Pwr_clicked();

    void on_actionSettings_triggered();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    QLabel *statusLabel;
    QByteArray Buffer;
    qint16 localPort;
    qint16 remotePort;
    QString remoteIP;
    bool isAlive;
};

#endif // MAINWINDOW_H
