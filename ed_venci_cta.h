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

#ifndef ED_VENCI_CTA_H
#define ED_VENCI_CTA_H

#include <QtGui>
#include "ui_ed_venci_cta.h"



class ed_venci_cta : public QDialog {
    Q_OBJECT
      public:
                ed_venci_cta();
                void pasa_cta_descrip(QString qcodigo, QString descrip);

      private:
                Ui::ed_venci_cta ui;
                QString codigo;

      private slots:
                void aceptar();
                void checkdiafijo();
                void botoncta_tesorpulsado();
                void cta_tesor_cambiada();
                void cta_tesor_finedicion();
};



#endif 
