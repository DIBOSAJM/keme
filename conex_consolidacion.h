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

#ifndef CONEX_CONSOLIDACION_H
#define CONEX_CONSOLIDACION_H

#include <QtGui>
#include "ui_conex_consolidacion.h"



class conex_consolidacion : public QDialog {
    Q_OBJECT
      public:
                conex_consolidacion();
                void pasainfo(QString controlador, QString basedatos, QString usuario,
                              QString clave, QString host, QString puerto,
                              QString codigo, QString descripcion, bool infodiario);
                void recupera_info(QString *controlador, QString *basedatos,
                                   QString *usuario,
                                   QString *clave, QString *host, QString *puerto,
                                   QString *codigo, QString *descripcion,
                                   bool *infodiario);


      private:
                Ui::conex_consolidacion ui;
                QStringList listaconex;
                void pasaarchconex();

      private slots:
                void actucamposconexion(int indice);
                void chequea_conexion();

};



#endif
