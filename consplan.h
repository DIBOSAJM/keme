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

#ifndef CONSPLAN_H
#define CONSPLAN_H

#include <QtGui>
#include "ui_consplan.h"
#include "arbol.h"

class consplan  : public QDialog {
    Q_OBJECT
	public:
                consplan();
        private:
                Ui::consplan ui;
                TreeModel modeloarbol;
                void cargaarbol();
                void insertacuentaarbol(QString cadena1,QString cadena2);
                void insertacuentaauxiliararbol(QString cadena1,QString cadena2);
                void generalatex();
        private slots:
                void imprimir();
                void latex();
                void inf_tipo_proveed();

};


#endif 
