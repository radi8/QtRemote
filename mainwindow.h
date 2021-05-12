#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void loadFile(const QString &fileName);

    QByteArray mySendString;

    enum { // Receive commands from tcp client. Send commands to I2C slave.
      CMD_PWR_ON = 1, //Start the enum from 1
      CMD_PWR_OFF,
      CMD_RLY1_ON,    // HiQSDR
      CMD_RLY1_OFF,
      CMD_RLY2_ON,    //HermesLite
      CMD_RLY2_OFF,
      CMD_RLY3_ON,    // Linear
      CMD_RLY3_OFF,
      CMD_RLY4_ON,    // Tuner
      CMD_RLY4_OFF,
      CMD_TUNE_DN,
      CMD_TUNE_UP,
      CMD_RADIO_0,    // No antenna selected
      CMD_RADIO_1,    // wire Antenna selected
      CMD_RADIO_2,
      CMD_RADIO_3,
      CMD_RADIO_4,
      CMD_READ_A0,    // Shack voltage
      CMD_READ_A1,
      CMD_READ_A2,
      CMD_READ_D2,    // Digital input via opto-coupler
      CMD_READ_D3,
      CMD_SET_LED_HI,
      CMD_SET_LED_LO,
      CMD_STATUS,
      CMD_ID // Always keep this last
    };

    enum { // Receive commands from remoteArduino (I2C slave) via ESP01.
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

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void getData();
    void readSettings();
    void writeSettings();
    void on_pushButton_Clear_clicked();
    void on_pushButton_Close_clicked();
    void on_pushButton_Pwr_clicked();
    void on_actionSettings_triggered();
    void on_pBtn_Relay2_clicked();
    void on_pBtn_Relay1_clicked();
    void on_pBtn_Relay3_clicked();
    void on_pBtn_Relay4_clicked();
    void on_pBtn_Tune_pressed();
    void on_pBtn_Tune_released();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QLabel *statusLabel;
    qint16 localPort;
    qint16 remotePort;
    QString remoteIP;
    QString myMessage;
    void sendData();
    void processReceived(QByteArray recdBuf);
//    const QRect availableGeometry;
};

#endif // MAINWINDOW_H
