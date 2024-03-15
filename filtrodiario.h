/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#ifndef FILTRODIARIO_H
#define FILTRODIARIO_H

#include <QtGui>
#include "ui_filtrodiario.h"


class filtrodiario : public QDialog {
    Q_OBJECT
	public:
                filtrodiario(bool qcomadecimal);
                void pasafiltro(QString qfiltro);
                QString filtro();
                void pasacondicionesfiltrodefecto(QString filtro);
        private:
                Ui::filtrodiario ui;
                bool comadecimal;
                QString condicionesfiltrodefecto;
                void activayo();
                void desactivayo();
        private slots:
                void botonadicpulsado();
                void combocambiado();
                void contenidocambiado();
                void textocambiado();
                void borrafiltro();
                void filtroinicial();
                void finediccontenido();

};





#endif 
 
