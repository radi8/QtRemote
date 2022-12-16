#include <math.h>
#include "Meter.h"

#define CENTER_X 75
#define CENTER_Y 85

Meter::Meter(QString title) {
    image=new QImage(150,75,QImage::Format_ARGB32); // Size of meter i.e. width x height
    image->fill(Qt::white); // Background colour = white

    QPainter painter(image);

    painter.setFont(QFont("Arial", 8));

    painter.setPen(QPen(Qt::black, 4));
    painter.drawRect(0, 0, 150, 75);

    painter.setPen(QPen(Qt::black, 2));
    painter.drawArc(5, 15, 140, 140, 30*16, 120*16); //120 deg of arc starting at 30 deg.
    painter.drawArc(10, 20, 130, 130, 30*16, 120*16);


    // put in the markers
    painter.setPen(QPen(Qt::black, 1));
    for(int x = 1; x < 20; x++) {
        calculateLine(x, 65, 73);
        painter.drawLine(dxmin, dymin, dxmax, dymax);
    }
    for(int x = 0; x <= 20; x += 4) {
        painter.setPen(Qt::black);
        calculateLine(x, 60, 75); // 0
        painter.drawLine(dxmin, dymin, dxmax, dymax);
        painter.setPen(Qt::blue);
        painter.drawText(dxmax-5, dymax,QString::number(x));
    }
    painter.setPen(Qt::red);
    painter.drawText(4,2,image->width(),image->height(),Qt::AlignBottom||Qt::AlignHCenter,title);
//    image->fill(Qt::darkGray);
}

void Meter::calculateLine(double volts, double minRadius, double maxRadius) {

    double degrees=240-(volts*6);
    double radians = degrees*(3.14159265358979323846/180.0);
    double sine   = sin(radians);
    double cosine = cos(radians);

    dxmin = CENTER_X + (int)(minRadius * sine);
    dymin = CENTER_Y + (int)(minRadius * cosine);

    dxmax = CENTER_X + (int)(maxRadius * sine);
    dymax = CENTER_Y + (int)(maxRadius * cosine);

}

QImage Meter::getImage(double dbm) {
    QImage qImage(*image);
    QPainter painter(&qImage);

    painter.setPen(QPen(Qt::red, 2));
//    painter.setPen(Qt::blue);
    calculateLine(dbm,0,75);
    painter.drawLine(dxmin, dymin, dxmax, dymax);

    painter.setPen(Qt::black);
    strDbm.asprintf("%2.2f V",dbm);
    QRectF r1(image->width()-80, image->height()-15, 75, 20);
    painter.drawText(r1,Qt::AlignRight,strDbm);

    return qImage;
}
