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


#include "lotes_fact.h"
#include "basedatos.h"
#include "lotes.h"
#include "factura.h"
#include <QMessageBox>

lotes_fact::lotes_fact() : QDialog() {
    ui.setupUi(this);


        ui.codigolineEdit->setFocus();
         QSqlTableModel *modelo = new QSqlTableModel;
         modelo->setTable("lote_fact");
         modelo->setSort(0,Qt::AscendingOrder);
         modelo->select();
         modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
         modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
         modeloreg=modelo;
         ui.tabla->setModel(modelo);
         ui.tabla->setColumnWidth(0,60);
         ui.tabla->setColumnWidth(1,350);
         ui.tabla->hideColumn(23);
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

lotes_fact::~lotes_fact()
{
    delete ui.tabla;
    delete modeloreg;
}

void lotes_fact::compruebacodigo()
{
    QString caddescrip;

 if (ui.codigolineEdit->text().contains(' '))
    {
      QMessageBox::warning( this, tr("DISEÑO DE LOTES"),
      tr("En la composición del código no pueden haber espacios"));
      ui.codigolineEdit->clear();
      return;
    }

 if (ui.codigolineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("DISEÑO DE LOTES"),
      tr("El código no puede estar vacío"));
      ui.codigolineEdit->clear();
      return;
    }


 if (!basedatos::instancia()->existecodigo_lote(ui.codigolineEdit->text(),&caddescrip))
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


void lotes_fact::descripcioncambiada()
{
    if(ui.descriplineEdit->hasFocus()) ui.guardarpushButton->setEnabled(true);
}

void lotes_fact::codigocambiado()
{
    if (ui.codigolineEdit->text().length() == 0) {
      ui.descriplineEdit->setEnabled(false);
    }
    else {
      ui.descriplineEdit->setEnabled(true);
    }
}

void lotes_fact::botonguardarpulsado()
{
    QString caddescrip;

      if  (basedatos::instancia()->existecodigo_lote(ui.codigolineEdit->text(),&caddescrip))
        basedatos::instancia()->guardacambios_lote(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
      else
       {
        basedatos::instancia()->insertfacturas_lote(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
        if  (basedatos::instancia()->existecodigo_lote(ui.codigolineEdit->text(),&caddescrip))
          {
            /*
            edita_doc *e = new edita_doc();
            e->pasacodigo(ui.codigolineEdit->text());
            e->exec();
            delete(e);*/
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
     modeloreg->setTable("lote_fact");
     modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
     modeloreg->setSort(0,Qt::AscendingOrder);
     modeloreg->select();
}


void lotes_fact::botoncancelarpulsado()
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

void lotes_fact::botoneliminarpulsado()
{
   if (QMessageBox::question(
            this,
            tr("FACTURAS PREDEFINIDAS"),
            tr("¿ Desea borrar el elemento seleccionado ?")) ==QMessageBox::No ) return;
     basedatos::instancia()->eliminalote_fact (ui.codigolineEdit->text());
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

void lotes_fact::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        ui.codigolineEdit->setText(
           modeloreg->record(indiceactual.row()).value("codigo").toString());
        ui.descriplineEdit->setText(
           modeloreg->record(indiceactual.row()).value("descripcion").toString());
        ui.cancelarpushButton->setEnabled(true);
        ui.borrarpushButton->setEnabled(true);
        ui.editarpushButton->setEnabled(true);
        ui.descriplineEdit->setEnabled(true);
        ui.descriplineEdit->setFocus();
        ui.codigolineEdit->setReadOnly(true);
       }
}


void lotes_fact::eddoc()
{
  QString codigo=ui.codigolineEdit->text();
  QString descrip=ui.descriplineEdit->text();

  lotes *l = new lotes(codigo,descrip);
  l->exec();
  delete (l);
    /*
  QString codigo=ui.codigolineEdit->text();
  // QString qdescrip;
  QSqlQuery q=basedatos::instancia()->datos_cabecera_fact_predef(codigo);
  // "select descrip, serie, contabilizable, con_ret,"
  // "con_re, tipo_ret, tipo_doc, notas, pie1, pie2 "
  // "from facturas_predef
  if (q.isActive())
    {
     if (q.next())
      {
        factura *f = new factura;
        QString descrip, serie;
        bool contabilizable, con_ret, con_re;
        QString tipo_ret;
        QString tipo_doc, notas, pie1, pie2;
        descrip=q.value(0).toString();
        serie=q.value(1).toString();
        contabilizable=q.value(2).toBool();
        con_ret=q.value(3).toBool();
        con_re=q.value(4).toBool();
        tipo_ret=q.value(5).toString();
        tipo_doc=q.value(6).toString();
        notas=q.value(7).toString();
        pie1=q.value(8).toString();
        pie2=q.value(9).toString();
        f->pasa_fact_predef(codigo,descrip,
                            serie,
                            contabilizable,
                            con_ret,
                            con_re,
                            tipo_ret,
                            tipo_doc,
                            notas,
                            pie1,
                            pie2);
        f->exec();
        delete(f);
      }
      else return;
     }
    refrescar();
    ui.codigolineEdit->clear();
    ui.codigolineEdit->setReadOnly(false);
    ui.codigolineEdit->setFocus();
    ui.descriplineEdit->clear();
    ui.guardarpushButton->setDisabled(true);
    ui.cancelarpushButton->setDisabled(true);
    ui.borrarpushButton->setDisabled(true);
    ui.editarpushButton->setDisabled(true);
    */
}

void lotes_fact::refrescar()
{
  //
    modeloreg->setTable("lote_fact");
    modeloreg->setSort(0,Qt::AscendingOrder);
    modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
    modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
    modeloreg->select();

}
