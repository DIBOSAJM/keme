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

#ifndef IVASOP_AUT_H
#define IVASOP_AUT_H

#include <QtGui>
#include "ui_ivasop_aut.h"
#include "etiqueta_fich.h"

class ivasop_aut : public QDialog {
    Q_OBJECT
	public:
                ivasop_aut(bool concomadecimal, bool condecimales);
                void recuperadatos( 
                      QString *cuentabase, QString *baseimponible, QString *qclaveiva, QString *qtipoiva,
                           QString *cuentaiva, QString *cuotaiva,
                      QString *cuentabase2, QString *baseimponible2, QString *qclaveiva2, QString *qtipoiva2,
                           QString *cuentaiva2, QString *cuotaiva2,
                      QString *cuentabase3, QString *baseimponible3, QString *qclaveiva3, QString *qtipoiva3,
                      QString *cuentaiva3, QString *cuotaiva3,
                      QString *ctafra, QString *qprorrata, QDate *qfechafra, QDate *qfechacontable,
                      QString *documento,
                      QDate *qfechaop, QString *qclaveop, bool *rectificativa,
                      QString *rectificada, QString *nfacturas, QString *finicial, QString *ffinal,
                      bool *verifica, QString *binversion, QString *afectacion, bool *agrario,
                      QString *nombre, QString *cif, QString *import, QString *cajaiva,
                      QString *cuenta_ret, QString *tipo_ret, QString *retencion, QString *clave_ret,
                      bool *ret_arrendamiento,
                        bool *iva_exento,
                        QString *cta_base_exento,
                        QString *base_imponible_exento,
                        QString *externo);
                  QString fichdocumento();
                  void pasa_fecha(QDate fecha);
                  void pasa_datos_ini(QString externo, QString cuenta_proveedor, QString cuenta_gasto, QString cuenta_iva_soportado, QString cuenta_ret_irpf,
                                      QDate fecha, QJsonObject importes_info, QString factura, QString fichero);
                  QString concepto();

        private:
                Ui::ivasop_aut ui;
                bool comadecimal;
                bool decimales;
                void actutotalfra();
                void formaconcepto();
                CustomQLabel *etiqueta;

       private slots:
                void comboivacambiado();
                void comboivacambiado2();
                void comboivacambiado3();
                void ctabasecambiada();
                void ctabasecambiada2();
                void ctabasecambiada3();
                void ctabase_exenta_cambiada();
                void ctabasefinedicion();
                void ctabasefinedicion2();
                void ctabasefinedicion3();
                void ctabase_exenta_finedicion();
                void ctaivacambiada();
                void ctaivacambiada2();
                void ctaivacambiada3();
                void ctaivafinedicion();
                void ctaivafinedicion2();
                void ctaivafinedicion3();
                void ctafracambiada();
                void ctafrafinedicion();
                void basecambiada();
                void basefinedicion();
                void basecambiada2();
                void basefinedicion2();
                void basecambiada3();
                void basefinedicion3();
                void base_exenta_cambiada();
                void base_exenta_finedicion();
                void totalfrafinedicion();
                void botonaceptar();
                void buscactabase();
                void buscactabase2();
                void buscactabase3();
                void buscactabase_exenta();
                void buscactaiva();
                void buscactaiva2();
                void buscactaiva3();
                void buscactafra();
                void buscactaret();
                void ctaretfinedic();
                void chequeabotonaceptar();
                void grboxcambiado1();
                void grboxcambiado2();
                void fechafracambiada();
                void cargacombooperaciones();
                void buscacopiadoc();
                void visdoc();
                void borraasignaciondoc();
                void afectacionfinedicion();
                void concepto_cambiado();
                void tiporetfinedic();
                void arrendamiento_cambiado();
                void iva_exento_cambiado();
                void buscar_externo();
                void externo_cambiado();
                void fichero_soltado();
};





#endif 
 
