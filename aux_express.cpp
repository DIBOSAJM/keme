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

#include "aux_express.h"
#include "basedatos.h"
#include "funciones.h"
#include "datos_accesorios.h"
#include "privilegios.h"
#include <QMessageBox>

aux_express::aux_express() : QDialog() {
    ui.setupUi(this);


    ui.codigolineEdit->setFocus();
     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("plancontable");

     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modeloreg=modelo;

     QString cadena;
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

     modelo->setFilter (cadena);
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modeloreg=modelo;
     ui.tabla->setModel(modelo);
     // ui.tabla->setSortingEnabled(true);
     ui.tabla->setColumnWidth(0,100);
     ui.tabla->setColumnWidth(1,350);
     ui.tabla->hideColumn(2);
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->show();

     connect(ui.codigolineEdit,SIGNAL(editingFinished()),this,SLOT(compruebacodigo()));
     connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado()));
     connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.guardarpushButton,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
     connect(ui.datospushButton,SIGNAL(clicked()),this,SLOT(botondatospulsado()));
     connect(ui.cancelarpushButton,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
     connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
     connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));

     if (!privilegios[datos_accesorios_cta]) ui.datospushButton->setEnabled(false);
}
 
aux_express::~aux_express()
{
  delete ui.tabla;
  delete modeloreg;
}

void aux_express::pasacodigo(QString codigo)
{
  ui.codigolineEdit->setText(codigo);
  refrescar(filtrocodigo());

}

void aux_express::compruebacodigo()
{

 if (ui.codigolineEdit->text().contains(' '))
    {
       QMessageBox::warning( this, tr("TIPOS DE VENCIMIENTOS"),
      tr("En la composición del código no pueden haber espacios"));
      ui.codigolineEdit->clear();
      return;
    }

 if (ui.codigolineEdit->text().length()==0)
    {
      return;
    }

 if (!cod_longitud_variable())
   ui.codigolineEdit->setText(expandepunto(ui.codigolineEdit->text(),anchocuentas()));

 bool auxok=true;
 QString descripcion;
 if (!cod_longitud_variable())
    {
     if (!ui.codigolineEdit->text().length()==anchocuentas()) auxok=false;
    }
    else {
        if (ui.codigolineEdit->text().length()<4) auxok=false;
        if (existecodigoplan(ui.codigolineEdit->text(),&descripcion)
            && !esauxiliar(ui.codigolineEdit->text())) auxok=false;
    }

if (!auxok) return;



 if (!basedatos::instancia()->existecodigoplan(ui.codigolineEdit->text(),&descripcion))
    {
       ui.cancelarpushButton->setEnabled(true);
       ui.descriplineEdit->setEnabled(true);
       ui.descriplineEdit->setFocus();
       ui.codigolineEdit->setReadOnly(true);
     }
     else // codigo correcto
            {
              disconnect(ui.descriplineEdit);
              ui.descriplineEdit->setText(descripcion);
          ui.cancelarpushButton->setEnabled(true);
          ui.borrarpushButton->setEnabled(true);
              ui.descriplineEdit->setEnabled(true);
	      ui.descriplineEdit->setFocus();
          ui.codigolineEdit->setReadOnly(true);
              connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
            }
}


void aux_express::descripcioncambiada()
{
    if(ui.descriplineEdit->hasFocus())
      {
        ui.guardarpushButton->setEnabled(true);
        if (privilegios[datos_accesorios_cta]) ui.datospushButton->setEnabled(true);
      }
}

QString aux_express::filtrocodigo()
{
    QString cadena;
       if (es_sqlite())
          {
           cadena="codigo like '";
           cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
           cadena+="%'";
          }
       else
         {
          cadena+="position('";
          cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
          cadena+="' in codigo)=1";
         }

       return cadena;
}


void aux_express::codigocambiado()
{

    if ((ui.codigolineEdit->text().length()==anchocuentas())
        || (ui.codigolineEdit->text().length()>3 && cod_longitud_variable()))
      {
       ui.descriplineEdit->setEnabled(true);
       if (!cod_longitud_variable()) ui.descriplineEdit->setFocus();
      }
        else
             {
              ui.descriplineEdit->setText("");
              ui.descriplineEdit->setEnabled(false);
              ui.borrarpushButton->setEnabled(false);
              // return;
             }


    refrescar(filtrocodigo());

}

void aux_express::botonguardarpulsado()
{
    guardarpulsado();
    accept();
}

void aux_express::guardarpulsado()
{

    QString cadena;
    if (existecodigoplan(ui.codigolineEdit->text(),&cadena))
         {
          guardacambiosplan(ui.codigolineEdit->text(),ui.descriplineEdit->text());
         }
     else
      {
       if (cod_longitud_variable())
          inserta_auxiliar_enplan(ui.codigolineEdit->text(),ui.descriplineEdit->text());
       else
         insertaenplan(ui.codigolineEdit->text(),ui.descriplineEdit->text());

       if (!existecodigosaldosubcuenta(ui.codigolineEdit->text()))
          insertasaldosubcuenta(ui.codigolineEdit->text());

      }

     // refrescar(filtrocodigo());
}

void aux_express::refrescar(QString filtro)
{
    modeloreg->setTable("plancontable");

    modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
    modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));

    QString cadena;
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
    if (!filtro.isEmpty()) cadena+=" and ";
    cadena+=filtro;
    modeloreg->setFilter (cadena);
    modeloreg->setSort(0,Qt::AscendingOrder);
    modeloreg->select();
    ui.tabla->setModel(modeloreg);
    //ui.tabla->setSortingEnabled(true);
    ui.tabla->setColumnWidth(0,100);
    ui.tabla->setColumnWidth(1,350);
    ui.tabla->hideColumn(2);
    ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

    ui.tabla->show();

}

void aux_express::botoncancelarpulsado()
{
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.datospushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     refrescar(filtrocodigo());

}

void aux_express::botoneliminarpulsado()
{
   if (QMessageBox::question(
            this,
            tr("CUENTAS AUXILIARES"),
            tr("¿ Desea borrar la cuenta seleccionada ?")) == QMessageBox::No ) return;

   if (cuentaendiario(ui.codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Edición de subcuentas"),
       tr("No se puede elimimar la subcuenta porque existe en el diario"));
       return;
    }

   if (cuentaenpresupuesto(ui.codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Edición de subcuentas"),
       tr("No se puede elimimar la subcuenta porque existe en presupuestos"));
       return;
    }

   if (cuentaen_ajustes_conci(ui.codigolineEdit->text()))
     {
       QMessageBox::warning( this, tr("Edición de subcuentas"),
       tr("No se puede elimimar la subcuenta porque existe en ajustes de conciliación"));
       return;
     }

   if (cuentaencuenta_ext_conci(ui.codigolineEdit->text()))
     {
       QMessageBox::warning( this, tr("Edición de subcuentas"),
       tr("No se puede elimimar la subcuenta porque existe en externa de conciliación"));
       return;
     }

   eliminasubcuenta(ui.codigolineEdit->text());


   ui.codigolineEdit->clear();
   ui.codigolineEdit->setReadOnly(false);
   ui.codigolineEdit->setFocus();
   ui.descriplineEdit->clear();
   ui.guardarpushButton->setDisabled(true);
   ui.datospushButton->setDisabled(true);
   ui.cancelarpushButton->setDisabled(true);
   ui.borrarpushButton->setDisabled(true);
   refrescar(filtrocodigo());

}

void aux_express::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        QString codigo=modeloreg->record(indiceactual.row()).value("codigo").toString();
        QString descrip=modeloreg->record(indiceactual.row()).value("descripcion").toString();
        ui.codigolineEdit->setText(codigo);
        ui.descriplineEdit->setText(descrip);
        ui.cancelarpushButton->setEnabled(true);
        ui.borrarpushButton->setEnabled(true);
        ui.descriplineEdit->setEnabled(true);
        ui.descriplineEdit->setFocus();
        ui.codigolineEdit->setReadOnly(true);
       }
}

void aux_express::botondatospulsado()
{
    QString cuenta=ui.codigolineEdit->text();
    guardarpulsado();


    if (cuenta.isEmpty()) return;
    if (!existesubcuenta(cuenta)) return;
    datos_accesorios *d = new datos_accesorios();
    d->cargacodigo( cuenta );
    d->exec();
    delete(d);

    accept();
}
