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

#include "buscasubcuenta.h"
#include "funciones.h"
#include "basedatos.h"
#include <QSqlDatabase>
#include <QSqlRecord>


buscasubcuenta::buscasubcuenta(QString codigoini) : QDialog() {
    ui.setupUi(this);


     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("plancontable");
     QString cadena=filtroini();
     /*
     if (!cod_longitud_variable())
        {
         if ( es_sqlite() ) cadena="length(codigo)=";
         else cadena="char_length(codigo)=";
         QString cadnum;
         cadnum.setNum(anchocuentas());
         cadena+=cadnum;
        }
        else {
            if ( es_sqlite() ) { cadena = "auxiliar = 1"; }
            else { cadena = "auxiliar"; }
        }
        */
     if (codigoini.length()>0)
         {
          ui.SubcuentaLineEdit->setText(codigoini);
          if ( es_sqlite() )
             {
               cadena+=" and codigo like '";
               cadena+=ui.SubcuentaLineEdit->text().left(-1).replace("'","''");
               cadena+="%'";
             }
          else
              {
               cadena+=" and position('";
               cadena+=ui.SubcuentaLineEdit->text().left(-1).replace("'","''");
               cadena+="' in codigo)=1";
              }
         }
     modelo->setFilter(cadena);
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("CUENTA"));
     modeloplan=modelo;
     ui.tableView->setModel(modelo);
     ui.tableView->setColumnWidth(0,100);
     ui.tableView->setColumnWidth(1,410);
     ui.tableView->hideColumn(2);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tableView->show();

    connect(ui.ComienceRadioButton,SIGNAL(toggled(bool)),this,SLOT(ComienceRadioBotoncambiado()));
    connect(ui.ContengaRadioButton,SIGNAL(toggled(bool)),this,SLOT(ContengaRadioBotoncambiado()));
    connect(ui.SubcuentaLineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigosubcuentacambiado()));
    connect(ui.TituloLineEdit,SIGNAL(textChanged(QString)),this,SLOT(titulocuentacambiado()));
    connect(ui.SubcuentaLineEdit,SIGNAL(editingFinished ()),this,SLOT(fuerafocosubcuenta()));
    connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablaclickada()));


}

QString buscasubcuenta::filtroini()
{
    QString cadena;
    if (!cod_longitud_variable())
       {
        if ( es_sqlite() ) cadena="length(codigo)=";
        else cadena="char_length(codigo)=";
        QString cadnum;
        cadnum.setNum(anchocuentas());
        cadena+=cadnum;
       }
       else
            { cadena = "auxiliar"; }
       return cadena;
}

buscasubcuenta::~buscasubcuenta()
{
  delete ui.tableView;
  delete modeloplan;
}


void buscasubcuenta::ComienceRadioBotoncambiado()
{
 if (ui.SubcuentaLineEdit->text().length()>0) codigosubcuentacambiado();
 if (ui.TituloLineEdit->text().length()>0) titulocuentacambiado();
}


void buscasubcuenta::ContengaRadioBotoncambiado()
{
 if (ui.SubcuentaLineEdit->text().length()>0) codigosubcuentacambiado();
 if (ui.TituloLineEdit->text().length()>0) titulocuentacambiado();
}

void buscasubcuenta::recargartabla(QString cadena) {
    modeloplan->clear();
    modeloplan->setTable("plancontable");
    modeloplan->setFilter(cadena);//
    modeloplan->setSort(0,Qt::AscendingOrder);

    modeloplan->select();
    ui.tableView->setModel(modeloplan);
    ui.tableView->setColumnWidth(0,100);
    ui.tableView->setColumnWidth(1,410);
    ui.tableView->hideColumn(2);

}

void buscasubcuenta::codigosubcuentacambiado()
{
  if (ui.TituloLineEdit->text().length()>0) 
    {
      QString guardacadena=ui.SubcuentaLineEdit->text();
      ui.TituloLineEdit->setText("");
      ui.SubcuentaLineEdit->setText(guardacadena);
     }
  QString cadena=filtroini();
  /*if (!cod_longitud_variable())
      {
         if (es_sqlite()) cadena="length(codigo)=";
            else cadena="char_length(codigo)=";
         QString cadnum;
         cadnum.setNum(anchocuentas());
         cadena+=cadnum;
      }
      else {
        if (es_sqlite()) { cadena = "auxiliar = 1"; }
        else {  cadena="auxiliar"; }
      } */
  // si vacío fijar filtro inicial 
  if (ui.SubcuentaLineEdit->text().length()==0)
     {
        recargartabla(cadena);
        // PlanDataTable->refresh();
        return;
     }
  if (ui.ComienceRadioButton->isChecked())
      {
       if (es_sqlite())
          {
           cadena+=" and codigo like '";
           cadena+=ui.SubcuentaLineEdit->text().left(-1).replace("'","''");
           cadena+="%'";
          }
       else
         {
          cadena+=" and position('";
          cadena+=ui.SubcuentaLineEdit->text().left(-1).replace("'","''");
          cadena+="' in codigo)=1";
         }
  recargartabla(cadena);
      }
    else
       {
         if (es_sqlite())
            {
             cadena+=" and codigo like '%";
             cadena+=ui.SubcuentaLineEdit->text().left(-1).replace("'","''");
             cadena+="%'";
            }
          else
              {
               cadena+=" and position('";
               cadena+=ui.SubcuentaLineEdit->text().left(-1).replace("'","''");
               cadena+="' in codigo)>0";
              }
  recargartabla(cadena);
       }
}



void buscasubcuenta::titulocuentacambiado()
{
    if (ui.SubcuentaLineEdit->text().length()>0)
       {
        QString guardacadena=ui.TituloLineEdit->text();
        ui.SubcuentaLineEdit->setText("");
        ui.TituloLineEdit->setText(guardacadena);
       }
  QString cadena=filtroini();
  /* if (!cod_longitud_variable())
      {
         if (es_sqlite()) cadena="length(codigo)=";
         else cadena="char_length(codigo)=";
         QString cadnum;
         cadnum.setNum(anchocuentas());
         cadena+=cadnum;
      }
      else {
        if (es_sqlite()) { cadena = "auxiliar = 1"; }
        else {  cadena="auxiliar"; }
      } */

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
             cadena+=" and descripcion like '";
             cadena+=ui.TituloLineEdit->text().left(-1).replace("'","''");
             cadena+="%'";
            }
          else
             {
              cadena+=" and position(upper('";
              cadena+=ui.TituloLineEdit->text().left(-1).replace("'","''");
              cadena+="') in upper(descripcion))=1";
             }
        recargartabla(cadena);
        // PlanDataTable->refresh();
      }
    else
       {
         if (es_sqlite())
            {
             cadena+=" and descripcion like '%";
             cadena+=ui.TituloLineEdit->text().left(-1).replace("'","''");
             cadena+="%'";
            }
          else
              {
               cadena+=" and position(upper('";
               cadena+=ui.TituloLineEdit->text().left(-1).replace("'","''");
               cadena+="') in upper(descripcion))>0";
              }
         recargartabla(cadena);
         // PlanDataTable->refresh();
     }
}


QString buscasubcuenta::seleccioncuenta( void )
{
 if ((ui.SubcuentaLineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) || 
    (cod_longitud_variable() && esauxiliar(ui.SubcuentaLineEdit->text())))
    {
     if (existesubcuenta(ui.SubcuentaLineEdit->text()))
         return ui.SubcuentaLineEdit->text();
     else return "";
    }
 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
    return modeloplan->record(indiceactual.row()).value("codigo").toString();
  else if (modeloplan->rowCount()>0)
          return modeloplan->record(0).value("codigo").toString();
 return "";
}


void buscasubcuenta::tablaclickada()
{
 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
    ui.SubcuentaLineEdit->setText(modeloplan->record(indiceactual.row()).value("codigo").toString());
  else if (modeloplan->rowCount()>0)
          ui.SubcuentaLineEdit->setText(modeloplan->record(0).value("codigo").toString());
 accept();
}

void buscasubcuenta::fuerafocosubcuenta()
{
  if (cod_longitud_variable()) return;
  QString codigo=ui.SubcuentaLineEdit->text();
  codigo=expandepunto(codigo,anchocuentas());
  ui.SubcuentaLineEdit->setText(codigo);
}


void buscasubcuenta::cambiatitulo(QString titulo)
{
  setWindowTitle(titulo);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

buscacuenta::buscacuenta(QString codigoini) : buscasubcuenta(codigoini)
{
   // buscasubcuenta(codigoini);
    codigosubcuentacambiado();
}

QString buscacuenta::filtroini()
{
    QString cadena;
    if (!cod_longitud_variable())
       {
        if ( es_sqlite() ) cadena="length(codigo)<";
        else cadena="char_length(codigo)<";
        QString cadnum;
        cadnum.setNum(anchocuentas());
        cadena+=cadnum;
       }
       else
            { cadena = "not auxiliar"; }
     return cadena;
}

void buscacuenta::fuerafocosubcuenta()
{
  return;
}


QString buscacuenta::seleccioncuenta( void )
{
 if ((ui.SubcuentaLineEdit->text().length()<anchocuentas() && !cod_longitud_variable()) ||
    (cod_longitud_variable() && !esauxiliar(ui.SubcuentaLineEdit->text())))
    {
     QString descrip;
     if (existecodigoplan(ui.SubcuentaLineEdit->text(),&descrip))
         return ui.SubcuentaLineEdit->text();
     else return "";
    }
 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
    return modeloplan->record(indiceactual.row()).value("codigo").toString();
  else if (modeloplan->rowCount()>0)
          return modeloplan->record(0).value("codigo").toString();
 return "";
}
