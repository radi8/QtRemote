#include "mysettings.h"
#include "ui_mysettings.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

mySettings::mySettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mySettings)
{
    ui->setupUi(this);
    readSettings();
//    connect(this, SIGNAL(sendText(QString,int)), MainWindow, SLOT(setPbtnText(QString,int)));

}

mySettings::~mySettings()
{
    delete ui;
}

void mySettings::on_pBtn_01_clicked()
{
    emit sendText(ui->lineEdit_sIP->text(), 1);
    emit sendText(ui->lineEdit_sPort->text(), 2);
    emit sendText(ui->lineEdit_rly01->text(), 3);
    emit sendText(ui->lineEdit_rly02->text(), 4);
    emit sendText(ui->lineEdit_rly03->text(), 5);
    emit sendText(ui->lineEdit_rly04->text(), 6);
    emit sendText(ui->lineEdit_rly05->text(), 7);
    emit sendText(ui->lineEdit_rBtn01->text(), 8);
    emit sendText(ui->lineEdit_rBtn02->text(), 9);
    emit sendText(ui->lineEdit_rBtn03->text(), 10);
    emit sendText(ui->lineEdit_rBtn04->text(), 11);
    emit sendText(ui->lineEdit_rBtn05->text(), 12);
    // Now save the labels
    writeSettings();
    close();
}

void mySettings::on_pBtn_02_clicked()
{
    close(); // Don't save any changes
}

void mySettings::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup("relaySettings");
    if(settings.contains("IP_address"))ui->lineEdit_sIP->setText(settings.value("IP_address").toString());
    if(settings.contains("Port"))ui->lineEdit_sPort->setText(settings.value("Port").toString());
    if(settings.contains("Relay1"))ui->lineEdit_rly01->setText(settings.value("Relay1").toString());
    if(settings.contains("Relay2"))ui->lineEdit_rly02->setText(settings.value("Relay2").toString());
    if(settings.contains("Relay3"))ui->lineEdit_rly03->setText(settings.value("Relay3").toString());
    if(settings.contains("Relay4"))ui->lineEdit_rly04->setText(settings.value("Relay4").toString());
    if(settings.contains("Relay5"))ui->lineEdit_rly05->setText(settings.value("Relay5").toString());
    settings.endGroup(); // end of group "relaySettings"

    settings.beginGroup("antennaSettings");
    if(settings.contains("Antenna1"))ui->lineEdit_rBtn01->setText(settings.value("Antenna1").toString());
    if(settings.contains("Antenna2"))ui->lineEdit_rBtn02->setText(settings.value("Antenna2").toString());
    if(settings.contains("Antenna3"))ui->lineEdit_rBtn03->setText(settings.value("Antenna3").toString());
    if(settings.contains("Antenna4"))ui->lineEdit_rBtn04->setText(settings.value("Antenna4").toString());
    if(settings.contains("Antenna5"))ui->lineEdit_rBtn05->setText(settings.value("Antenna5").toString());
    settings.endGroup(); // end of group "antennaSettings"
}

void mySettings::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    settings.beginGroup("relaySettings");
    settings.setValue("IP_address",ui->lineEdit_sIP->text());
    settings.setValue("Port",ui->lineEdit_sPort->text());
    settings.setValue("Relay1",ui->lineEdit_rly01->text());
    settings.setValue("Relay2",ui->lineEdit_rly02->text());
    settings.setValue("Relay3",ui->lineEdit_rly03->text());
    settings.setValue("Relay4",ui->lineEdit_rly04->text());
    settings.setValue("Relay5",ui->lineEdit_rly05->text());
    settings.endGroup(); // end of group "relaySettings"

    settings.beginGroup("antennaSettings");
    settings.setValue("Antenna1",ui->lineEdit_rBtn01->text());
    settings.setValue("Antenna2",ui->lineEdit_rBtn02->text());
    settings.setValue("Antenna3",ui->lineEdit_rBtn03->text());
    settings.setValue("Antenna4",ui->lineEdit_rBtn04->text());
    settings.setValue("Antenna5",ui->lineEdit_rBtn05->text());
    settings.endGroup(); // end of group "antennaSettings"
}
