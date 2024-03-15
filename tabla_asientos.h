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

#ifndef TABLA_ASIENTOS_H
#define TABLA_ASIENTOS_H

#include <QtGui>
#include "ui_tabla_asientos.h"
#include "asignainputdiario.h"
#include "etiqueta_fich.h"

class tabla_asientos  : public QDialog {
    Q_OBJECT
	public:
                tabla_asientos(bool concomadecimal, bool condecimales, QString qusuario);
                ~tabla_asientos();
                void preparaaib();
                void preparaeib();
                void preparaautofactura();
                void preparaautofactura_no_ue();
                void preparapr_servicios_ue();
                void prepara_isp_op_interiores();
                void pasanumasiento( QString  cadasiento);
                void pasadatos(int fila, QString col0, QString col1, QString col2, QString col3, QString col4,
                                QString col5, QString col6, QString col7, QString col8, QString col9,
                                QString col10, QString col11, QString col12, QString col13, QString col14,
                                QString col15, QString col16, QString col17, QString col18, QString col19,
                                QString col20, QString col21, QString col22,
                                QString col23, QString col24, QString col25, QString col26,
                                QString col27, QString col28, QString col29, QString col30,
                                QString col31, QString col32, QString col33, QString col34, QString col35,
                                QString col36, QString col37, QString col38, QString col39,
                                QString col40, QString col41, QString col42, QString col43,
                                QString col44, QString col45, QString col46, QString col47,
                                QString col48, QString col49, QString col50, QString col51,
                               QString col52, QString col53, QString col54, QString col55,
                               QString col56, QString col57, QString col58, QString externo, QString concepto_sii);
                void pasafechaasiento( QDate lafecha);
                void pasadiario(QString qdiario);
                void pasanocerrar(bool noclose);
                void pasadatos1( int fila, QString col0, QString col1, QString col2,
                                 QString col3, QString col4, QString col16);
                void pasadatos2(int fila, QString col5, QString col6, QString col7,
                                QString col8, QString col9,
                                QString col10, QString col11, QString col12, QString col13,
                                QString col14,
                                QString col15, QString col17, QString col18, QString col19,
                                QString fechaop20, QString claveop21, QString bicosto22,
                                QString rectificada, QString nfacturas, QString finicial,
                                QString ffinal, QString binversion, QString afecto,
                                QString agrario,
                                QString nombre, QString cif,
                                QString importacion, QString exportacion,
                                QString exenta_nodeduc, QString isp_op_interiores,
                                QString caja_iva, bool no347=false,
                                bool arrto_ret=false, bool arrto_sin_ret=false, QString dua="", QString info_donacion="");
                void pasadatos_ret(int fila, QString cta_retenido,
                                   QString arrendamiento, QString clave, QString base_ret,
                                   QString tipo_ret, QString retencion, QString ing_a_cta,
                                   QString ing_a_cta_rep, QString nombre_ret,
                                   QString cif_ret, QString provincia);
                void activaparaedicion();
                void chequeatotales();
                void soportadoautonomo();
                void repercutidoautonomo();
                void activa_aibautonomo();
                void activa_eibautonomo();
                void quitavar_aibautonomo();
                void quitavar_eibautonomo();
                void make_linea_autofactura_no_ue(int linea);
                void make_linea_prest_servicio_ue(int linea);
                void aibautonomo();
                void eibautonomo();
                bool botonincorporaractivado();
                double sumadebe();
                double sumahaber();
                void pasacadenaprorrata(QString cad);
                void borra_valores_iva(int fila);
                bool esparaedicion();
                void pasainfoexento(bool ivaexentorecibidas,  QString ctabaseexento,
                                    QString baseimponibleexento, QString cuentafraexento,
                                    QDate fechaexent, QDate fechaopexento, QString claveopexento,
                                    QString qfrectificativaexento,
                                    QString qfrectificadaexento, QString qnfacturasexento, QString qinicialexento,
                                    QString qfinalexento, bool binversion,
                                    QString qnombre, QString qcif,
                                    bool importacion, bool exportacion, bool nodeduccion, bool ventas_fuera_tac,
                                    bool qemitidas_int_isp, bool donacion, QString clave_donacion, bool donacion_especie, bool donacion_2ejer, QString comunidad_autonoma,
                                    double porcent_deduc_autonomia, QString dua);
                void pasainfoaib1(QString cbase, QString bi, QString clave_iva,
                                  QString ctaivasop, QString ctafra, QDate fecha_fra,
                                  QDate fecha_op, QString bicoste, bool binversion, bool oro_inversion=false);
                void pasainfoaib2(QString ctaivarep); // solo pasa cuenta de IVA repercutido AIB/Autofactura
                void pasainfoeib(QString cbase, QString bi,QString cuenta_fra, QDate fecha_fra,
                                 QDate fechaop, QString claveop);
                void pasafichdoc(QString cadena);
                void pasafichprov(QString cadena);
                void selec_nosujeta();
                void activa_modoconsulta();
                void crea_tabla_ci(int fila);
                void pasainputacion(int fila, QString ci, double total, double porcentaje);
                void actualiza_inputacion(int fila, double total, bool modoconsulta);
                void marca_ci(int fila);
                bool cuenta_para_analitica(QString codigo);
                void realiza_asignainputdiario(int fila, QString importe);
                void pasalineafactura(int fila);
                qlonglong qpasefactura();
                void activa_eib_exterior();
                void desactiva_eib_exterior();
                void activa_fila_nosujeta(int fila);
                void activa_fila_prservicios_ue(int fila);
                void pasa_datos_ref(int fila, QString cta_retenido,
                                                    bool ret_arrendamiento,
                                                    QString clave, QString base_percepciones,
                                                    QString tipo_ret, QString retencion,
                                                    QString ing_cta, QString ing_cta_repercutido,
                                                    QString nombre_ret, QString cif_ret,
                                                    QString provincia);
                void evita_msj_recepcion();
                void pasa_num_recepcion(QString cadena);
                void fuerzacontado();
                void fuerza_sin_vencimientos();
                void marca_regul_iva();
                QString ruta_docs();
                void activa_exportacionexento();
                void activa_nodeduccionexento();
                void evita_pregunta_venci();
                void pasa_fecha_iva_aut(QDate fecha);
                void pasa_doc(QString doc);
                void desactiva_avisar_cta_libro_fact_no_diario();
                void pasa_externo(QString codigo);
                void pasa_concepto_sii(QString codigo);
                void predefinido();
        private:
                Ui::tabla_asientos ui;
                QString cadasientoreal;
                bool comadecimal;
                bool decimales;
                bool esaib;
                bool eseib;
                bool esautofactura;
                bool esaibautonomo;
                bool eseibautonomo;
                bool esautofactura_no_ue;
                bool es_isp_op_interiores;
                bool espr_servicios_ue;
                bool opnosujeta;
                bool nocerrar;
                bool no_msj_recepcion;
                bool evitar_pregunta_venci;
                bool espredefinido;
                QString gprorrata;
                QString cverdadero,cfalso;
                QStringList pasesvto;
                // Estas variables guardan el contenido de la línea de IVA exento
                // en el caso de edición de un asiento
                bool ivaexentorecibidas;
                QString ctabaseexento;
                QString baseimponibleexento;
                QString cuentafraexento;
                QDate fechaexento;
                QDate fechaopexento;
                QString claveopexento;
                QString frectificativaexento;
                QString frectificadaexento;
                QString nfacturasexento;
                QString inicialexento;
                QString finalexento;
                bool binversionexento;
                bool forzarcontado;
                bool sinvencimientos;
                QString nombreexento;
                QString cifexento;
                bool importacionexento;
                bool exportacionexento;
                bool nodeduccionexento;
                bool ventas_nacionales_fuera_tac;
                bool emitidas_int_isp;
                QString registro_donacion;

                QString aib_autof_cbase;
                QString aib_autof_bi;
                QString aib_autof_clave_iva;
                QString aib_autof_ctaivarep;
                QString aib_autof_ctaivasop;
                QString aib_autof_ctafra;
                QDate aib_autof_fecha_fra;
                QDate aib_fecha_op;
                QString aib_bicoste;
                bool aib_binversion;
                bool isp_oroinversion;

                QString eib_cbase;
                QString eib_bi;
                QString eib_cuenta_fra;
                QDate eib_fecha_fra;
                QDate eib_fechaop;
                QString eib_claveop;
                QString usuario;
                QString guardaejercicio;
                bool modoconsulta;
                int lineafactura;
                qlonglong pasefactura;
                bool eib_exterior;
                QList<asignainputdiario*> inputaciones;
                bool ret_procesada;
                QString directorio_doc;
                QDate fecha_factura;
                bool hay_fecha_factura;
                QString docini;
                QString dua_ex;
                // --------------------------------------------------------------
                // Esto es para activar paso de fecha asiento a iva autónomo
                bool pasar_fecha_iva_aut;
                QDate fecha_iva_aut;
                bool avisar_cta_libro_fact_no_diario;
                // -------------------------------------------------------------
                void procesaivasoportado(int fila,int columna);
                void procesaivarepercutido(int fila, int columna);
                void procesaretencion(int fila,int columna);
                int ultimafila();
                void procesabotonivarepercutido();
                void incorporaedicion();
                void descuadreazul();
                void descuadrerojo();
                void actualizaestado();
                bool procesa_aib(QString documento);
                bool procesa_op_exenta();
                void procesaivaasociado(int fila, int columna,QString cuentabase,
                                        QString cuentaiva, QString tipoiva, QString importe);
                void procesaivaasociado_financ(int fila, int columna, QString cuentafin,
                                              QString cuentaiva, QString tipoiva, QString total, QString cta_base_iva);
                void procesavenci(QString pase);
                void procesaiva_cta(int fila, int columna,
                                         QString tipoiva);
                void procesa_ret_apunte(int fila,
                                        int columna,
                                        QString qcuentaretencion,
                                        QString qbase_percepciones,
                                        double tipo_ret,
                                        QString tipo_operacion_ret,
                                        bool ret_arrendamiento);
                void borratodo(bool preguntar);
                void limpiaiva(int fila);
                void limpiaret(int fila);
                CustomQLabel *etiqueta;

       private slots:
                void posceldacambiada ( int currentRow, int col, int previousRow,
                                        int previousColumn );
                void borralinea( void );
                void botonborrartodo();
                void botonivapulsado();
                void aibcambiado();
                void eibcambiado();
                void exentacambiado();
                void regIVAcambiado();
                void botonesperapulsado();
                void pegar();
                void procesabotonci();
                void mayor();
                void fechacambiada();
                void cancelar();
                void muevearriba();
                void mueveabajo();
                void buscacopiadoc();
                void visdocdef();
                void impridocdef();
                void suprimedocdef();
                void borraasignaciondoc();
                void boton_retencion_pulsado();
                void procesa_celda_analitica(int fila, int columna);
                void buscar_externo();
                void campo_externo_cambiado();
                void borra_externo();
                void ver_externo();
                void imputctas();
                void fichero_soltado();

       public slots:
                void contenidocambiado(int fila,int columna);
                void incorporar();
                void activa_opexenta();
                void visdoc();

};


#endif
