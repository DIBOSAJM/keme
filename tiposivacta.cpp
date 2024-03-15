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

#include "funciones.h"
#include "basedatos.h"
#include "tiposivacta.h"
#include "buscasubcuenta.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QMessageBox>

CustomSqlModelT2::CustomSqlModelT2(QObject *parent)
        : QSqlTableModel(parent)
    {
     comadecimal=haycomadecimal();
    }



QVariant CustomSqlModelT2::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == 1 || index.column() ==2)
	       {
                 if (comadecimal) return value.fromValue(convacoma(value.toString()));
	       }
        }
        if (role == Qt::TextAlignmentRole &&
             (index.column() == 1 || index.column()==2 ))
           return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        return value;
    }



tiposivacta::tiposivacta(bool es_soportado) : QDialog() {
    ui.setupUi(this);

soportado=es_soportado;

if (conigic()) setWindowTitle(tr("TIPOS DE IGIC"));

    ui.CodigoLineEdit->setFocus();
    CustomSqlModelT2 *modelo = new CustomSqlModelT2;
     modelo->setTable("tiposiva");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CLAVE"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("TIPO"));
     modelo->setHeaderData(2, Qt::Horizontal, tr("R.E."));
     modelo->setHeaderData(3, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modelo->setHeaderData(4, Qt::Horizontal, tr("CUENTA REP"));
     modelo->setHeaderData(5, Qt::Horizontal, tr("CUENTA SOP"));
     modelotiposivacta=modelo;
     ui.tabla->setModel(modelo);
     ui.tabla->setColumnWidth(0,60);
     ui.tabla->setColumnWidth(3,350);
     ui.tabla->hideColumn(3);
     if(soportado) ui.tabla->hideColumn(4);
       else ui.tabla->hideColumn(5);
     if (soportado) setWindowTitle(tr("Cuentas IVA soportado"));
        else setWindowTitle(tr("Cuentas de IVA repercutido"));
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->show();

     connect(ui.CodigoLineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
     connect(ui.cuentalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuentacambiada()));
     connect(ui.cuentalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaexpandepunto()));
     //connect(ui.tipolineEdit,SIGNAL(textChanged(QString)),this,SLOT(tipocambiado()));
     //connect(ui.relineEdit,SIGNAL(textChanged(QString)),this,SLOT(recambiado()));
     //connect(ui.DescripcionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.BotonGuardar,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
     connect(ui.BotonCancelar,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
     // connect(ui.BotonEliminar,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
     connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));
     connect(ui.ctapushButton,SIGNAL(clicked()),this,SLOT(buscacta()));

     //connect(ui.tipolineEdit,SIGNAL(editingFinished ()),this,SLOT(finedictipo()));
     //connect(ui.relineEdit,SIGNAL(editingFinished ()),this,SLOT(finedicre()));

     comadecimal=haycomadecimal();
}


tiposivacta::~tiposivacta()
{
  delete ui.tabla;
  delete modelotiposivacta;
}


void tiposivacta::compruebacodigo()
{
    // QString caddescrip;

 if (ui.CodigoLineEdit->text().contains(' '))
    {
       QMessageBox::warning( this, tr("EDICIÓN DE TIPOS DE IVA"),
      tr("En la composición del código no pueden haber espacios"));
      ui.CodigoLineEdit->clear();
      return;
    }

 if (ui.CodigoLineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("EDICIÓN DE CONCEPTOS"),
      tr("El código no puede estar vacío"));
      ui.CodigoLineEdit->clear();
      return;
    }

   QString cuenta;

   if (basedatos::instancia()->cuentatipoiva(ui.CodigoLineEdit->text(),&cuenta,soportado))
            {
              ui.cuentalineEdit->setText(cuenta);
          ui.BotonCancelar->setEnabled(true);
              ui.cuentalineEdit->setEnabled(true);
              ui.ctapushButton->setEnabled(true);
              ui.cuentalineEdit->setFocus();
          ui.CodigoLineEdit->setReadOnly(true);
          // Guardarboton->setEnabled(true);
            }
}


void tiposivacta::cuentacambiada()
{
    if(existesubcuenta(ui.cuentalineEdit->text()))
       {
        ui.BotonGuardar->setEnabled(true);
        ui.nombrectalabel->setText(descripcioncuenta(ui.cuentalineEdit->text()));
       }
     else
       {
         ui.BotonGuardar->setEnabled(false);
         ui.nombrectalabel->clear();
       }
}

void tiposivacta::ctaexpandepunto()
{
  ui.cuentalineEdit->setText(expandepunto(ui.cuentalineEdit->text(),anchocuentas()));
}


/*
void tiposivacta::tipocambiado()
{
    if(ui.tipolineEdit->hasFocus())  ui.BotonGuardar->setEnabled(true);
}

void tiposivacta::recambiado()
{
    if(ui.relineEdit->hasFocus())  ui.BotonGuardar->setEnabled(true);
}
*/


void tiposivacta::botonguardarpulsado()
{
    QString caddescrip,cadtipo,cadre;
  
      if  (existeclaveiva(ui.CodigoLineEdit->text(),&cadtipo,&cadre,&caddescrip))
        {
          if (soportado)
             basedatos::instancia()->updatecuentatipoiva_soportado(ui.CodigoLineEdit->text(),
                                                       ui.cuentalineEdit->text());
            else
              basedatos::instancia()->updatecuentatipoiva (ui.CodigoLineEdit->text(),
                                                        ui.cuentalineEdit->text());

        }
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.cuentalineEdit->clear();
     ui.cuentalineEdit->setDisabled(true);
     ui.ctapushButton->setDisabled(true);
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     modelotiposivacta->clear();
     modelotiposivacta->setTable("tiposiva");
     modelotiposivacta->select();
}


void tiposivacta::botoncancelarpulsado()
{
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.cuentalineEdit->clear();
     ui.cuentalineEdit->setDisabled(true);
     ui.ctapushButton->setDisabled(true);
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     // ui.BotonEliminar->setDisabled(true);
}



void tiposivacta::pasacodigo( QString qconcepto )
{
 ui.CodigoLineEdit->setText(qconcepto);
}

void tiposivacta::tablapulsada()
{
  QModelIndex indiceactual=ui.tabla->currentIndex();
  if (indiceactual.isValid())
   {
    ui.CodigoLineEdit->setText(modelotiposivacta->record(indiceactual.row()).value("clave").toString());
    ui.cuentalineEdit->setText(soportado ? modelotiposivacta->record(indiceactual.row()).value("cuenta").toString()
              :modelotiposivacta->record(indiceactual.row()).value("cuenta_sop").toString());
    ui.BotonCancelar->setEnabled(true);
    ui.cuentalineEdit->setFocus();
    ui.CodigoLineEdit->setReadOnly(true);
    compruebacodigo();
   }
}

void tiposivacta::buscacta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentalineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.cuentalineEdit->setText(cadena2);
       else ui.cuentalineEdit->setText("");
    delete labusqueda;

}
