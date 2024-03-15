/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#ifndef DESVIACION_CTA_H
#define DESVIACION_CTA_H

#include <QtGui>
#include "ui_desviacion_cta.h"



class desviacion_cta : public QDialog {
    Q_OBJECT
      public:
                desviacion_cta(QString qcuenta,QString qejercicio);
      private:
                Ui::desviacion_cta ui;
                bool comadecimal,decimales;
                QString ejercicio,cuenta;
                void generalatex();
      private slots:
                void imprime();
                void copiar();
};



#endif 
 
 
