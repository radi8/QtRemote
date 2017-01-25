#include "mysettings.h"
#include "ui_mysettings.h"

mySettings::mySettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mySettings)
{
    ui->setupUi(this);
}

mySettings::~mySettings()
{
    delete ui;
}
