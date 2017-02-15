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

    mySendString = "/gpio/1\r";
    qDebug() << "connecting...";

    sendData();


//  Test the connection
//    sendData("09 Request full status report");

//    buttonValue = 5; // debug data while testing remove in final (don't forget header declaration also)
    ui->frame->meter_dbm = 14.5; // debug data while testing
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendData()
{
    // this is not blocking call
    tcpSocket->connectToHost(remoteIP, remotePort);

    // we need to wait...
    if(!tcpSocket->waitForConnected(5000))
    {
        qDebug() << "Error: " << tcpSocket->errorString();
    }
//    QByteArray Data;

//    Data.append(mySendString);
//    tcpSocket->writeDatagram(Data,QHostAddress(remoteIP),remotePort);
//    ui->statusBar->showMessage("Not connected to remote");
//    ui->statusBar->clearMessage();

}

// When data arrives from the remote end, this routine strips off the html
// headers and places the resulting string into "myMessage". The message
// will take the form of: cmdNum cmdVal The cmdval may be another number
// or an information string. The numbers and values are extracted and converted
// to long integers or a string if cmdVal is a message.
void MainWindow::readyRead()
{
/*  enum {  Displayed here for easy reference
        _volts,
        _amps,
        _analog2,
        _tuneState,
        _pwrSwitch,
        _rly1,
        _rly2,
        _antenna,
        _message,
    }; */
    QByteArray Buffer;
    QString myMessage;
    long cmdNumber = 0;
    char * pEnd;

    Buffer.resize(tcpSocket->readBufferSize());

    qDebug() << "reading...";
    // read the data from the socket
    Buffer = tcpSocket->readAll();
    qDebug() << Buffer;
    myMessage = Buffer;

    if(myMessage.left(4) != "GPIO") {
        cmdNumber = strtol(Buffer, &pEnd, 10);
    } else {
        cmdNumber = _message;
    }

    switch (cmdNumber) {
    case _volts:
//        Buffer = "03 1"; //debug data for power switch state, remove in final version
//        buttonValue = 0; // Power on
        break;
    case _amps:
//        Buffer = "03 0"; //debug data for power switch state, remove in final version
//        buttonValue = 0; // Power off
        break;
    case _analog2:

        break;
    case _tuneState:

        break;
    case _pwrSwitch:

        break;
    case _rly1:

        break;
    case _rly2:

        break;
    case _antenna:

        break;
//    case rly1:

//        break;
    case _message:
        ui->textBrowser->append( "Message: " + myMessage);
        break;
    default:
        // if nothing else matches, do the default
        // default is optional
      break;
    }
//    processBuffer();
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

/*
void MainWindow::sendRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QTe);
    out.setVersion(QDataStream::Qt_5_1);
}
*/
void MainWindow::processBuffer()
{
    long cmd;
    long cmdValue;
    char * pEnd;
    double cmdVal;

    cmd = strtol(Buffer, &pEnd, 10);
    cmdValue = strtol (pEnd,&pEnd,10);
    cmdVal = cmdValue / 1000.0;
qDebug() << "command received = " << cmd << " and command value = " << cmdVal;

    switch (cmd) {
    case 1:
        ui->frame->meter_dbm = cmdVal;
        ui->frame->sub_meter_dbm = cmdVal;
        ui->frame->update();
        break;
    case 2:
        ui->frame->meter_dbm = cmdVal;
        ui->frame->sub_meter_dbm = cmdVal;
        ui->frame->update();
        break;
    case 3:
        // Power switch status. (0 = power off, 1 = Power on)
        if (cmdValue) {
            ui->textBrowser->append("Power on");
        } else {
            ui->textBrowser->append("Power off");
        }
        break;
    case 4:
        // Relay 1 status (0 = released, 1 = operated)
        if (cmdValue) {
            ui->textBrowser->append("Relay 1 is operated");
        } else {
            ui->textBrowser->append("Relay 1 is released");
        }
        break;
    case 5:
        // Relay 2 status (0 = released, 1 = operated)
        if (cmdValue) {
            ui->textBrowser->append("Relay 2 is operated");
        } else {
            ui->textBrowser->append("Relay 2 is released");
        }
        break;
    case 6:
        // Tuning completed notification
        break;
    case 7:
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
    default:
        // if nothing else matches, do the default
        // default is optional
      break;
    }
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
        CMD_READ_D2,
        CMD_READ_D3,
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

