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

#include "itemplanamort.h"
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include "aux_express.h"
#include "introci.h"
#include <QSqlRecord>
#include <QMessageBox>
#include "busca_externo.h"

CustomSqlModelIT::CustomSqlModelIT(QObject *parent)
        : QSqlTableModel(parent)
    {
     comadecimal=haycomadecimal();
    }



QVariant CustomSqlModelIT::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() ==2)
	       {
                 if (comadecimal) return value.fromValue(convacoma(value.toString()));
	       }
        }
        if (role == Qt::TextAlignmentRole &&
             (index.column()==2 ))
           return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        return value;
    }


itemplanamort::itemplanamort() : QDialog() {
    ui.setupUi(this);

 QStringList columnas;
 columnas << tr("EJERCICIO") << tr("AMORTIZACION") ;

 comadecimal=haycomadecimal();
 decimales=haydecimales();

 if (! conanalitica() || basedatos::instancia()->analitica_tabla())
     ui.tabWidget->setTabEnabled(1,false);

 ui.amortable->setHorizontalHeaderLabels(columnas);
 ui.amortable->setColumnWidth(1,110);
 ui.amortable->setColumnWidth(1,125);
 QString cadfecha;
 QDate apfecha = basedatos::instancia()->selectMinaerturaejerciciosnocerrado();
 ui.funcdateEdit->setDate(QDate::currentDate());
 cadfecha=apfecha.toString("yyyy-MM-dd");

 ui.amortable->setRowCount( basedatos::instancia()->selectCountcodigoejerciciosapertura(cadfecha) );

 QSqlQuery query = basedatos::instancia()->selectAperturacodigoejerciciosaperturaorderapertura(cadfecha);
 int pos=0;
 if (query.isActive())
 while (query.next())
       {
        QTableWidgetItem *newItem = new QTableWidgetItem(query.value(1).toString());
	    ui.amortable->setItem(pos,0,newItem);
	    pos++;
       }
    
 ui.amortable->setCurrentCell(0,1);
 ui.ctaaalineEdit->setText(basedatos::instancia()->cuentas_aa());
 ui.ctaamlineEdit->setText(basedatos::instancia()->cuentas_ag());

    connect(ui.ctaaalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuentaaacambiada()));
    connect(ui.ctaamlineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaamcambiada()));

    connect(ui.ctaaalineEdit,SIGNAL( editingFinished ()),this,SLOT(fuerafococtaaa()));
    connect(ui.ctaamlineEdit,SIGNAL( editingFinished ()),this,SLOT(fuerafococtaam()));

    connect(ui.cta_proveedor_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuenta_prov_cambiada()));
    connect(ui.cta_proveedor_lineEdit,SIGNAL(editingFinished()),this,SLOT(fuerafococta_prov()));

    connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(externo_cambiado()));
    connect(ui.externo_pushButton,SIGNAL(clicked(bool)),this,SLOT(buscaexterno()));

    connect(ui.aapushButton,SIGNAL( clicked()),this,SLOT(buscactaaa()));
    connect(ui.gpushButton,SIGNAL( clicked()),this,SLOT(buscactaam()));

    connect(ui.proveedor_pushButton,SIGNAL( clicked()),this,SLOT(buscactaprov()));


    connect(ui.f1checkBox,SIGNAL(stateChanged(int) ),this,SLOT(f1cambiado()));

    connect(ui.coefcontablelineEdit,SIGNAL( editingFinished ()),this,SLOT(coefcontcambiado()));
    connect(ui.coeffiscallineEdit,SIGNAL( editingFinished ()),this,SLOT(coeffiscalcambiado()));
    connect(ui.residuallineEdit,SIGNAL( editingFinished ()),this,SLOT(residualcambiado()));
    connect(ui.coeffiscallineEdit,SIGNAL( editingFinished ()),this,SLOT(coeffiscalcambiado()));

    connect(ui.perscheckBox,SIGNAL(stateChanged(int) ),this,SLOT(tablapersoncheck()));

    connect(ui.amortable,SIGNAL(currentCellChanged (int,int,int,int)),this,
       SLOT(tablaposcambiada( int , int )));

    connect(ui.amortable,SIGNAL(cellChanged(int,int)),this,
       SLOT(tablavalorcambiado(int)));

    connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(botonaceptar()));


    connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(fijadescripciones()));
    connect(ui.buscapushButton,SIGNAL(clicked()),this,SLOT(buscapulsado()));


     connect(ui.cilineEdit,SIGNAL(editingFinished()),this,SLOT(compruebaci()));
     connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(cicambiado()));
     connect(ui.asignacionlineEdit,SIGNAL(textChanged(QString)),this,SLOT(asignacioncambiada()));
     connect(ui.aceptarcipushButton,SIGNAL(clicked()),this,SLOT(aceptarcipulsado()));
     connect(ui.cancelarcipushButton,SIGNAL(clicked()),this,SLOT(botoncancelarcipulsado()));
     connect(ui.borrarcipushButton,SIGNAL(clicked()),this,SLOT(botonborrarcipulsado()));
     connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablacipulsada()));

     connect(ui.bajacheckBox, SIGNAL(stateChanged(int)),this,SLOT(check_baja_cambiado()));

     if (!basedatos::instancia()->hay_externos()) {
         ui.externo_Label->hide();
         ui.externo_lineEdit->hide();
         ui.externo_pushButton->hide();
         ui.descrip_externo_lineEdit->hide();
     }
}


itemplanamort::~itemplanamort()
{
  if (conanalitica()) delete modeloamortci;
}


void itemplanamort::pasaelemactivo( QString codcta )
{
  ctaactivo=codcta;
  QString cadena=codcta;
  cadena+=": ";
  cadena+=descripcioncuenta(codcta);
  ui.ctaactivolabel->setText(cadena);

if (conanalitica())
  {
   CustomSqlModelIT *modelo = new CustomSqlModelIT();
   modelo->setTable("ci_amort");
   modelo->setSort(1,Qt::AscendingOrder);
   QString cadfiltro="cuenta='";
   cadfiltro+=ctaactivo;
   cadfiltro+="'";
   modelo->setFilter(cadfiltro);
   modelo->select();
   modelo->setHeaderData(1, Qt::Horizontal, tr("CI"));
   modelo->setHeaderData(2, Qt::Horizontal, tr("Asignación"));
   modeloamortci=modelo;
   ui.tableView->setModel(modelo);
   ui.tableView->setColumnWidth(1,100);
   ui.tableView->setColumnWidth(2,100);
   ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );
   ui.tableView->hideColumn(0);
   ui.tableView->show();
   actualizasumaci();
  }


}



void itemplanamort::cuentaaacambiada()
{
    QString descrip;
  if (esauxiliar(ui.ctaaalineEdit->text()))
    {
       existecodigoplan(ui.ctaaalineEdit->text(),&descrip);
       ui.descripaalineEdit->setText(descrip);
  } else ui.descripaalineEdit->setText("");

}



void itemplanamort::ctaamcambiada()
{
    QString descrip;
  if (esauxiliar(ui.ctaamlineEdit->text()))
    {
       existecodigoplan(ui.ctaamlineEdit->text(),&descrip);
       ui.descripamlineEdit->setText(descrip);
    } else ui.descripamlineEdit->setText("");

}


void itemplanamort::fuerafococtaaa()
{
 QString resultante;
 QString cadena;
 QString descrip;
 
 ui.ctaaalineEdit->disconnect(SIGNAL(editingFinished ()));
 resultante=expandepunto(ui.ctaaalineEdit->text(),anchocuentas());
 if (resultante!=ui.ctaaalineEdit->text()) ui.ctaaalineEdit->setText(resultante);
 
 if (!basedatos::instancia()->cuentas_aa().trimmed().isEmpty())
  if (!ui.ctaaalineEdit->text().startsWith(basedatos::instancia()->cuentas_aa().trimmed()))
   {
     QMessageBox::warning( this, tr("Plan de amortizaciones"),
                   tr("ERROR, la cuenta no es correcta"));
     ui.ctaaalineEdit->setText(basedatos::instancia()->cuentas_aa().trimmed());

   }

 if ((ui.ctaaalineEdit->text().length()==anchocuentas() && ! cod_longitud_variable()) ||
     (ui.ctaaalineEdit->text().length()>3 && cod_longitud_variable()))
    if (!existecodigoplan(ui.ctaaalineEdit->text(),&cadena))
	  {
       // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
        if (QMessageBox::question(this,tr("Elemento plan amortizaciones"),
                               tr("Esa cuenta auxiliar no existe, ¿ Desea añadirla ?"))==QMessageBox::Yes)
        {

                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.ctaaalineEdit->text());
                   lasubcuenta2->exec();
	               delete lasubcuenta2;
                   existecodigoplan(ui.ctaaalineEdit->text(),&descrip);
                   ui.descripaalineEdit->setText(descrip);
                  }
	    else ui.ctaaalineEdit->setText("");
      }
 connect(ui.ctaaalineEdit,SIGNAL( editingFinished ()),this,SLOT(fuerafococtaaa()));
}


void itemplanamort::fuerafococtaam()
{
 QString resultante;
 QString cadena;
 QString descrip;
 
 ui.ctaamlineEdit->disconnect(SIGNAL(editingFinished ()));
 resultante=expandepunto(ui.ctaamlineEdit->text(),anchocuentas());
 if (resultante!=ui.ctaamlineEdit->text()) ui.ctaamlineEdit->setText(resultante);
 
 if (!basedatos::instancia()->cuentas_ag().trimmed().isEmpty())
  if (!ui.ctaamlineEdit->text().startsWith(basedatos::instancia()->cuentas_ag().trimmed()))
   {
     QMessageBox::warning( this, tr("Plan de amortizaciones"),
                   tr("ERROR, la cuenta no es correcta"));
     ui.ctaamlineEdit->setText(basedatos::instancia()->cuentas_ag().trimmed());
   }

 if ((ui.ctaamlineEdit->text().length()==anchocuentas() && ! cod_longitud_variable()) ||
     (ui.ctaamlineEdit->text().length()>3 && cod_longitud_variable()))
    if (!existecodigoplan(ui.ctaamlineEdit->text(),&cadena))
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
        if (QMessageBox::question(this,tr("Elemento plan amortizaciones"),
                               tr("Esa cuenta auxiliar no existe, ¿ Desea añadirla ?"))==QMessageBox::Yes)
           {
                       aux_express *lasubcuenta2=new aux_express();
		       lasubcuenta2->pasacodigo(ui.ctaamlineEdit->text());
		       lasubcuenta2->exec();
	               delete lasubcuenta2;

                       existecodigoplan(ui.ctaamlineEdit->text(),&descrip);
                       ui.descripamlineEdit->setText(descrip);
		      }
	    else ui.ctaamlineEdit->setText("");
	}
 connect(ui.ctaamlineEdit,SIGNAL( editingFinished ()),this,SLOT(fuerafococtaam()));
}


void itemplanamort::buscactaaa()
{
    QString cadena2;
    QString inicial;
    // QString descrip;
    if (ui.ctaaalineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) inicial="";
      else inicial=ui.ctaaalineEdit->text();

    buscasubcuenta *labusqueda=new buscasubcuenta(inicial);
    int resultado;
    resultado=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()==0 || resultado!= QDialog::Accepted) return;
    delete labusqueda;

    if (esauxiliar(cadena2)) { 
	ui.ctaaalineEdit->setText(cadena2);
    }
    else { ui.ctaaalineEdit->setText("");  ui.descripaalineEdit->setText(""); }

    if (!ui.ctaaalineEdit->text().isEmpty()) fuerafococtaaa();

}


void itemplanamort::buscactaam()
{
    QString cadena2;
    QString inicial;
    // QString descrip;
    if (ui.ctaamlineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) inicial="";
      else inicial=ui.ctaamlineEdit->text();

    buscasubcuenta *labusqueda=new buscasubcuenta(inicial);
    int resultado;
    resultado=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()==0 || resultado!= QDialog::Accepted) return;
    delete labusqueda;

    if (esauxiliar(cadena2)) { 
	ui.ctaamlineEdit->setText(cadena2);
    }
    else { ui.ctaamlineEdit->setText("");  ui.descripamlineEdit->setText(""); }

    if (!ui.ctaamlineEdit->text().isEmpty()) fuerafococtaam();
}


void itemplanamort::f1cambiado()
{
    if (ui.f1checkBox->isChecked()) 
      {
         ui.coeffiscallineEdit->setEnabled(false);
         ui.coeffiscallineEdit->setText("");
         ui.textLabel5->setEnabled(false);
       }
    else  
        { 
	ui.coeffiscallineEdit->setEnabled(true);
                ui.textLabel5->setEnabled(true);
       }
}



void itemplanamort::coefcontcambiado()
{
  if (ui.coefcontablelineEdit->text().length()>0)
     ui.coefcontablelineEdit->setText(formateanumero(convapunto(ui.coefcontablelineEdit->text()).toDouble(),
                                      comadecimal,decimales));
}


void itemplanamort::residualcambiado()
{
  if  (ui.residuallineEdit->text().length()>0)
      ui.residuallineEdit->setText(formateanumero(convapunto(ui.residuallineEdit->text()).toDouble(),
                                      comadecimal,decimales));
}


void itemplanamort::coeffiscalcambiado()
{
  if  (ui.coeffiscallineEdit->text().length()>0)
     ui.coeffiscallineEdit->setText(formateanumero(convapunto(ui.coeffiscallineEdit->text()).toDouble(),
                                      comadecimal,decimales));
}


void itemplanamort::tablapersoncheck()
{
  if (ui.perscheckBox->isChecked())
     {
       ui.amortable->setEnabled(true);
       ui.textLabel4->setEnabled(false);
       ui.coefcontablelineEdit->setText("");
       ui.coefcontablelineEdit->setEnabled(false);
       ui.textLabel1_3->setEnabled(false);
       ui.residuallineEdit->setText("");
       ui.residuallineEdit->setEnabled(false);
       ui.amortable->setFocus();
     }
     else
            {
              ui.amortable->setEnabled(false);
              ui.textLabel4->setEnabled(true);
              ui.coefcontablelineEdit->setEnabled(true);
              ui.textLabel1_3->setEnabled(true);
              ui.residuallineEdit->setEnabled(true);
            }
}


void itemplanamort::tablaposcambiada( int row, int col )
{
   if (col!=1) ui.amortable->setCurrentCell(row,1);
}


void itemplanamort::tablavalorcambiado(int fila)
{
         bool corr=false;
         double valor=convapunto(ui.amortable->item(fila,1)->text()).toDouble(&corr);
         if (!corr || (valor<0.001 && valor>-0.001))
	 {
	     ui.amortable->item(fila,1)->setText("");
	     return;
	 }
         if (!ui.amortable->item(fila,1)->text().contains('.') &&
              !ui.amortable->item(fila,1)->text().contains(','))
	 {
	     ui.amortable->item(fila,1)->setText(ui.amortable->item(fila,1)->text().remove(' '));
	     ui.amortable->item(fila,1)->setText(ui.amortable->item(fila,1)->text()+".00");
	 }
         ui.amortable->item(fila,1)->setText(formateanumero(convapunto(
                                      ui.amortable->item(fila,1)->text()).toDouble(),
                                      comadecimal,decimales));

}


void itemplanamort::botonaceptar()
{
  // hay que comprobar si existen las  cuentas de am.acum y am gasto

  if (!existesubcuenta(ui.ctaaalineEdit->text()) || !existesubcuenta(ui.ctaamlineEdit->text()))
    {
         QMessageBox::warning( this, tr("Plan de amortizaciones"),
		       tr("ERROR, alguna/s de las cuentas suministradas no son correctas"));
         return;
    }
  // comprobar si ya existe en el plan la cuenta de activo, en cuyo caso habría que modificar
  // no insertar

 // QMessageBox::warning( this, tr("Plan de amortizaciones"),
 //    tr("Verificación"));

if (cuentaenplanamort(ctaactivo))
   {

    basedatos::instancia()->updatePlanamortizaciones9cuenta_activo(
            ui.funcdateEdit->date() ,
            ui.codigoelemlineEdit->text() ,
            ui.ctaaalineEdit->text() ,
            ui.ctaamlineEdit->text() ,
            ui.perscheckBox->isChecked() ,
            ui.residuallineEdit->text() ,
            ui.coefcontablelineEdit->text() ,
            ui.coeffiscallineEdit->text() ,
            ui.f1checkBox->isChecked() ,
            ui.bajacheckBox->isChecked(),
            ui.baja_dateEdit->date(),
            ui.motivo_baja_lineEdit->text(),
            ui.cta_proveedor_lineEdit->text(),
            ctaactivo, ui.externo_lineEdit->text() );

     // accept();
     // return;
   }
 else // hay que insertar
       {
            basedatos::instancia()->insertPlanamortizaciones( ctaactivo ,
                   ui.funcdateEdit->date() ,
                   ui.codigoelemlineEdit->text() ,
                   ui.ctaaalineEdit->text() ,
                   ui.ctaamlineEdit->text() ,
                   ui.residuallineEdit->text() ,
                   ui.coefcontablelineEdit->text() ,
                   ui.coeffiscallineEdit->text() ,
                   ui.f1checkBox->isChecked() ,
                   ui.perscheckBox->isChecked(),
                   ui.bajacheckBox->isChecked(),
                   ui.baja_dateEdit->date(),
                   ui.motivo_baja_lineEdit->text(),
                   ui.cta_proveedor_lineEdit->text(),
                   ui.externo_lineEdit->text()
                   );
       }
   // QMessageBox::warning( this, tr("Plan de amortizaciones"),
    //   cadquery.right(80));

    basedatos::instancia()->deleteAmortperscuenta(ctaactivo);

    for (int veces=0;veces<ui.amortable->rowCount();veces++)
        {
           if (ui.amortable->item(veces,1)==0) continue;
           if (ui.amortable->item(veces,1)->text().length()==0) continue;

            basedatos::instancia()->insertAmortpers( ctaactivo , ui.amortable->item(veces,0)->text() , ui.amortable->item(veces,1)->text() );
        }
   accept();
}


void itemplanamort::pasainfo(QString codigo, QDate fecha, QString cuentaaa,
                             QString cuentaam,QString coefcontable, QString residual,
                             QString coeffiscal, bool acelerada,bool amortpersonalizada,
                             bool baja, QDate fecha_baja, QString motivo_baja,
                             QString cuenta_proveedor, QString externo)
{
  QString descrip;
  double contable=convapunto(coefcontable).toDouble()*100;
  double fiscal=convapunto(coeffiscal).toDouble()*100;
  ui.codigoelemlineEdit->setText(codigo);
  ui.funcdateEdit->setDate(fecha);
  ui.ctaaalineEdit->setText(cuentaaa);
  existecodigoplan(ui.ctaaalineEdit->text(),&descrip);
  ui.descripaalineEdit->setText(descrip);
  ui.ctaamlineEdit->setText(cuentaam);
  existecodigoplan(ui.ctaamlineEdit->text(),&descrip);
  ui.descripamlineEdit->setText(descrip);
  ui.coefcontablelineEdit->setText(formateanumero(contable,comadecimal,decimales));
  ui.residuallineEdit->setText(formateanumero(convapunto(residual).toDouble(),
                                      comadecimal,decimales));
  ui.coeffiscallineEdit->setText(formateanumero(fiscal,comadecimal,decimales));
  ui.externo_lineEdit->setText(externo);
  ui.bajacheckBox->setChecked(baja);
  if (baja)
     {
      ui.baja_dateEdit->setDate(fecha_baja);
      ui.motivo_baja_lineEdit->setText(motivo_baja);
     }
  ui.cta_proveedor_lineEdit->setText(cuenta_proveedor);
  if (acelerada) 
    {
      ui.f1checkBox->setChecked(true); 
      ui.textLabel5->setEnabled(false);
    }
  else 
         {
           ui.f1checkBox->setChecked(false);
           ui.textLabel5->setEnabled(true);
          }
  QSqlQuery query;
  if (amortpersonalizada)
    {
      ui.perscheckBox->setChecked(true);
      ui.amortable->setEnabled(true);
      ui.textLabel4->setEnabled(false);
      ui.coefcontablelineEdit->setText("");
      ui.coefcontablelineEdit->setEnabled(false);
      ui.textLabel1_3->setEnabled(false);
      ui.residuallineEdit->setText("");
      ui.residuallineEdit->setEnabled(false);

       query = basedatos::instancia()->selectEjercicioimporteaperturaamortpersejerciciosorderapertura(ctaactivo); 
       while (query.next())
            {
	     for (int veces=0;veces<ui.amortable->rowCount();veces++)
	         {
	          if (ui.amortable->item(veces,0)->text()==query.value(0).toString())
                   {
                    if (ui.amortable->item(veces,1)==0)
                        {
                         QTableWidgetItem *newItem = new QTableWidgetItem("");
                         ui.amortable->setItem(veces,1,newItem);
                        }
	            ui.amortable->item(veces,1)->setText(query.value(1).toString());
                   }
	         }
            }
     }
}



void itemplanamort::buscapulsado()
{
 introci *c = new introci();
 c->pasaci(ui.cilineEdit->text());
 c->exec();
 ui.cilineEdit->setText(c->cadenaci());
 delete(c);
}


void itemplanamort::fijadescripciones()
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



void itemplanamort::compruebaci()
{
 // fin edición ci
 if (!ciok(ui.cilineEdit->text()))
    {
         QMessageBox::warning( this, tr("Plan de amortizaciones"),
		       tr("ERROR, código de imputación incorrecto"));
         ui.cilineEdit->clear();
         return;
    }
}

void itemplanamort::cicambiado()
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

void itemplanamort::asignacioncambiada()
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


void itemplanamort::aceptarcipulsado()
{
 // insertamos o modificamos registro
 bool existe = basedatos::instancia()->selectCici_amortcicuenta( ui.cilineEdit->text() ,ctaactivo );

  double asignacion=ui.asignacionlineEdit->text().toDouble()/100;
  QString cadnum;
  cadnum.setNum(asignacion,'f',5);
  if (existe)
     {
        basedatos::instancia()->updateCi_amortasignacioncuentaci(cadnum, ctaactivo, ui.cilineEdit->text() );
     }
     else
         {
          // insertamos
        basedatos::instancia()->insertCi_amort( ctaactivo, ui.cilineEdit->text(), cadnum );
         }
  ui.cilineEdit->clear();

  modeloamortci->clear();

   modeloamortci->setTable("ci_amort");
   modeloamortci->setSort(1,Qt::AscendingOrder);
   QString cadfiltro="cuenta='";
   cadfiltro+=ctaactivo;
   cadfiltro+="'";
   modeloamortci->setFilter(cadfiltro);

  modeloamortci->select();
  actualizasumaci();
}

void itemplanamort::botoncancelarcipulsado()
{
  ui.cilineEdit->clear();
  ui.asignacionlineEdit->clear();

}

void itemplanamort::botonborrarcipulsado()
{
  if (ui.cilineEdit->text().length()==0) return;

  if (QMessageBox::question(this,tr("Borrar elemento CI"),
                            tr("¿ Desea borrar el CI '%1' ?").arg(ui.cilineEdit->text()))==QMessageBox::Yes)
     {
        basedatos::instancia()->deleteCi_amortcuentaci( ctaactivo, ui.cilineEdit->text() );
      ui.cilineEdit->clear();
      ui.asignacionlineEdit->clear();
     }
  modeloamortci->select();
  actualizasumaci();
}


void itemplanamort::tablacipulsada()
{
     QModelIndex indiceactual=ui.tableView->currentIndex();
     if (indiceactual.isValid())
       {
        ui.cilineEdit->setText(modeloamortci->record(indiceactual.row()).value("ci").toString());
        double valor=modeloamortci->record(indiceactual.row()).value("asignacion").toDouble()*100;
        QString cadnum;
        cadnum.setNum(valor,'f',4);
        ui.asignacionlineEdit->setText(cadnum);

        ui.asignacionlineEdit->setFocus();
        ui.aceptarcipushButton->setEnabled(false);
       }
}

void itemplanamort::actualizasumaci()
{
    QSqlQuery query = basedatos::instancia()->selectSumasignacion100ci_amortcuenta(ctaactivo);
    if ( (query.isActive()) && (query.first()) )
    {
        if (comadecimal)
            ui.sumalineEdit->setText(convacoma(query.value(0).toString()));
        else
            ui.sumalineEdit->setText(query.value(0).toString());
    }
}


void itemplanamort::cuenta_prov_cambiada()
{
    QString descrip;
  if (esauxiliar(ui.cta_proveedor_lineEdit->text()))
    {
       existecodigoplan(ui.cta_proveedor_lineEdit->text(),&descrip);
       ui.descrip_proveedor_lineEdit->setText(descrip);
  } else ui.descrip_proveedor_lineEdit->setText("");

}

void itemplanamort::externo_cambiado() {
    ui.descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(ui.externo_lineEdit->text()));
}


void itemplanamort::fuerafococta_prov()
{
 QString resultante;

 resultante=expandepunto(ui.cta_proveedor_lineEdit->text(),anchocuentas());
 if (resultante!=ui.cta_proveedor_lineEdit->text())
     ui.cta_proveedor_lineEdit->setText(resultante);

}


void itemplanamort::buscactaprov()
{
    QString cadena2;
    QString inicial;
    // QString descrip;
    inicial=ui.cta_proveedor_lineEdit->text();

    buscasubcuenta *labusqueda=new buscasubcuenta(inicial);
    int resultado;
    resultado=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()==0 || resultado!= QDialog::Accepted) return;
    delete labusqueda;

    if (esauxiliar(cadena2)) {
        ui.cta_proveedor_lineEdit->setText(cadena2);
    }
    else { ui.cta_proveedor_lineEdit->setText("");  ui.descrip_proveedor_lineEdit->setText(""); }

    if (!ui.cta_proveedor_lineEdit->text().isEmpty()) fuerafococta_prov();
}

void itemplanamort::buscaexterno() {
    busca_externo *b = new busca_externo();
    int resultado=b->exec();
    if (resultado==QDialog::Accepted)
       {
        ui.externo_lineEdit->setText(b->codigo_elec());
        //ui.descrip_externo_lineEdit->setText(b->nombre_elec());

       }
    delete(b);

}


void itemplanamort::check_baja_cambiado()
{
    ui.baja_dateEdit->setEnabled(ui.bajacheckBox->isChecked());
    ui.motivo_baja_lineEdit->setEnabled(ui.bajacheckBox->isChecked());
}
