#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QDialog>

namespace Ui {
class mySettings;
}

class mySettings : public QDialog
{
    Q_OBJECT

public:
    explicit mySettings(QWidget *parent = 0);
    ~mySettings();

signals:
    void sendText(QString, int);

private slots:
    void on_pBtn_01_clicked();
    void on_pBtn_02_clicked();

private:
    Ui::mySettings *ui;
    void readSettings();
    void writeSettings();
};

#endif // MYSETTINGS_H
