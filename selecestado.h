/* ----------------------------------------------------------------------------------
    KEME-Contabilidad ; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

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

#ifndef SELECESTADO_H
#define SELECESTADO_H

#include <QtGui>
#include "ui_selecestado.h"


class selecestado : public QDialog {
    Q_OBJECT
	public:
                selecestado(QString qusuario);
                QString elecactual();
                QString usuario;
        private:
                Ui::selecestado ui;
                void refresca();
        private slots:
                void nuevogenestado();
                void edgenestado();
                void calculaestado();
                void borrarestado();
                void exportestado();
                void importestado();

};



#endif 
