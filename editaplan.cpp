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

#include "editaplan.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>



editaplan::editaplan() : QDialog() {
  ui.setupUi(this);

// esta porción de código es para asegurarnos de que la ventana
// sale por el centro del escritorio
 ui.treeWidget->setColumnCount(2);
 QStringList columnas;
 columnas << tr("CÓDIGO") << tr("DESCRIPCIÓN");
 ui.treeWidget->setHeaderLabels(columnas);

 ui.treeWidget->expandAll();
 cargaarbol();
  // ui.treeView->setModel(&modeloarbol);
  // ui.treeView->expandAll();
  // ui.treeView->resizeColumnToContents(0);
  // ui.treeView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

  ui.codigolineEdit->setFocus();
  connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado(QString)));
  connect(ui.codigolineEdit,SIGNAL(editingFinished()),this,SLOT(codigofinedicion()));
  connect(ui.treeWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(filaclicked()));
  connect(ui.descripcionlineEdit,SIGNAL( textChanged(QString)),this,SLOT(descripcambiada()));
  connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(botonaceptar()));
  connect(ui.borrarpushButton,SIGNAL( clicked()),this,SLOT(botonborrar()));

}

void editaplan::cargaarbol()
{
  QSqlQuery query = basedatos::instancia()->selectCodigodescripcionplancontableordercodigo();
  if ( query.isActive() ) {
          while ( query.next() )
              {

                insertacuentaarbol_inicial(query.value(0).toString(),query.value(1).toString());
                // QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento,datos);
                //QMessageBox::information( this, tr("Edición de plan contable"),
                //                             tr("procesada ")+ query.value(0).toString());
             }
           primer_elemento_ini();
          }
}


QTreeWidgetItem *editaplan::buscacadena(QString cadena, bool *encontrada)
{
    QModelIndex indice;
    // QTreeWidgetItem *guarda=ui.Estado1treeWidget->currentItem();
    indice = ui.treeWidget->currentIndex();
    if (indice.isValid())
     {
       while (ui.treeWidget->indexAbove(ui.treeWidget->currentIndex()).isValid())
           ui.treeWidget->setCurrentIndex(ui.treeWidget->indexAbove(ui.treeWidget->currentIndex()));
       while (true)
         {
           QTreeWidgetItem *elemento;
           elemento=ui.treeWidget->currentItem();
           //ui.treeWidget->expandItem(elemento);
           if (cadena==elemento->text(0))
              {
               *encontrada=true;
               return ui.treeWidget->currentItem();
              }
           if (!ui.treeWidget->indexBelow(ui.treeWidget->currentIndex()).isValid()) break;
           ui.treeWidget->setCurrentIndex(ui.treeWidget->indexBelow(ui.treeWidget->currentIndex()));
         }
     }
    return 0;
}

QTreeWidgetItem *editaplan::buscacadena_ini(QString cadena, bool *encontrada)
{
    int pos=0;
    int elementos=lista_codigo.count();
    while (pos<elementos)
      {
       if (lista_codigo.at(pos)==cadena)
          {
           *encontrada=true;
           return lista_puntero_item.at(pos);
          }
       pos++;
      }
    return 0;
}


void editaplan::primer_elemento_ini()
{
    int elementos=lista_codigo.count();
    if (elementos>0)
       {
         ui.treeWidget->setCurrentItem(lista_puntero_item.at(0));
       }
}


void editaplan::insertacuentaarbol(QString cadena1,QString cadena2)
{
  QString cadactual;
  cadactual=cadena1;
  bool seencuentra=false;
  QTreeWidgetItem *elemento;
  // QListViewItem *elemento;
  QStringList datos;
  datos << cadena1 << cadena2;
  for (int veces=1; veces<cadactual.length(); veces++)
      {
        QString extract;
        extract=cadactual.left(cadactual.length()-veces);
        bool encontrada=false;
        elemento=buscacadena(extract,&encontrada);
        if (encontrada)
          {
            if (elemento!=0) {
            QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento,datos);
            // new QTreeWidgetItem(elemento,datos);
            elemento4->setExpanded(true);
            ui.treeWidget->setCurrentItem(elemento4);
            seencuentra=true;
            break;
           }
          }
       }
   if (!seencuentra)
      {
       //QMessageBox::information( this, tr("Edición de plan contable"),
       //                             tr("no se encuentra ")+ datos.at(0));
       QTreeWidgetItem *elemento = new QTreeWidgetItem(ui.treeWidget,datos);
       elemento->setExpanded(true);
       ui.treeWidget->setCurrentItem(elemento);
      }
}

void editaplan::insertacuentaarbol_inicial(QString cadena1,QString cadena2)
{
  QString cadactual;
  cadactual=cadena1;
  bool seencuentra=false;
  QTreeWidgetItem *elemento;
  // QListViewItem *elemento;
  QStringList datos;
  datos << cadena1 << cadena2;
  for (int veces=1; veces<cadactual.length(); veces++)
      {
        QString extract;
        extract=cadactual.left(cadactual.length()-veces);
        bool encontrada=false;
        elemento=buscacadena_ini(extract,&encontrada);
        if (encontrada)
          {
            if (elemento!=0) {
            QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento,datos);
            lista_codigo << cadena1;
            lista_puntero_item << elemento4;
            // new QTreeWidgetItem(elemento,datos);
            elemento4->setExpanded(true);
            seencuentra=true;
            break;
           }
          }
       }
   if (!seencuentra)
      {
       //QMessageBox::information( this, tr("Edición de plan contable"),
       //                             tr("no se encuentra ")+ datos.at(0));
       QTreeWidgetItem *elemento = new QTreeWidgetItem(ui.treeWidget,datos);
       lista_codigo << cadena1;
       lista_puntero_item << elemento;
       elemento->setExpanded(true);
       // ui.treeWidget->setCurrentItem(elemento);
      }
}


void editaplan::codigocambiado(QString codigo)
{

 if (codigo.length()>0) ui.descripcionlineEdit->setEnabled(true);
     else
          {
           ui.descripcionlineEdit->setText("");
           ui.descripcionlineEdit->setEnabled(false);
           return;
          }
 QTreeWidgetItem *elemento;
 elemento=NULL;
 bool encontrada=false;
 // elemento=modeloarbol.buscacadena(modeloarbol.raiz(),codigo,&encontrada);
 elemento=buscacadena(codigo,&encontrada);
 if (encontrada)
     {
      ui.treeWidget->setCurrentItem(elemento);
      ui.descripcionlineEdit->setText(descripcioncuenta(codigo));
      ui.borrarpushButton->setEnabled(true);
     }
     else 
          { 
            ui.borrarpushButton->setEnabled(false);
            ui.descripcionlineEdit->setText("");
          }
}

void editaplan::codigofinedicion()
{

  if (ui.codigolineEdit->text().length()>1)
      if (!subgrupocongrupo(ui.codigolineEdit->text()))
             {
              QMessageBox::warning( this, tr("Edición de plan contable"),
              tr("ERROR: EL CÓDIGO %1 NO TIENE GRUPO DEFINIDO").arg(ui.codigolineEdit->text()) );
              ui.codigolineEdit->setFocus();
              ui.codigolineEdit->setText("");
             }
/*
  if (ui.codigolineEdit->text().length()==3)
      if (!cuentaconsubgrupo(ui.codigolineEdit->text()))
             {
              QMessageBox::warning( this, tr("Edición de plan contable"),
              tr("ERROR: EL CÓDIGO %1 NO TIENE SUBGRUPO DEFINIDO").arg(ui.codigolineEdit->text()) );
              ui.codigolineEdit->setFocus();
              ui.codigolineEdit->setText("");
             }

  if (ui.codigolineEdit->text().length()>3)
      if (!codigoconcuenta3digitos(ui.codigolineEdit->text()))
             {
              QMessageBox::warning( this, tr("Edición de plan contable"),
              tr("ERROR: EL CÓDIGO %1 NO ESTÁ ASIGNADO A CUENTA DE TRES DÍGITOS").arg(ui.codigolineEdit->text()) );
              ui.codigolineEdit->setFocus();
              ui.codigolineEdit->setText("");
             }
 */

  if (ui.codigolineEdit->text().length()>=anchocuentas() && !cod_longitud_variable())
      {
              QMessageBox::warning( this, tr("Edición de plan contable"),
              tr("ERROR: La longitud del código %1 es igual o superior "
                 "al ancho de las subcuentas").arg(ui.codigolineEdit->text()) );
              ui.codigolineEdit->setFocus();
              ui.codigolineEdit->setText("");
      }
}


void editaplan::filaclicked()
{
    if (ui.treeWidget->currentItem()!=0)
       {
        if (!esauxiliar(ui.treeWidget->currentItem()->text(0)))
          {
           ui.codigolineEdit->setText(ui.treeWidget->currentItem()->text(0));
           ui.descripcionlineEdit->setText(ui.treeWidget->currentItem()->text(1));
           ui.aceptarpushButton->setEnabled(false);
          }
        else
            {
             ui.codigolineEdit->clear();
             ui.descripcionlineEdit->clear();
             ui.aceptarpushButton->setEnabled(false);
            }
       }

    /*
 QList<QVariant> datos = modeloarbol.datosfiladeindice(indice);
 if (!esauxiliar(datos.at(0).toString()))
   {
    ui.codigolineEdit->setText(datos.at(0).toString());
    ui.descripcionlineEdit->setText(datos.at(1).toString());
    ui.aceptarpushButton->setEnabled(false);
   }
   else
       {
        ui.codigolineEdit->clear();
        ui.descripcionlineEdit->clear();
        ui.aceptarpushButton->setEnabled(false);
       }
       */
}

void editaplan::descripcambiada()
{
 if (ui.descripcionlineEdit->text().length()==0) ui.aceptarpushButton->setEnabled(false);
   else ui.aceptarpushButton->setEnabled(true);
}

void editaplan::botonaceptar()
{
 QString cadena;
 if (existecodigoplan(ui.codigolineEdit->text(),&cadena))
      {
       guardacambiosplan(ui.codigolineEdit->text(),ui.descripcionlineEdit->text());
       existecodigoplan(ui.codigolineEdit->text(),&cadena);
       bool encontrada;
       QTreeWidgetItem *elemento;
       elemento=buscacadena(ui.codigolineEdit->text(), &encontrada);
       // elemento=modeloarbol.buscacadena(modeloarbol.raiz(),ui.codigolineEdit->text(),&encontrada);
       if (encontrada)
          {
           ui.treeWidget->currentItem()->setText(1,cadena);
           ui.treeWidget->setCurrentItem(elemento);
           // modeloarbol.setData(elemento, cadena, Qt::EditRole);
           // comprobar datos del modelo
           // QVariant dato=elemento->data(1);
           //QMessageBox::information( this, tr("Edición de plan contable"),
           //                  tr("ENCONTRADA")+dato.toString());
          }
      }
  else
      {
       insertaenplan(ui.codigolineEdit->text(),ui.descripcionlineEdit->text());
       if (existecodigoplan(ui.codigolineEdit->text(),&cadena))
           {
            //QTreeWidgetItem *elemento;
            insertacuentaarbol(ui.codigolineEdit->text(),cadena);
            }
      }

 QMessageBox::information( this, tr("Edición de plan contable"),
                             tr("Se han efectuado los cambios en el plan contable"));

 ui.codigolineEdit->setText("");
 ui.descripcionlineEdit->setText("");

}

void editaplan::botonborrar()
{

   if (ui.codigolineEdit->text().length()<=3)
    {
     if (!itemplaneliminado(ui.codigolineEdit->text()))
       {
        QMessageBox::warning( this, tr("Edición de plan contable"),
        tr("No se puede elimimar el código porque éste se subdivide en otros"));
        return;
       }
    }
   else basedatos::instancia()->deletePlancontablecodigo (ui.codigolineEdit->text());

   // elemento=buscacadena(ui.codigolineEdit->text(), &encontrada);
   // elemento=modeloarbol.buscacadena(modeloarbol.raiz(),ui.codigolineEdit->text(),&encontrada);
   /*if (encontrada)
      {
       ui.treeWidget->currentItem()->setText(1,cadena);
       ui.treeWidget->setCurrentItem(elemento);
       //ui.treeWidget->
       // modeloarbol.setData(elemento, cadena, Qt::EditRole);
       // comprobar datos del modelo
       // QVariant dato=elemento->data(1);
       //QMessageBox::information( this, tr("Edición de plan contable"),
       //                  tr("ENCONTRADA")+dato.toString());
      }   */
/*
   ui.treeView->close();
   ui.treeView->setModel(NULL);
   modeloarbol.borraarbol();
   cargaarbol();
   //modeloarbol.resetea();
   ui.treeView->setModel(&modeloarbol);
   ui.treeView->show();
   ui.treeView->expandAll();
   */
   QMessageBox::information( this, tr("Edición de plan contable"),
                             tr("Se ha eliminado el código seleccionado"));
   lista_codigo.clear();
   lista_puntero_item.clear();
   ui.treeWidget->clear();
   ui.codigolineEdit->setText("");
   ui.descripcionlineEdit->setText("");
   cargaarbol();

}
