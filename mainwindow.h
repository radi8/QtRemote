#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QUdpSocket>
#include <QTcpSocket>
#include <QAbstractSocket>
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

    QByteArray mySendString;
    QByteArray Buffer;

    enum { // Send these commands to ESP01
        CMD_PWR_ON = 1,
        CMD_PWR_OFF,
        CMD_TUNE,
        CMD_READ_A0,
        CMD_READ_A1,
        CMD_READ_A2,
        CMD_READ_D2,
        CMD_READ_D3,
        CMD_SET_RLY1_ON,
        CMD_SET_RLY1_OFF,
        CMD_SET_RLY2_ON,
        CMD_SET_RLY2_OFF,
        CMD_SET_LED_HI,
        CMD_SET_LED_LO,
        CMD_STATUS,
        CMD_ID // Always keep this last
    };

    enum { // Receive these commands/responses from ESP01
        _pwrSwitch = CMD_ID + 1,
        _tuneState,
        _volts,
        _amps,
        _analog2,
        _digital2,
        _digital3,
        _rly1,
        _rly2,
        _antenna,
        _message
    };
public slots:

private slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

    void on_pushButton_Clear_clicked();
    void on_pushButton_Close_clicked();
    void on_pushButton_Pwr_clicked();
    void on_actionSettings_triggered();
    void on_pushButton_Tune_clicked();
    void on_pushButton_2_clicked();
    void on_pBtn_Relay2_clicked();
    void on_pBtn_Relay1_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QLabel *statusLabel;
    qint16 localPort;
    qint16 remotePort;
    QString remoteIP;
    void sendData();
};

#endif // MAINWINDOW_H
