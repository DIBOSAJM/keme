/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#ifndef CREAEMPRESA_H
#define CREAEMPRESA_H

#include <QtGui>
#include "ui_creaempresa.h"

class creaempresa : public QDialog {
    Q_OBJECT
	public:
                creaempresa();
                bool creabd();
                bool creabd_segunda();
                void creatablasiniciales();
                void importaplan();
                void cargaplan();
                QString planvacio();
                void actufichconex(QString archivo);
                void preparacopia();
                void importacion();
        private:
                Ui::creaempresa ui;
                bool errores_import, nocontinuar;
                QList<int> apuntes_origen, apuntes_conta;
                QString import_plancontable(QTextStream *stream);
                QString import_ejercicios(QTextStream *stream);
                QString import_diario(QTextStream *stream);
                QString import_diario_pg(QTextStream *stream);
                QString import_datos_auxiliares(QTextStream *stream);
                QString import_plan_amortizaciones(QTextStream *stream);
                QString import_libro_facturas(QTextStream *stream);
                QString import_libro_facturas_pg(QTextStream *stream);
                QString import_vencimientos(QTextStream *stream);
                QString import_ci(QTextStream *stream);
                QString import_diario_ci(QTextStream *stream);
                bool es_lin_etiqueta(QString linea);
                int importar_conta(QString cadfich);
                void chequea_error();
                bool facturasrep(QTextStream *stream);
                bool noejercicio(QTextStream *stream);
                bool diario_asientos_descuadre(QTextStream *stream);
        private slots:
                void longvariablecambiado();
};


#endif 
