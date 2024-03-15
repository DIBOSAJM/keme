/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#ifndef EDITAPLAN_H
#define EDITAPLAN_H

#include <QtGui>
#include "ui_editaplan.h"
// #include "arbol.h"

class editaplan : public QDialog {
    Q_OBJECT
	public:
                editaplan();
                void insertacuentaarbol(QString cadena1,QString cadena2);
                void insertacuentaarbol_inicial(QString cadena1,QString cadena2);
                void cargaarbol();

        private:
                Ui::editaplan ui;
                // TreeModel modeloarbol;
                QStringList lista_codigo;
                QList<QTreeWidgetItem*> lista_puntero_item;
                QTreeWidgetItem *buscacadena(QString cadena, bool *encontrada);
                QTreeWidgetItem *buscacadena_ini(QString cadena, bool *encontrada);
                void primer_elemento_ini();
        private slots :
                void codigocambiado(QString codigo);
                void codigofinedicion();
                void filaclicked();
                void descripcambiada();
                void botonaceptar();
                void botonborrar();

};


#endif 
 
