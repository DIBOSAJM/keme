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

#include "funciones.h"
#include "edtabla.h"
#include "basedatos.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QMessageBox>

edtabla::edtabla(QString qtabla) : QDialog() {
    ui.setupUi(this);

nombretabla=qtabla;
setWindowTitle(tr("Edición de tabla: ")+nombretabla);
    ui.CodigoLineEdit->setFocus();
     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable(nombretabla);
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modeloedtabla=modelo;
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
 
edtabla::~edtabla()
{
  delete ui.tabla;
  delete modeloedtabla;
}


void edtabla::compruebacodigo()
{
    QString caddescrip;

 if (ui.CodigoLineEdit->text().contains(' '))
    {
       QMessageBox::warning( this, tr("EDICIÓN DE REGISTROS"),
      tr("En la composición del código no pueden haber espacios"));
      ui.CodigoLineEdit->clear();
      return;
    }

 if (ui.CodigoLineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("EDICIÓN DE REGISTROS"),
      tr("El código no puede estar vacío"));
      ui.CodigoLineEdit->clear();
      return;
    }


 if (!basedatos::instancia()->existecodigotabla(nombretabla,ui.CodigoLineEdit->text(),&caddescrip))
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


void edtabla::descripcioncambiada()
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


void edtabla::botonguardarpulsado()
{
    QString caddescrip;
  
      if  (basedatos::instancia()->existecodigotabla(nombretabla,ui.CodigoLineEdit->text(),&caddescrip))
          basedatos::instancia()->updatetabladescripcionclave (nombretabla, ui.CodigoLineEdit->text(),
                                                               ui.DescripcionLineEdit->text());
      else 
        basedatos::instancia()->inserttabla(nombretabla,ui.CodigoLineEdit->text(),ui.DescripcionLineEdit->text());
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
     modeloedtabla->setTable(nombretabla);
     modeloedtabla->setSort(0,Qt::AscendingOrder);
     modeloedtabla->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modeloedtabla->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modeloedtabla->select();
}


void edtabla::botoncancelarpulsado()
{
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}


void edtabla::botoneliminarpulsado()
{
     basedatos::instancia()->deleteclavetabla (nombretabla, ui.CodigoLineEdit->text());
     modeloedtabla->select();
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.DescripcionLineEdit->clear();
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}



void edtabla::pasacodigo( QString qconcepto )
{
 ui.CodigoLineEdit->setText(qconcepto);
}

void edtabla::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        ui.CodigoLineEdit->setText(modeloedtabla->record(indiceactual.row()).value("codigo").toString());
        ui.DescripcionLineEdit->setText(modeloedtabla->record(indiceactual.row()).value("descripcion").toString());
        ui.BotonCancelar->setEnabled(true);
        ui.BotonEliminar->setEnabled(true);
        ui.DescripcionLineEdit->setEnabled(true);
        ui.DescripcionLineEdit->setFocus();
        ui.CodigoLineEdit->setReadOnly(true);
       }
}
