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

#ifndef FILTRAR_VENCI_H
#define FILTRAR_VENCI_H

#include <QtGui>
#include "ui_filtrar_venci.h"



class filtrar_venci : public QDialog {
    Q_OBJECT
      public:
                filtrar_venci(QString cuentabanco);
                void asigna_id_domiciliacion(QString id);
                void para_pagos();
      private:
                Ui::filtrar_venci ui;
                bool comadecimal,decimales;
                void refrescar_ordencol(int col);
                bool pagos=false;

      private slots:
                void botoncodigo_pulsado();
                void boton_borra_codigo_cta_banco();
                void refrescar();
                void selectodo();
                void selecnada();
                void auxbanco_finedic();
                void imp_desde_finedic();
                void imp_hasta_finedic();
                void cta_desde_finedic();
                void cta_hasta_finedic();
                void orden_col();

};



#endif 
