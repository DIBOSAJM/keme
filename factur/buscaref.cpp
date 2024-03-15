/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C) José Manuel Díez Botella

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

#include "buscaref.h"
#include "funciones.h"
#include "basedatos.h"
#include <QSqlDatabase>
#include <QSqlRecord>


buscaref::buscaref(QString codigoini) : QDialog() {
    ui.setupUi(this);

     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("referencias");
     QString cadena;
     if (codigoini.length()>0)
         {
          ui.codigolineEdit->setText(codigoini);
          if ( es_sqlite() )
             {
               if (! cadena.isEmpty()) cadena=" and ";
               cadena+=" codigo like '";
               cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
               cadena+="%'";
             }
          else
              {
              if (! cadena.isEmpty()) cadena=" and ";
               cadena+=" position('";
               cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
               cadena+="' in codigo)=1";
              }
         }
     modelo->setFilter(cadena);
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
     modeloref=modelo;
     ui.tableView->setModel(modelo);
     ui.tableView->setColumnWidth(0,100);
     ui.tableView->setColumnWidth(1,410);
     ui.tableView->hideColumn(2);
     ui.tableView->hideColumn(3);
     ui.tableView->hideColumn(4);
     ui.tableView->hideColumn(5);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tableView->show();

    connect(ui.ComienceRadioButton,SIGNAL(toggled(bool)),this,SLOT(ComienceRadioBotoncambiado()));
    connect(ui.ContengaRadioButton,SIGNAL(toggled(bool)),this,SLOT(ContengaRadioBotoncambiado()));
    connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado()));
    connect(ui.TituloLineEdit,SIGNAL(textChanged(QString)),this,SLOT(titulocuentacambiado()));
    connect(ui.codigolineEdit,SIGNAL(editingFinished ()),this,SLOT(fuerafococodigo()));
    connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablaclickada()));


}


buscaref::~buscaref()
{
  delete ui.tableView;
  delete modeloref;
}


void buscaref::ComienceRadioBotoncambiado()
{
 if (ui.codigolineEdit->text().length()>0) codigocambiado();
 if (ui.TituloLineEdit->text().length()>0) titulocuentacambiado();
}


void buscaref::ContengaRadioBotoncambiado()
{
 if (ui.codigolineEdit->text().length()>0) codigocambiado();
 if (ui.TituloLineEdit->text().length()>0) titulocuentacambiado();
}

void buscaref::recargartabla(QString cadena) {
    modeloref->clear();
    modeloref->setTable("referencias");
    modeloref->setFilter(cadena);//
    modeloref->setSort(0,Qt::AscendingOrder);
    modeloref->select();
    ui.tableView->setModel(modeloref);
}

void buscaref::codigocambiado()
{
  if (ui.TituloLineEdit->text().length()>0) 
    {
      QString guardacadena=ui.codigolineEdit->text();
      ui.TituloLineEdit->setText("");
      ui.codigolineEdit->setText(guardacadena);
     }
  QString cadena;
    // si vacío fijar filtro inicial
  if (ui.codigolineEdit->text().length()==0)
     {
        recargartabla(cadena);
        // PlanDataTable->refresh();
        return;
     }
  if (ui.ComienceRadioButton->isChecked())
      {
       if (es_sqlite())
          {
           cadena+=" codigo like '";
           cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
           cadena+="%'";
          }
       else
         {
          cadena+=" position('";
          cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
          cadena+="' in codigo)=1";
         }
       recargartabla(cadena);
      }
    else
       {
         if (es_sqlite())
            {
             cadena+=" codigo like '%";
             cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
             cadena+="%'";
            }
          else
              {
               cadena+=" position('";
               cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
               cadena+="' in codigo)>0";
              }
  recargartabla(cadena);
       }
}



void buscaref::titulocuentacambiado()
{
    if (ui.codigolineEdit->text().length()>0)
       {
        QString guardacadena=ui.TituloLineEdit->text();
        ui.codigolineEdit->setText("");
        ui.TituloLineEdit->setText(guardacadena);
       }
  QString cadena;
  // si vacío fijar filtro inicial 
  if (ui.TituloLineEdit->text().length()==0)
     {
        recargartabla(cadena);
        // PlanDataTable->refresh();
        return;
     }
  if (ui.ComienceRadioButton->isChecked())
      {
         if (es_sqlite())
            {
             cadena+=" descrip like '";
             cadena+=ui.TituloLineEdit->text().left(-1).replace("'","''");
             cadena+="%'";
            }
          else
             {
              cadena+=" position(upper('";
              cadena+=ui.TituloLineEdit->text().left(-1).replace("'","''");
              cadena+="') in upper(descrip))=1";
             }
        recargartabla(cadena);
        // PlanDataTable->refresh();
      }
    else
       {
         if (es_sqlite())
            {
             cadena+=" descrip like '%";
             cadena+=ui.TituloLineEdit->text().left(-1).replace("'","''");
             cadena+="%'";
            }
          else
              {
               cadena+=" position(upper('";
               cadena+=ui.TituloLineEdit->text().left(-1).replace("'","''");
               cadena+="') in upper(descrip))>0";
              }
         recargartabla(cadena);
         // PlanDataTable->refresh();
     }
}


QString buscaref::seleccioncuenta( void )
{
 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
    return modeloref->record(indiceactual.row()).value("codigo").toString();
  else if (modeloref->rowCount()>0)
          return modeloref->record(0).value("codigo").toString();
 return "";
}


void buscaref::tablaclickada()
{
 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
    ui.codigolineEdit->setText(modeloref->record(indiceactual.row()).value("codigo").toString());
  else if (modeloref->rowCount()>0)
          ui.codigolineEdit->setText(modeloref->record(0).value("codigo").toString());
 accept();
}

void buscaref::fuerafococodigo()
{
  if (cod_longitud_variable()) return;
  QString codigo=ui.codigolineEdit->text();
  codigo=expandepunto(codigo,anchocuentas());
  ui.codigolineEdit->setText(codigo);
}


void buscaref::cambiatitulo(QString titulo)
{
  setWindowTitle(titulo);
}
