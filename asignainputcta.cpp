/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#include "asignainputcta.h"
#include "funciones.h"
#include "basedatos.h"
#include "introci.h"
#include <QSqlRecord>
#include <QMessageBox>

CustomSqlModelITA::CustomSqlModelITA(QObject *parent)
        : QSqlTableModel(parent)
    {
     comadecimal=haycomadecimal();
    }



QVariant CustomSqlModelITA::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() ==2)
	       {
                 double importe=value.toDouble()*100;
                 QString cadnum;
                 cadnum.setNum(importe,'f',2);
                 if (comadecimal) return value.fromValue(convacoma(cadnum));
	       }
        }
        if (role == Qt::TextAlignmentRole &&
             (index.column()==2 ))
           return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        return value;
    }


asignainputcta::asignainputcta() : QDialog() {
    ui.setupUi(this);

    // connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(botonaceptar()));


    connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(fijadescripciones()));
    connect(ui.buscapushButton,SIGNAL(clicked()),this,SLOT(buscapulsado()));


     connect(ui.cilineEdit,SIGNAL(editingFinished()),this,SLOT(compruebaci()));
     connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(cicambiado()));
     connect(ui.asignacionlineEdit,SIGNAL(textChanged(QString)),this,SLOT(asignacioncambiada()));
     connect(ui.aceptarcipushButton,SIGNAL(clicked()),this,SLOT(aceptarcipulsado()));
     connect(ui.cancelarcipushButton,SIGNAL(clicked()),this,SLOT(botoncancelarcipulsado()));
     connect(ui.borrarcipushButton,SIGNAL(clicked()),this,SLOT(botonborrarcipulsado()));
     connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablacipulsada()));


}

asignainputcta::~asignainputcta()
{
  delete modeloamortci;
}


void asignainputcta::pasacta( QString codcta )
{
  ctaactivo=codcta;
  QString cadena=codcta;
  cadena+=": ";
  cadena+=descripcioncuenta(codcta);
  ui.ctaactivolabel->setText(cadena);

   CustomSqlModelITA *modelo = new CustomSqlModelITA();
   modelo->setTable("input_cta");
   modelo->setSort(1,Qt::AscendingOrder);
   QString cadfiltro="auxiliar='";
   cadfiltro+=ctaactivo;
   cadfiltro+="'";
   modelo->setFilter(cadfiltro);
   modelo->select();
   modelo->setHeaderData(1, Qt::Horizontal, tr("CI"));
   modelo->setHeaderData(2, Qt::Horizontal, tr("% Asignación"));
   modeloamortci=modelo;
   ui.tableView->setModel(modelo);
   ui.tableView->setColumnWidth(1,100);
   ui.tableView->setColumnWidth(2,100);
   ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );
   ui.tableView->hideColumn(0);
   ui.tableView->hideColumn(3);
   ui.tableView->show();
   actualizasumaci();
}


void asignainputcta::buscapulsado()
{
 introci *c = new introci();
 c->pasaci(ui.cilineEdit->text());
 c->exec();
 ui.cilineEdit->setText(c->cadenaci());
 delete(c);
}


void asignainputcta::fijadescripciones()
{

  QString codigo=ui.cilineEdit->text();
  QString cadena,descripcion;
  QString qnivel=0;
  ui.nivel1lineEdit->setText("");
  ui.nivel2lineEdit->setText("");
  ui.nivel3lineEdit->setText("");

 if (codigo.length()==0) return;

 bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
 if (encontrada && qnivel.toInt()==1)
    ui.nivel1lineEdit->setText(descripcion);

 if (codigo.length()<=3) return;

 encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
 int elnivel=qnivel.toInt();
 if (encontrada && elnivel==2)
     ui.nivel2lineEdit->setText(descripcion);

 if (codigo.length()<=6) return;

 encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
 if (encontrada && qnivel.toInt()==3)
     ui.nivel3lineEdit->setText(descripcion);

}



void asignainputcta::compruebaci()
{
 // fin edición ci
 if (ui.cilineEdit->text().isEmpty()) return;
 if (!ciok(ui.cilineEdit->text()))
    {
         QMessageBox::warning( this, tr("Plan de amortizaciones"),
		       tr("ERROR, código de imputación incorrecto"));
         ui.cilineEdit->clear();
         return;
    }
}

void asignainputcta::cicambiado()
{
 if (ciok(ui.cilineEdit->text()))
    {
     ui.asignacionlineEdit->setEnabled(true);
    }
    else
        {
         ui.asignacionlineEdit->clear();
         ui.asignacionlineEdit->setEnabled(false);
        }

}

void asignainputcta::asignacioncambiada()
{
 if (comadecimal) ui.asignacionlineEdit->setText(convacoma(ui.asignacionlineEdit->text()));
 double valor=convapunto(ui.asignacionlineEdit->text()).toDouble();
 if (valor>0.00001)
   {
    ui.aceptarcipushButton->setEnabled(true);
    ui.aceptarcipushButton->setDefault(true);
   }
   else ui.aceptarcipushButton->setEnabled(false);
}


void asignainputcta::aceptarcipulsado()
{
 // insertamos o modificamos registro
 bool existe = basedatos::instancia()->existe_input_cuenta( ui.cilineEdit->text() ,ctaactivo );

  double asignacion=convapunto(ui.asignacionlineEdit->text()).toDouble()/100;
  QString cadnum;
  cadnum.setNum(asignacion,'f',5);
  if (existe)
     {
        basedatos::instancia()->updateCi_input_cta(cadnum, ctaactivo, ui.cilineEdit->text() );
     }
     else
         {
          // insertamos
          basedatos::instancia()->insertCi_input_cta ( ctaactivo, ui.cilineEdit->text(), cadnum );
         }
  ui.cilineEdit->clear();
  ui.cilineEdit->setFocus();
  modeloamortci->clear();

   modeloamortci->setTable("input_cta");
   modeloamortci->setSort(1,Qt::AscendingOrder);
   QString cadfiltro="auxiliar='";
   cadfiltro+=ctaactivo;
   cadfiltro+="'";
   modeloamortci->setFilter(cadfiltro);

  modeloamortci->select();
  actualizasumaci();
}

void asignainputcta::botoncancelarcipulsado()
{
  ui.cilineEdit->clear();
  ui.asignacionlineEdit->clear();

}

void asignainputcta::botonborrarcipulsado()
{
  if (ui.cilineEdit->text().length()==0) return;

  if (QMessageBox::question(this,tr("Borrar elemento CI"),
                            tr("¿ Desea borrar el CI '%1' ?").arg(ui.cilineEdit->text()))==QMessageBox::Yes)
     {
        basedatos::instancia()->delete_input_cta( ctaactivo, ui.cilineEdit->text() );
      ui.cilineEdit->clear();
      ui.asignacionlineEdit->clear();
     }
  modeloamortci->select();
  actualizasumaci();
}


void asignainputcta::tablacipulsada()
{
     QModelIndex indiceactual=ui.tableView->currentIndex();
     if (indiceactual.isValid())
       {
        ui.cilineEdit->setText(modeloamortci->record(indiceactual.row()).value("CI").toString());
        double valor=modeloamortci->record(indiceactual.row()).value("imputacion").toDouble()*100;
        QString cadnum;
        cadnum.setNum(valor,'f',2);
        ui.asignacionlineEdit->setText(cadnum);

        ui.asignacionlineEdit->setFocus();
        ui.aceptarcipushButton->setEnabled(false);
       }
}

void asignainputcta::actualizasumaci()
{
    QSqlQuery query = basedatos::instancia()->selectSumasignacion100input_cta(ctaactivo);
    if ( (query.isActive()) && (query.first()) )
    {
        if (comadecimal)
            ui.sumalineEdit->setText(convacoma(query.value(0).toString()));
        else
            ui.sumalineEdit->setText(query.value(0).toString());
    }
}
