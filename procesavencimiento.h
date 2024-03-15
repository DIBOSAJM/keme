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

#ifndef PROCESAVENCIMIENTO_H
#define PROCESAVENCIMIENTO_H

#include <QtGui>
#include "ui_procesavencimiento.h"


class procesavencimiento : public QDialog {
    Q_OBJECT
	public:
                procesavencimiento(QString qusuario);
                void cargadatos(QString numvenci);
                void asignaasiento(QString elasiento);
                void pasa_fecha_realizacion(QDate fecha);
                void pasa_documento(QString documento);

        private:
                Ui::procesavencimiento ui;
                bool comadecimal;
                bool decimales;
                QString concepto, gasiento, usuario;
                QString guarda_ejercicio;
        private slots:
                void cuentacambiada();
                void cuenta_gasto_cambiada();
                void finediccuenta();
                void finediccuenta_gastos();
                void buscacuenta();
                void buscacuenta2();
                void finedic_gastos();
        public slots:
               void botonprocesarpulsado();
};



#endif
