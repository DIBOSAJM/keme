/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licenvenci_ctasa Pública General GNU publicada
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
#include "venci_ctas.h"
#include "buscasubcuenta.h"
#include "ed_venci_cta.h"

venci_ctas::venci_ctas() : QDialog() {
    ui.setupUi(this);

// Cargamos combo de tipos de vencimiento
QStringList tipos_venci;
tipos_venci=basedatos::instancia()->listatiposdescripvenci();
ui.codigocomboBox->addItems(tipos_venci);

// -------------------------------------------------------------
     QSqlQueryModel *model = new QSqlQueryModel;

     model->setQuery("SELECT venci_cta.codigo as codigo, plancontable.descripcion as descripcion, "
                     " venci_cta.venci as venci FROM "
                     "venci_cta, plancontable where "
                     "venci_cta.codigo=plancontable.codigo order by venci_cta.codigo");
     model->setHeaderData(0, Qt::Horizontal, tr("Código"));
     model->setHeaderData(1, Qt::Horizontal, tr("Descripción"));
     model->setHeaderData(2, Qt::Horizontal, tr("Tipo vencim."));

     modelovenci_ctas=model;

     ui.tableView->setModel(modelovenci_ctas);
     ui.tableView->setColumnWidth(0,100);
     ui.tableView->setColumnWidth(1,400);
     ui.tableView->setColumnWidth(2,100);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );


     ui.tableView->show();

    connect(ui.buscapushButton,SIGNAL(clicked()),SLOT(botoncuentapulsado()));
    connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado(QString)));
    connect(ui.cancelarpushButton,SIGNAL(clicked()),SLOT(cancela()));
    connect(ui.guardarpushButton,SIGNAL(clicked()),this,SLOT(guardar()));
    connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(filaclicked()));
    connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botonborrar()));
    connect(ui.editarpushButton,SIGNAL(clicked()),this,SLOT(editar()));
    connect(ui.tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(filaclicked()));
}



venci_ctas::~venci_ctas()
{
  delete ui.tableView;
  delete modelovenci_ctas;
}


void venci_ctas::codigocambiado(QString codigo)
{
 // simplemente comprobamos si existe la cuenta
 // y mostramos su descripción
 if (!esauxiliar(codigo))
    {
     // mostramos descripción
     ui.descripcionlineEdit->setText(descripcioncuenta(codigo));
     if (basedatos::instancia()->existe_venci_cta(ui.codigolineEdit->text()))
        {
         QString tipo=basedatos::instancia()->tipo_venci_asignado_cta(ui.codigolineEdit->text());
         int indice=0;
         for (int veces=0; veces< ui.codigocomboBox->count(); veces++)
            {
             if (ui.codigocomboBox->itemText(veces).section("//",0,0).trimmed() == tipo)
             { indice=veces; break; }
            }
         ui.codigocomboBox->setCurrentIndex(indice);
        }
        else ui.codigocomboBox->setCurrentIndex(0);
    }
    else ui.descripcionlineEdit->clear();
}

void venci_ctas::cancela()
{
  ui.codigolineEdit->clear();
  ui.codigolineEdit->setFocus();
}

void venci_ctas::filaclicked()
{

 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
     {
      ui.codigolineEdit->setText(modelovenci_ctas->record(indiceactual.row()).value("codigo").toString());
      ui.descripcionlineEdit->setText(modelovenci_ctas->record(indiceactual.row()).value("descripcion").toString());
      QString venci=modelovenci_ctas->record(indiceactual.row()).value("venci").toString();
      int indice=0;
      for (int veces=0; veces< ui.codigocomboBox->count(); veces++)
         {
          if (ui.codigocomboBox->itemText(veces).section("//",0,0).trimmed() == venci)
          { indice=veces; break; }
         }
      ui.codigocomboBox->setCurrentIndex(indice);

     }
}



void venci_ctas::botonborrar()
{
  // borrar asignación de vencimiento
  if (ui.codigolineEdit->text().length()>0)
     basedatos::instancia()->borra_venci_cta(ui.codigolineEdit->text());
  recargartabla();

}


void venci_ctas::guardar()
{

  // añade cuenta auxiliar si no existe
  // en primer lugar hay que comprobar si existe la cuenta
    QString descrip;
    QString codvenci=ui.codigocomboBox->currentText().section("//",0,0).trimmed();
  if (existecodigoplan(ui.codigolineEdit->text(),&descrip))
    {
      if (!basedatos::instancia()->existe_venci_cta(ui.codigolineEdit->text()))
         {
          // implementar añadir subcuenta
          // ojo con contenido del combo          
          basedatos::instancia()->insert_venci_cta(ui.codigolineEdit->text(),
                                                   codvenci);
         }
         else
             {
               // editar
              basedatos::instancia()->update_venci_cta(ui.codigolineEdit->text(),
                                                      codvenci);
             }
      editar();
      recargartabla();
    }

}

void venci_ctas::recargartabla()
{
    modelovenci_ctas->clear();
    QSqlQuery query("SELECT venci_cta.codigo as codigo, plancontable.descripcion as descripcion, "
                    " venci_cta.venci FROM "
                    "venci_cta, plancontable where "
                    "venci_cta.codigo=plancontable.codigo order by venci_cta.codigo");
    modelovenci_ctas->setQuery(query);
    modelovenci_ctas->setHeaderData(0, Qt::Horizontal, tr("Código"));
    modelovenci_ctas->setHeaderData(1, Qt::Horizontal, tr("Descripción"));
    modelovenci_ctas->setHeaderData(2, Qt::Horizontal, tr("Tipo vencim."));

    ui.tableView->setModel(modelovenci_ctas);
    ui.tableView->setColumnWidth(0,100);
    ui.tableView->setColumnWidth(1,400);
    ui.tableView->setColumnWidth(2,100);

    ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

    ui.tableView->show();
}


void venci_ctas::botoncuentapulsado()
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


void venci_ctas::editar()
{
  ed_venci_cta *e = new ed_venci_cta();
  e->pasa_cta_descrip(ui.codigolineEdit->text(), ui.descripcionlineEdit->text());
  e->exec();
  delete e;
  recargartabla();
}
