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

#ifndef GENERADORESTADOS_H
#define GENERADORESTADOS_H

#include <QtGui>
#include "ui_generadorestados.h"


class generadorestados : public QDialog {
    Q_OBJECT
	public:
                generadorestados();
                void cargaestado( QString titulo );
                ~generadorestados();
        private:
                Ui::generadorestados ui;
                bool cambios;
                bool chequeanodos1();
                bool chequeanodos2();
                bool chequeaformula(QString laformula);
                bool estadocorrecto();
                void grabarestado();
                QTreeWidgetItem *buscanodo1(QString cadena);
                QTreeWidgetItem *buscanodo2(QString cadena);
                int numdiarios;
                QStringList listacuentas(QString cadena);

        private slots:
                void activarcambiado();
                void saldosmediospulsado();
                void nuevoelemento1();
                void asignacamposedit1();
                void insertanodohijo1();
                void nuevoelemento2();
                void asignacamposedit2();
                void insertanodohijo2();
                void borraelemento1();
                void borraelemento2();
                void segundacheckcambio();
                void apartado1cambiado();
                void apartado2cambiado();
                void clave1cambiada();
                void clave2cambiada();
                void formula1cambiada();
                void formula2cambiada();
                void parte1correcta();
                void parte2correcta();
                void okestado();
                void botongrabarpulsado();
                void botonsalir();
                void desglosecheckcambio();
                void desglose_ctas_cambiado();
                void chequeacuentas();
};





#endif 

