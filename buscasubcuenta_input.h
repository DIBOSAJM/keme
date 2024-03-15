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

#ifndef BUSCASUBCUENTA_INPUT_H
#define BUSCASUBCUENTA_INPUT_H
#include <QSqlTableModel>

#include <QtGui>
#include "ui_buscasubcuenta_input.h"


class buscasubcuenta_input : public QDialog {
    Q_OBJECT
	public:
                buscasubcuenta_input();
        private:
                Ui::buscasubcuenta_input ui;
                QStringList codigo,codigo_origen;
                QStringList descripcion,descripcion_origen;
                QString seleccioncuenta( int *fila );
                void marca(int fila);
                void desmarca(int fila);
            private slots:
                void subcuenta_cambiada();
                void titulo_cambiado();
                void procesaimputaciones();
                void copia_cuenta();
                void elimina_copia();
};





#endif 
 
 
