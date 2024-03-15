/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

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

#ifndef PRESUPCUENTA_H
#define PRESUPCUENTA_H

#include <QtGui>
#include "ui_presupcuenta.h"


class presupcuenta : public QDialog {
    Q_OBJECT
	public:
                presupcuenta(QString ejercicio,QString cuenta);

        private:
                Ui::presupcuenta ui;
                QString ejercicio;
                QString cuenta;
                bool comadecimal,decimales;
                void actualizaporc(int fila);
                void generalatex();
        private slots:
                void checkperiodoscambiado();
                void celdafinedic();
                void posicioncambiada ( int, int);
                void presupcambiado();
                void grabar();
                void imprime();
                void repartir_periodos();
   };


#endif
 
