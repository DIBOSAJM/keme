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

#include "listaseries.h"
#include "basedatos.h"
#include "serie.h"
#include <QMessageBox>

listaseries::listaseries() : QDialog() {
    ui.setupUi(this);

    ui.codigolineEdit->setFocus();
     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("series_fact");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
     modelo->setHeaderData(2, Qt::Horizontal, tr("PROX.NÚM."));
     modeloreg=modelo;
     ui.tabla->setModel(modelo);
     ui.tabla->setColumnWidth(0,60);
     ui.tabla->setColumnWidth(1,350);
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->hideColumn(2);
     ui.tabla->show();

     connect(ui.codigolineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
     connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado()));
     connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.guardarpushButton,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
     connect(ui.cancelarpushButton,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
     connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
     connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));
     connect(ui.editarpushButton,SIGNAL(clicked()),this,SLOT(edserie()));
 }

 
listaseries::~listaseries()
{
  delete ui.tabla;
  delete modeloreg;
}


void listaseries::compruebacodigo()
{
    QString caddescrip;    

 if (ui.codigolineEdit->text().contains(' ')
     || ui.codigolineEdit->text().contains('/')
     || ui.codigolineEdit->text().contains('\\')
     || ui.codigolineEdit->text().contains(':')
     || ui.codigolineEdit->text().contains('*')
     || ui.codigolineEdit->text().contains('?')
     || ui.codigolineEdit->text().contains('|')
     || ui.codigolineEdit->text().contains('<')
     || ui.codigolineEdit->text().contains('>')
     )
    {
      QMessageBox::warning( this, tr("SERIES DE FACTURACIÓN"),
      tr("Carácter no permitido en el código"));
      ui.codigolineEdit->clear();
      return;
    }

 if (ui.codigolineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("SERIES DE FACTURACIÓN"),
      tr("El código no puede estar vacío"));
      ui.codigolineEdit->clear();
      return;
    }


 if (!basedatos::instancia()->existecodigoserie(ui.codigolineEdit->text(),&caddescrip))
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


void listaseries::descripcioncambiada()
{
    if(ui.descriplineEdit->hasFocus()) ui.guardarpushButton->setEnabled(true);
}

void listaseries::codigocambiado()
{
    if (ui.codigolineEdit->text().contains(' ')
        || ui.codigolineEdit->text().contains('/')
        || ui.codigolineEdit->text().contains('\\')
        || ui.codigolineEdit->text().contains(':')
        || ui.codigolineEdit->text().contains('*')
        || ui.codigolineEdit->text().contains('?')
        || ui.codigolineEdit->text().contains('|')
        || ui.codigolineEdit->text().contains('<')
        || ui.codigolineEdit->text().contains('>')
        )
        {
         QString contenido=ui.codigolineEdit->text();
         contenido.remove(' ');
         contenido.remove('/');
         contenido.remove('\\');
         contenido.remove(':');
         contenido.remove('*');
         contenido.remove('?');
         contenido.remove('|');
         contenido.remove('<');
         contenido.remove('>');
         ui.codigolineEdit->setText(contenido);
        }


    if (ui.codigolineEdit->text().length() == 0) {
      ui.descriplineEdit->setEnabled(false);
    }
    else {
      ui.descriplineEdit->setEnabled(true);
    }
}

void listaseries::botonguardarpulsado()
{
    QString caddescrip;
  
      if  (basedatos::instancia()->existecodigoserie(ui.codigolineEdit->text(),&caddescrip))
        basedatos::instancia()->guardacambiosserie(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
      else
       {
         basedatos::instancia()->insertserie(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
         QString codigo=ui.codigolineEdit->text();
         QString qdescrip;
         qlonglong num=0;

         if (basedatos::instancia()->existecodigoserienum (codigo, &qdescrip, &num))
           {
            serie *s = new serie();
            s->pasadatos(codigo, qdescrip, num);
            s->exec();
            delete(s);
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
     modeloreg->setTable("series_fact");
     modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
     modeloreg->setHeaderData(2, Qt::Horizontal, tr("PRÓX.NÚM."));
     modeloreg->setSort(0,Qt::AscendingOrder);
     modeloreg->select();
}


void listaseries::botoncancelarpulsado()
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

void listaseries::botoneliminarpulsado()
{
   if (QMessageBox::question(
            this,
            tr("SERIES"),
            tr("¿ Desea borrar el elemento seleccionado ?")) ==QMessageBox::No) return;
     basedatos::instancia()->eliminaserie(ui.codigolineEdit->text());
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

void listaseries::tablapulsada()
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


void listaseries::edserie()
{
  QString codigo=ui.codigolineEdit->text();
  QString qdescrip;
  qlonglong num;

  basedatos::instancia()->existecodigoserienum (codigo, &qdescrip, &num);

    serie *s = new serie();
    s->pasadatos(codigo, qdescrip, num);
    s->exec();
    delete(s);

    modeloreg->setTable("series_fact");
    modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
    modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
    modeloreg->setHeaderData(2, Qt::Horizontal, tr("PRÓX.NÚM."));
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
