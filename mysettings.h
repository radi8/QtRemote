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

private:
    Ui::mySettings *ui;
};

#endif // MYSETTINGS_H
