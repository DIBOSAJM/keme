/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#include "buscaconcepto.h"
#include "funciones.h"
#include "basedatos.h"
#include <QSqlDatabase>
#include <QSqlRecord>


buscaconcepto::buscaconcepto(QString codigoini) : QDialog() {
    ui.setupUi(this);

     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("conceptos");
     QString cadena;
     if (codigoini.length()>0)
       {
         if ( es_sqlite() ) {
            cadena = "clave like '"+ ui.claveLineEdit->text().left(-1).replace("'","''") +"%'";
         }
         else {
            ui.claveLineEdit->setText(codigoini);
            cadena="position('";
            cadena+=ui.claveLineEdit->text().left(-1).replace("'","''");
            cadena+="' in clave)=1";
         }
       }
     if (cadena.length()>0) modelo->setFilter(cadena);
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CLAVE"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modeloconceptos=modelo;
     ui.tableView->setModel(modelo);
     ui.tableView->setColumnWidth(0,100);
     ui.tableView->setColumnWidth(1,410);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tableView->show();

    connect(ui.ComienceRadioButton,SIGNAL(toggled(bool)),this,SLOT(ComienceRadioBotoncambiado()));
    connect(ui.ContengaRadioButton,SIGNAL(toggled(bool)),this,SLOT(ContengaRadioBotoncambiado()));
    connect(ui.claveLineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigoconceptocambiado()));
    connect(ui.descripcionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripconceptocambiada()));
    connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(accept()));


}


buscaconcepto::~buscaconcepto()
{
  delete ui.tableView;
  delete modeloconceptos;
}


void buscaconcepto::ComienceRadioBotoncambiado()
{
 if (ui.claveLineEdit->text().length()>0) codigoconceptocambiado();
 if (ui.descripcionLineEdit->text().length()>0) descripconceptocambiada();
}


void buscaconcepto::ContengaRadioBotoncambiado()
{
 if (ui.claveLineEdit->text().length()>0) codigoconceptocambiado();
 if (ui.descripcionLineEdit->text().length()>0) descripconceptocambiada();
}


void buscaconcepto::codigoconceptocambiado()
{
  if (ui.descripcionLineEdit->text().length()>0) 
    {
      QString guardacadena=ui.claveLineEdit->text();
      ui.descripcionLineEdit->setText("");
      ui.claveLineEdit->setText(guardacadena);
     }
  QString cadena;
  if (ui.claveLineEdit->text().length()==0)
     {
     modeloconceptos->clear();
     modeloconceptos->setTable("conceptos");
     modeloconceptos->select();
        modeloconceptos->setFilter("clave != ''");
        // PlanDataTable->refresh();
        return;
     }

  if (ui.ComienceRadioButton->isChecked())
      {
        if ( es_sqlite() ) {
            cadena = "clave like '"+ ui.claveLineEdit->text().left(-1).replace("'","''") +"%'";
        }
        else {
            cadena="position('";
            cadena+=ui.claveLineEdit->text().left(-1).replace("'","''");
            cadena+="' in clave)=1";
        }
     modeloconceptos->clear();
     modeloconceptos->setTable("conceptos");
     modeloconceptos->select();
        modeloconceptos->setFilter(cadena);
        // PlanDataTable->refresh();
      }
    else
       {
        if ( es_sqlite() ) {
            cadena = "clave like '%"+ ui.claveLineEdit->text().left(-1).replace("'","''") +"%'";
        }
        else {
            cadena="position('";
            cadena+=ui.claveLineEdit->text().left(-1).replace("'","''");
            cadena+="' in clave)>0";
        }
     modeloconceptos->clear();
     modeloconceptos->setTable("conceptos");
     modeloconceptos->select();
         modeloconceptos->setFilter(cadena);
         // PlanDataTable->refresh();
     }
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

void buscaconcepto::descripconceptocambiada()
{
    if (ui.claveLineEdit->text().length()>0)
       {
        QString guardacadena=ui.descripcionLineEdit->text();
        ui.claveLineEdit->setText("");
        ui.descripcionLineEdit->setText(guardacadena);
       }
  QString cadena="";
  // si vac� fijar filtro inicial 
  if (ui.descripcionLineEdit->text().length()==0)
     {
     modeloconceptos->clear();
     modeloconceptos->setTable("conceptos");
     modeloconceptos->select();
        modeloconceptos->setFilter("descripcion != ''");
        return;
     }
  if (ui.ComienceRadioButton->isChecked())
      {
        if ( es_sqlite() ) {
            cadena = "descripcion like '"+ ui.descripcionLineEdit->text().left(-1).replace("'","''") +"%'";
        }
        else {
            cadena = "position(upper('";
            cadena += ui.descripcionLineEdit->text().left(-1).replace("'","''");
            cadena += "') in upper(descripcion))=1";
        }
     modeloconceptos->clear();
     modeloconceptos->setTable("conceptos");
     modeloconceptos->select();
        modeloconceptos->setFilter(cadena);
        // PlanDataTable->refresh();
      }
    else
       {
        if ( es_sqlite() ) {
            cadena = "descripcion like '%"+ ui.descripcionLineEdit->text().left(-1).replace("'","''") +"%'";
        }
        else {
            cadena="position(upper('";
            cadena+=ui.descripcionLineEdit->text().left(-1).replace("'","''");
            cadena+="') in upper(descripcion))>0";
        }
     modeloconceptos->clear();
     modeloconceptos->setTable("conceptos");
     modeloconceptos->select();
         modeloconceptos->setFilter(cadena);
         // PlanDataTable->refresh();
     }

}


QString buscaconcepto::seleccionconcepto( void )
{
 if (ui.claveLineEdit->text().length()>0)
    {
     QString cadena;
     if (existecodigoconcepto(ui.claveLineEdit->text(),&cadena))
         return ui.claveLineEdit->text();
     // else return "";
    }
 else if (ui.tableView->currentIndex().row()==-1) {
   return "";
 }
// if (modeloplan->rowCount()>0)
//    {
  QModelIndex indiceactual=ui.tableView->currentIndex();
     if (indiceactual.isValid())
        return modeloconceptos->record(indiceactual.row()).value("clave").toString();
      else if (modeloconceptos->rowCount()>0)
                return modeloconceptos->record(0).value("clave").toString();
//    }
return "";
}

