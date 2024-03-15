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

#ifndef IVASOPORTADO_H
#define IVASOPORTADO_H

#include <QtGui>
#include "ui_ivasoportado.h"


class ivasoportado : public QDialog {
    Q_OBJECT
	public:
                ivasoportado(bool concomadecimal, bool condecimales);
                void recuperadatos( QString *cuentabase, QString *baseimponible, QString *qclaveiva,
                                    QString *qtipoiva, QString *cuentaiva, QString *cuotaiva,
                                    QString *ctafra, QDate *qfechafra, QString *soportado,
                                    QString *prorrata, QString *rectificativa, QDate *fechaop,
                                    QString *claveop,
                                    QString *bicoste,
                                    QString *frectif, QString *numfact,
                                    QString *facini, QString *facfinal, QString *bieninversion,
                                    QString *afectacion, QString *qagrario, QString *qnombre,
                                    QString *qcif, QString *import, QString *cajaiva, bool *no347,
                                    bool *arrto_ret, bool *arrto_sin_ret, QString *dua);
                QString valor_retencion();
                void pasadatos( QString cuentabase, QString baseimponible, QString qcuentaiva,
                                QString cuotaiva, QString ctafra, QDate qfechafra );
                void pasadatos2( QString cuentabase, QString baseimponible, QString qclaveiva,
                                 QString qtipoiva, QString cuentaiva, QString cuotaiva,
                                 QString ctafra, QDate qfechafra,QString prorrata,
                                 QString rectificativa,
                                 QDate fechaop, QString claveop,
                                    QString bicoste,
                               QString frectif, QString numfact,
                               QString facini, QString facfinal, QString binversion,
                               QString afectacion, QString agrario,
                               QString nombre, QString cif, QString import, QString cajaiva, bool no347=false,
                               bool arrto_ret=false, bool arrto_sin_ret=false, QString dua="");
                void pasadatos_fin(QString ctafra, QString baseimponible,
                                    QString qcuentaiva,
                                    QDate qfechafra, QString porc_ret , QString cta_base_iva);
                void pasadatos_base( QString ctabase, QString baseimponible, QString qcuentaiva,
                                                  QDate qfechafra, QString porc_ret );
                void hazbaseoblig();
                void pasatipoiva(QString tipo);
                void fococtafra();
                void fococtabi();
                void modoconsulta();
                void desactiva_base_tipo();

        private:
                Ui::ivasoportado ui;
                bool baseoblig;
                bool comadecimal;
                bool decimales;
                bool cambiando_cuota;
       private slots:
                void comboivacambiado();
                void ctabasecambiada();
                void ctabasefinedicion();
                void ctaivacambiada();
                void ctaivafinedicion();
                void ctafracambiada();
                void ctafrafinedicion();
                void basecambiada();
                void basefinedicion();
                void cuotacambiada();
                void totalfracambiado();
                void totalfrafinedicion();
                void botonaceptar();
                void buscactabase();
                void buscactaiva();
                void buscactafra();
                void chequeabotonaceptar();
                void cargacombooperaciones();
                void fechafracambiada();
                void bicostecambiada();
                void afectacionfinedicion();
};





#endif 
 
