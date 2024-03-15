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


#include "listarefs.h"
#include "basedatos.h"
#include "edita_ref.h"
#include <QMessageBox>

listarefs::listarefs() : QDialog() {
    ui.setupUi(this);

        ui.codigolineEdit->setFocus();
         QSqlTableModel *modelo = new QSqlTableModel;
         modelo->setTable("referencias");
         modelo->setSort(0,Qt::AscendingOrder);
         modelo->select();
         modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
         modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
         modeloreg=modelo;
         ui.tabla->setModel(modelo);
         ui.tabla->setColumnWidth(0,60);
         ui.tabla->setColumnWidth(1,350);
         ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

         // ui.tabla->hideColumn(2);
         ui.tabla->show();

         connect(ui.codigolineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
         connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado()));
         connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
         connect(ui.guardarpushButton,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
         connect(ui.cancelarpushButton,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
         connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
         connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));
         connect(ui.editarpushButton,SIGNAL(clicked()),this,SLOT(eddoc()));
}

listarefs::~listarefs()
{
    delete ui.tabla;
    delete modeloreg;
}

void listarefs::compruebacodigo()
{
    QString caddescrip;

 if (ui.codigolineEdit->text().contains(' '))
    {
      QMessageBox::warning( this, tr("REFERENCIAS"),
      tr("En la composición del código no pueden haber espacios"));
      ui.codigolineEdit->clear();
      return;
    }

 if (ui.codigolineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("REFERENCIAS"),
      tr("El código no puede estar vacío"));
      ui.codigolineEdit->clear();
      return;
    }


 if (!basedatos::instancia()->existecodigoref(ui.codigolineEdit->text(),&caddescrip))
    {
       ui.cancelarpushButton->setEnabled(true);
       ui.descriplineEdit->setEnabled(true);
       ui.descriplineEdit->setFocus();
       ui.codigolineEdit->setReadOnly(true);
     }
     else // codigo correcto
            {
              disconnect(ui.descriplineEdit);
              ui.descriplineEdit->setText(caddescrip);
              ui.cancelarpushButton->setEnabled(true);
              ui.borrarpushButton->setEnabled(true);
              ui.editarpushButton->setEnabled(true);
              ui.descriplineEdit->setEnabled(true);
              ui.descriplineEdit->setFocus();
              ui.codigolineEdit->setReadOnly(true);
              connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this
                      ,SLOT(descripcioncambiada()));
            }
}


void listarefs::descripcioncambiada()
{
    if(ui.descriplineEdit->hasFocus()) ui.guardarpushButton->setEnabled(true);
}

void listarefs::codigocambiado()
{
    if (ui.codigolineEdit->text().length() == 0) {
      ui.descriplineEdit->setEnabled(false);
    }
    else {
      ui.descriplineEdit->setEnabled(true);
    }
}

void listarefs::botonguardarpulsado()
{
    QString caddescrip;

      if  (basedatos::instancia()->existecodigoref(ui.codigolineEdit->text(),&caddescrip))
        basedatos::instancia()->guardacambiostipos_doc(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
      else
       {
         basedatos::instancia()->insertref(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
         QString codigo=ui.codigolineEdit->text();
         if  (basedatos::instancia()->existecodigoref(codigo,&caddescrip))
          {
            edita_ref *e = new edita_ref();
            e->pasacodigo(codigo);
            e->exec();
            delete(e);
          }
       }
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     ui.editarpushButton->setDisabled(true);
     modeloreg->setTable("referencias");
     modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
     modeloreg->setSort(0,Qt::AscendingOrder);
     modeloreg->select();
}


void listarefs::botoncancelarpulsado()
{
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     ui.editarpushButton->setDisabled(true);
}

void listarefs::botoneliminarpulsado()
{
   if (QMessageBox::question(
            this,
            tr("REFERENCIAS"),
            tr("¿ Desea borrar el elemento seleccionado ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==1 ) return;
     basedatos::instancia()->eliminaref(ui.codigolineEdit->text());
     modeloreg->select();
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     ui.editarpushButton->setDisabled(true);
}

void listarefs::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        ui.codigolineEdit->setText(
           modeloreg->record(indiceactual.row()).value("codigo").toString());
        ui.descriplineEdit->setText(
           modeloreg->record(indiceactual.row()).value("descrip").toString());
        ui.cancelarpushButton->setEnabled(true);
        ui.borrarpushButton->setEnabled(true);
        ui.editarpushButton->setEnabled(true);
        ui.descriplineEdit->setEnabled(true);
        ui.descriplineEdit->setFocus();
        ui.codigolineEdit->setReadOnly(true);
       }
}


void listarefs::eddoc()
{
  QString codigo=ui.codigolineEdit->text();
  QString qdescrip;
  // qlonglong num;


  if  (basedatos::instancia()->existecodigoref(codigo,&qdescrip))
  {      
      edita_ref *e = new edita_ref();
      e->pasacodigo(codigo);
      e->exec();
      delete(e);
  }


    modeloreg->setTable("referencias");
    modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
    modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
    modeloreg->setSort(0,Qt::AscendingOrder);
    modeloreg->select();
    ui.codigolineEdit->clear();
    ui.codigolineEdit->setReadOnly(false);
    ui.codigolineEdit->setFocus();
    ui.descriplineEdit->clear();
    ui.guardarpushButton->setDisabled(true);
    ui.cancelarpushButton->setDisabled(true);
    ui.borrarpushButton->setDisabled(true);
    ui.editarpushButton->setDisabled(true);
}
