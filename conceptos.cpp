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

#include "funciones.h"
#include "conceptos.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QMessageBox>

conceptos::conceptos() : QDialog() {
    ui.setupUi(this);

    ui.CodigoLineEdit->setFocus();
     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("conceptos");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modeloconceptos=modelo;
     ui.tabla->setModel(modelo);
     ui.tabla->setColumnWidth(0,60);
     ui.tabla->setColumnWidth(1,350);
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->show();

     connect(ui.CodigoLineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
     connect(ui.CodigoLineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.DescripcionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.BotonGuardar,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
     connect(ui.BotonCancelar,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
     connect(ui.BotonEliminar,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
     connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));

}
 
conceptos::~conceptos()
{
  delete ui.tabla;
  delete modeloconceptos;
}


void conceptos::compruebacodigo()
{
    QString caddescrip;

 if (ui.CodigoLineEdit->text().contains(' '))
    {
       QMessageBox::warning( this, tr("EDICIÓN DE CONCEPTOS"),
      tr("En la composición del código no pueden haber espacios"));
      ui.CodigoLineEdit->clear();
      return;
    }

 if (ui.CodigoLineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("EDICIÓN DE CONCEPTOS"),
      tr("El código no puede estar vacío"));
      ui.CodigoLineEdit->clear();
      return;
    }


 if (!existecodigoconcepto(ui.CodigoLineEdit->text(),&caddescrip))
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


void conceptos::descripcioncambiada()
{
    if (ui.CodigoLineEdit->text().length() == 0) {
      ui.DescripcionLineEdit->setEnabled(false);
    }
    else {
      ui.DescripcionLineEdit->setEnabled(true);
    }

    if ( (ui.DescripcionLineEdit->text().length() != 0) && (ui.CodigoLineEdit->text().length() != 0) ) {
      ui.BotonGuardar->setEnabled(true);
    } else {
      ui.BotonGuardar->setEnabled(false);
    }
}


void conceptos::botonguardarpulsado()
{
    QString caddescrip;
  
      if  (existecodigoconcepto(ui.CodigoLineEdit->text(),&caddescrip))
	guardacambiosconcepto(ui.CodigoLineEdit->text(),ui.DescripcionLineEdit->text());
      else 
	insertaenconceptos(ui.CodigoLineEdit->text(),ui.DescripcionLineEdit->text());       
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
     modeloconceptos->setTable("conceptos");
     modeloconceptos->setSort(0,Qt::AscendingOrder);
     modeloconceptos->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modeloconceptos->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modeloconceptos->select();
}


void conceptos::botoncancelarpulsado()
{
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}


void conceptos::botoneliminarpulsado()
{
     eliminaconcepto(ui.CodigoLineEdit->text());
     modeloconceptos->select();
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}



void conceptos::pasacodigo( QString qconcepto )
{
 ui.CodigoLineEdit->setText(qconcepto);
}

void conceptos::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        ui.CodigoLineEdit->setText(modeloconceptos->record(indiceactual.row()).value("clave").toString());
        ui.DescripcionLineEdit->setText(modeloconceptos->record(indiceactual.row()).value("descripcion").toString());
        ui.BotonCancelar->setEnabled(true);
        ui.BotonEliminar->setEnabled(true);
        ui.DescripcionLineEdit->setEnabled(true);
        ui.DescripcionLineEdit->setFocus();
        ui.CodigoLineEdit->setReadOnly(true);
       }
}
