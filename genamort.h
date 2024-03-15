/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2; aplicación para llevar contabilidades

    Copyright (C) José Manuel Díez Botella

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

#ifndef GENAMORT_H
#define GENAMORT_H

#include <QtGui>
#include "ui_genamort.h"


class genamort : public QDialog {
    Q_OBJECT
	public:
                genamort(QString qusuario);
        private:
                Ui::genamort ui;
                QString usuario;
                QString vartodos;
                QString varenero;
                QString varfebrero;
                QString varmarzo;
                QString varabril;
                QString varmayo;
                QString varjunio;
                QString varjulio;
                QString varagosto;
                QString varseptiembre;
                QString varoctubre;
                QString varnoviembre;
                QString vardiciembre;
                QString valamortpers( QString ejercicio, QString cuenta );
        private slots:
                void procesa();
                void eliminar();

};



#endif
