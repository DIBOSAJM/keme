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
#include "tiposiva.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QMessageBox>


CustomSqlModelT::CustomSqlModelT(QObject *parent)
        : QSqlTableModel(parent)
    {
     comadecimal=haycomadecimal();
    }



QVariant CustomSqlModelT::data(const QModelIndex &index, int role) const
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



tiposiva::tiposiva() : QDialog() {
    ui.setupUi(this);

if (conigic()) setWindowTitle(tr("TIPOS DE IGIC"));

    ui.CodigoLineEdit->setFocus();
    CustomSqlModelT *modelo = new CustomSqlModelT;
     modelo->setTable("tiposiva");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CLAVE"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("TIPO"));
     modelo->setHeaderData(2, Qt::Horizontal, tr("R.E."));
     modelo->setHeaderData(3, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modelotiposiva=modelo;
     ui.tabla->setModel(modelo);
     ui.tabla->setColumnWidth(0,60);
     ui.tabla->setColumnWidth(3,350);
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->show();

     connect(ui.CodigoLineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
     connect(ui.tipolineEdit,SIGNAL(textChanged(QString)),this,SLOT(tipocambiado()));
     connect(ui.relineEdit,SIGNAL(textChanged(QString)),this,SLOT(recambiado()));
     connect(ui.DescripcionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.BotonGuardar,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
     connect(ui.BotonCancelar,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
     connect(ui.BotonEliminar,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
     connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));

     connect(ui.tipolineEdit,SIGNAL(editingFinished ()),this,SLOT(finedictipo()));
     connect(ui.relineEdit,SIGNAL(editingFinished ()),this,SLOT(finedicre()));

     comadecimal=haycomadecimal();
}


tiposiva::~tiposiva()
{
  delete ui.tabla;
  delete modelotiposiva;
}


void tiposiva::compruebacodigo()
{
    QString caddescrip;

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

   QString cadtipo,cadre;
 if (!existeclaveiva(ui.CodigoLineEdit->text(),&cadtipo,&cadre,&caddescrip))
    {
       ui.BotonCancelar->setEnabled(true);
       ui.tipolineEdit->setEnabled(true);
       ui.relineEdit->setEnabled(true);
       ui.DescripcionLineEdit->setEnabled(true);
       ui.tipolineEdit->setFocus();
       ui.CodigoLineEdit->setReadOnly(true);
     }
     else // codigo correcto
            {
              ui.tipolineEdit->setText(convacoma(cadtipo));
              ui.relineEdit->setText(convacoma(cadre));
              ui.DescripcionLineEdit->setText(caddescrip);
          ui.BotonCancelar->setEnabled(true);
          ui.BotonEliminar->setEnabled(true);
              ui.tipolineEdit->setEnabled(true);
              ui.relineEdit->setEnabled(true);
              ui.DescripcionLineEdit->setEnabled(true);
	      ui.tipolineEdit->setFocus();
          ui.CodigoLineEdit->setReadOnly(true);
          // Guardarboton->setEnabled(true);
            }
}


void tiposiva::descripcioncambiada()
{
    if(ui.DescripcionLineEdit->hasFocus())  ui.BotonGuardar->setEnabled(true);
}


void tiposiva::tipocambiado()
{
    if(ui.tipolineEdit->hasFocus())  ui.BotonGuardar->setEnabled(true);
}

void tiposiva::recambiado()
{
    if(ui.relineEdit->hasFocus())  ui.BotonGuardar->setEnabled(true);
}

void tiposiva::finedictipo()
{
 if (!cadnumvalido(convapunto(ui.tipolineEdit->text())) || ui.tipolineEdit->text().length()==0)
    ui.tipolineEdit->setText("0");
 if (comadecimal) ui.tipolineEdit->setText(convacoma(ui.tipolineEdit->text()));
     else ui.tipolineEdit->setText(convapunto(ui.tipolineEdit->text()));
}

void tiposiva::finedicre()
{
 if (!cadnumvalido(convapunto(ui.relineEdit->text())) || ui.relineEdit->text().length()==0)
    ui.relineEdit->setText("0");
 if (comadecimal) ui.relineEdit->setText(convacoma(ui.relineEdit->text()));
     else ui.relineEdit->setText(convapunto(ui.relineEdit->text()));
}


void tiposiva::botonguardarpulsado()
{
    QString caddescrip,cadtipo,cadre;
  
      if  (existeclaveiva(ui.CodigoLineEdit->text(),&cadtipo,&cadre,&caddescrip))
	guardacambiostiposiva(ui.CodigoLineEdit->text(),convapunto(ui.tipolineEdit->text()),
                         convapunto(ui.relineEdit->text()),ui.DescripcionLineEdit->text());
      else 
	insertatipoiva(ui.CodigoLineEdit->text(),convapunto(ui.tipolineEdit->text()),
                         convapunto(ui.relineEdit->text()),ui.DescripcionLineEdit->text());
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.tipolineEdit->clear();
     ui.tipolineEdit->setDisabled(true);
     ui.relineEdit->clear();
     ui.relineEdit->setDisabled(true);
     ui.DescripcionLineEdit->clear();
     ui.DescripcionLineEdit->setDisabled(true);
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
     modelotiposiva->clear();
     modelotiposiva->setTable("tiposiva");
     modelotiposiva->select();
}


void tiposiva::botoncancelarpulsado()
{
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.tipolineEdit->clear();
     ui.tipolineEdit->setDisabled(true);
     ui.relineEdit->clear();
     ui.relineEdit->setDisabled(true);
     ui.DescripcionLineEdit->clear();
     ui.DescripcionLineEdit->setDisabled(true);
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}


void tiposiva::botoneliminarpulsado()
{
     eliminatipoiva(ui.CodigoLineEdit->text());
     modelotiposiva->clear();
     modelotiposiva->setTable("tiposiva");
     modelotiposiva->select();
     ui.CodigoLineEdit->clear();
     ui.CodigoLineEdit->setReadOnly(false);
     ui.CodigoLineEdit->setFocus();
     ui.tipolineEdit->clear();
     ui.tipolineEdit->setDisabled(true);
     ui.relineEdit->clear();
     ui.relineEdit->setDisabled(true);
     ui.DescripcionLineEdit->clear();
     ui.DescripcionLineEdit->setDisabled(true);
     ui.BotonGuardar->setDisabled(true);
     ui.BotonCancelar->setDisabled(true);
     ui.BotonEliminar->setDisabled(true);
}



void tiposiva::pasacodigo( QString qconcepto )
{
 ui.CodigoLineEdit->setText(qconcepto);
}

void tiposiva::tablapulsada()
{
  QModelIndex indiceactual=ui.tabla->currentIndex();
  if (indiceactual.isValid())
   {
    ui.CodigoLineEdit->setText(modelotiposiva->record(indiceactual.row()).value("clave").toString());
    if (comadecimal)
       ui.tipolineEdit->setText(convacoma(modelotiposiva->record(indiceactual.row()).value("tipo").toString()));
      else
       ui.tipolineEdit->setText(modelotiposiva->record(indiceactual.row()).value("tipo").toString());
    if (comadecimal)
       ui.relineEdit->setText(convacoma(modelotiposiva->record(indiceactual.row()).value("re").toString()));
      else
       ui.relineEdit->setText(modelotiposiva->record(indiceactual.row()).value("re").toString());
    ui.DescripcionLineEdit->setText(modelotiposiva->record(indiceactual.row()).value("descripcion").toString());
    ui.BotonCancelar->setEnabled(true);
    ui.BotonEliminar->setEnabled(true);
    ui.tipolineEdit->setEnabled(true);
    ui.tipolineEdit->setFocus();
    ui.relineEdit->setEnabled(true);
    ui.DescripcionLineEdit->setEnabled(true);
    ui.CodigoLineEdit->setReadOnly(true);
   }
}
