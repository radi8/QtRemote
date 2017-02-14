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

    mySendString = "/gpio/0\r\n\r\n\r\n";
    qDebug() << "connecting...";

    sendData();


//  Test the connection
//    sendData("09 Request full status report");

    buttonValue = 5; // debug data while testing remove in final (don't forget header declaration also)
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


void MainWindow::readyRead()
{
    QByteArray Buffer;
    int j = 0;
    int k = 0;
    int m = 0;

    Buffer.resize(tcpSocket->readBufferSize());

    qDebug() << "reading...";
    // read the data from the socket
    Buffer = tcpSocket->readAll();
//    qDebug() << Buffer;

    // Strip off all the html information from beginning and end of body
    while ((j = Buffer.indexOf("\n", j)) != -1) {
        m = k;
        k = j;
//        qDebug() << "Found LF tag at index position" << j << "and k = " << k << "and m = " << m;
        ++j;
    }
    m++;
    Buffer = Buffer.mid(m, k - m - 7);
//    ui->textBrowser->append(QVariant(m).toString()); // debug information
//    ui->textBrowser->append(QVariant(k).toString()); // debug information
    ui->textBrowser->append(Buffer);
//    qDebug() << "k = " << k << "and m = " << m;
/*
    QHostAddress sender;
    quint16 senderPort;
    QString myMessage;

    socket->readDatagram(Buffer.data(),Buffer.size(),&sender,&senderPort);
    myMessage = sender.toString() + QStringLiteral(" : %1").arg(senderPort);

    ui->textBrowser->append( "Message from: " + myMessage);
    ui->textBrowser->append( "Message: " + Buffer);
//  Let the user know that the connection is alive.
    ui->statusBar->showMessage("Connected to " + remoteIP + " : " +QString::number(remotePort));

    switch (buttonValue) {
    case 1:
        Buffer = "03 1"; //debug data for power switch state, remove in final version
        buttonValue = 0; // Power on
        break;
    case 2:
        Buffer = "03 0"; //debug data for power switch state, remove in final version
        buttonValue = 0; // Power off
        break;
    case 3:
        Buffer = "01 12600"; //debug data for meter value, remove in final version
        buttonValue = 0;
        break;
    case 4:
        Buffer = "02 0"; //debug data for meter value, remove in final version
        buttonValue = 0;
        break;
    default:
        // if nothing else matches, do the default
        // default is optional
      break;
    }
    processBuffer();
    */
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

void MainWindow::on_pushButton_Pwr_clicked()
{   
    if (ui->pushButton_Pwr->isChecked()) {
        ui->pushButton_Pwr->setStyleSheet("background-color: rgb(255, 155, 155)"); //Light Red
//        sendData("01 Power 0n");
        mySendString = "/gpio/0\r\n\r\n\r\n";
        sendData();
        buttonValue = 1; // debug remove on final version
    } else {
        ui->pushButton_Pwr->setStyleSheet("background-color: rgb(85, 255, 0)"); //Green
//        sendData("02 Power 0ff");
        mySendString = "/gpio/1\r\n\r\n\r\n";
        sendData();
        buttonValue = 2; // debug remove on final version
    }
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

void MainWindow::on_pushButton_Tune_clicked()
{
    ui->textBrowser->append("Tune button clicked");
//    sendData("03 Start autotune");
}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pBtn_Relay1_clicked()
{
    buttonValue = 3;  // debug data

    if (ui->pBtn_Relay1->isChecked()) {
        ui->textBrowser->append("Relay1 On clicked");
//        sendData("05 Operate Relay1");
    }
    else {
        ui->textBrowser->append("Relay1 Off clicked");
//        sendData("06 Release Relay1");
    }
}

void MainWindow::on_pBtn_Relay2_clicked()
{
    buttonValue = 4;  // debug data

    if (ui->pBtn_Relay2->isChecked()) {
        ui->textBrowser->append("Relay2 On clicked");
//        sendData("07 Operate Relay2");
    }
    else {
        ui->textBrowser->append("Relay2 Off clicked");
//        sendData("08 Release Relay1");
    }
}

