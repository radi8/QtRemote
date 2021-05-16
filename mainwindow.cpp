#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mysettings.h"
#include "ui_mysettings.h"
#include <qthread.h>
#include <QCloseEvent>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->pushButton_Pwr->setStyleSheet("background-color: rgb(85, 255, 0)"); //Green
    ui->pushButton_Pwr->setStyleSheet("background-color: rgb(0, 170, 0)"); //Green

    setWindowTitle(tr("Remote controller"));
    statusLabel = new QLabel(this);
    ui->statusBar->addWidget(statusLabel);
    ui->statusBar->showMessage("Not connected to remote");

// Connecting to remote
    localPort = 7200;
    remoteIP = "192.168.1.70";
    remotePort = 8475;

    tcpSocket = new QTcpSocket(this);
    QTimer *timer = new QTimer(this);

//    ui->frame->meter_dbm = 13.6;
//    ui->frame->sub_meter_dbm = 8.5;
//    ui->frame->update();

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(timer, SIGNAL(timeout()), this, SLOT(getData()));

    readSettings();

    timer->start(20000); //DEBUG in final version make the timer "timer->start(500)"

    qDebug() << "connecting...";
    qDebug() <<  QApplication::style()->objectName();
} //end constructor



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPbtnText(QString txt, int btn)
{
    switch (btn) {
    case 1: //IP Address
        remoteIP = txt;
        break;
    case 2: // Port
        remotePort = txt.toShort(); // port is qint16 == short
        break;
    case 3: //First relay
        ui->pBtn_Relay1->setText(txt);
        break;
    case 4:
        ui->pBtn_Relay2->setText(txt);
        break;
    case 5:
        ui->pBtn_Relay3->setText(txt);
        break;
    case 6:
        ui->pBtn_Relay4->setText(txt);
        break;
    case 7:
        ui->pBtn_Tune->setText(txt);
        break;
    case 8: // First antenna switch
        ui->radioButton_1->setText(txt);
        break;
    case 9:
        ui->radioButton_2->setText(txt);
        break;
    case 10:
        ui->radioButton_3->setText(txt);
        break;
    case 11:
        ui->radioButton_4->setText(txt);
        break;
    case 12:
        ui->radioButton_5->setText(txt);
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    ui->pushButton_Pwr->setChecked(false);
    on_pushButton_Pwr_clicked();
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

//    settings->beginGroup("Geometry");
    settings.beginGroup("Geometry");
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
    settings.endGroup();

    settings.beginGroup("tcpSettings");
    if(settings.contains("IP_address")) remoteIP = settings.value("IP_address").toString();
    if(settings.contains("Port")) remotePort = (settings.value("Port")).toString().toShort();
    settings.endGroup(); // end of group "tcpSettings"

    settings.beginGroup("relaySettings");
    if(settings.contains("Relay1"))ui->pBtn_Relay1->setText(settings.value("Relay1").toString());
    if(settings.contains("Relay2"))ui->pBtn_Relay2->setText(settings.value("Relay2").toString());
    if(settings.contains("Relay3"))ui->pBtn_Relay3->setText(settings.value("Relay3").toString());
    if(settings.contains("Relay4"))ui->pBtn_Relay4->setText(settings.value("Relay4").toString());
    if(settings.contains("Relay5"))ui->pBtn_Tune->setText(settings.value("Relay5").toString());
    settings.endGroup(); // end of group "relaySettings"

    settings.beginGroup("antennaSettings");
    if(settings.contains("Antenna1"))ui->radioButton_1->setText(settings.value("Antenna1").toString());
    if(settings.contains("Antenna2"))ui->radioButton_2->setText(settings.value("Antenna2").toString());
    if(settings.contains("Antenna3"))ui->radioButton_3->setText(settings.value("Antenna3").toString());
    if(settings.contains("Antenna4"))ui->radioButton_4->setText(settings.value("Antenna4").toString());
    if(settings.contains("Antenna5"))ui->radioButton_5->setText(settings.value("Antenna5").toString());
    settings.endGroup(); // end of group "antennaSettings"
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup("Geometry");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

//-----------------------------------------------------------------------------------------------------------------

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
    if (ui->pushButton_Pwr->isChecked()) {
        mySendString = (QByteArray::number(CMD_READ_A0, 10) + "\r");
        sendData();
    }
}

void MainWindow::readyRead()
{
    QByteArray Buffer;

    Buffer = tcpSocket->readAll();
    processReceived(Buffer);
    qDebug() << Q_FUNC_INFO << "Value of Buffer = " << Buffer;
    Buffer.clear();
/*
    // To avoid receiving more than one line at the time but only reading the first one.
    // Read as many lines as available by checking with canReadLine.
    while (tcpSocket->canReadLine())
        {
            Buffer = QByteArray(tcpSocket->readLine());
            processReceived(Buffer);
            qDebug() << Q_FUNC_INFO << "Value of Buffer = " << Buffer;
        }
*/
}

void MainWindow::processReceived(QByteArray Buffer)
// When data arrives from the remote end, this routine places the sent string
// into "myMessage". The message will take the form of: cmdNum cmdVal The cmdval
// may be another number or an information string. The numbers and values are
// extracted and converted to long integers or a string if cmdVal is a message.
{
/*
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
*/

    long cmd;
    long cmdValue;
    char * pEnd;
    double cmdVal; // holds converted cmdValue as a float
    bool ok;

//    qDebug() << Q_FUNC_INFO << "Buffer = " << Buffer;

    cmd = strtol(Buffer, &pEnd, 10);
//    cmd = Buffer.toLong(&ok, 10);
//    Buffer.replace('\r', '\0');
    Buffer.resize(Buffer.indexOf('\r'));
    if(cmd != _message) {
        cmdValue = strtol (pEnd, &pEnd,10);
    } else {
        myMessage = pEnd;
//        qDebug() << Q_FUNC_INFO << "At else statement: myMessage = " << myMessage;
    }

//qDebug() << "command received = " << cmd << " and command value = " << cmdValue;
//qDebug() << Q_FUNC_INFO << "Value of Buffer after cmd extracted = " << Buffer;
    switch (cmd) {
    case CMD_PWR_ON:
        // Power switch status. (0 = power off, 1 = Power on)
            ui->textBrowser->append("Power on");
        break;
    case CMD_PWR_OFF:
            ui->textBrowser->append("Power off");
        break;
    case CMD_RLY1_ON:
             ui->textBrowser->append("HiQSDR switched on");
        break;
    case CMD_RLY1_OFF:
            ui->textBrowser->append("HiQSDR switched off");
        break;
    case CMD_RLY2_ON:
            ui->textBrowser->append("HL2 switched on");
        break;
    case CMD_RLY2_OFF:
            ui->textBrowser->append("HL2 switched off");
        break;
    case CMD_RLY3_ON:
            ui->textBrowser->append("Linear switched on");
        break;
    case CMD_RLY3_OFF:
            ui->textBrowser->append("Linear switched off");
        break;
    case CMD_RLY4_ON:
            ui->textBrowser->append("Tuner switched on");
        break;
    case CMD_RLY4_OFF:
            ui->textBrowser->append("Tuner switched off");
        break;
    case CMD_TUNE_DN:
            ui->textBrowser->append("Tune button pressed");
        break;
    case CMD_TUNE_UP:
            ui->textBrowser->append("Tune button released");
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
    Buffer.clear();
}

void MainWindow::connected()
{
    ui->statusBar->showMessage("Connected to remote server...");
    tcpSocket->write(mySendString);
    tcpSocket->flush();
    qDebug() << Q_FUNC_INFO << "Wrote to server data in mySendString = " << mySendString;
    if (mySendString != "18\r")
    {
//        mySendString.resize(mySendString.indexOf('\r'));
//        ui->textBrowser->append(mySendString);
    }
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
    connect(&mSettings,SIGNAL(sendText(QString,int)), this, SLOT(setPbtnText(QString,int)));
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
        ui->pushButton_Pwr->setStyleSheet("background-color: rgb(255, 0, 0)"); //Red
        mySendString = (QByteArray::number(CMD_PWR_ON, 10) + "\r\n");
        sendData();
        getData();
    } else {
        ui->frame->meter_dbm = 0; // Zero the meter
        ui->frame->update();
        QThread::msleep(50); // Remote end with Arduino is slow so hardware delay here
        if (ui->pBtn_Relay1->isChecked()) {
            mySendString = (QByteArray::number(CMD_RLY1_OFF, 10) + "\r\n");
            sendData();
            ui->pBtn_Relay1->setChecked(false);
            QThread::msleep(50);
        }
        if (ui->pBtn_Relay2->isChecked()) {
            mySendString = (QByteArray::number(CMD_RLY2_OFF, 10) + "\r\n");
            sendData();
            ui->pBtn_Relay2->setChecked(false);
            QThread::msleep(50);
        }
        if (ui->pBtn_Relay3->isChecked()) {
            mySendString = (QByteArray::number(CMD_RLY3_OFF, 10) + "\r\n");
            sendData();
            ui->pBtn_Relay3->setChecked(false);
        }
        QThread::msleep(50);
        if (ui->pBtn_Relay4->isChecked()) {
            mySendString = (QByteArray::number(CMD_RLY4_OFF, 10) + "\r\n");
            sendData();
            ui->pBtn_Relay4->setChecked(false);
        }
        QThread::msleep(50);

        ui->pushButton_Pwr->setStyleSheet("background-color: rgb(0, 170, 0)"); //Green
        mySendString = (QByteArray::number(CMD_PWR_OFF, 10) + "\r\n");
        sendData();
    }
    qDebug() << Q_FUNC_INFO << "mySendString from pwr button = " << mySendString;
}

void MainWindow::on_pBtn_Relay1_clicked()
{
    if (ui->pushButton_Pwr->isChecked()) {
        if (ui->pBtn_Relay1->isChecked()) {
            mySendString = (QByteArray::number(CMD_RLY1_ON, 10) + "\r\n");
        } else {
            mySendString = (QByteArray::number(CMD_RLY1_OFF, 10) + "\r\n");
        }
        sendData();
        qDebug() << Q_FUNC_INFO << "mySendString = " << mySendString;
    } else {
        ui->pBtn_Relay1->setChecked(false);
    }
}

void MainWindow::on_pBtn_Relay2_clicked()
{
    if (ui->pushButton_Pwr->isChecked()) {
        if (ui->pBtn_Relay2->isChecked()) {
            mySendString = (QByteArray::number(CMD_RLY2_ON, 10) + "\r\n");
        } else {
            mySendString = (QByteArray::number(CMD_RLY2_OFF, 10) + "\r\n");
        }
        sendData();
        qDebug() << Q_FUNC_INFO << "mySendString = " << mySendString;
    } else {
        ui->pBtn_Relay2->setChecked(false);
    }
}


void MainWindow::on_pBtn_Relay3_clicked()
{
    if (ui->pushButton_Pwr->isChecked()) {
        if (ui->pBtn_Relay3->isChecked()) {
            mySendString = (QByteArray::number(CMD_RLY3_ON, 10) + "\r\n");
        } else {
            mySendString = (QByteArray::number(CMD_RLY3_OFF, 10) + "\r\n");
        }
        sendData();
        qDebug() << Q_FUNC_INFO << "mySendString = " << mySendString;
    } else {
        ui->pBtn_Relay3->setChecked(false);
    }
}
void MainWindow::on_pBtn_Relay4_clicked()
{
    if (ui->pushButton_Pwr->isChecked()) {
        if (ui->pBtn_Relay4->isChecked()) {
            mySendString = (QByteArray::number(CMD_RLY4_ON, 10) + "\r\n");
        } else {
            mySendString = (QByteArray::number(CMD_RLY4_OFF, 10) + "\r\n");
        }
        sendData();
        qDebug() << Q_FUNC_INFO << "mySendString = " << mySendString;
    } else {
        ui->pBtn_Relay4->setChecked(false);
    }
}

void MainWindow::on_pBtn_Tune_pressed()
{
    mySendString = (QByteArray::number(CMD_TUNE_DN, 10) + "\r\n");
    sendData();
    qDebug() << Q_FUNC_INFO << "mySendString from Tune button = " << mySendString;
}

void MainWindow::on_pBtn_Tune_released()
{
    mySendString = (QByteArray::number(CMD_TUNE_UP, 10) + "\r\n");
    sendData();
    qDebug() << Q_FUNC_INFO << "mySendString from Tune button = " << mySendString;
}
