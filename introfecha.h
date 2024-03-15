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

#ifndef INTROFECHA_H
#define INTROFECHA_H

#include <QtGui>
#include "ui_introfecha.h"


class introfecha : public QDialog {
    Q_OBJECT
	public:
                introfecha(QDate fecha);
                QDate fecha();
                QString diario();
                QString documento();
                bool nocerrar();
                void pasa_fecha_nocerrar(QDate fecha, bool nocerrar);
                int herramienta();
                void esconde_herramientas();
        private:
                Ui::introfecha ui;
                int herram;

        private slots:
                // void nocerrar_cambiado();
                void boton_varios_sop();

                void boton_aib();

                void boton_varios_rep();

                void boton_eib();

                void boton_predef();

};


#endif 
