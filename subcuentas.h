/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre, ya sea la versión 3 
    de la Licencia, o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil, pero 
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

#ifndef SUBCUENTAS_H
#define SUBCUENTAS_H

#include <QtGui>
#include "ui_subcuentas.h"
#include <QSqlTableModel>

class subcuentas  : public QDialog {
    Q_OBJECT
	public:
                subcuentas();
                void insertacuentaarbol(QString cadena1,QString cadena2);
                void insertacuentaarbol_inicial(QString cadena1,QString cadena2);
                // void insertacuentaauxiliararbol(QString cadena1,QString cadena2);
                void cargaarbol();
                void resetdatosaccesorios();
                void pasacodigo(QString qcodigo);
                void activaconexdatos();
                void desactivaconexdatos();
                void activacambiacod();


        private:
                Ui::subcuentas ui;
                //TreeModel modeloarbol;
                QString fototexto();
                bool modolista;
                void cargacomboIVA();
                void cargacombovenci();
                void cargacomboclaveidfiscal();
                void cargacombooperacionret();
                void cargacombopaises();
                void cargacombo_formas_pago();
                void cargacombos_tipoProveedor();

                QStringList lista_codigo;
                QList<QTreeWidgetItem*> lista_puntero_item;

                QTreeWidgetItem *buscacadena(QString cadena, bool *encontrada);
                QTreeWidgetItem *buscacadena_ini(QString cadena, bool *encontrada);

                QSqlTableModel *modeloreg;
                QString filtrocodigo();
                void refrescar(QString filtro);

                void primer_elemento_ini();

                void guarda_tipo_proveedor();



        private slots :
                void codigocambiado(QString codigo);
                void codigofinedicion();
                void filaclicked();
                void fila_tabla_clicked();
                void descripcambiada();
                void botonaceptar();
                void botonborrar();
                void grupodatospulsado();
                void desactivacambiacod();
                void importafoto();
                void borrafoto();
                void botonctaivapulsado();
                void botonctaretpulsado();
                void botoncta_tesorpulsado();
                void cta_IVA_cambiada();
                void cta_tesor_cambiada();
                void cta_ret_asig_cambiada();
                void cambia_alquilercheckBox();
                void cambia_alquiler_asig_checkBox();
                void cta_IVA_finedicion();
                void cta_tesor_finedicion();
                void cta_ret_asig_finedicion();
                void venciasoccambiado();
                void tiporet_finedicion();
                void checkdiafijo();
                void botoncodigo_pulsado();
                void nifcambiado();
                void ccccambiado();
                void ibancambiado();

                void cta_base_IVA_finedicion();
                void botoncta_base_ivapulsado();
                void cta_base_IVA_cambiada();

                void boton_ue_pulsado();

                void refresca_cuentas();
};


#endif
