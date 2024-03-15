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

#include "referencias.h"
#include "basedatos.h"
#include <QSqlQuery>


referencias::referencias() : QDialog() {
    ui.setupUi(this);

 connect(ui.parte1tableWidget,SIGNAL( currentCellChanged(int,int,int,int )),this,
           SLOT(posceldatabla1cambiada()));

 connect(ui.parte2tableWidget,SIGNAL( currentCellChanged(int,int,int,int )),this,
           SLOT(posceldatabla2cambiada()));

 connect(ui.borrarpushButton,SIGNAL( clicked()),this,SLOT(borracolref()));

 connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(aceptar()));

 ui.parte1tableWidget->setColumnWidth(1,380);
 ui.parte2tableWidget->setColumnWidth(1,380);
}


void referencias::cargaestado( QString vtitulo )
{
   titulo=vtitulo;
   QString vcolref;

   QSqlQuery query = basedatos::instancia()->select4Cabeceraestadostitulo(titulo);
   if ( (query.isActive()) && (query.first()) ) {
           ui.titulolabel->setText(query.value(0).toString());
           ui.titulo1label->setText(query.value(1).toString());
           ui.titulo2label->setText(query.value(2).toString());
           vcolref=query.value(3).toString();
   }

    QStringList columnas;
    columnas << tr("NODO") << tr("APARTADO") << vcolref;

   ui.parte1tableWidget->setHorizontalHeaderLabels(columnas);
   ui.parte2tableWidget->setHorizontalHeaderLabels(columnas);

   int filas = basedatos::instancia()->selectCountnodoestadostituloparte1(titulo, true);

   ui.parte1tableWidget->setRowCount(filas);

   query = basedatos::instancia()->select3Estadostituloparte1ordernodo(titulo, true);
   if ( query.isActive() )
   {
        int fila=0;
        while (query.next())
        {
            for (int veces=0;veces<3;veces++)
            {
                QTableWidgetItem *newItem = new QTableWidgetItem(query.value(veces).toString());
                ui.parte1tableWidget->setItem(fila,veces,newItem);
            }
            fila++;
        }
   }

   // if (filas>0) ui.parte1tableWidget->setCurrentCell(0,2);

   filas = basedatos::instancia()->selectCountnodoestadostituloparte1( titulo, false);

   if (filas==0)
       {
         ui.tabWidget->setTabEnabled(2,false);
         return;
       }

   ui.parte2tableWidget->setCurrentCell(0,2);

   ui.parte2tableWidget->setRowCount(filas);

   query = basedatos::instancia()->select3Estadostituloparte1ordernodo(titulo, false);
   if ( query.isActive() )
       {
          int fila=0;
          while (query.next())
                 {
                  for (int veces=0;veces<3;veces++)
                       {
                        QTableWidgetItem *newItem = new QTableWidgetItem(query.value(veces).toString());
                        ui.parte2tableWidget->setItem(fila,veces,newItem);
                       }
                  fila++;
                 }
       }


}


void referencias::posceldatabla1cambiada()
{
 if (ui.parte1tableWidget->currentColumn()!=2)
     ui.parte1tableWidget->setCurrentCell(ui.parte1tableWidget->currentRow(),2);
}


void referencias::posceldatabla2cambiada()
{
 if (ui.parte2tableWidget->currentColumn()!=2)
     ui.parte2tableWidget->setCurrentCell(ui.parte2tableWidget->currentRow(),2);
}


void referencias::borracolref()
{
 for (int veces=0;veces<ui.parte1tableWidget->rowCount();veces++)
       ui.parte1tableWidget->item(veces,2)->setText("");

 for (int veces=0;veces<ui.parte2tableWidget->rowCount();veces++)
       ui.parte2tableWidget->item(veces,2)->setText("");
}

void referencias::aceptar()
{
 for (int veces=0;veces<ui.parte1tableWidget->rowCount();veces++)
      {
        basedatos::instancia()->updateEstadosreferencianodotituloparte1( ui.parte1tableWidget->item(veces,2)->text() , ui.parte1tableWidget->item(veces,0)->text() , titulo, true );
      }

 for (int veces=0;veces<ui.parte2tableWidget->rowCount();veces++)
      {
        basedatos::instancia()->updateEstadosreferencianodotituloparte1( ui.parte2tableWidget->item(veces,2)->text(), ui.parte2tableWidget->item(veces,0)->text() , titulo, false );
      }

 accept();

}

