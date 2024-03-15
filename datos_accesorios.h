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

#ifndef DATOS_ACCESORIOS_H
#define DATOS_ACCESORIOS_H

#include <QtGui>
#include "ui_datos_accesorios.h"



class datos_accesorios : public QDialog {
    Q_OBJECT
      public:
                datos_accesorios();
                void cargacodigo(QString codigo);

      private:
                Ui::datos_accesorios ui;
                void fijasololectura();
                void fijaescritura();
                QString fototexto();
                void cargacombovenci();
                void cargacomboclaveidfiscal();
                void cargacombopaises();
                void cargacombo_formas_pago();

      private slots:                
                void botoneditar();
                void botonsalir();
                void importafoto();
                void borrafoto();
                void guardadatosacces();
                void botoncta_tesorpulsado();
                void cta_tesor_cambiada();
                void cta_tesor_finedicion();
                void checkdiafijo();
                void nifcambiado();
                void ccccambiado();
                void ibancambiado();

                void boton_ue_pulsado();

};



#endif 
