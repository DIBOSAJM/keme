/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

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

#include "columnas_ecpn_pymes.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"

columnas_ecpn_pymes::columnas_ecpn_pymes() : QDialog() {
    ui.setupUi(this);

QSqlQuery q=basedatos::instancia()->carga_cols_evpn_pymes();

if (q.isActive())
    while (q.next())
    {
     int numero=q.value(0).toInt();
     switch(numero)
      {
       case 1 :
           ui.columnalineEdit->setText(q.value(1).toString());
           ui.contenidolineEdit->setText(q.value(2).toString());
           break;
      case 2 :
          ui.columnalineEdit_2->setText(q.value(1).toString());
          ui.contenidolineEdit_2->setText(q.value(2).toString());
          break;
      case 3 :
          ui.columnalineEdit_3->setText(q.value(1).toString());
          ui.contenidolineEdit_3->setText(q.value(2).toString());
          break;
      case 4 :
          ui.columnalineEdit_4->setText(q.value(1).toString());
          ui.contenidolineEdit_4->setText(q.value(2).toString());
          break;
      case 5 :
          ui.columnalineEdit_5->setText(q.value(1).toString());
          ui.contenidolineEdit_5->setText(q.value(2).toString());
          break;
      case 6 :
          ui.columnalineEdit_6->setText(q.value(1).toString());
          ui.contenidolineEdit_6->setText(q.value(2).toString());
          break;
      case 7 :
          ui.columnalineEdit_7->setText(q.value(1).toString());
          ui.contenidolineEdit_7->setText(q.value(2).toString());
          break;
      case 8 :
          ui.columnalineEdit_8->setText(q.value(1).toString());
          ui.contenidolineEdit_8->setText(q.value(2).toString());
          break;
      case 9 :
          ui.columnalineEdit_9->setText(q.value(1).toString());
          ui.contenidolineEdit_9->setText(q.value(2).toString());
          break;
      case 10 :
          ui.columnalineEdit_10->setText(q.value(1).toString());
          ui.contenidolineEdit_10->setText(q.value(2).toString());
          break;
      case 11 :
          ui.columnalineEdit_11->setText(q.value(1).toString());
          ui.contenidolineEdit_11->setText(q.value(2).toString());
          break;
      }
    }

connect(ui.aceptarpushButton, SIGNAL(clicked()),this, SLOT(aceptar()));

}


void columnas_ecpn_pymes::aceptar()
{
  // borrar tabla cols_evpn_pymes
    basedatos::instancia()->borra_cols_evpn_pymes();
  // insertar elementos
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("1",
            ui.columnalineEdit->text(), ui.contenidolineEdit->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("2",
            ui.columnalineEdit_2->text(), ui.contenidolineEdit_2->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("3",
            ui.columnalineEdit_3->text(), ui.contenidolineEdit_3->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("4",
            ui.columnalineEdit_4->text(), ui.contenidolineEdit_4->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("5",
            ui.columnalineEdit_5->text(), ui.contenidolineEdit_5->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("6",
            ui.columnalineEdit_6->text(), ui.contenidolineEdit_6->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("7",
            ui.columnalineEdit_7->text(), ui.contenidolineEdit_7->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("8",
            ui.columnalineEdit_8->text(), ui.contenidolineEdit_8->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("9",
            ui.columnalineEdit_9->text(), ui.contenidolineEdit_9->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("10",
            ui.columnalineEdit_10->text(), ui.contenidolineEdit_10->text());
    basedatos::instancia()->inserta_reg_cols_evpn_pymes("11",
            ui.columnalineEdit_11->text(), ui.contenidolineEdit_11->text());
    accept();
}
