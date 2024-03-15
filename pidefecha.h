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

#ifndef PIDEFECHA_H
#define PIDEFECHA_H

#include <QtGui>
#include "ui_pidefecha.h"



class pidefecha : public QDialog {
    Q_OBJECT
      public:
                pidefecha();
                void asignaetiqueta(QString nombre);
                QDate fecha(void);
                void pasafecha(QDate fecha);
                void asigna_concepto(QString concepto);
                void asigna_documento(QString documento);
                QString recupera_concepto();
                QString recupera_documento();
                void esconde_concepto_documento();
      private:
                Ui::pidefecha ui;

};



#endif 
