#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mysettings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_Pwr->setStyleSheet("background-color: rgb(85, 255, 0)"); //Green
    setWindowTitle(tr("Remote controller"));
    statusLabel = new QLabel(this);
    ui->statusBar->addWidget(statusLabel);
    ui->statusBar->showMessage("Not connected to remote");

// Connecting to remote
    remoteIP = "192.168.1.70";
    localPort = 7200;
    remotePort = 8475;

    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    mySendString = (QByteArray::number(CMD_READ_A1, 10) + "\r");
    sendData();
    qDebug() << "connecting...";

//    ui->frame->meter_dbm = 14.5; // debug data while testing
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendData()
// Sends the text contained in QByteArray mySendString
{
    // this is not blocking call
    tcpSocket->connectToHost(remoteIP, remotePort);

    // we need to wait...
    if(!tcpSocket->waitForConnected(5000))
    {
        qDebug() << "Error: " << tcpSocket->errorString();
    }
}

void MainWindow::readyRead()
// When data arrives from the remote end, this routine places the sent string
// into "myMessage". The message will take the form of: cmdNum cmdVal The cmdval
// may be another number or an information string. The numbers and values are
// extracted and converted to long integers or a string if cmdVal is a message.
{
/*
    enum { // Receive these commands/responses from ESP01
        _pwrSwitch = 1, //Start the emun from 1
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
*/

    long cmd;
    long cmdValue;
    char * pEnd;
    double cmdVal;

    Buffer.resize(tcpSocket->readBufferSize());

    qDebug() << "reading...";
    // read the data from the socket into Buffer
    Buffer = tcpSocket->readAll();

    cmd = strtol(Buffer, &pEnd, 10);
    cmdValue = strtol (pEnd, &pEnd,10);

qDebug() << "@MainWindow::readyRead(): Value of Buffer = " << Buffer;
qDebug() << "command received = " << cmd << " and command value = " << cmdValue;

    switch (cmd) {
    case _pwrSwitch:
        // Power switch status. (0 = power off, 1 = Power on)
        if (cmdValue) {
            ui->textBrowser->append("Power on");
        } else {
            ui->textBrowser->append("Power off");
        }
        break;
    case _tuneState:
        // Tuning completed notification
        break;
    case _volts:
        cmdVal = cmdValue / 204.6;
        ui->frame->meter_dbm = cmdVal;
        ui->frame->sub_meter_dbm = cmdVal;
        ui->frame->update();
        break;
    case _amps:
        cmdVal = cmdValue / 204.6;
        ui->frame->meter_dbm = cmdVal;
        ui->frame->sub_meter_dbm = cmdVal;
        ui->frame->update();
        break;
    case _analog2:

        break;
    case _digital2:

        break;
    case _digital3:

        break;
    case _rly1:
        // Relay 1 status (0 = released, 1 = operated)
        if (cmdValue) {
            ui->textBrowser->append("Relay 1 is operated");
        } else {
            ui->textBrowser->append("Relay 1 is released");
        }
        break;
    case _rly2:
        // Relay 2 status (0 = released, 1 = operated)
        if (cmdValue) {
            ui->textBrowser->append("Relay 2 is operated");
        } else {
            ui->textBrowser->append("Relay 2 is released");
        }
        break;
    case _antenna:
        // Selected antenna status
        switch (cmdValue) {
        case 0:
            ui->textBrowser->append("No antenna selected");
            break;
        case 1:
            ui->textBrowser->append("Antenna 1 selected");
            break;
        case 2:
            ui->textBrowser->append("Antenna 2 selected");
            break;
        case 3:
            ui->textBrowser->append("Antenna 3 selected");
            break;
        default:
            break;
        }
        break;
    case _message:
        // Tuning completed notification
        break;
    default:
        // if nothing else matches, do the default
        qDebug() << "@processBuffer: Doing the default case";
        ui->textBrowser->append(Buffer);
      break;
    }
}

void MainWindow::connected()
{
    qDebug() << "connected...";
    qDebug() << mySendString;
    // Hey server, switch the gpio
    tcpSocket->write(mySendString);
}

void MainWindow::disconnected()
{
    qDebug() << "disconnected...";
}

void MainWindow::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void MainWindow::on_pushButton_Close_clicked()
{
    close();
}

void MainWindow::on_pushButton_Clear_clicked()
{
    ui->textBrowser->clear();
}


void MainWindow::on_actionSettings_triggered()
{
    mySettings mSettings;
    mSettings.setModal(true);
    mSettings.exec();
}
/*
    enum { // enum placed here for easy reference
        CMD_PWR_ON = 1, //Start the enum from 1
        CMD_PWR_OFF,
        CMD_TUNE,
        CMD_READ_A0,
        CMD_READ_A1,
        CMD_READ_A2,
        CMD_D2_HI,
        CMD_D2_LO,
        CMD_D3_HI,
        CMD_D3_LO,
        CMD_SET_RLY1_ON,
        CMD_SET_RLY1_OFF,
        CMD_SET_RLY2_ON,
        CMD_SET_RLY2_OFF,
        CMD_SET_LED_HI,
        CMD_SET_LED_LO,
        CMD_STATUS,
        CMD_ID
    }; */
void MainWindow::on_pushButton_Pwr_clicked()
{

    if (ui->pushButton_Pwr->isChecked()) {
        ui->pushButton_Pwr->setStyleSheet("background-color: rgb(255, 155, 155)"); //Light Red
        mySendString = (QByteArray::number(CMD_PWR_ON, 10) + "\r");
        sendData();
    } else {
        ui->pushButton_Pwr->setStyleSheet("background-color: rgb(85, 255, 0)"); //Green
        mySendString = (QByteArray::number(CMD_PWR_OFF, 10) + "\r");
        sendData();
    }
    qDebug() << "mySendString from pwr button = " << mySendString;
}

void MainWindow::on_pushButton_Tune_clicked()
{
    ui->pushButton_Pwr->setStyleSheet("background-color: rgb(85, 255, 0)"); //Green
    mySendString = (QByteArray::number(CMD_TUNE, 10) + "\r");
    sendData();
    qDebug() << "mySendString from Tune button = " << mySendString;
}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pBtn_Relay1_clicked()
{
    if (ui->pBtn_Relay1->isChecked()) {
        mySendString = (QByteArray::number(CMD_SET_RLY1_ON, 10) + "\r");
    } else {
        mySendString = (QByteArray::number(CMD_SET_RLY1_OFF, 10) + "\r");
    }
    sendData();
    qDebug() << "mySendString from Relay1 button = " << mySendString;
}

void MainWindow::on_pBtn_Relay2_clicked()
{
    if (ui->pBtn_Relay2->isChecked()) {
        mySendString = (QByteArray::number(CMD_SET_RLY2_ON, 10) + "\r");
    } else {
        mySendString = (QByteArray::number(CMD_SET_RLY2_OFF, 10) + "\r");
    }
    sendData();
    qDebug() << "mySendString from Relay2 button = " << mySendString;
}

