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

#ifndef EDITCONFIG_H
#define EDITCONFIG_H

#include <QtGui>
#include "ui_editconfig.h"


class editconfig : public QDialog {
    Q_OBJECT
	public:
                editconfig();
        private:
                Ui::editconfig ui;
                QString fototexto();
	private slots:
            void aceptar();
            void igiccambiado();
            void prorratacambiada();
            void analitica_parc_cambiado();
            void sin_analitica_cambiado();
            void importafoto();
            void borrafoto();
            void proxpasecambiado();
            void tipo_proveedor_cheq();


            void on_prorrata_checkBox_stateChanged(int arg1);
            void on_niflineEdit_textChanged(const QString &arg1);
};





#endif 
