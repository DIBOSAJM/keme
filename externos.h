#ifndef EXTERNOS_H
#define EXTERNOS_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class externos;
}

class externos : public QDialog
{
    Q_OBJECT

public:
    explicit externos(QWidget *parent = 0);
    ~externos();
    void pasa_codigo(QString codigo);

private:
    Ui::externos *ui;
    QSqlQueryModel *modeloreg;
    void cargacomboclaveidfiscal();
    void cargacombopaises();
    void resetdatosaccesorios();
    QString filtrocodigo();
    void refrescar(QString filtro);
    void activaconexdatos();
    void desactivaconexdatos();
    void activacambiacod();
    QString fototexto();
    void cargacomboIVA();
    void cargacombovenci();
    void cargacombooperacionret();
    void cargacombo_formas_pago();

private slots:
    void codigocambiado(QString codigo);
    void botonaceptar();
    void desactivacambiacod();
    void codigofinedicion();
    void fila_tabla_clicked();
    void importafoto();
    void borrafoto();
    void botonctaivapulsado();
    void botonctaretpulsado();
    void botoncta_tesorpulsado();
    void cta_IVA_cambiada();
    void cta_tesor_cambiada();
    void cta_ret_asig_cambiada();
    void cambia_alquiler_asig_checkBox();
    void cta_IVA_finedicion();
    void cta_tesor_finedicion();
    void cta_ret_asig_finedicion();
    void checkdiafijo();
    void botoncuenta_pulsado();
    void cuenta_finedicion();
    void cta_base_IVA_finedicion();
    void botoncta_base_ivapulsado();
    void cta_base_IVA_cambiada();
    void boton_ue_pulsado();
    void refresca_externos();
    void nifcambiado();
    void ccccambiado();
    void ibancambiado();
    void buscar_externo();
    void boton_cancelar();
    void botonborrar();
    void venciasoccambiado();


};

#endif // EXTERNOS_H
