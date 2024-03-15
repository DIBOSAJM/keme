#ifndef VIES_MSJ_H
#define VIES_MSJ_H

#include <QDialog>

namespace Ui {
class vies_msj;
}

class vies_msj : public QDialog
{
    Q_OBJECT

public:
    explicit vies_msj(QWidget *parent = 0);
    ~vies_msj();
    void haz_num_incorrecto();
    void haz_num_correcto(QString razon, QString direccion);

private:
    Ui::vies_msj *ui;
};

#endif // VIES_MSJ_H
