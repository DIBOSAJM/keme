#ifndef PIDE_INFO_UNIF_H
#define PIDE_INFO_UNIF_H

#include <QDialog>

namespace Ui {
class pide_info_unif;
}

class pide_info_unif : public QDialog
{
    Q_OBJECT

public:
    explicit pide_info_unif(QWidget *parent = 0);
    ~pide_info_unif();
    void asignaetiqueta(QString nombre);
    QDate fecha(void);
    void pasafecha(QDate fecha);
    void asigna_concepto(QString concepto);
    void asigna_documento(QString documento);
    QString recupera_concepto();
    QString recupera_documento();
    void esconde_concepto_documento();

    void pasa_externo(QString externo);
    void pasa_cuenta_tesoreria(QString cuenta);

    QString cuenta_tesoreria();
    QString externo();
    QString cuenta_gasto();
    QString importe_gasto();



private:
    Ui::pide_info_unif *ui;
    bool comadecimal;
    bool decimales;

private slots:
        void cuentacambiada();
        void cuenta_gasto_cambiada();
        void finediccuenta();
        void finediccuenta_gastos();
        void buscacuenta();
        void buscacuenta2();
        void finedic_gastos();
        void buscar_externo();
        void campo_externo_cambiado();
        void borra_externo();
        void ver_externo();

};

#endif // PIDE_INFO_UNIF_H
