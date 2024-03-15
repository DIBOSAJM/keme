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

#ifndef REGULARIZACION_H
#define REGULARIZACION_H

#include <QtGui>
#include "ui_regularizacion.h"



class regularizacion : public QDialog {
    Q_OBJECT
      public:
                regularizacion(QString usuario);
                void generar_ejercicio(QString qejercicio);
                bool escorrecto();
                void proc_externo();

      private:
                Ui::regularizacion ui;
                QString usuario;
                bool correcto, proceso_externo;
                void generar_analitica(QString qejercicio);
                void generar_analitica_tabla(QString qejercicio);
      private slots:
                void generar();

};



#endif 

