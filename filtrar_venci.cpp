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

#include "filtrar_venci.h"
#include "buscasubcuenta.h"
#include "basedatos.h"
#include <QCheckBox>

filtrar_venci::filtrar_venci(QString cta_banco) : QDialog() {
    ui.setupUi(this);

QString ejercicio=ejerciciodelafecha(QDate::currentDate());
if (ejercicio.length()==0) ejercicio = ejercicio_igual_o_maxigual(QDate::currentDate());
if (ejercicio.length()>0)
    {
     ui.venci_desdedateEdit->setDate(inicioejercicio(ejercicio));
     ui.venci_hastadateEdit->setDate(finejercicio(ejercicio));
     ui.op_desdedateEdit->setDate(inicioejercicio(ejercicio));
     ui.op_hastadateEdit->setDate(finejercicio(ejercicio));
    }
    else
         {
          ui.venci_desdedateEdit->setDate(QDate::currentDate());
          ui.venci_hastadateEdit->setDate(QDate::currentDate());
          ui.op_desdedateEdit->setDate(QDate::currentDate());
          ui.op_hastadateEdit->setDate(QDate::currentDate());
         }


ui.aux_bancolineEdit->setText(cta_banco);

comadecimal=haycomadecimal();
decimales=haydecimales();

ui.tableWidget->setColumnWidth(0,35);
ui.tableWidget->setColumnWidth(1,80);

ui.tableWidget->setColumnWidth(2,120);
ui.tableWidget->setColumnWidth(5,90);


ui.tableWidget->setColumnWidth(7,150);

connect(ui.codigopushButton,SIGNAL(clicked()),this,SLOT(botoncodigo_pulsado()));
connect(ui.borracodigopushButton,SIGNAL(clicked()),this,SLOT(boton_borra_codigo_cta_banco()));
connect(ui.refrescarpushButton,SIGNAL(clicked()),this,SLOT(refrescar()));

connect(ui.todospushButton,SIGNAL(clicked()),this, SLOT(selectodo()));
connect(ui.ningunopushButton,SIGNAL(clicked()),this,SLOT(selecnada()));

connect(ui.aux_bancolineEdit,SIGNAL(editingFinished()),this,SLOT(auxbanco_finedic()));

connect(ui.imp_desdelineEdit,SIGNAL(editingFinished()),this,SLOT(imp_desde_finedic()));
connect(ui.imp_hastalineEdit,SIGNAL(editingFinished()),this,SLOT(imp_hasta_finedic()));
connect(ui.cta_desdelineEdit,SIGNAL(editingFinished()),this,SLOT(cta_desde_finedic()));
connect(ui.cta_hastalineEdit,SIGNAL(editingFinished()),this,SLOT(cta_hasta_finedic()));

connect(ui.ordenarpushButton,SIGNAL(clicked()),this,SLOT(orden_col()));

}


void filtrar_venci::botoncodigo_pulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.aux_bancolineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.aux_bancolineEdit->setText(cadena2);
       else ui.aux_bancolineEdit->clear();
    delete(labusqueda);

}


void filtrar_venci::boton_borra_codigo_cta_banco()
{
  ui.aux_bancolineEdit->clear();
}


void filtrar_venci::refrescar()
{
    refrescar_ordencol(0);
}

void filtrar_venci::orden_col()
{
  int columna=0;

  if (ui.tableWidget->currentIndex().isValid())
    {
     columna=ui.tableWidget->currentColumn();
     if (columna>0) columna--;
    }
  refrescar_ordencol(columna);

}


void filtrar_venci::refrescar_ordencol(int col)
{
   // primero borramos contenido antiguo
    ui.tableWidget->clearContents();
    ui.tableWidget->setRowCount(0);

    QSqlQuery q=basedatos::instancia()->select_venci_filtrados(
                                   convapunto(ui.imp_desdelineEdit->text()),
                                   convapunto(ui.imp_hastalineEdit->text()),
                                   ui.cta_desdelineEdit->text(),
                                   ui.cta_hastalineEdit->text(),
                                   ui.vencigroupBox->isChecked(),
                                   ui.venci_desdedateEdit->date(),
                                   ui.venci_hastadateEdit->date(),
                                   ui.fecha_opgroupBox->isChecked(),
                                   ui.op_desdedateEdit->date(),
                                   ui.op_hastadateEdit->date(),
                                   ui.aux_bancolineEdit->text(),
                                   col, pagos);
    int fila=0;
    if (q.isActive())
        while (q.next())
          {
           if (q.value(3).toDouble()<0) continue;
           ui.tableWidget->insertRow(fila);

           QCheckBox *checkitem = new QCheckBox();
           ui.tableWidget->setCellWidget ( fila, 0, checkitem );

           QTableWidgetItem *newItem = new QTableWidgetItem(q.value(0).toString());
           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.tableWidget->setItem(fila,1,newItem);
           ui.tableWidget->setItem(fila,2,new QTableWidgetItem(q.value(1).toString()));
           ui.tableWidget->setItem(fila,3,
                                   new QTableWidgetItem(q.value(2).toDate().toString("dd-MM-yyyy")));
           QString importe; importe=formateanumerosep(q.value(3).toDouble(),comadecimal,decimales);
           QTableWidgetItem *newItem4 = new QTableWidgetItem(importe);
           newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.tableWidget->setItem(fila,4,newItem4);
           ui.tableWidget->setItem(fila,5,
                                   new QTableWidgetItem(q.value(4).toDate().toString("dd-MM-yyyy")));
           ui.tableWidget->setItem(fila,6,new QTableWidgetItem(q.value(5).toString()));
           ui.tableWidget->setItem(fila,7,new QTableWidgetItem(q.value(6).toString()));
           ui.tableWidget->setItem(fila,8,new QTableWidgetItem(q.value(7).toString()));
           QString razon_externo;
           if (!q.value(8).toString().isEmpty()) razon_externo=basedatos::instancia()->razon_externo(q.value(8).toString());
           ui.tableWidget->setItem(fila,9,new QTableWidgetItem(razon_externo));
           fila++;

          }

}

void filtrar_venci::asigna_id_domiciliacion(QString id)
{
    for (int veces=0; veces<ui.tableWidget->rowCount();veces++)
      {
       QCheckBox *checkitem;
       checkitem=(QCheckBox*)ui.tableWidget->cellWidget(veces,0);
       if (checkitem->isChecked())
         {
          // actualizar tabla vencimientos con domiciliacion = id
           basedatos::instancia()->pasa_domiciliacion_vto(
                   ui.tableWidget->item(veces,1)->text(), id);
         }
      }

}

void filtrar_venci::para_pagos()
{
  pagos=true;
}

void filtrar_venci::selectodo()
{
    for (int veces=0; veces<ui.tableWidget->rowCount();veces++)
      {
       QCheckBox *checkitem;
       checkitem=(QCheckBox*)ui.tableWidget->cellWidget(veces,0);
       checkitem->setChecked(true);
      }

}

void filtrar_venci::selecnada()
{
    for (int veces=0; veces<ui.tableWidget->rowCount();veces++)
      {
       QCheckBox *checkitem;
       checkitem=(QCheckBox*)ui.tableWidget->cellWidget(veces,0);
       checkitem->setChecked(false);
      }

}

void filtrar_venci::auxbanco_finedic()
{
  ui.aux_bancolineEdit->setText(expandepunto(ui.aux_bancolineEdit->text(),anchocuentas()));
}


void filtrar_venci::imp_desde_finedic()
{
   if (comadecimal) ui.imp_desdelineEdit->setText(convacoma(ui.imp_desdelineEdit->text()));
      else ui.imp_desdelineEdit->setText(convapunto(ui.imp_desdelineEdit->text()));

}

void filtrar_venci::imp_hasta_finedic()
{
    if (comadecimal) ui.imp_hastalineEdit->setText(convacoma(ui.imp_hastalineEdit->text()));
       else ui.imp_hastalineEdit->setText(convapunto(ui.imp_hastalineEdit->text()));

}

void filtrar_venci::cta_desde_finedic()
{
    ui.cta_desdelineEdit->setText(expandepunto(ui.cta_desdelineEdit->text(),anchocuentas()));

}

void filtrar_venci::cta_hasta_finedic()
{
    ui.cta_hastalineEdit->setText(expandepunto(ui.cta_hastalineEdit->text(),anchocuentas()));

}
