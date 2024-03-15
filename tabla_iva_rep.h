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

#ifndef TABLA_IVA_REP_H
#define TABLA_IVA_REP_H

#include <QtGui>
#include "ui_tabla_iva_rep.h"



class tabla_iva_rep : public QDialog {
    Q_OBJECT
      public:
                tabla_iva_rep(QString usuario, bool comadecimal, bool decimales);
                void pasa_info(QString qejercicio, bool qfechacontable, QDate qfechaini,
                               QDate qfechafin, bool qinteriores, bool qaib, bool qeib,
                               bool qautofactura, bool qrectificativa,
                               bool qautofacturanoue,
                               bool qprestserviciosue,
                               bool qporseries,
                               QString qserie_inicial, QString qserie_final,
                               bool qfiltrarnosujetas, bool qsolonosujetas,
                               bool qsinnosujetas, bool qexportacion,
                               bool qisp_interior, bool qexenta_no_deduc, bool ventas_fuera_tac,
                               QString qcuenta, QString qexterno);
                void pasa_info_liquidacion(QDate qfechaini, QDate qfechafin);
                void calcula_listas_sel();
                QStringList qselcuenta_iva();
                QStringList qseltipo();
                QList<double> qselbi();
                QList<double> qselcuota();
                QList<double> qseltotal();
                QList<double> qselcuota_liquidada();

      private:
                Ui::tabla_iva_rep ui;
                QString usuario;
                bool comadecimal,decimales;
                QString ejercicio;
                bool fechacontable;
                QDate fechaini;
                QDate fechafin;
                bool interiores;
                bool aib;
                bool eib;
                bool autofactura;
                bool rectificativa;
                bool autofacturanoue;
                bool prestserviciosue;
                bool porseries;
                QString serie_inicial;
                QString serie_final;
                bool filtrarnosujetas;
                bool solonosujetas;
                bool sinnosujetas;
                bool exportacion;
                bool isp_interior;
                bool exenta_no_deduc;
                bool paraliquidacion;
                bool ventas_fuera_tac;
                QStringList apunte, fecha_cont, fecha_fra, documento, cuenta, descripcion;
                QStringList nif, asiento, bi, tipo, cuota, tipore, re, total, cuota_liquidada;
                QStringList lfilas,  cuenta_iva;
                QHash<QString, int> hash;
                QString buscanumero, buscamargen, buscacadena;
                int col_busqueda;
                int cursor_valor_min_lista(QStringList lista);
                int cursor_cad_min_lista(QStringList lista);
                QStringList selcuenta_iva, seltipo;
                QList<double> selbi,selcuota,seltotal,selcuota_liquidada;
                QString cuentap;
                QString externo;
            private slots:
                void edit_asiento();
                void cons_asiento();
                void borra_asiento();
                void refrescar();
                void ordena_col();
                void busca_en_col();
                void desglose();
                void solo_ptes_criterio_caja();

};



#endif 
