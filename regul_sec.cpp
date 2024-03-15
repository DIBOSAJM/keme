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

#include "regul_sec.h"
#include "basedatos.h"
#include "funciones.h"

regul_sec::regul_sec(QString qcodigo, QString qdescripcion) : QDialog() {
    ui.setupUi(this);


QStringList diarios;

diarios << diario_no_asignado();
diarios << diario_regularizacion();
  QSqlQuery query = basedatos::instancia()->selectCodigoDiariosordercodigo();
  if (query.isActive())
  while (query.next())
     {
         diarios << query.value(0).toString();
     }

  ui.diariocomboBox->addItems(diarios);

 ui.tableWidget->setColumnWidth(0,200);  // descripción
 ui.tableWidget->setColumnWidth(1,110);  // cuenta regularización
 ui.tableWidget->setColumnWidth(2,110);  // cuenta regularización
 ui.tableWidget->setColumnWidth(3,350);  // cuentas implicadas
 ui.tableWidget->setColumnWidth(4,160);  // condición
 codigo=qcodigo;
 ui.label->setText(qcodigo+": "+qdescripcion);
 
 //ui.tableWidget->horizontalHeaderItem (1)->setTextAlignment(Qt::AlignJustify);

 // cargamos contenido de la tabla

 query = basedatos::instancia()->carga_regul_sec(codigo);
 QString diario;
 if ( query.isActive() ) {
   int pos=0;
   while (query.next())
     {
       ui.tableWidget->setItem(pos,0,new QTableWidgetItem(""));
       ui.tableWidget->item(pos,0)->setText(query.value(0).toString());

       ui.tableWidget->setItem(pos,1,new QTableWidgetItem(""));
       ui.tableWidget->item(pos,1)->setText(query.value(1).toString());

       ui.tableWidget->setItem(pos,2,new QTableWidgetItem(""));
       ui.tableWidget->item(pos,2)->setText(query.value(2).toString());

       ui.tableWidget->setItem(pos,3,new QTableWidgetItem(""));
       ui.tableWidget->item(pos,3)->setText(query.value(3).toString());

       ui.tableWidget->setItem(pos,4,new QTableWidgetItem(""));
       ui.tableWidget->item(pos,4)->setText(query.value(4).toString());

       diario=query.value(5).toString();
       // ui.tableWidget->setItem(pos,5,new QTableWidgetItem(""));
       // ui.tableWidget->item(pos,5)->setText(query.value(5).toString());
       pos++;
     }
  }

  for (int veces=0; veces<ui.diariocomboBox->count();veces++)
     {
      if (ui.diariocomboBox->itemText(veces)==diario)
        {
         ui.diariocomboBox->setCurrentIndex(veces);
         break;
        }
     }

    connect(ui.tableWidget,SIGNAL( cellChanged ( int , int )),this,
           SLOT(contenidocambiado(int,int )));
    connect(ui.tableWidget,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(posceldacambiada ( int )));
    connect(ui.aceptarpushButton,SIGNAL( clicked()),this,
           SLOT(botonaceptar()));

}
 

void regul_sec::posceldacambiada ( int row )
{
    if (ui.tableWidget->item(row,0)==0) ui.tableWidget->setCurrentCell(ultimafila(),0);
      else
          {
           if (ui.tableWidget->item(row,0)->text().length()==0) 
        	ui.tableWidget->setCurrentCell(ultimafila(),0);
          }
    if  (row>0 && ui.tableWidget->item(row-1,0)==0)
        	ui.tableWidget->setCurrentCell(ultimafila(),0);
        else
            {
             if  (row>0 && ui.tableWidget->item(row-1,0)->text().length()==0)
        	ui.tableWidget->setCurrentCell(ultimafila(),0);
            }
}

int regul_sec::ultimafila()
{
  int veces=0;
  while (veces<ui.tableWidget->rowCount ())
    {
      if (ui.tableWidget->item(veces,0)==0) break;
      if (ui.tableWidget->item(veces,0)->text().length()==0) break; 
      veces++;
    }
  if (veces==ui.tableWidget->rowCount()) return veces-1;
 return veces;
}


void regul_sec::contenidocambiado(int fila,int columna)
{
 if (ui.tableWidget->currentColumn()!=columna || ui.tableWidget->currentRow()!=fila) return;

 if (columna==1)
    {
     if (ui.tableWidget->item(fila,columna)!=0 && !cod_longitud_variable())
        ui.tableWidget->item(fila,columna)->setText(
                              expandepunto(ui.tableWidget->item(fila,columna)->text(),
					       anchocuentas()));
     if (ui.tableWidget->item(fila,2)==0)
        {
          ui.tableWidget->setItem(fila,2,new QTableWidgetItem(""));
        }
     if (ui.tableWidget->item(fila,2)->text().length()==0)
        ui.tableWidget->item(fila,2)->setText(ui.tableWidget->item(fila,columna)->text());
    }

 if (columna==2)
    {
     if (ui.tableWidget->item(fila,columna)!=0 && !cod_longitud_variable())
        ui.tableWidget->item(fila,columna)->setText(
                              expandepunto(ui.tableWidget->item(fila,columna)->text(),
					       anchocuentas()));
     if (ui.tableWidget->item(fila,1)==0)
        {
          ui.tableWidget->setItem(fila,1,new QTableWidgetItem(""));
        }
     if (ui.tableWidget->item(fila,1)->text().length()==0)
        ui.tableWidget->item(fila,1)->setText(ui.tableWidget->item(fila,columna)->text());
    }

}

void regul_sec::botonaceptar()
{

  basedatos::instancia()->borra_regul_sec(codigo);

  QString cadena="delete from regul_sec where codigo='";
  cadena+=codigo;
  cadena+="'";
  int limite=ultimafila();
  if (ui.tableWidget->item(limite,0)!=0)
     {
      if (ui.tableWidget->item(limite,0)->text().length()==0) limite--;
     }
     else limite--;

         

  // insertamos valores de ui.tableWidget en tabla
  QString diario;
  if (ui.diariocomboBox->currentText()!=diario_no_asignado())
      diario=ui.diariocomboBox->currentText();
  for (int veces=0; veces<=limite; veces++)
      {
       basedatos::instancia()->inserta_reg_sec(
               codigo,
               veces+1,
               ui.tableWidget->item(veces,0) !=0 ? ui.tableWidget->item(veces,0)->text() : "",
               ui.tableWidget->item(veces,1) !=0 ? ui.tableWidget->item(veces,1)->text() : "",
               ui.tableWidget->item(veces,2) !=0 ? ui.tableWidget->item(veces,2)->text() : "",
               ui.tableWidget->item(veces,3) !=0 ? ui.tableWidget->item(veces,3)->text() : "",
               ui.tableWidget->item(veces,4) !=0 ? ui.tableWidget->item(veces,4)->text() : "",
               diario);
      }
  accept();
}
