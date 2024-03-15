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

#ifndef IVAREPERCUTIDO_H
#define IVAREPERCUTIDO_H

#include <QtGui>
#include "ui_ivarepercutido.h"


class ivarepercutido : public QDialog {
    Q_OBJECT
	public:
                ivarepercutido(bool concomadecimal, bool condecimales);

        private:
                Ui::ivarepercutido ui;
                bool baseoblig;
                bool comadecimal;
                bool decimales;
        public:
                void recuperadatos( QString *cuentabase, QString *baseimponible, QString *qclaveiva,
                                    QString *qtipoiva, QString *qtipore, QString *cuentaiva,
                                    QString *cuotaiva, QString *ctafra, QDate *qfechafra,
                                    QString *soportado, QString *qrectif,
                                    QDate *fechaop, QString *claveop,
                                    QString *bicoste,
                                    QString *frectif, QString *numfact,
                                    QString *facini, QString *facfinal, QString *nombre,
                                    QString *cif, QString *caja, QString *arrto_ret, QString *arrto_sin_ret);
                void pasadatos( QString cuentabase, QString baseimponible, QString qcuentaiva,
                                QString cuotaiva, QString ctafra, QDate qfechafra, QString qrectificativa );
                void pasadatos2( QString cuentabase, QString baseimponible, QString qclaveiva,
                                 QString qtipoiva, QString qtipore, QString cuentaiva,
                                 QString cuotaiva, QString ctafra, QDate qfechafra, QString rectif,
                                 QDate fechaop, QString claveop,
                                 QString bicoste,
                               QString frectif, QString numfact,
                               QString facini, QString facfinal,
                               QString nombre, QString cif, QString cajaiva,
                                 QString arrto_ret="", QString arrto_sin_ret="");
                void pasadatos_fin( QString ctafra, QString baseimponible, QString qcuentaiva,
                                    QDate qfechafra, QString cta_base_iva);
                void hazbaseoblig();
                void pasatipoiva(QString tipo);
                void fococtafra();
                void fococtabi();
                void modoconsulta();


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
                void botonaceptar();
                void chequeabotonaceptar();
                void cuotacambiada();
                void buscactabase();
                void buscactaiva();
                void buscactafra();
                void cargacombooperaciones();
                void fechafracambiada();
                void bicostecambiada();

};



#endif 
 
