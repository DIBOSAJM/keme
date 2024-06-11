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

#include "concilia.h"
#include "funciones.h"
#include "basedatos.h"
#include <QPixmap>
#include "editafechaasiento.h"
#include "editarasiento.h"
#include "editapaseconci.h"
#include "basedatos.h"
#include "introfecha.h"
#include "tabla_asientos.h"
#include "privilegios.h"
#include "listavenci.h"
#include "contrapartidas.h"
#include "consmayor.h"
#include "cambiacuentapase.h"
#include "edit_conc_doc.h"
#include <QMessageBox>
#include <QProgressDialog>


concilia::concilia(bool qctaexterna,QString qcuenta,QDate inicial, QDate final, QString qusuario) : QDialog() {
 ui.setupUi(this);

 ctaexterna=qctaexterna;
 fichconta=tr("conci_cta");
 fichexterna=tr("conci_ext");
 fichinforme=tr("conci_inform");
 if (ctaexterna)
    {
     ui.adpartidapushButton->hide();
     ui.editalineapushButton->hide();
     ui.borrarpushButton->hide();
    }
    else
         {
          ui.margenlabel->hide();
          ui.margenspinBox->hide();
          ui.automatpushButton->hide();
          //ui.filtrar_label->hide();
          //ui.filtro_lineEdit->hide();
          //ui.criterio_label->hide();
          ui.criterio_comboBox->hide();
          ui.contabilizar_pushButton->hide();
          ui.oculta_pushButton->hide();
          //ui.line->hide();
          //ui.ActuFiltro_pushButton->hide();
          //ui.quitaFiltro_pushButton->hide();
         }

 QString cadena;
  // vamos a cargar la lista del combo box
  diferencias << "- -";
  QSqlQuery q = basedatos::instancia()->selectCodigodif_conciliacionordercodigo();
  if (q.isActive())
  while (q.next())
     {
      diferencias << q.value(0).toString();
     }

 comadecimal=haycomadecimal();
 decimales=haydecimales();
 cambios=false;
 ui.inicialdateEdit->setDate(inicial);
 ui.finaldateEdit->setDate(final);
 cadena=qcuenta;
 cadena+=": ";
 cadena+=descripcioncuenta(qcuenta);
 ui.titulolabel->setText(cadena);
 cuenta=qcuenta;
 QStringList columnas;
 columnas << tr("Asiento") << tr("Pase") << tr("Fecha");
 columnas << tr("Concepto") << tr("Debe") << tr("Haber");
 columnas << tr("Conc") << tr("conc") << tr("Saldo");
 columnas << tr("Tipo dif.") << tr("Diferencia");
 columnas << tr("Saldo Conc.");

 ui.fecha_filtro_dateEdit->setDate(final);

 ui.contabtableWidget->setHorizontalHeaderLabels(columnas);
 ui.contabtableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );

 ui.contabtableWidget->setColumnWidth(0,60);
 ui.contabtableWidget->setColumnWidth(1,60);
 ui.contabtableWidget->setColumnWidth(2,75);
 ui.contabtableWidget->setColumnWidth(3,200);
 ui.contabtableWidget->setColumnWidth(4,80);
 ui.contabtableWidget->setColumnWidth(5,80);
 ui.contabtableWidget->setColumnWidth(6,50);
 ui.contabtableWidget->setColumnWidth(8,80);
 ui.contabtableWidget->setColumnWidth(9,100);
 ui.contabtableWidget->setColumnWidth(10,80);
 ui.contabtableWidget->setColumnWidth(11,80);

 ui.contabtableWidget->hideColumn(7);
 ui.contabtableWidget->hideColumn(10);

 cargadatos();
 if (ctaexterna)
    cargadatosctaext();
  else
      cargadatosdifext();
// -------------------------------------------------------------------------------------
 QStringList columnas2;
 columnas2 << tr("Fecha");
 columnas2 << tr("Concepto") << tr("Debe") << tr("Haber");
 columnas2 << tr("Conc") << tr("conc") << tr("Saldo");
 columnas2 << tr("Tipo dif.") << tr("Diferencia") << tr("Línea");

 ui.externatableWidget->setColumnWidth(0,75);
 ui.externatableWidget->setColumnWidth(1,200);
 ui.externatableWidget->setColumnWidth(2,80);
 ui.externatableWidget->setColumnWidth(3,80);
 ui.externatableWidget->setColumnWidth(4,50);
 ui.externatableWidget->setColumnWidth(5,80);
 ui.externatableWidget->setColumnWidth(6,100);
 ui.externatableWidget->setColumnWidth(7,80);

 ui.externatableWidget->hideColumn(5);
 ui.externatableWidget->hideColumn(8);
 ui.externatableWidget->hideColumn(9);  // línea

 if (!ctaexterna)
    {
     ui.externatableWidget->hideColumn(6);
    }
    else ui.externatableWidget->showColumn(6);

 ui.externatableWidget->setHorizontalHeaderLabels(columnas2);
 ui.externatableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );

// -------------------------------------------------------------------------------------

 connect(ui.contabtableWidget,SIGNAL( cellDoubleClicked ( int, int ) ),this,
           SLOT(marca_y_saldoconci( )));

 connect(ui.externatableWidget,SIGNAL( cellDoubleClicked ( int, int ) ),this,
           SLOT(marcaactualext ( )));

 connect(ui.cancelarpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(salir ( )));

 connect(ui.conciliapushButton,SIGNAL( clicked ( ) ),this,
           SLOT(marca_y_saldoconci ( )));

 connect(ui.fechapushButton,SIGNAL( clicked ( ) ),this,
           SLOT(editafechaasien( )));

 connect(ui.editapushButton,SIGNAL( clicked ( ) ),this,
           SLOT(editaasiento( )));

 connect(ui.refrescarpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(refrescar( )));

 connect(ui.noconcicheckBox,SIGNAL(stateChanged(int)),SLOT(noconcistatus()));

 connect(ui.noconciextcheckBox,SIGNAL(stateChanged(int)),SLOT(noconciextstatus()));

 connect(ui.adpartidapushButton,SIGNAL( clicked ( ) ),this,
           SLOT(nuevopaseexterna()));

 connect(ui.editalineapushButton,SIGNAL( clicked ( ) ),this,
           SLOT(editapaseexterna()));

 connect(ui.conciliaextpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(marcaactualext ( )));

 connect(ui.aceptarpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(botonaceptar()));

 connect(ui.automatpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(concilia_automat()));

connect(ui.imprimecontpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(informecontab()));

connect(ui.imprimeexpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(informeexterna()));

connect(ui.consultainfpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(informeconciliacion()));

connect(ui.imprimeinfpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(imprimeconciliacion()));

connect(ui.eliminapushButton,SIGNAL( clicked ( ) ),this,
           SLOT(quitamarcasconci()));

connect(ui.borrarpushButton,SIGNAL( clicked ( ) ),this,
           SLOT(borrapaseexterna()));

connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));

connect(ui.borrapushButton,SIGNAL(clicked()),SLOT(borra_asiento()));

connect(ui.consultapushButton,SIGNAL(clicked()),SLOT(consulta_asiento()));

connect(ui.nuevopushButton,SIGNAL(clicked()),SLOT(nuevo_asiento()));

connect(ui.vencipushButton,SIGNAL(clicked()),SLOT(vencimientos()));

connect(ui.oculta_pushButton,SIGNAL(clicked()),SLOT(esconde_linea_externa()));

connect(ui.ActuFiltro_pushButton,SIGNAL(clicked()),SLOT(ejecuta_filtro_externa()));

connect(ui.quitaFiltro_pushButton,SIGNAL(clicked()),SLOT(quitafiltro()));

connect(ui.contabilizar_pushButton,SIGNAL(clicked(bool)),SLOT(contabiliza_externa()));

connect(ui.cont_linea_pushButton,SIGNAL(clicked(bool)),SLOT(contabiliza_externa_linea()));

connect(ui.venci_c2_pushButton,SIGNAL(clicked(bool)),SLOT(vencimientos()));

connect(ui.mayor_pushButton,SIGNAL(clicked(bool)),SLOT(consultamayor()));

connect(ui.valor1_lineEdit,SIGNAL(textChanged(QString)),SLOT(valor1cambiado()));

connect(ui.conc_doc_pushButton,SIGNAL(clicked(bool)),SLOT(edcondoc()));

connect(ui.cambia_cuenta_pushButton,SIGNAL(clicked(bool)),SLOT(cambiacuentaapunte()));

usuario=qusuario;

QString privileg=basedatos::instancia()->privilegios(qusuario);

if (privileg[nuev_asiento]=='0') ui.nuevopushButton->setEnabled(false);
if (privileg[edi_asiento]=='0') ui.editapushButton->setEnabled(false);
if (privileg[borrar_asiento]=='0') ui.borrapushButton->setEnabled(false);
// if (privileg[edit_fecha_asiento]=='0') ui.fechapushButton->setEnabled(false);

#ifdef NOEDITTEX
  ui.latexpushButton->hide();
#endif


}


void concilia::marcaactual()
{
 int fila=0;
 if (ui.contabtableWidget->currentItem()!=0)
    fila=ui.contabtableWidget->currentRow();
 else return;
 QLabel *etiqueta=(QLabel*)ui.contabtableWidget->cellWidget(fila,6);
 if (ui.contabtableWidget->item(fila,7)->text()=="0")
     {
      etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
      ui.contabtableWidget->item(fila,7)->setText("1");
      QComboBox  *comboitem=(QComboBox*)ui.contabtableWidget->cellWidget(fila,9);
      ui.contabtableWidget->removeCellWidget (fila, 9 );
      delete (comboitem);
      if (ui.noconcicheckBox->isChecked())
         ui.contabtableWidget->hideRow(fila);
     }
     else
         {
          etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
          ui.contabtableWidget->item(fila,7)->setText("0");

          QComboBox  *comboitem = new QComboBox();
          comboitem->addItems(diferencias);
          ui.contabtableWidget->setCellWidget ( fila, 9, comboitem );
         }
 cambios=true;
}

void concilia::marca_y_saldoconci()
{
    marcaactual();
    actualiza_saldos_conciliado();
}

void concilia::marca(int fila)
{
 QLabel *etiqueta=(QLabel*)ui.contabtableWidget->cellWidget(fila,6);
 if (ui.contabtableWidget->item(fila,7)->text()=="0")
     {
      etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
      ui.contabtableWidget->item(fila,7)->setText("1");
      QComboBox  *comboitem=(QComboBox*)ui.contabtableWidget->cellWidget(fila,9);
      ui.contabtableWidget->removeCellWidget (fila, 9 );
      delete (comboitem);
      if (ui.noconcicheckBox->isChecked())
         ui.contabtableWidget->hideRow(fila);
     }
 cambios=true;
}



void concilia::salir()
{
  if (cambios || cambiosdifconta() || cambiosdifext())
  {
   if (QMessageBox::question(
            this,
            tr("Abandonar conciliación"),
            tr("Hay cambios, ¿ desea salir ?")) ==QMessageBox::Yes )
                          accept();
         else return;
  }
  accept();
}


void concilia::editafechaasien()
{
   if (ui.contabtableWidget->currentItem()==0)
       {
         QMessageBox::warning( this, tr("CAMBIAR FECHA EN PASE"),
         tr("Para cambiar una fecha debe de seleccionar una celda de la línea correspondiente"));
         return;
       }
  int fila=ui.contabtableWidget->currentRow();
  QString asiento=asientoabsdepase(ui.contabtableWidget->item(fila,1)->text());
  QDate qfecha=basedatos::instancia()->select_fecha_diariopase(ui.contabtableWidget->item(fila,1)->text());

  editafechaasiento *e = new editafechaasiento();

  e->pasadatos(asiento , qfecha );

  int resultado=e->exec();
  delete(e);
  if (resultado==QDialog::Rejected) return;

  // ui.contabtableWidget->removeRow(fila);
  // tendríamos que borrar línea y luego cargar + actualizar (nuevo)

  /*actucuenta();

  ui.contabtableWidget->clearContents();
  ui.contabtableWidget->setRowCount(0);

  cargadatos();*/

  // borramos asiento
  int asientosel=asiento.toInt();
  for (int veces=0; veces<ui.contabtableWidget->rowCount(); veces++)
     {
      int numasiento=ui.contabtableWidget->item(veces,0)->text().toInt();
      if (numasiento==asientosel) { ui.contabtableWidget->removeRow(veces); if (veces>0) veces--;}
     }

  actualiza_tras_edicion();

  // cargadatosdifext();
  if (fila<ui.contabtableWidget->rowCount())
    ui.contabtableWidget->setCurrentCell(fila,0);
}


void concilia::vencimientos()
{
    int fila=ui.contabtableWidget->currentRow();
    listavenci *l = new listavenci(usuario);
    l->exec();
    delete(l);
    //actucuenta();

    //ui.contabtableWidget->clearContents();
    //ui.contabtableWidget->setRowCount(0);

   // cargadatos();
    // cargadatosdifext();

    actualiza_tras_edicion();

    if (fila<ui.contabtableWidget->rowCount())
      ui.contabtableWidget->setCurrentCell(fila,0);
}

void concilia::cargadatos()
{
   QProgressDialog progreso(tr("Cargando información ..."), 0, 0, 2);
    progreso.setMinimumDuration(0);
    progreso.setValue(1);
    progreso.show();
    QCoreApplication::processEvents();
  // calculamos acumulado de saldo inicial
    // primero observamos si el ejercicio anterior está cerrado
    QString ejercicio=ejerciciodelafecha(ui.inicialdateEdit->date());
    QDate fechainiejercicio=inicioejercicio(ejercicio);
    QDate fechaejercicioanterior=fechainiejercicio.addDays(-1);
    QString ejercicioanterior=ejerciciodelafecha(fechaejercicioanterior);

    QSqlQuery cons;
    if (ejerciciocerrado(ejercicioanterior) || escuentadegasto(cuenta) || escuentadeingreso(cuenta))
       {
        cons = basedatos::instancia()->selectSumdebesumhaberdiariofechascuenta(fechainiejercicio, ui.inicialdateEdit->date() , cuenta);
       }
       else cons = basedatos::instancia()->selectSumdebesumhaberdiariofechacuenta(ui.inicialdateEdit->date(), cuenta);

   // double saldoin=0;
   double sumadebe=0;
   double sumahaber=0;
   if ( (cons.isActive()) && (cons.first()) )
      {
        //saldoin=cons.value(0).toDouble()-cons.value(1).toDouble();
        sumadebe=cons.value(0).toDouble();
        sumahaber=cons.value(1).toDouble();
      }

 QSqlQuery query = basedatos::instancia()->select8Diariocuentafechasorder(cuenta , ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

 int fila=0;
 if (query.isActive())
 while (query.next())
     {
      QCoreApplication::processEvents();
      ui.contabtableWidget->insertRow(fila);
      for (int veces=0;veces<7;veces++)
             {
              if (veces==6) 
                {
                 QLabel  *etiqueta = new QLabel();
                 etiqueta->setAlignment(Qt::AlignHCenter);
                 QString contenido;
                 if (query.value(veces).toBool())
                    {
                     etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
                     ui.contabtableWidget->setCellWidget(fila,veces,etiqueta);
                     contenido="1";
                    }
                    else
                         {
                          etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
                          contenido="0";
                          ui.contabtableWidget->setCellWidget(fila,veces,etiqueta);
                         }
                 QTableWidgetItem *newItem = new QTableWidgetItem(contenido);
                 ui.contabtableWidget->setItem(fila,7,newItem);
                 continue;
                }
              QString contenido=query.value(veces).toString();
              if (veces==4 || veces==5)
                 {
                  contenido=formateanumero(query.value(veces).toDouble(),comadecimal,decimales);
                  if (veces==4) sumadebe+=query.value(veces).toDouble();
                  if (veces==5) sumahaber+=query.value(veces).toDouble();
                 }
              if (veces==2) contenido=query.value(2).toDate().toString("dd.MM.yyyy");
              QTableWidgetItem *newItem = new QTableWidgetItem(contenido);
              if (veces==0 || veces==1 || veces==4 || veces==5)
                 newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.contabtableWidget->setItem(fila,veces,newItem);
             }
      QTableWidgetItem *newItem = new QTableWidgetItem(formateanumero(sumadebe-sumahaber,
                                                        comadecimal,decimales));
      newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.contabtableWidget->setItem(fila,8,newItem);
      if (!query.value(6).toBool())
         {
          // escribimos la columna 10 con dif conciliación
          QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(7).toString());
          ui.contabtableWidget->setItem(fila,10,newItem2);
          // --------------------------------------------------
          QComboBox  *comboitem = new QComboBox();
          comboitem->addItems(diferencias);
          if (query.value(7).toString().length()>0)
            {
             for (int veces=0;veces<comboitem->count();veces++)
              {
               if (comboitem->itemText(veces)==query.value(7).toString())
                  {
                   comboitem->setCurrentIndex(veces);
                   break;
                  }
              }
            }
          ui.contabtableWidget->setCellWidget ( fila, 9, comboitem );
         }
         else  if (ui.noconcicheckBox->isChecked()) ui.contabtableWidget->hideRow(fila);
      QTableWidgetItem *newItem11 = new QTableWidgetItem("");
      newItem11->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.contabtableWidget->setItem(fila,11,newItem11);
      fila++;
     }
 if (ui.contabtableWidget->rowCount()<1) return;
 actualiza_saldos_conciliado();
}


void concilia::actucuenta()
{
    QProgressDialog progreso(tr("Actualizando información ..."), 0, 0, ui.contabtableWidget->rowCount()-1);
     progreso.setMinimumDuration(100);
     progreso.setValue(1);
     progreso.show();
     QCoreApplication::processEvents();

    QString cad;
    bool conciliado;
    for (int veces=0;veces<ui.contabtableWidget->rowCount();veces++)
    {
        progreso.setValue(veces+1);
        if (ui.contabtableWidget->item(veces,7)->text()=="1") {
            cad="";
            conciliado = true;
        }
        else {
            QComboBox  *comboitem=(QComboBox*)ui.contabtableWidget->cellWidget(veces,9);
            cad=comboitem->currentText();
            if (cad=="- -") cad="";
            conciliado = false;
        }

        basedatos::instancia()->updateDiarioconciliadodif_conciliacionpase(conciliado,cad, ui.contabtableWidget->item(veces,1)->text() );
        QCoreApplication::processEvents();
    }
}

void concilia::editaasiento()
{
   if (ui.contabtableWidget->currentItem()==0)
       {
         QMessageBox::warning( this, tr("CAMBIAR FECHA EN PASE"),
         tr("Para cambiar una fecha debe de seleccionar una celda de la línea correspondiente"));
         return;
       }
  int fila=ui.contabtableWidget->currentRow();
  QString asiento=asientoabsdepase(ui.contabtableWidget->item(fila,1)->text());
  QDate fecha=basedatos::instancia()->select_fecha_diariopase (
          ui.contabtableWidget->item(fila,1)->text());

  QString ejercicio=ejerciciodelafecha(fecha);
  QString qdiario;
  qdiario=diariodeasiento(asiento,ejercicio);
 
    if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
       {
         QMessageBox::warning( this, tr("Conciliación"),
			       tr("ERROR: No se pueden editar asientos del diario de apertura, "
				  "regularización o cierre\n"));
         return;
       }

    if (!existeasiento(asiento,ejercicio))
       {
         QMessageBox::warning( this, tr("Conciliación"),
			       tr("ERROR: El asiento seleccionado ya no existe"));
         return;
       }

  if  (!editarasiento(asiento, usuario,ejercicio)) return;

  // borramos asiento
  int asientosel=asiento.toInt();
  for (int veces=0; veces<ui.contabtableWidget->rowCount(); veces++)
     {
      int numasiento=ui.contabtableWidget->item(veces,0)->text().toInt();
      if (numasiento==asientosel) { ui.contabtableWidget->removeRow(veces); if (veces>0) veces--;}
     }

  /* actucuenta();

  ui.contabtableWidget->clearContents();
  ui.contabtableWidget->setRowCount(0);

  cargadatos();*/

  actualiza_tras_edicion();

  if (fila<ui.contabtableWidget->rowCount())
     ui.contabtableWidget->setCurrentCell(fila,0);
}


void concilia::refrescar()
{
  int fila=ui.contabtableWidget->currentRow();

  actucuenta();
  if (ctaexterna)
     grabactaext();
    else
        grabadifsext();

  ui.contabtableWidget->clearContents();
  ui.contabtableWidget->setRowCount(0);

  ui.externatableWidget->clearContents();
  ui.externatableWidget->setRowCount(0);


  cargadatos();

  QProgressDialog progreso(tr("Cargando cuenta externa ..."), 0, 0, 0);
  progreso.setMinimumDuration(0);
  progreso.show();

  QCoreApplication::processEvents();

  if (ctaexterna)
     cargadatosctaext();
    else
      cargadatosdifext();

  if (fila<ui.contabtableWidget->rowCount())
     ui.contabtableWidget->setCurrentCell(fila,0);
}

bool concilia::cambiosdifconta()
{
  for (int veces=0;veces<ui.contabtableWidget->rowCount();veces++)
      {
       if (ui.contabtableWidget->cellWidget(veces,9)==0) continue;
       QComboBox  *comboitem=(QComboBox*)ui.contabtableWidget->cellWidget(veces,9);
       QString cadena=comboitem->currentText();
       if (cadena=="- -") cadena="";
       if (ui.contabtableWidget->item(veces,10)!=0)
          if (cadena!=ui.contabtableWidget->item(veces,10)->text()) return true;
      }
  return false;
}

bool concilia::cambiosdifext()
{
  for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
      {
       if (ui.externatableWidget->cellWidget(veces,7)==0) continue;
       QComboBox  *comboitem=(QComboBox*)ui.externatableWidget->cellWidget(veces,7);
       QString cadena=comboitem->currentText();
       if (cadena=="- -") cadena="";
       if (ui.externatableWidget->item(veces,8)!=0)
          if (cadena!=ui.externatableWidget->item(veces,8)->text()) return true;
      }
  return false;
}

void concilia::noconcistatus()
{
  if (ui.noconcicheckBox->isChecked())
    {
     for (int veces=0;veces<ui.contabtableWidget->rowCount();veces++)
      {
       if (ui.contabtableWidget->item(veces,7)->text()=="1")
          ui.contabtableWidget->hideRow(veces);
      }
    }
    else
        {
         for (int veces=0;veces<ui.contabtableWidget->rowCount();veces++)
            {
               ui.contabtableWidget->showRow(veces);
            }
        }
}


void concilia::noconciextstatus()
{
  if (ui.noconciextcheckBox->isChecked())
    {
     for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
      {
       if (ui.externatableWidget->item(veces,5)->text()=="1")
          ui.externatableWidget->hideRow(veces);
        else  ui.externatableWidget->showRow(veces);
      }
    }
    else
        {
         for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
            {
               ui.externatableWidget->showRow(veces);
            }
        }
}

void concilia::quitafiltro()
{
    ui.filtro_lineEdit->clear();
    ui.fecha_sel_comboBox->setCurrentIndex(0);
    ui.valor1_lineEdit->clear();
    noconciextstatus();
}

void concilia::esconde_linea_externa()
{
    int fila=ui.externatableWidget->currentRow();
    if (fila<0 || fila>ui.externatableWidget->rowCount()-1) return;
    ui.externatableWidget->hideRow(fila);
}


void concilia::ejecuta_filtro_externa()
{
    if (ui.criterio_comboBox->currentIndex()==0)
      {
       for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
        {
         if (!ui.externatableWidget->item(veces,1)->text().trimmed().startsWith(ui.filtro_lineEdit->text(),Qt::CaseInsensitive)) // Qt::CaseInsensitive
            ui.externatableWidget->hideRow(veces);
          else ui.externatableWidget->showRow(veces);
         if (ui.externatableWidget->item(veces,5)->text()=="1" && ui.noconciextcheckBox->isChecked())
            ui.externatableWidget->hideRow(veces);
        }
      }
      else
          {
           for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
             {
               if (!ui.externatableWidget->item(veces,1)->text().trimmed().contains(ui.filtro_lineEdit->text(),Qt::CaseInsensitive)) // Qt::CaseInsensitive
                 ui.externatableWidget->hideRow(veces);
               else ui.externatableWidget->showRow(veces);
               if (ui.externatableWidget->item(veces,5)->text()=="1" && ui.noconciextcheckBox->isChecked())
                  ui.externatableWidget->hideRow(veces);
             }
           }

    if (ui.fecha_sel_comboBox->currentIndex()>0) {
        for (int veces=0;veces<ui.externatableWidget->rowCount();veces++) {
            QDate fecha_puente;
            fecha_puente=fecha_puente.fromString(ui.externatableWidget->item(veces,0)->text(),"dd.MM.yyyy");
            int pos=ui.fecha_sel_comboBox->currentIndex();
            //QMessageBox::warning( this, tr("Conciliación"),ui.externatableWidget->item(veces,0)->text());
            //QMessageBox::warning( this, tr("Conciliación"),ui.fecha_filtro_dateEdit->date().toString("dd-MM-yyyy"));
            switch (pos) {
               case 1:
                // igual
                if (fecha_puente!=ui.fecha_filtro_dateEdit->date())
                   ui.externatableWidget->hideRow(veces);
                break;
               case 2:
                // mayor
                if (fecha_puente<ui.fecha_filtro_dateEdit->date())
                   ui.externatableWidget->hideRow(veces);
                break;
               case 3:
                // menor
                if (fecha_puente>ui.fecha_filtro_dateEdit->date())
                   ui.externatableWidget->hideRow(veces);
                break;
            }
         }
        }

    if (!ui.valor1_lineEdit->text().isEmpty() || !ui.valor2_lineEdit->text().isEmpty()) {
        double valor1=convapunto(ui.valor1_lineEdit->text()).toDouble();
        double valor2=convapunto(ui.valor2_lineEdit->text()).toDouble();
        int col=2+ui.tipo_importe_comboBox->currentIndex();
        for (int veces=0;veces<ui.externatableWidget->rowCount();veces++){
          if (convapunto(ui.externatableWidget->item(veces,col)->text()).toDouble()<(valor1-0.001)) ui.externatableWidget->hideRow(veces);
          if (convapunto(ui.externatableWidget->item(veces,col)->text()).toDouble()>(valor2+0.001)) ui.externatableWidget->hideRow(veces);
        }
    }
}

void concilia::nuevopaseexterna()
{
  QDate fecha;
  QString descripcion;
  QString debe;
  QString haber;
  editapaseconci *e = new editapaseconci();
  int resultado=e->exec();
  if (resultado==QDialog::Accepted)
     {
      e->recuperadatos(&fecha,&descripcion,&debe,&haber);
      int fila=0;
      while (fila<ui.externatableWidget->rowCount())
           {
            QDate fechat=fechat.fromString(ui.externatableWidget->item(fila,0)->text(),"dd.MM.yyyy");
            if (fechat>fecha) break;
            fila++;
           }
      ui.externatableWidget->insertRow(fila);
      QTableWidgetItem *newItem  = new QTableWidgetItem(fecha.toString("dd.MM.yyyy"));
      QTableWidgetItem *newItem2 = new QTableWidgetItem(descripcion);
      double valdebe=convapunto(debe).toDouble();
      QTableWidgetItem *newItem3 = new QTableWidgetItem(formateanumero(valdebe,comadecimal,decimales));
      newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      double valhaber=convapunto(haber).toDouble();
      QTableWidgetItem *newItem4 = new QTableWidgetItem(formateanumero(valhaber,comadecimal,decimales));
      newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.externatableWidget->setItem(fila,0,newItem);
      ui.externatableWidget->setItem(fila,1,newItem2);
      ui.externatableWidget->setItem(fila,2,newItem3);
      ui.externatableWidget->setItem(fila,3,newItem4);

      QLabel  *etiqueta = new QLabel();
      etiqueta->setAlignment(Qt::AlignHCenter);
      etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
      ui.externatableWidget->setCellWidget(fila,4,etiqueta);
      QTableWidgetItem *newItemj = new QTableWidgetItem("0");
      ui.externatableWidget->setItem(fila,5,newItemj);

      QTableWidgetItem *newItem8 = new QTableWidgetItem("");
      ui.externatableWidget->setItem(fila,8,newItem8);
      // --------------------------------------------------
      QComboBox  *comboitem = new QComboBox();
      comboitem->addItems(diferencias);
      ui.externatableWidget->setCellWidget ( fila, 7, comboitem );

     }
  delete(e);
  cambios=true;
}


void concilia::marcaactualext()
{
 int fila=0;
 if (ui.externatableWidget->currentItem()!=0)
    fila=ui.externatableWidget->currentRow();
 else return;
 QLabel *etiqueta=(QLabel*)ui.externatableWidget->cellWidget(fila,4);
 if (ui.externatableWidget->item(fila,5)->text()=="0")
     {
      etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
      ui.externatableWidget->item(fila,5)->setText("1");
      QComboBox  *comboitem=(QComboBox*)ui.externatableWidget->cellWidget(fila,7);
      ui.externatableWidget->removeCellWidget (fila, 7 );
      delete (comboitem);
      if (ui.noconciextcheckBox->isChecked())
         ui.externatableWidget->hideRow(fila);
     }
     else
         {
          etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
          ui.externatableWidget->item(fila,5)->setText("0");

          QComboBox  *comboitem = new QComboBox();
          comboitem->addItems(diferencias);
          ui.externatableWidget->setCellWidget ( fila, 7, comboitem );
         }
 cambios=true;
}

void concilia::marcaext(int fila)
{
 QLabel *etiqueta=(QLabel*)ui.externatableWidget->cellWidget(fila,4);
 if (ui.externatableWidget->item(fila,5)->text()=="0")
     {
      etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
      ui.externatableWidget->item(fila,5)->setText("1");
      QComboBox  *comboitem=(QComboBox*)ui.externatableWidget->cellWidget(fila,7);
      ui.externatableWidget->removeCellWidget (fila, 7 );
      delete (comboitem);
      if (ui.noconciextcheckBox->isChecked())
         ui.externatableWidget->hideRow(fila);
     }
 cambios=true;
}

void concilia::editapaseexterna()
{
 int fila=0;
 if (ui.externatableWidget->currentItem()!=0)
    fila=ui.externatableWidget->currentRow();
 else return;

 if (ui.externatableWidget->item(fila,5)->text()=="1")
   {
     QMessageBox::warning( this, tr("EDITAR PASE"),
         tr("ERROR: pase conciliado no editable"));
         return;
   }

  QDate fecha=fecha.fromString(ui.externatableWidget->item(fila,0)->text(),"dd.MM.yyyy");
  QString descripcion=ui.externatableWidget->item(fila,1)->text();
  QString debe=ui.externatableWidget->item(fila,2)->text();
  QString haber=ui.externatableWidget->item(fila,3)->text();

  editapaseconci *e = new editapaseconci();
  e->pasadatos(fecha,descripcion,debe,haber);
  int resultado=e->exec();
  if (resultado==QDialog::Accepted)
     {
      e->recuperadatos(&fecha,&descripcion,&debe,&haber);
      ui.externatableWidget->item(fila,0)->setText(fecha.toString("dd.MM.yyyy"));
      ui.externatableWidget->item(fila,1)->setText(descripcion);
      double valdebe=convapunto(debe).toDouble();
      ui.externatableWidget->item(fila,2)->setText(formateanumero(valdebe,comadecimal,decimales));
      double valhaber=convapunto(haber).toDouble();
      ui.externatableWidget->item(fila,3)->setText(formateanumero(valhaber,comadecimal,decimales));
     }
  delete(e);
  cambios=true;
}

void concilia::grabadifsext()
{
  // se utiliza exclusivamente para guardar líneas de diferencias creadas por el usuario
  // en la cuenta externa

  // primero eliminamos información entre fechas antigua

    QProgressDialog progreso(tr("Actualizando información ..."), 0, 0, ui.externatableWidget->rowCount());
    progreso.setMinimumDuration(0);
    progreso.show();

    QString fecha, dif;
    bool conci;

    basedatos::instancia()->deleteAjustes_concifechascuenta(ui.inicialdateEdit->date() , ui.finaldateEdit->date() , cuenta);

  for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
   {
      progreso.setValue(veces+1);
      QCoreApplication::processEvents();
        QDate fechat=fechat.fromString(ui.externatableWidget->item(veces,0)->text(),"dd.MM.yyyy");
        fecha = fechat.toString("yyyy-MM-dd");

        if (ui.externatableWidget->item(veces,5)->text()!="1")
        {
            QComboBox  *comboitem=(QComboBox*)ui.externatableWidget->cellWidget(veces,7);
            dif=comboitem->currentText();
            if (dif=="- -") dif="";
        }

        if (ui.externatableWidget->item(veces,5)->text()=="1")
            conci = true;
        else conci = false;

        basedatos::instancia()->insertAjustes_conci(cuenta,fecha , ui.externatableWidget->item(veces,1)->text() , convapunto(ui.externatableWidget->item(veces,2)->text()) , convapunto(ui.externatableWidget->item(veces,3)->text()) , dif, conci);
   }
}


void concilia::botonaceptar()
{
 
  if (cambios || cambiosdifconta())
     {
      actucuenta();
      if (ctaexterna)
         grabactaext();
        else
           grabadifsext();
     }
  accept();
}


void concilia::cargadatosdifext()
{
// carga registros de diferencias generados por el usuario

 QSqlQuery query = basedatos::instancia()->select6Ajustes_concicuentafechasorderfecha(cuenta, ui.inicialdateEdit->date(), ui.finaldateEdit->date());

 int fila=0;
 if (query.isActive())
 while (query.next())
     {
      QCoreApplication::processEvents();
      ui.externatableWidget->insertRow(fila);
      QString contenido=query.value(0).toDate().toString("dd.MM.yyyy");
      QTableWidgetItem *newItem = new QTableWidgetItem(contenido);
      ui.externatableWidget->setItem(fila,0,newItem);
      contenido=query.value(1).toString();
      ui.externatableWidget->setItem(fila,1,new QTableWidgetItem(contenido));

      contenido=formateanumero(query.value(2).toDouble(),comadecimal,decimales);
      QTableWidgetItem *newItem2 = new QTableWidgetItem(contenido);
      newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.externatableWidget->setItem(fila,2,newItem2);

      contenido=formateanumero(query.value(3).toDouble(),comadecimal,decimales);
      QTableWidgetItem *newItem3 = new QTableWidgetItem(contenido);
      newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.externatableWidget->setItem(fila,3,newItem3);

      QLabel  *etiqueta = new QLabel();
      etiqueta->setAlignment(Qt::AlignHCenter);

      if (query.value(4).toBool())
          {
           etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
           ui.externatableWidget->setCellWidget(fila,4,etiqueta);
           contenido="1";
          }
          else
              {
                etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
                contenido="0";
                ui.externatableWidget->setCellWidget(fila,4,etiqueta);
              }

      ui.externatableWidget->setItem(fila,5,new QTableWidgetItem(contenido));

      if (!query.value(4).toBool())
         {
          // escribimos la columna 8 con dif conciliación
          QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(5).toString());
          ui.externatableWidget->setItem(fila,8,newItem2);
          // --------------------------------------------------
          QComboBox  *comboitem = new QComboBox();
          comboitem->addItems(diferencias);
          if (query.value(5).toString().length()>0)
            {
             for (int veces=0;veces<comboitem->count();veces++)
              {
               if (comboitem->itemText(veces)==query.value(5).toString())
                  {
                   comboitem->setCurrentIndex(veces);
                   break;
                  }
              }
            }
          ui.externatableWidget->setCellWidget ( fila, 7, comboitem );
         }
         else  if (ui.noconciextcheckBox->isChecked()) ui.externatableWidget->hideRow(fila);
      fila++;
     }
}


void concilia::cargadatosctaext()
{
// carga registros de la cuenta externa importada, almacenada en 'cuenta_ext_conci'

 QSqlQuery query = basedatos::instancia()->select8Cuenta_ext_concicuentafechasordernumero( cuenta, ui.inicialdateEdit->date(), ui.finaldateEdit->date() );
 // columnas: saldo 6, linea 9
 int fila=0;
 bool haysaldo=false;
 while (query.next())
     {
       QCoreApplication::processEvents();
      ui.externatableWidget->insertRow(fila);
      haysaldo=(query.value(6).toDouble()>0.001 || query.value(6).toDouble()<-0.001);
      for (int veces=0;veces<8;veces++)
             {
              if (veces==4) 
                {
                 QLabel  *etiqueta = new QLabel();
                 etiqueta->setAlignment(Qt::AlignHCenter);
                 QString contenido;
                 if (query.value(veces).toBool())
                    {
                     etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
                     ui.externatableWidget->setCellWidget(fila,veces,etiqueta);
                     contenido="1";
                    }
                    else
                         {
                          etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
                          contenido="0";
                          ui.externatableWidget->setCellWidget(fila,veces,etiqueta);
                         }
                 QTableWidgetItem *newItem = new QTableWidgetItem(contenido);
                 ui.externatableWidget->setItem(fila,5,newItem);
                 continue;
                }
              QString contenido=query.value(veces).toString();
              if (veces==2 || veces==3 || veces==6)
                 {
                  contenido=formateanumero(query.value(veces).toDouble(),comadecimal,decimales);
                 }
              if (veces==0) contenido=query.value(0).toDate().toString("dd.MM.yyyy");
              QTableWidgetItem *newItem = new QTableWidgetItem(contenido);
              if (veces==2 || veces==3 || veces==6)
                 newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              if (veces==7) {ui.externatableWidget->setItem(fila,9,newItem); continue;}
              if (veces!=5) ui.externatableWidget->setItem(fila,veces,newItem);
             }
      if (!query.value(4).toBool())
         {
          // escribimos la columna con dif conciliación
          QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(5).toString());
          ui.externatableWidget->setItem(fila,8,newItem2);
          // --------------------------------------------------
          QComboBox  *comboitem = new QComboBox();
          comboitem->addItems(diferencias);
          if (query.value(5).toString().length()>0)
            {
             for (int veces=0;veces<comboitem->count();veces++)
              {
               if (comboitem->itemText(veces)==query.value(5).toString())
                  {
                   comboitem->setCurrentIndex(veces);
                   break;
                  }
              }
            }
          ui.externatableWidget->setCellWidget ( fila, 7, comboitem );
         }
         else  if (ui.noconciextcheckBox->isChecked()) ui.externatableWidget->hideRow(fila);
      fila++;
     }
  if (!haysaldo) ui.externatableWidget->hideColumn(6);

}



void concilia::grabactaext()
{
  // se utiliza exclusivamente para actualizar la tabla con el fichero importado
  // de la cuenta externa

    QProgressDialog progreso(tr("Actualizando información ..."), 0, 0, ui.externatableWidget->rowCount());
    progreso.setMinimumDuration(0);
    progreso.show();

    bool conciliado;
    QString dif;

    QCoreApplication::processEvents();

  for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
     {
        progreso.setValue(veces);
        QCoreApplication::processEvents();
        if (ui.externatableWidget->item(veces,5)->text()=="1")
            conciliado = true;
        else conciliado = false;

        if (ui.externatableWidget->item(veces,5)->text()!="1")
        {
            QComboBox  *comboitem=(QComboBox*)ui.externatableWidget->cellWidget(veces,7);
            dif=comboitem->currentText();
            if (dif=="- -") dif="";
        }

        basedatos::instancia()->updateCuenta_ext_conciconciliadodif_conciliacionnumero( conciliado, dif, ui.externatableWidget->item(veces,9)->text() );

     }
}

void concilia::concilia_automat()
{
 int tope=ui.externatableWidget->rowCount();
 if (ui.margenspinBox->value()>0) tope=tope*2;
 QProgressDialog progreso(tr("Conciliando cuenta ..."), 0, 0, tope);

  // primera pasada; buscamos equivalencia perfecta entre fechas e importes
  for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
     {
      // captamos fecha (0), debe (2) y haber (3)
      progreso.setValue(veces+1);
      QApplication::processEvents();
      if (ui.externatableWidget->item(veces,5)->text()=="1") continue;
      QDate fecha=fecha.fromString(ui.externatableWidget->item(veces,0)->text(),"dd.MM.yyyy");
      double debe=convapunto(ui.externatableWidget->item(veces,2)->text()).toDouble();
      double haber=convapunto(ui.externatableWidget->item(veces,3)->text()).toDouble();
      for (int veces2=0;veces2<ui.contabtableWidget->rowCount();veces2++)
          {
            // fecha (2), debe (4), haber (5)
            QDate fechac=fechac.fromString(ui.contabtableWidget->item(veces2,2)->text(),"dd.MM.yyyy");
            double debe2=convapunto(ui.contabtableWidget->item(veces2,4)->text()).toDouble();
            double haber2=convapunto(ui.contabtableWidget->item(veces2,5)->text()).toDouble();
            if (fechac!=fecha) continue;
            if (debe>0.001 || debe <-0.001)
               {
                if (haber2-debe<0.001 && haber2-debe>-0.001)
                   {
                     // comprobamos que no hayan marcas de conciliado en uno u otro registro
                     if (ui.contabtableWidget->item(veces2,7)->text()=="1") continue;
                     // marcamos como conciliados
                     marcaext(veces);
                     marca(veces2);
                     break;
                   }
               }
            if (haber>0.001 || haber <-0.001)
               {
                if (debe2-haber<0.001 && debe2-haber>-0.001)
                   {
                     // comprobamos que no hayan marcas de conciliado en uno u otro registro
                     if (ui.contabtableWidget->item(veces2,7)->text()=="1") continue;
                     // marcamos como conciliados
                     marcaext(veces);
                     marca(veces2);
                     break;
                   }
               }
          }
     }

  
  // segunda pasada; nos conformamos con diferencia de fechas menor o igual que el spinbox
  if (ui.margenspinBox->value()==0) {actualiza_saldos_conciliado(); return;}
  int inicio=ui.externatableWidget->rowCount();
  for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
     {
      // captamos fecha (0), debe (2) y haber (3)
      progreso.setValue(inicio+veces+1);
      QApplication::processEvents();
      if (ui.externatableWidget->item(veces,5)->text()=="1") continue;
      QDate fecha=fecha.fromString(ui.externatableWidget->item(veces,0)->text(),"dd.MM.yyyy");
      double debe=convapunto(ui.externatableWidget->item(veces,2)->text()).toDouble();
      double haber=convapunto(ui.externatableWidget->item(veces,3)->text()).toDouble();
      for (int veces2=0;veces2<ui.contabtableWidget->rowCount();veces2++)
          {
            // fecha (2), debe (4), haber (5)
            QDate fechac=fechac.fromString(ui.contabtableWidget->item(veces2,2)->text(),"dd.MM.yyyy");
            double debe2=convapunto(ui.contabtableWidget->item(veces2,4)->text()).toDouble();
            double haber2=convapunto(ui.contabtableWidget->item(veces2,5)->text()).toDouble();
            if (fechac.daysTo(fecha)>ui.margenspinBox->value()) continue;
            if (fechac.daysTo(fecha)<-ui.margenspinBox->value()) continue;
            if (debe>0.001 || debe <-0.001)
               {
                if (haber2-debe<0.001 && haber2-debe>-0.001)
                   {
                     // comprobamos que no hayan marcas de conciliado en uno u otro registro
                     if (ui.contabtableWidget->item(veces2,7)->text()=="1") continue;
                     // marcamos como conciliados
                     marca(veces2);
                     marcaext(veces);
                     break;
                   }
               }
            if (haber>0.001 || haber <-0.001)
               {
                if (debe2-haber<0.001 && debe2-haber>-0.001)
                   {
                     // comprobamos que no hayan marcas de conciliado en uno u otro registro
                     if (ui.contabtableWidget->item(veces2,7)->text()=="1") continue;
                     // marcamos como conciliados
                     marca(veces2);
                     marcaext(veces);
                     break;
                   }
               }
          }
     }
  actualiza_saldos_conciliado();
}


void concilia::latexcontab()
{

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=fichconta;
   qfichero+=".tex";
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << cuenta << ": " << filtracad(descripcioncuenta(cuenta)) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|r|r|c|p{4cm}|r|r|c|r|l|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{9}{|c|} {\\textbf{";
   QString cadena;
   cadena=tr("EXTRACTO CONCILIACIÓN CUENTA");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // -------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("ASTO.") << "}} & ";
    stream << "{\\tiny{" << tr("Pase") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("DEBE") << "}} & ";
    stream << "{\\tiny{" << tr("HABER") << "}} & ";
    stream << "{\\tiny{" << tr("Conc.") << "}} & ";
    stream << "{\\tiny{" << tr("Saldo") << "}} & ";
    stream << "{\\tiny{" << tr("Diferencia") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    // stream << tr(" \\\\") << "\n";
    stream << "{\\tiny{" << tr("ASTO.") << "}} & ";
    stream << "{\\tiny{" << tr("Pase") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("DEBE") << "}} & ";
    stream << "{\\tiny{" << tr("HABER") << "}} & ";
    stream << "{\\tiny{" << tr("Conc.") << "}} & ";
    stream << "{\\tiny{" << tr("Saldo") << "}} & ";
    stream << "{\\tiny{" << tr("Diferencia") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ---------------------------------------------------------------------------------------

    for (int veces=0;veces<ui.contabtableWidget->rowCount();veces++)
       {
         stream << "{\\tiny " << filtracad(ui.contabtableWidget->item(veces,0)->text()) << "} & ";
         stream << "{\\tiny " << filtracad(ui.contabtableWidget->item(veces,1)->text()) << "} & ";
         stream << "{\\tiny " << filtracad(ui.contabtableWidget->item(veces,2)->text()) << "} & ";
         stream << "{\\tiny " << filtracad(ui.contabtableWidget->item(veces,3)->text()) << "} & ";
         stream << "{\\tiny " << filtracad(ui.contabtableWidget->item(veces,4)->text()) << "} & ";
         stream << "{\\tiny " << filtracad(ui.contabtableWidget->item(veces,5)->text()) << "} & ";
         QString c;
         if (ui.contabtableWidget->item(veces,7)->text()=="1") c="*";
         stream << "{\\tiny " << filtracad(c) << "} & ";
         stream << "{\\tiny " << filtracad(ui.contabtableWidget->item(veces,8)->text()) << "} & ";
         QString cad;
         if (filtracad(ui.contabtableWidget->item(veces,7)->text())=="1")
          cad="";
         else
           {
            QComboBox  *comboitem=(QComboBox*)ui.contabtableWidget->cellWidget(veces,9);
            cad=comboitem->currentText();
            if (cad=="- -") cad="";
           }
         stream << "{\\tiny " << filtracad(cad) << "} \\\\ \n  ";
         stream << "\\hline\n";
       }



    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";
    
    fichero.close();

}

void concilia::informecontab()
{
   latexcontab();

   int valor=imprimelatex2(fichconta);
   if (valor==1)
       QMessageBox::warning( this, tr("Conciliación"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Conciliación"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Conciliación"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void concilia::latexexterna()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=fichexterna;
   qfichero+=".tex";
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << tr("Información a conciliar con ") << cuenta << ": " << filtracad(descripcioncuenta(cuenta)) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|p{6cm}|r|r|c|r|l|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{7}{|c|} {\\textbf{";
   QString cadena;
   cadena=tr("EXTRACTO CONCILIACIÓN CUENTA");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // -------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("Fecha") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("DEBE") << "}} & ";
    stream << "{\\tiny{" << tr("HABER") << "}} & ";
    stream << "{\\tiny{" << tr("Conc.") << "}} & ";
    stream << "{\\tiny{" << tr("Saldo") << "}} & ";
    stream << "{\\tiny{" << tr("Diferencia") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    // stream << tr(" \\\\") << "\n";
    stream << "{\\tiny{" << tr("Fecha") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("DEBE") << "}} & ";
    stream << "{\\tiny{" << tr("HABER") << "}} & ";
    stream << "{\\tiny{" << tr("Conc.") << "}} & ";
    stream << "{\\tiny{" << tr("Saldo") << "}} & ";
    stream << "{\\tiny{" << tr("Diferencia") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ---------------------------------------------------------------------------------------

    for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
       {
         stream << "{\\tiny " << filtracad(ui.externatableWidget->item(veces,0)->text()) << "} & ";
         stream << "{\\tiny " << filtracad(ui.externatableWidget->item(veces,1)->text()) << "} & ";
         stream << "{\\tiny " << filtracad(ui.externatableWidget->item(veces,2)->text()) << "} & ";
         stream << "{\\tiny " << filtracad(ui.externatableWidget->item(veces,3)->text()) << "} & ";
         QString c;
         if (ui.externatableWidget->item(veces,5)->text()=="1") c="*";
         stream << "{\\tiny " << filtracad(c) << "} & ";
         if (ctaexterna)
            {
             stream << "{\\tiny " << filtracad(ui.externatableWidget->item(veces,6)->text()) << "} & ";
            }
            else stream << " & ";
         QString cad;
         if (ui.externatableWidget->item(veces,5)->text()=="1")
          cad="";
         else
           {
            QComboBox  *comboitem=(QComboBox*)ui.externatableWidget->cellWidget(veces,7);
            cad=comboitem->currentText();
            if (cad=="- -") cad="";
           }
         stream << "{\\tiny " << filtracad(cad) << "} \\\\ \n  ";
         stream << "\\hline\n";
       }



    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";
    
    fichero.close();

}


void concilia::informeexterna()
{
   latexexterna();

   int valor=imprimelatex2(fichexterna);
   if (valor==1)
       QMessageBox::warning( this, tr("Conciliación"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Conciliación"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Conciliación"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}

void concilia::latexinforme()
{
   // habría primero que grabarlo todo
   refrescar();
   // -----------------------------------
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=fichinforme;
   qfichero+=".tex";
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << tr("CONCILIACIÓN AL: ") << ui.finaldateEdit->date().toString("dd.MM.yyyy") << "}}";
    stream << "\\end{center}";


    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << tr("CUENTA: ") << cuenta << ": " << filtracad(descripcioncuenta(cuenta)) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|p{7cm}|r|r|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{4}{|c|} {\\textbf{";
   QString cadena;
   cadena=tr("CONCILIACIÓN CUENTA");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // -------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("Fecha") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("AJUSTE") << "}} & ";
    stream << "{\\tiny{" << tr("SALDO") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    // stream << tr(" \\\\") << "\n";
    stream << "{\\tiny{" << tr("Fecha") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("AJUSTE") << "}} & ";
    stream << "{\\tiny{" << tr("SALDO") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ---------------------------------------------------------------------------------------
    // SALDO SEGÚN CUENTA CONTABILIDAD
    double saldo=0;
    if (ui.contabtableWidget->item(0,8)!=0)
       {
         stream << "\\multicolumn{3}{|c|} {\\textbf{";
         stream << "{\\tiny " << tr("SALDO SEGÚN CONTABILIDAD") << "}}} & ";
         int ultfila=ui.contabtableWidget->rowCount()-1;
         saldo=convapunto(ui.contabtableWidget->item(ultfila,8)->text()).toDouble();
         //saldo+=convapunto(ui.contabtableWidget->item(0,5)->text()).toDouble(); // haber
         //saldo-=convapunto(ui.contabtableWidget->item(0,4)->text()).toDouble(); // debe
         stream << "{\\tiny " << formateanumerosep(saldo,comadecimal,decimales) << "} \\\\ \n";
         stream << "\\hline\n";
       }
    stream << "\\multicolumn{4}{|l|} {\\textbf{";
    stream << "{\\tiny " << tr("MÁS AJUSTES NO CONCILIADOS CONTABLIDAD") << "}}} \\\\ \n";
    stream << "\\hline\n";
    // trabajamos según contenido de tablas
    // buscamos correcciones positivas en cuenta contabilidad

    QSqlQuery query = basedatos::instancia()->selectDif_conciliaciondiariocuentafechasgrouporderdif_conciliacion( cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

    if (query.isActive())
    while (query.next())
       {
        stream << " & " << "{\\tiny " << filtracad(descripciondiferencia(query.value(0).toString()))
               << "} &  &  \\\\ \n";
        stream << "\\hline\n";

        QSqlQuery query2 = basedatos::instancia()->select3Diariocuentafechasconciliadodif_conciliacionorderfecha( cuenta, ui.inicialdateEdit->date(), ui.finaldateEdit->date(), query.value(0).toString() );

        if (query2.isActive())
        while (query2.next())
            {
             stream << "{\\tiny " << query2.value(0).toDate().toString("dd.MM.yyyy") << "} & ";
             stream << "{\\tiny " << filtracad(query2.value(1).toString()) << "} & ";
             double valor=query2.value(2).toDouble();
             stream << "{\\tiny " << formateanumerosep(valor,comadecimal,decimales) << "} & ";
             saldo+=valor;
             stream << "{\\tiny " << formateanumerosep(saldo,comadecimal,decimales) << "}\\\\ \n  ";
             stream << "\\hline\n";
            }
       }


    stream << "\\multicolumn{4}{|l|} {\\textbf{";
    stream << "{\\tiny " << tr("MAS AJUSTES NO CONCILIADOS CUENTA EXTERNA") << "}}}  \\\\ \n";
    stream << "\\hline\n";
    // trabajamos según contenido de tablas
    // buscamos correcciones positivas en cuenta externa

    if (ctaexterna)
        query = basedatos::instancia()->selectDifconciliacioncuenta_ext_concicuentafechasgrouporderdif_conciliacion( cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() );
     else
        query = basedatos::instancia()->selectDifconciliacionajustes_concicuentafechasgrouporderdif_conciliacion( cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

    if (query.isActive())
    while (query.next())
       {
        stream << " & " << "{\\tiny " << filtracad(descripciondiferencia(query.value(0).toString()))
               << "} &  &  \\\\ \n";
        stream << "\\hline\n";
        QString cad2;
        QSqlQuery query2;

        if (ctaexterna)
            query2 = basedatos::instancia()->select3Cuenta_ext_concicuentafechasdif_conciliacionorderfecha(cuenta, ui.inicialdateEdit->date() ,ui.finaldateEdit->date() , query.value(0).toString() );
          else  
            query2 = basedatos::instancia()->select3Ajustes_concicuentafechasdif_conciliacionorderfecha(cuenta, ui.inicialdateEdit->date() ,ui.finaldateEdit->date() , query.value(0).toString() );

        if (query2.isActive())
        while (query2.next())
            {
             stream << "{\\tiny " << query2.value(0).toDate().toString("dd.MM.yyyy") << "} & ";
             stream << "{\\tiny " << filtracad(query2.value(1).toString()) << "} & ";
             double valor=query2.value(2).toDouble();
             stream << "{\\tiny " << formateanumerosep(valor,comadecimal,decimales) << "} & ";
             saldo+=valor;
             stream << "{\\tiny " << formateanumerosep(saldo,comadecimal,decimales) << "}\\\\ \n  ";
             stream << "\\hline\n";
            }
       }

    stream << "\\multicolumn{4}{|l|} {\\textbf{";
    stream << "{\\tiny " << tr("MENOS AJUSTES NO CONCILIADOS CONTABLIDAD") << "}}} \\\\ \n";
    stream << "\\hline\n";
    // trabajamos según contenido de tablas
    // buscamos correcciones negativas en cuenta contabilidad
    query = basedatos::instancia()->selectDif_conciliaciondiariocuentafechasdebegrouporderdif_conciliacion( cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

    if (query.isActive())
    while (query.next())
       {
        stream << " & " << "{\\tiny " << filtracad(descripciondiferencia(query.value(0).toString()))
               << "} &  &  \\\\ \n";
        stream << "\\hline\n";

        QSqlQuery query2 = basedatos::instancia()->select3Diariocuentafechasdebedif_conciliacionorderfecha(cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() , query.value(0).toString() );

        if (query2.isActive())
        while (query2.next())
            {
             stream << "{\\tiny " << query2.value(0).toDate().toString("dd.MM.yyyy") << "} & ";
             stream << "{\\tiny " << filtracad(query2.value(1).toString()) << "} & ";
             double valor=-query2.value(2).toDouble();
             stream << "{\\tiny " << formateanumerosep(valor,comadecimal,decimales) << "} & ";
             saldo+=valor;
             stream << "{\\tiny " << formateanumerosep(saldo,comadecimal,decimales) << "}\\\\ \n  ";
             stream << "\\hline\n";
            }
       }


    stream << "\\multicolumn{4}{|l|} {\\textbf{";
    stream << "{\\tiny " << tr("MENOS AJUSTES NO CONCILIADOS CUENTA EXTERNA") << "}}} \\\\ \n";
    stream << "\\hline\n";
    // trabajamos según contenido de tablas
    // buscamos correcciones positivas en cuenta externa

    if (ctaexterna)
        query = basedatos::instancia()->selectDif_conciliacioncuenta_ext_concicuentafechasdebegrouporderdif_conciliacion( cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() );
     else
        query = basedatos::instancia()->selectDif_conciliacionajustes_concicuentafechasdebegrouporderdif_conciliacion( cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

    // query.exec(cadena);
    while (query.next())
       {
        stream << " & " << "{\\tiny " << filtracad(descripciondiferencia(query.value(0).toString()))
               << "} &  &  \\\\ \n";
        stream << "\\hline\n";
        QString cad2;
        QSqlQuery query2;
        if (ctaexterna)
            query2 = basedatos::instancia()->select3Cuenta_ext_concicuentafechasdebedif_conciliacionorderfecha(cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() , query.value(0).toString() );
          else
            query2 = basedatos::instancia()->select3Ajustes_concicuentafechasdebedif_conciliacionorderfecha(cuenta, ui.inicialdateEdit->date() , ui.finaldateEdit->date() , query.value(0).toString() );

        while (query2.next())
            {
             stream << "{\\tiny " << query2.value(0).toDate().toString("dd.MM.yyyy") << "} & ";
             stream << "{\\tiny " << filtracad(query2.value(1).toString()) << "} & ";
             double valor=-1*query2.value(2).toDouble();
             stream << "{\\tiny " << formateanumerosep(valor,comadecimal,decimales) << "} & ";
             saldo+=valor;
             stream << "{\\tiny " << formateanumerosep(saldo,comadecimal,decimales) << "}\\\\ \n  ";
             stream << "\\hline\n";
            }
       }


    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}


void concilia::informeconciliacion()
{
   latexinforme();
   int valor=consultalatex2(fichinforme);
   if (valor==1)
       QMessageBox::warning( this, tr("Conciliación"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Conciliación"),
                             tr("PROBLEMAS al llamar a ")+visordvi());

}


void concilia::imprimeconciliacion()
{
   latexinforme();

   int valor=imprimelatex2(fichinforme);
   if (valor==1)
       QMessageBox::warning( this, tr("Conciliación"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Conciliación"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Conciliación"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());
}


void concilia::quitamarcasconci()
{

  for (int veces=0;veces<ui.contabtableWidget->rowCount();veces++)
      {
        if (ui.contabtableWidget->item(veces,7)->text()=="1")
           {
            QLabel *etiqueta=(QLabel*)ui.contabtableWidget->cellWidget(veces,6);
            etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
            ui.contabtableWidget->item(veces,7)->setText("0");
            QComboBox  *comboitem = new QComboBox();
            comboitem->addItems(diferencias);
            ui.contabtableWidget->setCellWidget ( veces, 9, comboitem );
           }
      }

  for (int veces=0;veces<ui.externatableWidget->rowCount();veces++)
      {
        if (ui.externatableWidget->item(veces,5)->text()=="1")
         {
          QLabel *etiqueta=(QLabel*)ui.externatableWidget->cellWidget(veces,4);
          etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
          ui.externatableWidget->item(veces,5)->setText("0");
          QComboBox  *comboitem = new QComboBox();
          comboitem->addItems(diferencias);
          ui.externatableWidget->setCellWidget ( veces, 7, comboitem );
         }
      }
  actualiza_saldos_conciliado();
}


void concilia::borrapaseexterna()
{
 int fila=0;
 if (ui.externatableWidget->currentItem()!=0)
    fila=ui.externatableWidget->currentRow();
 else return;
 ui.externatableWidget->removeRow(fila);
 cambios=true;
}


void concilia::latex()
{
    latexinforme();
    int valor=editalatex(fichinforme);
    if (valor==1)
        QMessageBox::warning( this, tr("Conciliación"),
                              tr("PROBLEMAS al llamar al editor Latex"));
}


void concilia::borra_asiento()
{
    if (ui.contabtableWidget->currentItem()==0)
        {
          QMessageBox::warning( this, tr("CAMBIAR FECHA EN PASE"),
          tr("Para borrar un asiento debe de seleccionar una celda de la línea correspondiente"));
          return;
        }
   int fila=ui.contabtableWidget->currentRow();
   QString asiento=asientoabsdepase(ui.contabtableWidget->item(fila,1)->text());
   QDate fecha=basedatos::instancia()->select_fecha_diariopase (ui.contabtableWidget->item(fila,1)->text());

   QString ejercicio=ejerciciodelafecha(fecha);
   QString qdiario;
   qdiario=diariodeasiento(asiento,ejercicio);

     if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
        {
          QMessageBox::warning( this, tr("Conciliación"),
                                tr("ERROR: No se pueden editar asientos del diario de apertura, "
                                   "regularización o cierre\n"));
          return;
        }

     if (!existeasiento(asiento,ejercicio))
        {
          QMessageBox::warning( this, tr("Conciliación"),
                                tr("ERROR: El asiento seleccionado ya no existe"));
          return;
        }

     borrarasientofunc(asiento,ejercicio,true);

   /*actucuenta();

   ui.contabtableWidget->clearContents();
   ui.contabtableWidget->setRowCount(0);

   cargadatos();*/

     // borramos asiento
     int asientosel=asiento.toInt();
     for (int veces=0; veces<ui.contabtableWidget->rowCount(); veces++)
        {
         int numasiento=ui.contabtableWidget->item(veces,0)->text().toInt();
         if (numasiento==asientosel) { ui.contabtableWidget->removeRow(veces); if (veces>0) veces--;}
        }

     /* actucuenta();

     ui.contabtableWidget->clearContents();
     ui.contabtableWidget->setRowCount(0);

     cargadatos();*/

     //actualiza_tras_edicion();


   if (fila<ui.contabtableWidget->rowCount())
      ui.contabtableWidget->setCurrentCell(fila,0);

}


void concilia::consulta_asiento()
{
    if (ui.contabtableWidget->currentItem()==0)
        {
          QMessageBox::warning( this, tr("CAMBIAR FECHA EN PASE"),
          tr("Para cambiar una fecha debe de seleccionar una celda de la línea correspondiente"));
          return;
        }
   int fila=ui.contabtableWidget->currentRow();
   QString asiento=asientoabsdepase(ui.contabtableWidget->item(fila,1)->text());
   QDate fecha=basedatos::instancia()->select_fecha_diariopase (ui.contabtableWidget->item(fila,1)->text());

   QString ejercicio=ejerciciodelafecha(fecha);
   QString qdiario;
   qdiario=diariodeasiento(asiento,ejercicio);

     if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
        {
          QMessageBox::warning( this, tr("Conciliación"),
                                tr("ERROR: No se pueden editar asientos del diario de apertura, "
                                   "regularización o cierre\n"));
          return;
        }

     if (!existeasiento(asiento,ejercicio))
        {
          QMessageBox::warning( this, tr("Conciliación"),
                                tr("ERROR: El asiento seleccionado ya no existe"));
          return;
        }

     consultarasiento(asiento, usuario,ejercicio);

}

void concilia::nuevo_asiento()
{
    int fila=ui.contabtableWidget->currentRow();
    int resultado=0;
    //introfecha i = new introfecha(QDate::currentDate());
    introfecha i(QDate::currentDate());
    i.esconde_herramientas();
    i.adjustSize();
    resultado=i.exec();
    QDate fecha=i.fecha();
    QString diario=i.diario();
    QString doc=i.documento();
    //delete(i);
    if (!resultado==QDialog::Accepted) return;
    tabla_asientos *t = new tabla_asientos(comadecimal,decimales,usuario);
    t->pasafechaasiento(fecha);
    t->pasadiario(diario);
    t->pasa_doc(doc);
    resultado=t->exec();
    delete(t);

    if (resultado!=QDialog::Accepted) return;

    actualiza_tras_edicion();

    /*actucuenta();

    ui.contabtableWidget->clearContents();
    ui.contabtableWidget->setRowCount(0);

    cargadatos();*/

    if (fila<ui.contabtableWidget->rowCount())
       ui.contabtableWidget->setCurrentCell(fila,0);

}

void concilia::actualiza_saldos_conciliado()
{
  double saldoinicial;
  if (ui.contabtableWidget->rowCount()==0) return;
  // debe 4 -- haber 5 -- saldo 8 -- saldo conciliados 11

  saldoinicial=convapunto(ui.contabtableWidget->item(0,8)->text()).toDouble() -
                  convapunto(ui.contabtableWidget->item(0,4)->text()).toDouble() +
                     convapunto(ui.contabtableWidget->item(0,5)->text()).toDouble();
  double acumulado=saldoinicial;
  for (int veces=0; veces<ui.contabtableWidget->rowCount(); veces++)
      {
       if (ui.contabtableWidget->item(veces,7)->text()=="1")
         {
          acumulado+= convapunto(ui.contabtableWidget->item(veces,4)->text()).toDouble() -
                      convapunto(ui.contabtableWidget->item(veces,5)->text()).toDouble();

         }
       // actualizar columna de saldo de conciliado
       ui.contabtableWidget->item(veces,11)->setText(formateanumero(acumulado,comadecimal,
                                                                    decimales));
      }

}


void concilia::contabiliza_externa_linea()
{
  /*QString aux1, aux2, concepto, valorcontra;
  bool coeficiente;
  bool verificar, concepto_cuaderno;*/

  if (ui.externatableWidget->currentItem()==0) return;

  contrapartidas *c = new contrapartidas();

  c->pasa_valores(aux1, aux2, concepto, valorcontra, coeficiente, verificar, concepto_cuaderno);
  c->activa_una_linea();
  int cod=c->exec();
  if (cod==QDialog::Accepted)
     c->valores(&aux1, &aux2, &concepto, &valorcontra, &coeficiente, &verificar, &concepto_cuaderno);
  bool guardar_valores=c->guardar_valores();
  delete(c);

  if (!(cod==QDialog::Accepted))
  {
      return;
  }

  QProgressDialog progreso(tr("Contabilizando ..."), 0, 0, 0);
  progreso.setMinimumDuration(0);
  progreso.show();

  int linea_actual=ui.externatableWidget->currentRow();
  QCoreApplication::processEvents();
  if (ui.externatableWidget->isRowHidden(linea_actual)) return;
  if (ui.externatableWidget->item(linea_actual,5)->text()=="1") return;
  QDate fecha;
  fecha=fecha.fromString(ui.externatableWidget->item(linea_actual,0)->text(),"dd.MM.yyyy");
  tabla_asientos *tablaasiento=new tabla_asientos(comadecimal,decimales,usuario);
  tablaasiento->pasafechaasiento(fecha);
  // primero pasamos apunte de cuenta en conciliación
  if (concepto_cuaderno) concepto=ui.externatableWidget->item(linea_actual,1)->text();
  tablaasiento->pasadatos1(0,cuenta,concepto,ui.externatableWidget->item(linea_actual,3)->text(),
                           ui.externatableWidget->item(linea_actual,2)->text(),"","");
  bool esdebe_cta_finan=convapunto(ui.externatableWidget->item(linea_actual,3)->text()).toDouble()>0.001 ||
               convapunto(ui.externatableWidget->item(linea_actual,2)->text()).toDouble()<-0.001;
  double valorcontranum=0;
  double valorapunte=convapunto(ui.externatableWidget->item(linea_actual,3)->text()).toDouble()+
             convapunto(ui.externatableWidget->item(linea_actual,2)->text()).toDouble();

  if (!valorcontra.isEmpty() && (convapunto(valorcontra).toDouble()<-0.00001 || convapunto(valorcontra).toDouble()>0.00001))
          {
            if (coeficiente)
               {
                valorcontranum=valorapunte*convapunto(valorcontra).toDouble();
               }
               else valorcontranum=convapunto(valorcontra).toDouble();
            valorapunte=valorapunte-valorcontranum;
          }
  QString cadvalorapunte; cadvalorapunte=formatea_redondeado(valorapunte,comadecimal, decimales);
  tablaasiento->pasadatos1(1,aux1,concepto,esdebe_cta_finan ? 0 : cadvalorapunte,
                       esdebe_cta_finan ? cadvalorapunte: 0,"","");
  // miramos si la cuenta es de analítica y analítica tabla
  // -------------------------------------------------------
  bool procesar_analitica=false;
  if (basedatos::instancia()->analitica_tabla() && conanalitica() &&
                             (escuentadegasto(aux1) ||
                              escuentadeingreso(aux1)))
     procesar_analitica=true;
  if (basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
       codigo_en_analitica(aux1))
     procesar_analitica=true;
  if (procesar_analitica)
          {
           tablaasiento->realiza_asignainputdiario(1,cadvalorapunte );
          }
  // -------------------------------------------------------------
  // terceras línea  si procede
  if (valorcontranum>0.001 || valorcontranum<-0.001)
          {
           if (valorcontranum<0)
             {
               valorcontranum=valorcontranum*-1;
               esdebe_cta_finan=!esdebe_cta_finan;
             }
           QString cadvalorapunte; cadvalorapunte=formatea_redondeado(valorcontranum,comadecimal, decimales);
           tablaasiento->pasadatos1(2,aux2,concepto,esdebe_cta_finan ? 0 : cadvalorapunte,
                            esdebe_cta_finan ? cadvalorapunte : 0,"","");
           // miramos si la cuenta es de analítica y analítica tabla
           // -------------------------------------------------------
           bool procesar_analitica=false;
           if (basedatos::instancia()->analitica_tabla() && conanalitica() &&
                                 (escuentadegasto(aux2) ||
                                  escuentadeingreso(aux2)))
               procesar_analitica=true;
           if (basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
               codigo_en_analitica(aux2))
              procesar_analitica=true;
           if (procesar_analitica)
              {
               tablaasiento->realiza_asignainputdiario(2,cadvalorapunte );
              }
          }

 tablaasiento->chequeatotales();

 if (!verificar)
         {
           if (tablaasiento->botonincorporaractivado())
              {
                 tablaasiento->incorporar();
                 delete tablaasiento;
              }           
         }
       else {
             tablaasiento->exec();
             delete tablaasiento;
       }

 if (!guardar_valores) {
           aux1.clear();
           aux2.clear();
           concepto.clear();
           valorcontra.clear();
           coeficiente=false;
           verificar=false;
           concepto_cuaderno=false;
       }

  progreso.close();
  QMessageBox::information( this, tr("Generación de asiento"),
                                  tr("ASIENTO CONTABILIZADO"));
  // refrescar();

  actualiza_tras_edicion();
}


void concilia::contabiliza_externa()
{
    if (QMessageBox::question(
           this,
           tr("Conciliación"),
           tr("¿ Desea contabilizar todos los apuntes no conciliados en la selección ?")) ==QMessageBox::No )
                         return;

  QString aux1, aux2, concepto, valorcontra;
  bool coeficiente;
  bool verificar, concepto_cuaderno;
  contrapartidas *c = new contrapartidas();
  int cod=c->exec();
  c->valores(&aux1, &aux2, &concepto, &valorcontra, &coeficiente, &verificar, &concepto_cuaderno);
  delete(c);
  if (!cod==QDialog::Accepted) return;

  QProgressDialog progreso(tr("Contabilizando ..."), 0, 0, 0);
  progreso.setMinimumDuration(0);
  progreso.show();


  bool primero=true;
  // procesamos
  for (int veces=0; veces<ui.externatableWidget->rowCount(); veces++)
      {
       QCoreApplication::processEvents();
       if (ui.externatableWidget->isRowHidden(veces)) continue;
       if (ui.externatableWidget->item(veces,5)->text()=="1") continue;
       QDate fecha;
       fecha=fecha.fromString(ui.externatableWidget->item(veces,0)->text(),"dd.MM.yyyy");
       tabla_asientos *tablaasiento=new tabla_asientos(comadecimal,decimales,usuario);
       // tablaasiento->pasanocerrar(true);
       tablaasiento->pasafechaasiento(fecha);
       // primero pasamos apunte de cuenta en conciliación
       if (concepto_cuaderno) concepto=ui.externatableWidget->item(veces,1)->text();
       tablaasiento->pasadatos1(0,cuenta,concepto,ui.externatableWidget->item(veces,3)->text(),
                                ui.externatableWidget->item(veces,2)->text(),"","");
       bool esdebe_cta_finan=convapunto(ui.externatableWidget->item(veces,3)->text()).toDouble()>0.001 ||
               convapunto(ui.externatableWidget->item(veces,2)->text()).toDouble()<-0.001;
       double valorcontranum=0;
       double valorapunte=convapunto(ui.externatableWidget->item(veces,3)->text()).toDouble()+
               convapunto(ui.externatableWidget->item(veces,2)->text()).toDouble();
       // if (valorapunte<-0.001) valorapunte=valorapunte*-1;

       if (!valorcontra.isEmpty() && (convapunto(valorcontra).toDouble()<-0.00001 || convapunto(valorcontra).toDouble()>0.00001))
          {
           //QMessageBox::warning( this, tr("CONCI"),tr("DOS CONTRAPARTIDAS"));

            if (coeficiente)
               {
                valorcontranum=valorapunte*convapunto(valorcontra).toDouble();
               }
               else valorcontranum=convapunto(valorcontra).toDouble();
            valorapunte=valorapunte-valorcontranum;
          }
       QString cadvalorapunte; cadvalorapunte=formatea_redondeado(valorapunte,comadecimal, decimales);
       tablaasiento->pasadatos1(1,aux1,concepto,esdebe_cta_finan ? 0 : cadvalorapunte,
                        esdebe_cta_finan ? cadvalorapunte: 0,"","");
       // miramos si la cuenta es de analítica y analítica tabla
       // -------------------------------------------------------
       bool procesar_analitica=false;
       if (basedatos::instancia()->analitica_tabla() && conanalitica() &&
                             (escuentadegasto(aux1) ||
                              escuentadeingreso(aux1)))
           procesar_analitica=true;
       if (basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
           codigo_en_analitica(aux1))
          procesar_analitica=true;
       if (procesar_analitica)
          {
           tablaasiento->realiza_asignainputdiario(1,cadvalorapunte );
          }
       // -------------------------------------------------------------
       // terceras línea  si procede
       if (valorcontranum>0.001 || valorcontranum<-0.001)
          {
           if (valorcontranum<0)
             {
               valorcontranum=valorcontranum*-1;
               esdebe_cta_finan=!esdebe_cta_finan;
             }
           QString cadvalorapunte; cadvalorapunte=formatea_redondeado(valorcontranum,comadecimal, decimales);
           tablaasiento->pasadatos1(2,aux2,concepto,esdebe_cta_finan ? 0 : cadvalorapunte,
                            esdebe_cta_finan ? cadvalorapunte : 0,"","");
           // miramos si la cuenta es de analítica y analítica tabla
           // -------------------------------------------------------
           bool procesar_analitica=false;
           if (basedatos::instancia()->analitica_tabla() && conanalitica() &&
                                 (escuentadegasto(aux2) ||
                                  escuentadeingreso(aux2)))
               procesar_analitica=true;
           if (basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
               codigo_en_analitica(aux2))
              procesar_analitica=true;
           if (procesar_analitica)
              {
               tablaasiento->realiza_asignainputdiario(2,cadvalorapunte );
              }
          }

       tablaasiento->chequeatotales();

       if (!primero || !verificar)
         {
           if (tablaasiento->botonincorporaractivado())
              {
                 tablaasiento->incorporar();
                 delete tablaasiento;
                 continue;
              }
         }

       primero=false;
       tablaasiento->exec();
       delete tablaasiento;
       // preguntamos si deseamos seguir generando asientos
       // .................
       if (QMessageBox::question(
         this,
         tr("Generación de asientos"),
         tr("¿ Desea salir del proceso ?")) ==QMessageBox::Yes ) return;
     }

  progreso.close();
  QMessageBox::information( this, tr("Generación de asientos"),
                                  tr("SE HAN CONTABILIZADO LOS ASIENTOS"));
  // refrescar();

  actualiza_tras_edicion();
}







void concilia::actualiza_tras_edicion()
{
   QProgressDialog progreso(tr("Cargando información ..."), 0, 0, 2);
    progreso.setMinimumDuration(0);
    progreso.setValue(1);
    progreso.show();
    QCoreApplication::processEvents();


 QSqlQuery query = basedatos::instancia()->select8Diariocuentafechasorder(cuenta , ui.inicialdateEdit->date() ,
                                                                          ui.finaldateEdit->date() );

 // asiento,pase,fecha,concepto,debe,haber,conciliado,dif_conciliacion

 int fila=0;
 int filas=ui.contabtableWidget->rowCount();
 QStringList apuntes;

 double saldoini=0;

 if (ui.contabtableWidget->rowCount()>0)
     saldoini=convapunto(ui.contabtableWidget->item(0,8)->text()).toDouble()-
             convapunto(ui.contabtableWidget->item(0,4)->text()).toDouble() +
             convapunto(ui.contabtableWidget->item(0,5)->text()).toDouble();

 while (fila<filas)
   {
    apuntes << ui.contabtableWidget->item(fila,1)->text();
    fila++;
   }

 if (query.isActive())
 while (query.next())
     {
      QCoreApplication::processEvents();
      if (apuntes.contains(query.value(1).toString())) continue;
      // buscamos posición a insertar
      int pos=0;
      bool asignado=false;
      for (int veces=0; veces<ui.contabtableWidget->rowCount(); veces++)
         {
          QDate fecha_tabla;
          fecha_tabla=fecha_tabla.fromString(ui.contabtableWidget->item(veces,2)->text(),"dd.MM.yyyy");
          if (fecha_tabla>query.value(2).toDate())
             {
              pos=veces;
              asignado=true;
              break;
             }
         }
      if (!asignado) pos=ui.contabtableWidget->rowCount();
      ui.contabtableWidget->insertRow(pos);
      for (int veces=0;veces<7;veces++)
             {
              if (veces==6)
                {
                 QLabel  *etiqueta = new QLabel();
                 etiqueta->setAlignment(Qt::AlignHCenter);
                 QString contenido;
                 if (query.value(veces).toBool())
                    {
                     etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
                     ui.contabtableWidget->setCellWidget(pos,veces,etiqueta);
                     contenido="1";
                    }
                    else
                         {
                          etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
                          contenido="0";
                          ui.contabtableWidget->setCellWidget(pos,veces,etiqueta);
                         }
                 QTableWidgetItem *newItem = new QTableWidgetItem(contenido);
                 ui.contabtableWidget->setItem(pos,7,newItem);
                 continue;
                }
              QString contenido=query.value(veces).toString();
              if (veces==4 || veces==5)
                 {
                  contenido=formateanumero(query.value(veces).toDouble(),comadecimal,decimales);
                 }
              if (veces==2) contenido=query.value(2).toDate().toString("dd.MM.yyyy");
              QTableWidgetItem *newItem = new QTableWidgetItem(contenido);
              if (veces==0 || veces==1 || veces==4 || veces==5)
                 newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.contabtableWidget->setItem(pos,veces,newItem);
             }
      QTableWidgetItem *newItem = new QTableWidgetItem("0");
      newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.contabtableWidget->setItem(pos,8,newItem);
      if (!query.value(6).toBool())
         {
          // escribimos la columna 10 con dif conciliación
          QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(7).toString());
          ui.contabtableWidget->setItem(pos,10,newItem2);
          // --------------------------------------------------
          QComboBox  *comboitem = new QComboBox();
          comboitem->addItems(diferencias);
          if (query.value(7).toString().length()>0)
            {
             for (int veces=0;veces<comboitem->count();veces++)
              {
               if (comboitem->itemText(veces)==query.value(7).toString())
                  {
                   comboitem->setCurrentIndex(veces);
                   break;
                  }
              }
            }
          ui.contabtableWidget->setCellWidget ( pos, 9, comboitem );
         }
         else  if (ui.noconcicheckBox->isChecked()) ui.contabtableWidget->hideRow(pos);
      QTableWidgetItem *newItem11 = new QTableWidgetItem("");
      newItem11->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.contabtableWidget->setItem(pos,11,newItem11);
     }
 if (ui.contabtableWidget->rowCount()<1) return;

 // actualiamos saldo de la cuenta
 int numfilas=ui.contabtableWidget->rowCount();
 double saldo=saldoini;
 for (int veces=0; veces<numfilas; veces++)
    {
     saldo+=convapunto(ui.contabtableWidget->item(veces,4)->text()).toDouble()-
             convapunto(ui.contabtableWidget->item(veces,5)->text()).toDouble();
     ui.contabtableWidget->item(veces,8)->setText(formateanumero(saldo,comadecimal,decimales));
    }

 actualiza_saldos_conciliado();
}


void concilia::consultamayor()
{
 consmayor *c = new consmayor(comadecimal,decimales,usuario);
 // c->pasadatos( punterodiario->subcuentaactual(), punterodiario->fechapaseactual(), punterodiario->externoactual() );
 c->exec();
 delete(c);
 actualiza_tras_edicion();
}

void concilia::valor1cambiado() {
    ui.valor2_lineEdit->setText(ui.valor1_lineEdit->text());
}


void concilia::edcondoc() {
    if (ui.contabtableWidget->currentItem()==0)
        {
          QMessageBox::warning( this, tr("CAMBIAR CONCEPTO/DOCUMENTO EN APUNTE"),
          tr("Para cambiar una fecha debe de seleccionar una celda de la línea correspondiente"));
          return;
        }
   int fila=ui.contabtableWidget->currentRow();
   QString qapunte=ui.contabtableWidget->item(fila,1)->text();
   QString asiento=ui.contabtableWidget->item(fila,0)->text();
   QDate qfecha=basedatos::instancia()->select_fecha_diariopase(ui.contabtableWidget->item(fila,1)->text());
   QString documento = basedatos::instancia()->documento_pase(qapunte);
   edit_conc_doc *e = new edit_conc_doc();
   e->pasadatos( qapunte, qfecha, ui.contabtableWidget->item(fila,3)->text(),
                 documento );
   e->todo_el_asiento();
   e->exec();
   delete(e);

   // borramos asiento
   int asientosel=asiento.toInt();
   for (int veces=0; veces<ui.contabtableWidget->rowCount(); veces++)
      {
       int numasiento=ui.contabtableWidget->item(veces,0)->text().toInt();
       if (numasiento==asientosel) { ui.contabtableWidget->removeRow(veces); if (veces>0) veces--;}
      }

   actualiza_tras_edicion();

   if (fila<ui.contabtableWidget->rowCount())
     ui.contabtableWidget->setCurrentCell(fila,0);

}


void concilia::cambiacuentaapunte() {

    if (ui.contabtableWidget->currentItem()==0)
        {
          QMessageBox::warning( this, tr("CAMBIAR CUENTA EN APUNTE"),
          tr("Para cambiar la cuenta debe de seleccionar una celda de la línea correspondiente"));
          return;
        }

    int fila=ui.contabtableWidget->currentRow();

    cambiacuentapase *c = new cambiacuentapase();

    QString qapunte=ui.contabtableWidget->item(fila,1)->text();
    QString asiento=ui.contabtableWidget->item(fila,0)->text();
    QString codigo=cuenta;
    QDate qfecha=basedatos::instancia()->select_fecha_diariopase(ui.contabtableWidget->item(fila,1)->text());

    QString qdebe=convapunto(ui.contabtableWidget->item(fila,4)->text());

    QString qhaber=convapunto(ui.contabtableWidget->item(fila,5)->text());

    c->pasainfo( qapunte, codigo, qfecha, qdebe, qhaber );
    c->exec();
    delete(c);

    // borramos asiento
    int asientosel=asiento.toInt();
    for (int veces=0; veces<ui.contabtableWidget->rowCount(); veces++)
       {
        int numasiento=ui.contabtableWidget->item(veces,0)->text().toInt();
        if (numasiento==asientosel) { ui.contabtableWidget->removeRow(veces); if (veces>0) veces--;}
       }


    actualiza_tras_edicion();

    if (fila<ui.contabtableWidget->rowCount())
      ui.contabtableWidget->setCurrentCell(fila,0);

}
