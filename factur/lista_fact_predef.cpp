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


#include "lista_fact_predef.h"
#include "basedatos.h"
#include "edita_doc.h"
#include "factura.h"
#include <QMessageBox>

lista_fact_predef::lista_fact_predef() : QDialog() {
    ui.setupUi(this);


        ui.codigolineEdit->setFocus();
         QSqlTableModel *modelo = new QSqlTableModel;
         modelo->setTable("facturas_predef");
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

lista_fact_predef::~lista_fact_predef()
{
    delete ui.tabla;
    delete modeloreg;
}

void lista_fact_predef::compruebacodigo()
{
    QString caddescrip;

 if (ui.codigolineEdit->text().contains(' '))
    {
      QMessageBox::warning( this, tr("FACTURAS PREDEFINIDAS"),
      tr("En la composición del código no pueden haber espacios"));
      ui.codigolineEdit->clear();
      return;
    }

 if (ui.codigolineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("FACTURAS PREDEFINIDAS"),
      tr("El código no puede estar vacío"));
      ui.codigolineEdit->clear();
      return;
    }


 if (!basedatos::instancia()->existecodigo_fact_predef(ui.codigolineEdit->text(),&caddescrip))
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


void lista_fact_predef::descripcioncambiada()
{
    if(ui.descriplineEdit->hasFocus()) ui.guardarpushButton->setEnabled(true);
}

void lista_fact_predef::codigocambiado()
{
    if (ui.codigolineEdit->text().length() == 0) {
      ui.descriplineEdit->setEnabled(false);
    }
    else {
      ui.descriplineEdit->setEnabled(true);
    }
}

void lista_fact_predef::botonguardarpulsado()
{
    QString caddescrip;

      if  (basedatos::instancia()->existecodigo_fact_predef(ui.codigolineEdit->text(),&caddescrip))
        basedatos::instancia()->guardacambios_facturas_predef(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
      else
       {
        basedatos::instancia()->insertfacturas_predef(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
        if  (basedatos::instancia()->existecodigo_fact_predef(ui.codigolineEdit->text(),&caddescrip))
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
     modeloreg->setTable("facturas_predef");
     modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
     modeloreg->setSort(0,Qt::AscendingOrder);
     modeloreg->select();
}


void lista_fact_predef::botoncancelarpulsado()
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

void lista_fact_predef::botoneliminarpulsado()
{
   if (QMessageBox::question(
            this,
            tr("FACTURAS PREDEFINIDAS"),
            tr("¿ Desea borrar el elemento seleccionado ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==1 ) return;
     basedatos::instancia()->eliminafra_predefinida(ui.codigolineEdit->text());
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

void lista_fact_predef::tablapulsada()
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


void lista_fact_predef::eddoc()
{
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
        QString tipo_doc, notas, pie1, pie2, concepto_sii;
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
        concepto_sii=q.value(10).toString();
        f->pasa_fact_predef(codigo,descrip,
                            serie,
                            contabilizable,
                            con_ret,
                            con_re,
                            tipo_ret,
                            tipo_doc,
                            notas,
                            pie1,
                            pie2,
                            concepto_sii);
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
}

void lista_fact_predef::refrescar()
{
  //
    modeloreg->setTable("facturas_predef");
    modeloreg->setSort(0,Qt::AscendingOrder);
    modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
    modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
    modeloreg->select();

}
