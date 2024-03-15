/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#ifndef BSS_H
#define BSS_H

#include <QtGui>
#include "ui_bss.h"
#include <QSqlTableModel>


class bss : public QDialog {
    Q_OBJECT
	public:
                bss(QString usuario);
        private:
                Ui::bss ui;
                bool comadecimal;
                bool decimales;
                int numdiarios;
                QString fichero;
                QString usuario;
                bool hay_analitica_tabla;
                QStringList aux_saldos_iniciales_ej1;
                QList<double> saldos_iniciales_aux_ej1;
                QStringList aux_saldos_iniciales_ej2;
                QList<double> saldos_iniciales_aux_ej2;

                QStringList codigo_cuenta;
                QList<bool> es_auxiliar;

                QStringList aux_saldos_hasta_fecha_ej1;
                QList<double> saldos_hasta_fecha_ej1;
                QStringList aux_saldos_hasta_fecha_ej2;
                QList<double> saldos_hasta_fecha_ej2;

                QStringList aux_sumas_ej1;
                QList<double> sumasdebe_ej1;
                QList<double> sumashaber_ej1;
                QStringList aux_sumas_ej2;
                QList<double> sumasdebe_ej2;
                QList<double> sumashaber_ej2;


                QString sumadh(bool subcuenta, bool debe, QString codigo, QDate desde,
                               QDate hasta);
                double saldo_inicial_ej1(QString aux);
                double saldo_inicial_ej2(QString aux);
                double saldo_inicial_princi_ej1(QString cod);
                double saldo_inicial_princi_ej2(QString cod);

                double saldo_hasta_fecha_ej1(QString aux);
                double saldo_hasta_fecha_ej2(QString aux);
                double saldo_hasta_fecha_princi_ej1(QString cod);
                double saldo_hasta_fecha_princi_ej2(QString cod);

                void sumas_debe_haber(QString aux, QString *debe, QString *haber);
                void sumas_debe_haber_compa(QString aux, QString *debe, QString *haber);

                void sumas_debe_haber_princi(QString cod, QString *debe, QString *haber);
                void sumas_debe_haber_princi_compa(QString cod, QString *debe, QString *haber);

                void carga_sumas();

                void actualiza_totales_si();
                void actualiza_totales();

                void informe_bss();
                void informe_bcomp();

                void generalatex();
                void generalatex2();
                bool noinfovisible();
        private slots:
                void actualizar();
                void varsinmovs();
                void txtexport();
                void imprimir();
                void gentex();
                void fijadescripciones();
                void buscapulsado();
                void activaactualiza();
                void var_saldos_iniciales();
                void genxml();
                void generaXML();
                void generaXML2();
                bool sindiarioapertura();
                void latex();
                void consultamayor();
                void comparativo_check();

                void on_externos_pushButton_clicked();
};




#endif 
 
