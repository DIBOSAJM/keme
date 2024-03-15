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

#ifndef EDITAFECHAASIENTO_H
#define EDITAFECHAASIENTO_H

#include <QtGui>
#include "ui_editafechaasiento.h"


class editafechaasiento : public QDialog {
    Q_OBJECT
	public:
                editafechaasiento();
                void pasadatos( QString asiento, QDate fecha );

        private:
                Ui::editafechaasiento ui;
                QDate gfecha;
                QString gasiento;
        private slots:
                void procesar();

};





#endif
