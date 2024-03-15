/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licenbloqueactaa Pública General GNU publicada
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

#include "bloqueacta.h"
#include <QMessageBox>
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include <QSqlRecord>
#include "ed_bloqueo_cta.h"


bloqueacta::bloqueacta() : QDialog() {
    ui.setupUi(this);

     QSqlQueryModel *model = new QSqlQueryModel;
     model->setQuery("SELECT bloqueos_cta.codigo as codigo, plancontable.descripcion as descripcion FROM "
                     "bloqueos_cta, plancontable where "
                     "bloqueos_cta.codigo=plancontable.codigo order by bloqueos_cta.codigo");
     model->setHeaderData(0, Qt::Horizontal, tr("Código"));
     model->setHeaderData(1, Qt::Horizontal, tr("Descripción"));

     modelobloqueacta=model;

     ui.tableView->setModel(modelobloqueacta);
     ui.tableView->setColumnWidth(0,150);
     ui.tableView->setColumnWidth(1,400);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );


     ui.tableView->show();


     connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado(QString)));
     connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(filaclicked()));
     connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botonborrar()));
     connect(ui.adpushButton,SIGNAL(clicked()),this,SLOT(adicion()));
     connect(ui.codigolineEdit,SIGNAL(editingFinished()),this,SLOT(codigofinedicion()));
     connect(ui.buscapushButton,SIGNAL(clicked()),SLOT(botonsubcuentapulsado()));
     connect(ui.edpushButton,SIGNAL(clicked()),SLOT(editar_bloqueo()));
}




bloqueacta::~bloqueacta()
{
  delete ui.tableView;
  delete modelobloqueacta;
}


void bloqueacta::codigocambiado(QString codigo)
{
 // simplemente comprobamos si existe la cuenta (auxiliar)
 // y mostramos su descripción
 if (esauxiliar(codigo))
    {
     // mostramos descripción
     ui.descripcionlineEdit->setText(descripcioncuenta(codigo));
    }
    else ui.descripcionlineEdit->clear();
}



void bloqueacta::filaclicked()
{

 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
     {
      ui.codigolineEdit->setText(modelobloqueacta->record(indiceactual.row()).value("codigo").toString());
      ui.descripcionlineEdit->setText(modelobloqueacta->record(indiceactual.row()).value("descripcion").toString());
     }
}



void bloqueacta::botonborrar()
{
  // borrar cuenta de bloqueos
  if (ui.codigolineEdit->text().length()>0)
     basedatos::instancia()->borra_bloqueo_cta(ui.codigolineEdit->text());
  recargartabla();

}


void bloqueacta::adicion()
{
  // añade cuenta auxiliar si no existe en bloqueos
  // en primer lugar hay que comprobar si existe la cuenta auxiliar
  if (existesubcuenta(ui.codigolineEdit->text()))
    {
      if (!basedatos::instancia()->existe_bloqueo_cta_gen(ui.codigolineEdit->text()))
         {
          // implementar añadir subcuenta
          basedatos::instancia()->insert_bloqueo_cta(ui.codigolineEdit->text());
          ed_bloqueo_cta *e=new ed_bloqueo_cta();
          e->pasa_cta_descrip(ui.codigolineEdit->text(), ui.descripcionlineEdit->text());
          e->exec();
          delete e;
         }
      recargartabla();
    }
}

void bloqueacta::recargartabla()
{
    modelobloqueacta->clear();
    QSqlQuery query("SELECT bloqueos_cta.codigo as codigo, plancontable.descripcion as descripcion FROM "
                     "bloqueos_cta, plancontable where "
                     "bloqueos_cta.codigo=plancontable.codigo order by bloqueos_cta.codigo");
    modelobloqueacta->setQuery(query);
     modelobloqueacta->setHeaderData(0, Qt::Horizontal, tr("Código"));
     modelobloqueacta->setHeaderData(1, Qt::Horizontal, tr("Descripción"));

     // modelobloqueacta->select();

     ui.tableView->setModel(modelobloqueacta);
     ui.tableView->setColumnWidth(0,100);
     ui.tableView->setColumnWidth(1,300);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tableView->show();

}


void bloqueacta::codigofinedicion()
{
    ui.codigolineEdit->setText(expandepunto(ui.codigolineEdit->text(),anchocuentas()));
}


void bloqueacta::botonsubcuentapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.codigolineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.codigolineEdit->setText(cadena2);
       else ui.codigolineEdit->clear();
    delete labusqueda;
}

void bloqueacta::editar_bloqueo()
{
    QModelIndex indiceactual=ui.tableView->currentIndex();
    if (!indiceactual.isValid()) return;

     ed_bloqueo_cta *e=new ed_bloqueo_cta();
     e->pasa_cta_descrip(modelobloqueacta->record(indiceactual.row()).value("codigo").toString(),
                         modelobloqueacta->record(indiceactual.row()).value("descripcion").toString());
     e->exec();
     delete e;
}
