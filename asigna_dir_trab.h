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

#ifndef ASIGNA_DIR_TRAB_H
#define ASIGNA_DIR_TRAB_H

#include <QtGui>
#include "ui_asigna_dir_trab.h"


enum conexiones {nombrebase, descrip, fecha_actu, hora_actu, usuario, clave, host,
                 puerto, controlador, ultimo_apunte, import_tipos_diario};

class asigna_dir_trab : public QDialog {
    Q_OBJECT
      public:
                asigna_dir_trab();
                void pasa_listas_info(QStringList conex_rutas, QStringList rutas_trabajo, QStringList rutas_docs);
                void recupera_listas_info(QStringList *conex_rutas, QStringList *rutas_trabajo, QStringList *rutas_docs);
      private:
                Ui::asigna_dir_trab ui;

      private slots:
                void conexion();
                void boton_aceptar();
                void borra_conex();
                void nueva_conex();

};



#endif
