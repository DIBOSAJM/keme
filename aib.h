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

#ifndef AIB_H
#define AIB_H

#include <QtGui>
#include "ui_aib.h"


class aib : public QDialog {
    Q_OBJECT
	public:
                aib(bool concomadecimal, bool condecimales);
                void pasadatos( QString cuentabase, QString baseimponible, QString qcuentaivasop,
                                QString qcuentaivarep, QString cuotaiva, QString ctafra,
                                QDate qfechafra, QString prorrata );
                void pasadatos2(QString cuentabase, QString baseimponible, QString qclaveiva,
                                 QString qtipoiva, QString cuentaiva, QString cuentareper, QString cuotaiva,
                                 QString ctafra, QDate qfechafra, QDate fechaop, QString bicoste,
                                 bool binversion, bool oro_inversion);
                void hazbaseoblig();
                void recuperadatos( QString *cuentabase, QString *baseimponible, QString *qclaveiva,
                                    QString *qtipoiva, QString *cuentaivasop, QString *cuentaivarep,
                                    QString *cuotaiva, QString *ctafra, QDate *qfechafra,
                                    QString *qprorrata, QString *qautofactura, QDate *fechaop,
                                    QString *bicoste, QString *qautofacturanoue, QString *bieninversion,
                                    QString *isp_op_interiores, QString *oro_inversion);
                QString externo();
                void esconde_externo();
                void selec_autofactura();
                void selec_autofactura_no_ue();
                void selec_isp_op_interiores();
                void modoconsulta();

        private:
                Ui::aib ui;
                bool comadecimal;
                bool decimales;
                bool baseoblig;
                static void buscacta(QLineEdit *);
                void chequea_checks();
                QString cta_iva_sop_AIB, cta_iva_rep_AIB, cta_iva_sop_AIS,  cta_iva_rep_AIS,
                        cta_iva_sop_AES, cta_iva_rep_AES, cta_iva_sop_OI, cta_iva_rep_OI ;
       private slots:
                void comboivacambiado();
                void ctabasecambiada();
                void ctabasefinedicion();
                void ctaivacambiada();
                void ctaivafinedicion();
                void ctafracambiada();
                void ctafrafinedicion();
                void cuentarepcambiada();
                void ctarepfinedicion();
                void basecambiada();
                void basefinedicion();
                void botonaceptar();
                void chequeabotonaceptar();
                // void cuotacambiada();
                void buscactabase();
                void buscactaiva();
                void buscactafra();
                void buscactarep();
                void fechafracambiada();
                void bicostecambiada();
                void autofacturaue_cambiada();
                void autofacturanoue_cambiada();
                void isp_interiores_cambiada();
                void aib_check_cambiada();
                void buscar_externo();
                void externo_cambiado();

};





#endif 
