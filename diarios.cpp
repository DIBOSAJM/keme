/* ----------------------------------------------------------------------------------
    KEME-Contabilidad, aplicación para llevar contabilidades

    Copyright (C) José Manuel Díez Botella

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

#include "funciones.h"
#include "diarios.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QMessageBox>

diarios::diarios() : QDialog() {
    ui.setupUi(this);

    ui.CodigoLineEdit->setFocus();
     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("diarios");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modelodiarios=modelo;
     ui.tabla->setModel(modelo);
     ui.tabla->setColumnWidth(0,100);
     ui.tabla->setColumnWidth(1,310);
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->show();

     connect(ui.CodigoLineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
     connect(ui.DescripcionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.BotonGuardar,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
     connect(ui.BotonCancelar,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
     connect(ui.BotonEliminar,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
     connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));
     ui.DescripcionLineEdit->setEnabled(false);

}
 
diarios::~diarios()
{
  delete ui.tabla;
  delete modelodiarios;
}


void diarios::compruebacodigo()
{
    QString caddescrip;

 if (ui.CodigoLineEdit->text().contains(' '))
    {
       QMessageBox::warning( this, tr("EDICIÓN DE DIARIOS"),
      tr("En la composición del código no pueden haber espacios"));
      ui.CodigoLineEdit->clear();
      return;
    }

 if (ui.CodigoLineEdit->text()==diario_apertura() || ui.CodigoLineEdit->text()==diario_no_asignado() ||
     ui.CodigoLineEdit->text()==diario_regularizacion() || ui.CodigoLineEdit->text()==diario_cierre())
    {
       QMessageBox::warning( this, tr("EDICIÓN DE DIARIOS"),
      tr("ERROR: código de diario reservado"));
      ui.CodigoLineEdit->clear();
      return;
    }

 if (ui.CodigoLineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("EDICIÓN DE DIARIOS"),
      tr("El código no puede estar vacío"));
      ui.CodigoLineEdit->clear();
      return;
    }


 if (!existecodigodiario(ui.CodigoLineEdit->text(),&caddescrip))
    {
       ui.BotonCancelar->setEnabled(true);
       ui.DescripcionLineEdit->setEnabled(true);
       ui.DescripcionLineEdit->setFocus();
       ui.CodigoLineEdit->setReadOnly(true);
     }
     else // codigo correcto
            {
              ui.DescripcionLineEdit->setText(caddescrip);
          ui.BotonCancelar->setEnabled(true);
          ui.BotonEliminar->setEnabled(true);
              ui.DescripcionLineEdit->setEnabled(true);
	      ui.DescripcionLineEdit->setFocus();
          ui.CodigoLineEdit->setReadOnly(true);
    // Guardarboton->setEnabled(true);
            }
}


void diarios::descripcioncambiada()
{
    if(ui.DescripcionLineEdit->hasFocus())  ui.BotonGuardar->setEnabled(true);
}


void diarios::botonguardarpulsado()
{
    QString caddescrip;
  
      if  (existecodigodiario(ui.CodigoLineEdit->text(),&caddescrip))
	guardacambiosdiario(ui.CodigoLineEdit->text(),ui.DescripcionLineEdit->text());
      else 
	insertaendiarios(ui.CodigoLineEdit->text(),ui.DescripcionLineEdit->text());
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.DescripcionLineEdit->setDisabled(true);
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
     modelodiarios->clear();
     modelodiarios->setTable("diarios");
     modelodiarios->setSort(0,Qt::AscendingOrder);
     modelodiarios->select();
}


void diarios::botoncancelarpulsado()
{
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.DescripcionLineEdit->setDisabled(true);
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}


void diarios::botoneliminarpulsado()
{
     if (!eliminadiario(ui.CodigoLineEdit->text()))
       QMessageBox::warning( this, tr("EDICIÓN DE DIARIOS"),
         tr("ERROR: El código existe en el diario"));

     modelodiarios->clear();
     modelodiarios->setTable("diarios");
     modelodiarios->setSort(0,Qt::AscendingOrder);
     modelodiarios->select();
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.DescripcionLineEdit->setDisabled(true);
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}



void diarios::pasacodigo( QString qconcepto )
{
 ui.CodigoLineEdit->setText(qconcepto);
}

void diarios::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        ui.CodigoLineEdit->setText(modelodiarios->record(indiceactual.row()).value("codigo").toString());
        ui.DescripcionLineEdit->setText(modelodiarios->record(indiceactual.row()).value("descripcion").toString());
        ui.BotonCancelar->setEnabled(true);
        ui.BotonEliminar->setEnabled(true);
        ui.DescripcionLineEdit->setEnabled(true);
        ui.DescripcionLineEdit->setFocus();
        ui.CodigoLineEdit->setReadOnly(true);
       }
}
