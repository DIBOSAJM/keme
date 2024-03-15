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

#ifndef EDASIENTOMODELO_H
#define EDASIENTOMODELO_H

#include <QtGui>
#include "ui_edasientomodelo.h"


class edasientomodelo : public QDialog {
    Q_OBJECT
	public:
                edasientomodelo();
                void cargadatos( QString asientomodelo );
        private:
                Ui::edasientomodelo ui;
                bool cambios;
                bool comadecimal;
                bool chequeadatos();

        private slots:
                void celdavarcambiada( int row);
                void borralineavar();
                void cambiosvar(int row,int col);
                void activacambiado();
                void detalleposcambiada( int row, int col);
                void borralineadetalle();
                void formulacambiada();
                void detallecontenidocambiado( int row, int col );
                void guardarinfo();
                void botonsalir();
                void muevearriba();
                void mueveabajo();
                void tipo_combo_cambiado();
                void entregasboxpulsado();
                void adqboxpulsado();

};



#endif 
 
