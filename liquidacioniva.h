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

#ifndef LIQUIDACIONIVA_H
#define LIQUIDACIONIVA_H

#include <QtGui>
#include "ui_liquidacioniva.h"
#include "tabla_iva_sop.h"
#include "tabla_iva_rep.h"



class liquidacioniva : public QDialog {
    Q_OBJECT
      public:
                liquidacioniva(QString usuario);
                ~liquidacioniva();
      private:
                Ui::liquidacioniva ui;
                tabla_iva_sop *ts_caja_corr, *ts_caja_inv;
                tabla_iva_rep *ts_caja_rep;
                bool comadecimal,decimales;
                QString usuario;
                QString nif, razon, nombre, declaracion_anterior, cta_banco, tipo_operacion;
                QString pfnombre, pfapellidos;
                bool escomplementaria, essustitutiva, redeme;
                double compensar;
                void borrar();
                void generalatex();
                bool genfich303(QString nombre);
      private slots:
                void calcular();
                void fechacambiada();
                void imprimir();
                void imprimir_latex();
                void latex();
                void copia();
                void ver_iva_caja_soportado_corrientes();
                void ver_iva_caja_soportado_inversion();
                void ver_iva_caja_repercutido();
                void gen_asiento();
                void mod303();
                void actufechas();
                void consulta_mayor();
                void informe();

};



#endif 
 
