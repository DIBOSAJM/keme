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

#include "funciones.h"
#include "dif_conc.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QMessageBox>

dif_conc::dif_conc() : QDialog() {
    ui.setupUi(this);

    ui.codigolineEdit->setFocus();
     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("dif_conciliacion");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modelodif_conc=modelo;
     ui.tabla->setModel(modelo);
     ui.tabla->setColumnWidth(0,90);
     ui.tabla->setColumnWidth(1,320);
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->show();

     connect(ui.codigolineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
     connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.descripcionlineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.BotonGuardar,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
     connect(ui.BotonCancelar,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
     connect(ui.BotonEliminar,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
     connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));

}
 
dif_conc::~dif_conc()
{
  delete ui.tabla;
  delete modelodif_conc;
}


void dif_conc::compruebacodigo()
{
    QString caddescrip;

 if (ui.codigolineEdit->text().contains(' '))
    {
       QMessageBox::warning( this, tr("EDICIÓN DE dif_conc"),
      tr("En la composición del código no pueden haber espacios"));
      ui.codigolineEdit->clear();
      return;
    }

 if (ui.codigolineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("EDICIÓN DE dif_conc"),
      tr("El código no puede estar vacío"));
      ui.codigolineEdit->clear();
      return;
    }


 if (!existecodigodif_conc(ui.codigolineEdit->text()))
    {
       ui.BotonCancelar->setEnabled(true);
       ui.descripcionlineEdit->setEnabled(true);
       ui.descripcionlineEdit->setFocus();
       ui.codigolineEdit->setReadOnly(true);
     }
     else // codigo correcto
            {
              ui.descripcionlineEdit->setText(caddescrip);
          ui.BotonCancelar->setEnabled(true);
          ui.BotonEliminar->setEnabled(true);
              ui.descripcionlineEdit->setEnabled(true);
	      ui.descripcionlineEdit->setFocus();
          ui.codigolineEdit->setReadOnly(true);
    // Guardarboton->setEnabled(true);
            }
}


void dif_conc::descripcioncambiada()
{
    if (ui.codigolineEdit->text().length() == 0) {
      ui.descripcionlineEdit->setEnabled(false);
    }
    else {
      ui.descripcionlineEdit->setEnabled(true);
    }

    if ( (ui.descripcionlineEdit->text().length() != 0) && (ui.codigolineEdit->text().length() != 0) ) {
      ui.BotonGuardar->setEnabled(true);
    } else {
      ui.BotonGuardar->setEnabled(false);
    }
}


void dif_conc::botonguardarpulsado()
{
    QString caddescrip;
  
      if  (existecodigodif_conc(ui.codigolineEdit->text()))
	guardacambiosdif_conc(ui.codigolineEdit->text(),ui.descripcionlineEdit->text());
      else 
	insertaendif_conc(ui.codigolineEdit->text(),ui.descripcionlineEdit->text());
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descripcionlineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
     modelodif_conc->clear();
     modelodif_conc->setTable("dif_conciliacion");
     modelodif_conc->select();
}


void dif_conc::botoncancelarpulsado()
{
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descripcionlineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}


void dif_conc::botoneliminarpulsado()
{
     if (conceptodif_usado(ui.codigolineEdit->text()))
        {
         QMessageBox::warning( this, tr("BORRAR REGISTRO"),
          tr("ERROR: EL CÓDIGO A BORRAR ESTÁ SIENDO UTILIZADO"));
         return;
        }
     eliminaconceptodif(ui.codigolineEdit->text());
     modelodif_conc->clear();
     modelodif_conc->setTable("dif_conciliacion");
     modelodif_conc->select();
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descripcionlineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}



void dif_conc::pasacodigo( QString qconcepto )
{
 ui.codigolineEdit->setText(qconcepto);
}

void dif_conc::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        ui.codigolineEdit->setText(modelodif_conc->record(indiceactual.row()).value("codigo").toString());
        ui.descripcionlineEdit->setText(modelodif_conc->record(indiceactual.row()).value("descripcion").toString());
        ui.BotonCancelar->setEnabled(true);
        ui.BotonEliminar->setEnabled(true);
        ui.descripcionlineEdit->setEnabled(true);
        ui.descripcionlineEdit->setFocus();
        ui.codigolineEdit->setReadOnly(true);
       }
}
