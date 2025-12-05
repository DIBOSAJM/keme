/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

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

#ifndef FACTURA_H
#define FACTURA_H

#include <QtGui>
#include <QDomElement>
#include "ui_factura.h"



class factura : public QDialog {
    Q_OBJECT
      public:
                factura();
                void quita_docs_vf();
                void pasa_cabecera_doc(QString serie,
                                       QString numero,
                                       QString cuenta,
                                       QDate fecha_fac, QDate fecha_asiento,
                                       QDate fecha_op,
                                       bool contabilizable,
                                       bool con_ret,
                                       bool re,
                                       QString tipo_ret,
                                       QString retencion,
                                       QString tipo_doc,
                                       QString notas,
                                       QString pie1,
                                       QString pie2,
                                       QString anticipos=""
                                       , QString externo="", QString concepto_sii="",
                                       QString c_a_rol1="", QString c_a_rol2="", QString c_a_rol3="",
                                       bool aceptada_errores=false, QString serie_rect="", QString num_rect="", QString tipo_rectificativa="");
                void solo_cabecera_doc(QString serie,
                                       QString numero,
                                       QString cuenta,
                                       QDate fecha_fac,
                                       QDate fecha_asiento,
                                       QDate fecha_op,
                                       bool contabilizable,
                                       bool con_ret,
                                       bool re,
                                       QString tipo_ret,
                                       QString retencion, QString externo,
                                       QString tipo_doc
                                       );
                void carga_lineas_doc(QString serie,
                                       QString numero);
                void addnotas(QString notas);
                void activa_consulta();
                void pasa_fact_predef(QString codigo, QString descrip,
                                      QString serie,
                                       bool contabilizable,
                                       bool con_ret,
                                       bool re,
                                       QString tipo_ret,
                                       QString tipo_doc,
                                       QString notas,
                                       QString pie1,
                                       QString pie2,
                                       QString concepto_sii
                                       );
                bool imprime_informe(QString *serie, QString *factura);

      private:
                Ui::factura ui;
                QString descripfact;
                QString seriefact;
                QString pie1;
                QString pie2;
                QString moneda;
                QString codigo_moneda;
                QStringList codigoiva;
                QList<double> tipoiva, tipore, baseimponible;
                bool modo_consulta=false;
                int tipo_operacion;
                bool contabilizable;
                bool verifactu=false;
                bool rectificativa=false;
                bool coma,decimales;
                bool predefinida;
                double gsuplidos=0;
                QString codigopredef;
                void actualizatotales();
                int ultimafila();
                void latexdoc(QString serie, QString numero);
                void terminar(bool impri);
                QString ltipoiva(QString codigo);
                QString ltipore(QString codigo);
                QString texto_linea[100];
                QString pedido_linea[100];
                QString albaran_linea[100];
                QString expediente_linea[100];
                QString contrato_linea[100];
                QStringList lista_suplidos;
                bool imprimir;
                QString nombrefichero_respuesta;
                bool aceptadoConErrores=false;
                void fijatipoiva(QString codigo, double tporuno);
                void fijatipore(QString codigo, double tporuno);
                void guarda_predef();
                void esconde_cta_anticipo();
                void muestra_cta_anticipo();
                bool envia_verifactu(QString serie, QString numero, QString *ghuella, QString *huella_anterior, bool pruebas=false);
                bool gen_fich_verifactu(QString nombrefich, QString serie, QString numero, QString *huella,
                                        QString *huella_anterior, bool ya_existente=false, bool subsanacion=false);
                bool xml_verifactu_consulta(QString nombrefich, QString serie, QString numero);
                bool xml_verifactu_anulacion(QString nombrefich, QString serie, QString numero, QString *nueva_huella);
                QDomElement fdesglose_interior(QDomDocument *doc, double *cuota_total, double *importe_total);
                QDomElement fdesglose_eib(QDomDocument *doc, double *importe_total);
                QDomElement fdesglose_eis(QDomDocument *doc, double *importe_total);
                QDomElement fdesglose_export(QDomDocument *doc, double *importe_total);
                QDomElement fdesglose_ext_isp(QDomDocument *doc, double *importe_total);

      private slots:
                void actudoc();
                void contenidocambiado(int fila,int columna);
                void posceldacambiada (int row , int columnt, int prevrow, int prevcol);
                void ctabasecambiada();
                void ctabasefinedicion();
                void buscactacli();
                void datoscuenta();
                void ret_cambiado();
                void aceptar();
                void gen_pdf();
                void borralinea();
                void muevearriba();
                void mueveabajo();

                void texto_linea_cambiado();
                void pedido_cambiado();
                void albaran_cambiado();
                void expediente_cambiado();
                void contrato_cambiado();

                void procesabotonci();
                void check_oculta_notas_lin();

                void forma_pago();

                void cta_anticipo_cambiada();
                void cta_anticipo_finedicion();
                void buscacta_anticipo();

                void buscar_externo();
                void campo_externo_cambiado();
                void borra_externo();
                void ver_externo();
                void check_facturae();

                void copia_lin_face();
                void fecha_factura_cambiada();

                void on_vf_consulta_pushButton_clicked();
                void on_vf_pruebas_pushButton_clicked();
                void on_subsanacion_pushButton_clicked();
                void on_vf_anulacion_pushButton_clicked();
                void on_serie_rectificada_comboBox_currentIndexChanged(int index);
};



#endif 
