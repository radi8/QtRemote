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

    socket = new QUdpSocket(this);
    socket->bind(QHostAddress(remoteIP),localPort);
//    socket->bind(QHostAddress::AnyIPv4,7200);
    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));

//  Test the connection
    isAlive = true;
    sendData("00");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendData(QString mySendString)
{
    QByteArray Data;

    Data.append(mySendString);
    socket->writeDatagram(Data,QHostAddress(remoteIP),remotePort);
}


void MainWindow::readyRead()
{
//    QByteArray Buffer;
    Buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;
    QString myMessage;

    socket->readDatagram(Buffer.data(),Buffer.size(),&sender,&senderPort);
    myMessage = sender.toString() + QStringLiteral(" : %1").arg(senderPort);

    ui->textBrowser->append( "Message from: " + myMessage);
    ui->textBrowser->append( "Message: " + Buffer);
//  bool isAlive is set true in constructor making first message received see it
//  as true so we message on status to show a live connection and set false.
    if(isAlive) {
        ui->statusBar->showMessage("Connected to " + remoteIP + " : " +QString::number(remotePort));
        isAlive = false;
    }
}

void MainWindow::on_pushButton_Close_clicked()
{
    close();
}

void MainWindow::on_pushButton_Pwr_clicked()
{
        if(ui->pushButton_Pwr->isChecked())
        {
            ui->textBrowser->append("Power On clicked");
            ui->pushButton_Pwr->setStyleSheet("background-color: rgb(255, 155, 155)"); //Light Red
            sendData("01 Power 0n");
        }
        else
        {
            ui->textBrowser->append("Power Off clicked");
            ui->pushButton_Pwr->setStyleSheet("background-color: rgb(85, 255, 0)"); //Green
            sendData("02 Power 0ff");
        }
}

void MainWindow::on_pushButton_Tune_clicked()
{
    ui->textBrowser->append("Tune button clicked");
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

void MainWindow::on_pushButton_2_clicked()
{
    QString mySendString;

    mySendString = "Hello from UDP Land";
    ui->statusBar->showMessage("Status");
    sendData(mySendString);
}
