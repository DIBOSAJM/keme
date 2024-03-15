/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Liceniva_ret_ctasa Pública General GNU publicada
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

#include <QMessageBox>
#include "funciones.h"
#include "basedatos.h"
#include <QSqlRecord>
#include "iva_ret_ctas.h"
#include "buscasubcuenta.h"
#include "ed_iva_ret_cta.h"

iva_ret_ctas::iva_ret_ctas() : QDialog() {
    ui.setupUi(this);

// -------------------------------------------------------------
// Cargamos combo de tipos de vencimiento

// -------------------------------------------------------------
     QSqlQueryModel *model = new QSqlQueryModel;

     model->setQuery("SELECT iva_ret_cta.codigo as codigo, plancontable.descripcion as descripcion "
                     " FROM "
                     "iva_ret_cta, plancontable where "
                     "iva_ret_cta.codigo=plancontable.codigo order by iva_ret_cta.codigo");
     model->setHeaderData(0, Qt::Horizontal, tr("Código"));
     model->setHeaderData(1, Qt::Horizontal, tr("Descripción"));

     modeloiva_ret_ctas=model;

     ui.tableView->setModel(modeloiva_ret_ctas);
     ui.tableView->setColumnWidth(0,100);
     ui.tableView->setColumnWidth(1,400);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );


     ui.tableView->show();

    connect(ui.buscapushButton,SIGNAL(clicked()),SLOT(botoncuentapulsado()));
    connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado(QString)));
    connect(ui.cancelarpushButton,SIGNAL(clicked()),SLOT(cancela()));
    connect(ui.guardarpushButton,SIGNAL(clicked()),this,SLOT(guardar()));
    connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(filaclicked()));
    connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botonborrar()));
    connect(ui.editarpushButton,SIGNAL(clicked()),this,SLOT(guardar()));
    connect(ui.tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(filaclicked()));
}



iva_ret_ctas::~iva_ret_ctas()
{
  delete ui.tableView;
  delete modeloiva_ret_ctas;
}


void iva_ret_ctas::codigocambiado(QString codigo)
{
 // simplemente comprobamos si existe la cuenta
 // y mostramos su descripción
 if (!esauxiliar(codigo))
     ui.descripcionlineEdit->setText(descripcioncuenta(codigo));
  else ui.descripcionlineEdit->clear();
}

void iva_ret_ctas::cancela()
{
  ui.codigolineEdit->clear();
  ui.codigolineEdit->setFocus();
}

void iva_ret_ctas::filaclicked()
{

 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
     {
      ui.codigolineEdit->setText(modeloiva_ret_ctas->record(indiceactual.row()).value("codigo").toString());
      ui.descripcionlineEdit->setText(modeloiva_ret_ctas->record(indiceactual.row()).value("descripcion").toString());
     }
}



void iva_ret_ctas::botonborrar()
{
  // borrar asignación de vencimiento
  if (ui.codigolineEdit->text().length()>0)
     basedatos::instancia()->borra_iva_ret_cta(ui.codigolineEdit->text());
  recargartabla();

}


void iva_ret_ctas::guardar()
{

  // añade cuenta auxiliar si no existe
  // en primer lugar hay que comprobar si existe la cuenta
    QString descrip;
  if (existecodigoplan(ui.codigolineEdit->text(),&descrip))
    {
      if (!basedatos::instancia()->existe_iva_ret_cta(ui.codigolineEdit->text()))
         {
          // implementar añadir subcuenta
          // ojo con contenido del combo          
          basedatos::instancia()->insert_iva_ret_cta(ui.codigolineEdit->text());
         }
      editar();
      recargartabla();
    }

}

void iva_ret_ctas::recargartabla()
{
    modeloiva_ret_ctas->clear();

    modeloiva_ret_ctas->setQuery("SELECT iva_ret_cta.codigo as codigo, plancontable.descripcion as descripcion "
                    " FROM "
                    "iva_ret_cta, plancontable where "
                    "iva_ret_cta.codigo=plancontable.codigo order by iva_ret_cta.codigo");
    modeloiva_ret_ctas->setHeaderData(0, Qt::Horizontal, tr("Código"));
    modeloiva_ret_ctas->setHeaderData(1, Qt::Horizontal, tr("Descripción"));

    ui.tableView->setModel(modeloiva_ret_ctas);
    ui.tableView->setColumnWidth(0,100);
    ui.tableView->setColumnWidth(1,400);

    ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

    ui.tableView->show();
}


void iva_ret_ctas::botoncuentapulsado()
{
    QString cadena2;
    buscacuenta *labusqueda=new buscacuenta(ui.codigolineEdit->text());
    labusqueda->cambiatitulo(tr("Búsqueda de cuentas"));
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    QString descrip;
    if (cod==QDialog::Accepted && existecodigoplan(cadena2,&descrip))
      ui.codigolineEdit->setText(cadena2);
       else ui.codigolineEdit->clear();
    delete labusqueda;
}


void iva_ret_ctas::editar()
{
  ed_iva_ret_cta *e = new ed_iva_ret_cta();
  e->pasa_cta_descrip(ui.codigolineEdit->text(), ui.descripcionlineEdit->text());
  e->exec();
  delete e;
  recargartabla();
}
