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

#ifndef INTROCI_H
#define INTROCI_H

#include <QtGui>
#include "ui_introci.h"
#include <QSqlTableModel>


class introci : public QDialog {
    Q_OBJECT
	public:
                introci();
                QString cadenaci();
                void pasaci(QString codigo);
                void hazparafiltro();

        private:
                Ui::introci ui;
                bool parafiltro;
        private slots:
                void combo1cambiado();
                void combo2cambiado();
                void combo3cambiado();
                void d1cambiado();
                void d2cambiado();
                void d3cambiado();
                void chequeabotonaceptar();
                void combo1textocambiado(QString cadena);
                void combo2textocambiado(QString cadena);
                void combo3textocambiado(QString cadena);
                void combo3finedictexto();


};





#endif 
