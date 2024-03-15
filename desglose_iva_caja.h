#ifndef DESGLOSE_IVA_CAJA_H
#define DESGLOSE_IVA_CAJA_H

#include <QDialog>
#include "funciones.h"

namespace Ui {
    class desglose_iva_caja;
}

class desglose_iva_caja : public QDialog
{
    Q_OBJECT

public:
    explicit desglose_iva_caja(QWidget *parent = 0);
    ~desglose_iva_caja();
    void pasa_info(QString apunte, QString documento, QDate fecha_fra, double totalfra,
                   double qcuotaiva);
    bool hay_cambios();
private:
    Ui::desglose_iva_caja *ui;
    bool comadecimal, decimales;
    QString apunte;
    double cuotaiva;
    QStringList num_vencis;
    bool cambios;
    void refrescar();
private slots:
    void forzar_liquidacion();
};

#endif // DESGLOSE_IVA_CAJA_H
