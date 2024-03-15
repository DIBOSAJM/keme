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

#ifndef CONSOLIDACION_SINC_H
#define CONSOLIDACION_SINC_H

#include <QtGui>
#include "ui_consolidacion_sinc.h"


enum conexiones {nombrebase, descrip, fecha_actu, hora_actu, usuario, clave, host,
                 puerto, controlador, ultimo_apunte, import_tipos_diario};

class consolidacion_sinc : public QDialog {
    Q_OBJECT
      public:
                consolidacion_sinc();

      private:
                Ui::consolidacion_sinc ui;
                QString conex[9][11];
                QStringList origen_auxiliares,conv_auxiliares,descrip_auxiliares;
                void actu_tabla();
                void consolida(int codigo);
                void consolida_ejercicio(int codigo, QString ejercicio);
                void actualizaplan(int codigo);

      private slots:
                void conexion();
                void boton_aceptar();
                void actualiza_conex();
                void actualiza_todas_conex();
                void borra_conex();

};



#endif
