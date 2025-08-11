/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre; ya sea la versión 3
    de la Licencia; o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil; pero
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario; consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

#ifndef TABLA_IVA_SOP_H
#define TABLA_IVA_SOP_H

#include <QtGui>
#include "ui_tabla_iva_sop.h"



class tabla_iva_sop : public QDialog {
    Q_OBJECT
      public:
                tabla_iva_sop(QString usuario, bool comadecimal, bool decimales);
                void pasa_info(QString qejercicio, bool qfechacontable,
                               QDate qfechaini, QDate qfechafin,
                               bool qinteriores, bool qaib, bool qautofactura,
                               bool qautofacturanoue,
                               bool qrectificativa, bool qporseries,
                               QString qserie_inicial, QString qserie_final,
                               bool qfiltrarbinversion, bool qsolobi, bool qsinbi,
                               bool qfiltrarnosujetas, bool qsolonosujetas,
                               bool qsinnosujetas, bool qagrario, bool isp_op_interiores,
                               bool importaciones, QString cuenta, QString qexterno);
                void pasa_info_liquidacion(QDate qfechaini, QDate qfechafin,
                                           bool binversion);
                QStringList qselcuenta_iva();
                QStringList qseltipo();
                QList<double> qselbi();
                QList<double> qselcuota();
                QList<double> qseltotal();
                QList<double> qselcuota_efectiva();
                QList<double> qselcuota_liquidada();
                void calcula_listas_sel();
      private:
                Ui::tabla_iva_sop ui;
                QString usuario;
                bool comadecimal,decimales;
                QString ejercicio;
                bool fechacontable;
                QDate fechaini;
                QDate fechafin;
                bool interiores;
                bool aib;
                bool autofactura;
                bool rectificativa;
                bool autofacturanoue;
                bool porseries;
                QString serie_inicial;
                QString serie_final;
                bool filtrarbinversion;
                bool solobi;
                bool sinbi;
                bool filtrarnosujetas;
                bool solonosujetas;
                bool sinnosujetas;
                bool agrario;
                bool isp_op_interiores;
                bool importaciones;
                bool solo_caja;
                bool paraliquidacion;
                QStringList apunte, fecha_cont, fecha_fra, documento, cuenta, descripcion;
                QStringList nif, asiento, bi, tipo, cuota, total, prorrata, afectacion;
                QStringList cuota_efectiva, recepcion, cuota_liquidada, lfilas;
                QStringList cuenta_iva;
                QHash<QString, int> hash;
                int cursor_valor_min_lista(QStringList lista);
                int cursor_cad_min_lista(QStringList lista);
                QString buscanumero, buscamargen, buscacadena;
                int col_busqueda;
                QStringList selcuenta_iva, seltipo;
                QList<double> selbi,selcuota,seltotal,selcuota_efectiva,selcuota_liquidada;
                QString cuenta_factura,externo;
            private slots:
                void edit_asiento();
                void cons_asiento();
                void borra_asiento();
                void refrescar();
                void ordena_col();
                void busca_en_col();
                void desglose();
                void chequea_solo_criterio_caja();
                void solo_ptes_criterio_caja();

                void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
};



#endif 
