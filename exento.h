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

#ifndef EXENTO_H
#define EXENTO_H

#include <QtGui>
#include "ui_exento.h"


class exento : public QDialog {
    Q_OBJECT
	public:
                exento(bool concomadecimal, bool condecimales);
                void pasadatos(QString cuentabase, QString baseimponible, QString ctafra,
                                QDate qfechafra, QDate fechaop,
                                    QString claveop, QString rectificativa, QString frectif,
                                    QString numfact,
                               QString facini, QString facfinal, QString binversion,
                               QString op_no_sujeta, QString cliente, QString cif,
                               QString import, QString exportacion,
                               QString exenta_no_deduc, QString ventas_fuera_tac, QString emitida_isp_int, QString info_donacion="", QString dua="");
                void recuperadatos(QString *cuentabase, QString *baseimponible,  QString *ctafra,
                                    QDate *qfechafra, QDate *fechaop,
                                    QString *claveop, QString *rectificativa, QString *frectif,
                                    QString *numfact,
                                    QString *facini, QString *facfinal, QString *bieninversion,
                                    QString *op_no_sujeta, QString *nombre, QString *cif,
                                    QString *importacion, QString *exportacion,
                                    QString *exenta_no_deduc, QString *vta_fuera_tac, QString *emitida_isp_int, QString *info_donacion=NULL, QString *dua=NULL);
                bool eslibroemitidas();
                void fuerzaemitidas();
                void modoconsulta();
        private:
                Ui::exento ui;
                bool comadecimal;
                bool decimales;
                void cargacombooperaciones();
        private slots:
                void ctabasecambiada();
                void ctabasefinedicion();
                void ctafracambiada();
                void ctafrafinedicion();
                void botonaceptar();
                void chequeabotonaceptar();
                void buscactabase();
                void buscactafra();
                void fechafracambiada();
                void cheqrecibidas_activado();
                void cheqemitidas_activado();
                void exportacion_activado();
                void vtas_fuera_tac_activado();
                void on_donacion_checkBox_toggled(bool checked);

                void on_imp_exentacheckBox_stateChanged(int arg1);
};



#endif 
