#include "smeter.h"

sMeter::sMeter(QWidget* parent) : QFrame(parent) {
    sMeterMain=new Meter("Volts");
    sMeterSub=new Meter("Amperes");
    meter_dbm = 0; // 0 to 20 full range
    sub_meter_dbm = 0;
    subRx = true;
}

sMeter::~sMeter() {

}

void sMeter::setSubRxState(bool state)
{
    subRx=state;
}

void sMeter::paintEvent(QPaintEvent*)
{
//qDebug() << "smeter.cpp - Meter value is equal to " << meter_dbm;
//return;
    // Draw the Main Rx S-Meter
    QPainter painter(this);
    QImage image=sMeterMain->getImage(meter_dbm);
    painter.drawImage(4,0,image);

    // Draw the Sub Rx S-Meter
  if(subRx) {
      image=sMeterSub->getImage(sub_meter_dbm);
     painter.drawImage(4,image.height()+1,image);
  }
}
