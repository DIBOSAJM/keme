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

#ifndef IVAREP_AUT_H
#define IVAREP_AUT_H

#include <QtGui>
#include "ui_ivarep_aut.h"
#include "etiqueta_fich.h"


class ivarep_aut : public QDialog {
    Q_OBJECT
	public:
                ivarep_aut(bool concomadecimal, bool condecimales);

        private:
                Ui::ivarep_aut ui;
                bool baseoblig;
                bool comadecimal;
                bool decimales;
                void cargacombooperaciones();
                void formaconcepto();
                CustomQLabel *etiqueta;

        public:
                void recuperadatos(
                  QString *cuentabase, QString *baseimponible, QString *qclaveiva, QString *qtipoiva,
                  QString *qtipore, QString *cuentaiva, QString *cuotaiva,

                  QString *cuentabase2, QString *baseimponible2, QString *qclaveiva2, QString *qtipoiva2,
                  QString *qtipore2, QString *cuentaiva2, QString *cuotaiva2,

                  QString *cuentabase3, QString *baseimponible3, QString *qclaveiva3, QString *qtipoiva3,
                  QString *qtipore3, QString *cuentaiva3, QString *cuotaiva3,

                  QString *ctafra, QDate *qfechafra, QDate *qfechacont, QString *qdoc,
                  QDate *qfechaop, QString *qclaveop, bool *rectificativa,
                  QString *rectificada, QString *nfacturas, QString *finicial, QString *ffinal,
                  bool *verifica, QString *nombre, QString *cif, bool *cajaiva, QString *externo );
                  QString fichdocumento();
                  void pasa_fecha(QDate fecha);
                  QString concepto();
        private slots:
                void actutotalfra();
                void comboivacambiado();
                void comboivacambiado2();
                void comboivacambiado3();
                void ctabasecambiada();
                void ctabasefinedicion();
                void ctabasecambiada2();
                void ctabasefinedicion2();
                void ctabasecambiada3();
                void ctabasefinedicion3();
                void ctaivacambiada();
                void ctaivafinedicion();
                void ctaivacambiada2();
                void ctaivafinedicion2();
                void ctaivacambiada3();
                void ctaivafinedicion3();
                void ctafracambiada();
                void ctafrafinedicion();
                void basecambiada();
                void basecambiada2();
                void basecambiada3();
                void basefinedicion();
                void basefinedicion2();
                void basefinedicion3();
                void botonaceptar();
                void chequeabotonaceptar();
                void buscactabase();
                void buscactabase2();
                void buscactabase3();
                void buscactaiva();
                void buscactaiva2();
                void buscactaiva3();
                void buscactafra();
                void grboxcambiado2();
                void grboxcambiado3();
                void fechafracambiada();
                void buscacopiadoc();
                void visdoc();
                void borraasignaciondoc();
                void documento_cambiado();
                void buscar_externo();
                void externo_cambiado();
                void fichero_soltado();
};



#endif 
 
