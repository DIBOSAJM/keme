#ifndef CONFIG_VERIFACTU_H
#define CONFIG_VERIFACTU_H

#include <QDialog>

namespace Ui {
class Config_Verifactu;
}

class Config_Verifactu : public QDialog
{
    Q_OBJECT

public:
    explicit Config_Verifactu(QWidget *parent = nullptr);
    ~Config_Verifactu();

private slots:
    void on_aceptar_pushButton_clicked();

private:
    Ui::Config_Verifactu *ui;
};

#endif // CONFIG_VERIFACTU_H
