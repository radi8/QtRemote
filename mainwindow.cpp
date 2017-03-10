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
    QTimer *timer = new QTimer(this);

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(timer, SIGNAL(timeout()), this, SLOT(getData()));

    timer->start(2000); //DEBUG in final version make the timer "timer->start(500)"
    mySendString = (QByteArray::number(CMD_ID, 10) + "\r"); // Get the slave address
    sendData();
    qDebug() << "connecting...";
}

MainWindow::~MainWindow()
{
    if (ui->pushButton_Pwr->isChecked() ) { // Power button is on
        mySendString = (QByteArray::number(CMD_PWR_OFF, 10) + "\r\n");
        sendData(); // Turn power off
//        qDebug() << "Power turned off before closing";
    }
    if (ui->pBtn_Relay1->isChecked() ) { // Power button is on
        mySendString = (QByteArray::number(CMD_SET_RLY1_OFF, 10) + "\r\n");
        sendData(); // Turn Relay1 off
//        qDebug() << "Relay1 turned off before closing";
    }
    if (ui->pBtn_Relay2->isChecked() ) { // Power button is on
        mySendString = (QByteArray::number(CMD_SET_RLY2_OFF, 10) + "\r\n");
        sendData(); // Turn Relay2 off
//        qDebug() << "Relay2 turned off before closing";
    }
    delete ui;
}

void MainWindow::sendData()
// Sends the text contained in QByteArray mySendString.
// It is possible that we may be receiving data from the server when a button click etc. gets us here
// so we wait for the receive to disconnect before sending the data. When we finally connect the slot
// "MainWindow::connected()" will be called and the data in the buffer is sent from there
{
    if (tcpSocket->state() != QAbstractSocket::UnconnectedState) {
        qDebug() << Q_FUNC_INFO << "socket already connected, waiting for process to complete..";
        tcpSocket->waitForDisconnected(100); //We are only blocking for 100 mSec max
    }
    // this is not blocking call
    tcpSocket->connectToHost(remoteIP, remotePort);

    // we need to wait...
    if(!tcpSocket->waitForConnected(5000))
    {
        qDebug() << Q_FUNC_INFO << "Error: " << tcpSocket->errorString();
        ui->statusBar->showMessage("Error: " + tcpSocket->errorString() );
    }
}

void MainWindow::getData()
{
    mySendString = (QByteArray::number(CMD_STATUS, 10) + "\r");
    sendData();
//    mySendString = (QByteArray::number(CMD_READ_A1, 10) + "\r");
//    sendData();
}

void MainWindow::readyRead()
{
    QByteArray Buffer;

    // To avoid receiving more than one line at the time but only reading the first one.
    // Read as many lines as available by checking with canReadLine.
    while (tcpSocket->canReadLine())
        {
            Buffer = QByteArray(tcpSocket->readLine());
            processReceived(Buffer);
            qDebug() << Q_FUNC_INFO << "Value of Buffer = " << Buffer;
        }
}

void MainWindow::processReceived(QByteArray Buffer)
// When data arrives from the remote end, this routine places the sent string
// into "myMessage". The message will take the form of: cmdNum cmdVal The cmdval
// may be another number or an information string. The numbers and values are
// extracted and converted to long integers or a string if cmdVal is a message.
{
/*
    enum { // Receive these commands/responses from ESP01
        _pwrSwitch = CMD_ID + 1, (17 as at 2017-3-10)
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

//    qDebug() << Q_FUNC_INFO << "Buffer = " << Buffer;
    cmd = strtol(Buffer, &pEnd, 10);
    if(cmd != _message) {
        cmdValue = strtol (pEnd, &pEnd,10);
    } else {
        myMessage = pEnd;
//        qDebug() << Q_FUNC_INFO << "At else statement: myMessage = " << myMessage;
    }

//qDebug() << "command received = " << cmd << " and command value = " << cmdValue;
//qDebug() << Q_FUNC_INFO << "Value of Buffer after cmd extracted = " << Buffer;
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
        cmdVal = cmdValue / 51.2;
        ui->frame->meter_dbm = cmdVal;
        ui->frame->update();
        break;
    case _amps:
        cmdVal = cmdValue / 51.2;
        ui->frame->sub_meter_dbm = cmdVal;
        ui->frame->update();
        break;
    case _analog2:
//        ui->textBrowser->append("Analog2 data returned");
        break;
    case _digital2:
//        ui->textBrowser->append("Digital 2 data returned");
        break;
    case _digital3:
//        ui->textBrowser->append("Digital 3 data returned");
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
        // Tuning completed notification etc.
        if(myMessage == " Receiving from server\r\n") ui->textBrowser->clear();
        ui->textBrowser->append(myMessage);
//        qDebug() << Q_FUNC_INFO << "Message = " << myMessage;
        break;
    default:
        // if nothing else matches, do the default
        qDebug() << Q_FUNC_INFO << "Doing the default case. Buffer = " << Buffer;
        ui->textBrowser->append(Buffer);
      break;
    }
}

void MainWindow::connected()
{
    qDebug() << Q_FUNC_INFO << "Writing to server data in mySendString = " << mySendString;

    ui->statusBar->showMessage("Connected to remote server...");
    tcpSocket->write(mySendString);
    tcpSocket->flush();
}

void MainWindow::disconnected()
{
    qDebug() << Q_FUNC_INFO << "disconnected by host...";
}

void MainWindow::bytesWritten(qint64 bytes)
{
    qDebug() << Q_FUNC_INFO << bytes << " bytes written...";
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
        mySendString = (QByteArray::number(CMD_PWR_ON, 10) + "\r\n");
        sendData();
    } else {
        ui->pushButton_Pwr->setStyleSheet("background-color: rgb(85, 255, 0)"); //Green
        mySendString = (QByteArray::number(CMD_PWR_OFF, 10) + "\r\n");
        sendData();
    }
    qDebug() << Q_FUNC_INFO << "mySendString from pwr button = " << mySendString;
}

void MainWindow::on_pushButton_Tune_clicked()
{
    mySendString = (QByteArray::number(CMD_TUNE, 10) + "\r\n");
    sendData();
    qDebug() << Q_FUNC_INFO << "mySendString from Tune button = " << mySendString;
}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pBtn_Relay1_clicked()
{
    if (ui->pBtn_Relay1->isChecked()) {
        mySendString = (QByteArray::number(CMD_SET_RLY1_ON, 10) + "\r\n");
    } else {
        mySendString = (QByteArray::number(CMD_SET_RLY1_OFF, 10) + "\r\n");
    }
    sendData();
    qDebug() << Q_FUNC_INFO << "mySendString = " << mySendString;
}

void MainWindow::on_pBtn_Relay2_clicked()
{
    if (ui->pBtn_Relay2->isChecked()) {
        mySendString = (QByteArray::number(CMD_SET_RLY2_ON, 10) + "\r\n");
    } else {
        mySendString = (QByteArray::number(CMD_SET_RLY2_OFF, 10) + "\r\n");
    }
    sendData();
    qDebug() << Q_FUNC_INFO << "mySendString = " << mySendString;
}

