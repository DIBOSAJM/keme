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

#ifndef BUSCAR_CAD_H
#define BUSCAR_CAD_H

#include <QtGui>
#include "ui_buscar_cad.h"



class buscar_cad : public QDialog {
    Q_OBJECT
      public:
                buscar_cad();
                void pasacadena(QString cadena);
                void activarprincipio(bool activar);
                QString contenido();
                bool a_partir_cursor();

      private:
                Ui::buscar_cad ui;

};



#endif 
