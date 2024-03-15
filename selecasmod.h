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

#ifndef SELECASMOD_H
#define SELECASMOD_H

#include <QtGui>
#include "ui_selecasmod.h"
// #include "arbol.h"


class selecasmod : public QDialog {
    Q_OBJECT
	public:
                selecasmod(QString qusuario);
                QString elecactual();
                void activar_msj_tabla();
        private:
                Ui::selecasmod ui;
                QString usuario;
                void refresca();
                // TreeModel modeloarbol;
                void cargaarbol();
                bool activa_msj_tabla;
                QStringList lista_codigo;
                QList<QTreeWidgetItem*> lista_puntero_item;

        private slots:
               // void seleccioncambiada();
               void nuevoasautomatico();
               void editaasmodelo();
               void execasmodelo();
               void borramodelo();
               void exportaasmodelo();
               void importaasmodelo();
               void tipos();
               void aspiradora();

    };



#endif 
