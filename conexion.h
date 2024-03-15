/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#ifndef CONEXION_H
#define CONEXION_H

#include <QtGui>
#include "ui_conexion.h"



class conexion : public QDialog {
    Q_OBJECT
      public:
                conexion();
		void pasaarchconex(QString archivo);
                void actulistaconex(void);
                void actufichconex(void);
                QString archconex;
                QStringList listaconex;
                QString controlador();
                QString basedatos();
                QString usuario();
                QString clave();
                QString host();
                QString puerto();
      private:
                Ui::conexion ui;
      private slots:
                void actucamposconexion(int indice);

};



#endif 
