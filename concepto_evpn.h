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

#ifndef CONCEPTO_EVPN_H
#define CONCEPTO_EVPN_H

#include <QtGui>
#include "ui_concepto_evpn.h"



class concepto_evpn : public QDialog {
    Q_OBJECT
      public:
                concepto_evpn(QString qapunte, QString cuenta, QString concepto,
                              QString debe, QString haber,
                              QString codconcepto);

      private:
                Ui::concepto_evpn ui;
                QString apunte;
      private slots:
                void terminar();


};



#endif 
