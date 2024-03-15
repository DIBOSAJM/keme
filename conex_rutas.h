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

#ifndef CONEX_RUTAS_H
#define CONEX_RUTAS_H

#include <QtGui>
#include "ui_conex_rutas.h"



class conex_rutas : public QDialog {
    Q_OBJECT
      public:
                conex_rutas();
                void pasainfo(QString basedatos, QString ruta_trabajo, QString ruta_docs);
                void recupera_info(QString *basedatos, QString *ruta_trabajo,
                                   QString *ruta_docs);


      private:
                Ui::conex_rutas ui;
                QStringList listaconex;
                void pasaarchconex();

      private slots:
                void actucamposconexion(int indice);
                void chequea_conexion();
                void boton_ruta_trabajo();
                void boton_ruta_docs();

};



#endif
