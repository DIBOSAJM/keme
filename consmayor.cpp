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

#include "consmayor.h"
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include "tabla_asientos.h"
#include "cuadimprimayor.h"
#include "editarasiento.h"
#include "datos_accesorios.h"
#include "privilegios.h"
#include "consultavencipase.h"
#include "introfecha.h"
#include "procesavencimiento.h"
#include "acumuladosmes.h"
#include "asigna_fichdoc.h"
#include <QMessageBox>
#include "busca_externo.h"
#include <qtrpt.h>
#include "concilia.h"
#include "pidecuenta.h"
#include "pidefecha.h"
#include "editafechaasiento.h"
#include "edit_conc_doc.h"
#include "cambiacuentapase.h"
#include "ivasoportado.h"
#include "ivarepercutido.h"
#include "eib.h"
#include "exento.h"
#include "retencion.h"

consmayor::consmayor(bool concomadecimal, bool condecimales, QString qusuario) : QDialog() {
   ui.setupUi(this);

  QDate fechaactual;
  QString cadfechaactual;
  comadecimal=concomadecimal;
  decimales=condecimales;
  cadfechaactual=fechaactual.currentDate().toString("yyyy-MM-dd");

  ui.inicialdateEdit->setDate( basedatos::instancia()->selectAperturaejerciciosaperturacierre( cadfechaactual, cadfechaactual ) );
  ui.finaldateEdit->setDate( basedatos::instancia()->selectCierreejerciciosaperturacierre( cadfechaactual, cadfechaactual ) );

 QStringList columnas;
 columnas << tr("CUENTA") << tr("FECHA") << tr("ASTO.") << tr("CONCEPTO");
 columnas << tr("DEBE") << tr("HABER") << tr("SALDO") << tr("PUNTEO");
 columnas << tr("DOCUMENTO") << tr("DIARIO") << tr("CI") << tr("APUNTE");

 ui.mayortable->setColumnCount(12);

 ui.mayortable->hideColumn(0); // el intervalo de cuentas está desactivado por defecto

 ui.mayortable->setEditTriggers ( QAbstractItemView::NoEditTriggers );

 ui.mayortable->setHorizontalHeaderLabels(columnas);

 ui.mayortable->setColumnWidth(1,80);
 ui.mayortable->setColumnWidth(2,60);
 ui.mayortable->setColumnWidth(3,300);
 ui.mayortable->setColumnWidth(4,80);
 ui.mayortable->setColumnWidth(5,80);
 ui.mayortable->setColumnWidth(6,80);
 ui.mayortable->setColumnWidth(7,50);
 ui.mayortable->setColumnWidth(8,125);
 ui.mayortable->setColumnWidth(9,80);
 ui.mayortable->setColumnWidth(10,125);

 if (!conanalitica()) ui.mayortable->hideColumn(10);

 connect(ui.buscapushButton,SIGNAL(clicked()),SLOT(botonsubcuentapulsado()));
 connect(ui.refrescarpushButton,SIGNAL(clicked()),SLOT(refrescar()));
 connect(ui.subcuentalineEdit,SIGNAL(editingFinished()),SLOT(finedicsubcuenta()));
 connect(ui.previapushButton,SIGNAL(clicked()),SLOT(subcuentaprevia()));
 connect(ui.siguientepushButton,SIGNAL(clicked()),SLOT(subcuentasiguiente()));
 connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprimir()));
 connect(ui.informe_tex_pushButton,SIGNAL(clicked()),SLOT(imprimir_latex()));
 connect(ui.subcuentalineEdit,SIGNAL(textChanged(QString)),SLOT(subcuentacambiada()));
 connect(ui.editarpushButton,SIGNAL(clicked()),SLOT(editarpulsado()));
 connect(ui.xmlpushButton,SIGNAL(clicked()),SLOT(genxml()));

 connect(ui.origencheckBox,SIGNAL(stateChanged(int)), SLOT(origenpercambiado()));
 connect(ui.fincheckBox,SIGNAL(stateChanged(int)), SLOT(finalpercambiado()));

 connect(ui.intervalogroupBox,SIGNAL(toggled(bool)),this,SLOT(intervalogroupcambiado()));

 connect(ui.copiarpushButton,SIGNAL(clicked()),SLOT(copiar()));
 connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borraasiento()));

 connect(ui.visdocpushButton,SIGNAL(clicked()),SLOT(visdoc()));
 connect(ui.texpushButton,SIGNAL(clicked()),SLOT(latex()));
 connect(ui.consultarpushButton,SIGNAL(clicked()),SLOT(consultaasiento()));

 connect(ui.datospushButton,SIGNAL(clicked()),SLOT(datoscuenta()));

 connect(ui.vtospushButton,SIGNAL(clicked()),SLOT(vencimientos()));
 connect(ui.nuevopushButton,SIGNAL(clicked()),SLOT(nuevo_asiento()));

 connect(ui.procvtopushButton,SIGNAL(clicked()),SLOT(procesa_vencimiento()));

 connect(ui.acumpushButton,SIGNAL(clicked()),SLOT(acumuladoscta()));

 connect(ui.asignafichdocpushButton,SIGNAL(clicked()),SLOT(asigna_ficherodocumento()));

 connect(ui.quita_externo_pushButton,SIGNAL(clicked()),SLOT(quita_externo()));

 connect(ui.externo_pushButton,SIGNAL(clicked()), SLOT(buscar_externo()));

 connect(ui.conci_pushButton,SIGNAL(clicked(bool)),SLOT(concilia_ext()));

 connect(ui.csvpushButton,SIGNAL(clicked(bool)),SLOT(importa_csv()));
 connect(ui.c43pushButton,SIGNAL(clicked(bool)),SLOT(importa_c43()));

 connect(ui.fecha_pushButton,SIGNAL(clicked(bool)),SLOT(editafechaasien()));
 connect(ui.concepto_pushButton,SIGNAL(clicked(bool)),SLOT(edcondoc()));
 connect(ui.iva_pushButton,SIGNAL(clicked(bool)),SLOT(ed_registro_iva()));
 connect(ui.ret_pushButton,SIGNAL(clicked(bool)),SLOT(ed_registro_ret()));

 // connect(ui.externolineEdit,SIGNAL(editingFinished()),SLOT(cod_externo_fin_edic()));
 connect(ui.externolineEdit,SIGNAL(textChanged(QString)),SLOT(cod_externo_cambiado()));
 connect(ui.borra_imp_pushButton,SIGNAL(clicked(bool)),SLOT(elimina_cuaderno()));

 connect(ui.punteo_conci_pushButton,SIGNAL(clicked(bool)),SLOT(actu_conci_punteo()));

 usuario=qusuario;

 QString privileg=basedatos::instancia()->privilegios(qusuario);

 if (!privileg.isEmpty()) {
   if (privileg[datos_accesorios_cta]=='0') ui.datospushButton->setEnabled(false);
   if (privileg[edi_asiento]=='0') ui.editarpushButton->setEnabled(false);
   if (privileg[borrar_asiento]=='0') ui.borrarpushButton->setEnabled(false);
   if (privileg[vencimientos_asignar]=='0') ui.vtospushButton->setEnabled(false);

   if (privileg[vencimientos_procesar]=='0') ui.procvtopushButton->setEnabled(false);

   if (privileg[nuev_asiento]=='0') ui.nuevopushButton->setEnabled(false);

   if (privileg[asigna_fichero]=='0') ui.asignafichdocpushButton->setEnabled(false);
 }

 ui.fotolabel->setFixedHeight(0);
 ui.fotolabel->setFixedWidth(0);

#ifdef NOEDITTEX
 ui.texpushButton->hide();
#endif

 if (!basedatos::instancia()->hay_externos())
    {
     ui.externo_label->hide();
     ui.externolineEdit->hide();
     ui.externo_pushButton->hide();
     ui.quita_externo_pushButton->hide();
     ui.descrip_externo_lineEdit->hide();
    }
}



void consmayor::botonsubcuentapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.subcuentalineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.subcuentalineEdit->setText(cadena2);
       else ui.subcuentalineEdit->setText("");
    delete labusqueda;

}



void consmayor::cargadatos()
{
   bool intervalo=false;
   QString condicion;
   if (ui.intervalogroupBox->isChecked()) intervalo=true;
   if (!intervalo)
      {
       if (ui.subcuentalineEdit->text().isEmpty() && ui.externolineEdit->text().isEmpty()) return;
       if (!existesubcuenta(ui.subcuentalineEdit->text()) && !ui.subcuentalineEdit->text().isEmpty()) return;

       if (escuentadegasto(ui.subcuentalineEdit->text()) &&
           escuentadeingreso(ui.subcuentalineEdit->text()) &&
           (conanalitica() || conanalitica_parc()))
         ui.mayortable->showColumn(9);
        else ui.mayortable->hideColumn(9);
       if (!ui.subcuentalineEdit->text().isEmpty()) {
          condicion="cuenta='";
          condicion+=ui.subcuentalineEdit->text();
          condicion+="'";
          }
       if (ui.subcuentalineEdit->text().isEmpty())
           ui.mayortable->showColumn(0);
         else ui.mayortable->hideColumn(0);
       QString imagen=basedatos::instancia()->fotocuenta (ui.subcuentalineEdit->text());
       QPixmap foto;
       if (imagen.length()>0)
          {
           QByteArray byteshexa;
           byteshexa.append ( imagen.toUtf8() );
           QByteArray bytes;
           bytes=bytes.fromHex ( byteshexa );
           foto.loadFromData ( bytes, "PNG");
           ui.fotolabel->setFixedHeight(100);
           ui.fotolabel->setFixedWidth(90);
          }
           else
               {
                ui.fotolabel->setFixedHeight(0);
                ui.fotolabel->setFixedWidth(0);
               }
       ui.fotolabel->setPixmap(foto);
      }
      else
           {
             if (conanalitica()) ui.mayortable->showColumn(9);
                else ui.mayortable->hideColumn(9);
             if (ui.iniciallineEdit->text().length()==0 || ui.finallineEdit->text().length()==0)
                {
                 QMessageBox::warning( this, tr("Consulta de mayor"),
	 	 tr("ERROR: se debe de suministrar información correcta para el intervalo de cuentas"));
                 return;
                }
             condicion+="cuenta>='";
             condicion+=ui.iniciallineEdit->text();
             condicion+="' and ";
             condicion+="cuenta<='";
             condicion+=ui.finallineEdit->text();
             condicion+="'";
             ui.nombrelabel->setText(tr("Intervalo: ")+ui.iniciallineEdit->text()+
                                      " - "+ui.finallineEdit->text());
           }

    if (!ui.externolineEdit->text().isEmpty()) {
        if (!ui.subcuentalineEdit->text().isEmpty() || ui.intervalogroupBox->isChecked()) condicion+=" and";
        condicion+=" externo='"+ui.externolineEdit->text().trimmed()+"'";
    }
    qlonglong num = basedatos::instancia()->selectCountasientodiariofechascondicion( ui.inicialdateEdit->date() ,ui.finaldateEdit->date() , condicion );
    if (num > 15000)
    {
	    switch( QMessageBox::warning( this, tr("Consultas de Mayor"),
            tr("La consulta excede de 15000 registros,\n"
            "¿ desea cargarlos de todas formas ?"),
            tr("&Sí"), tr("&No"), 0, 0,1 ) ) 
	    {
            case 0: // proseguimos.
                break;
            case 1: // Cancelamos la carga
                return;
            }
	}
   ui.mayortable->setRowCount(num);

   QSqlQuery query = basedatos::instancia()->select9Diariofechascondicionorderfechapase(
           ui.inicialdateEdit->date() , ui.finaldateEdit->date() , condicion );
   int fila=0;
   filas_borrador.clear();
    if (query.isActive())
    while ( query.next() ) {
        if (fila>=num) break; // prevenimos violación de segmento
        QTableWidgetItem *newItemcta = new QTableWidgetItem(query.value(0).toString());
        ui.mayortable->setItem(fila,0,newItemcta);
        QTableWidgetItem *newItem = new QTableWidgetItem(query.value(1).toDate().toString("dd.MM.yyyy"));
        ui.mayortable->setItem(fila,1,newItem);
        QTableWidgetItem *newItemx = new QTableWidgetItem(query.value(2).toString());
        newItemx->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.mayortable->setItem(fila,2,newItemx);  // asiento
        QTableWidgetItem *newItemp = new QTableWidgetItem(query.value(3).toString());
        ui.mayortable->setItem(fila,3,newItemp);  // concepto
        double valdebe=query.value(4).toDouble();

        // ***************************************************************************************
        // ***************************************************************************************
        if (valdebe>0.0001 || valdebe<-0.0001)
          {
           QTableWidgetItem *newItem = new QTableWidgetItem(formateanumero(valdebe,comadecimal,decimales));
           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.mayortable->setItem(fila,4,newItem);  // debe
          }
          else 
               {
                QTableWidgetItem *newItempp = new QTableWidgetItem("");
                ui.mayortable->setItem(fila,4,newItempp);
               }
        double valhaber=query.value(5).toDouble();
        if (valhaber>0.0001 || valhaber<-0.0001)
          {
           QTableWidgetItem *newItemjj = new QTableWidgetItem(formateanumero(valhaber,comadecimal,decimales));
           newItemjj->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.mayortable->setItem(fila,5,newItemjj);  // haber
          }
          else 
               {
                QTableWidgetItem *newItemkk = new QTableWidgetItem("");
                ui.mayortable->setItem(fila,5,newItemkk);
               }
        ui.mayortable->setItem(fila,7,new QTableWidgetItem("")); // punteo
        QTableWidgetItem *newItemj = new QTableWidgetItem(query.value(6).toString());
        ui.mayortable->setItem(fila,8,newItemj); // documento
        QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(7).toString());
        ui.mayortable->setItem(fila,9,newItem2); // diario
        QTableWidgetItem *newItem3 = new QTableWidgetItem(query.value(8).toString());
        ui.mayortable->setItem(fila,10,newItem3); // ci
        QTableWidgetItem *newItempase = new QTableWidgetItem(query.value(9).toString());
        newItempase->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.mayortable->setItem(fila,11,newItempase); // apunte
        if (!query.value(10).toBool()) filas_borrador << fila;
        fila++;
    }
  // calculamos acumulado de saldo inicial
    // primero observamos si el ejercicio anterior está cerrado
    QString ejercicio=ejerciciodelafecha(ui.inicialdateEdit->date());
    QDate fechainiejercicio=inicioejercicio(ejercicio);
    QDate fechaejercicioanterior=fechainiejercicio.addDays(-1);
    QString ejercicioanterior=ejerciciodelafecha(fechaejercicioanterior);
    if (ejerciciocerrado(ejercicioanterior) || escuentadegasto(ui.subcuentalineEdit->text()) || 
	   escuentadeingreso(ui.subcuentalineEdit->text()))
       {
        query = basedatos::instancia()->selectSumdebesumhaberdiariofechascondicion(
                 fechainiejercicio, ui.inicialdateEdit->date(), condicion);
       }
       else query = basedatos::instancia()->selectSumdebesumhaberdiariofechacondicion(
                    ui.inicialdateEdit->date(),condicion);
   double saldoin=0;
   double sumadebe=0;
   double sumahaber=0;
   if ( (query.isActive()) && (query.first()) )
      {
        saldoin=query.value(0).toDouble()-query.value(1).toDouble();
        // sumadebe=query.value(0).toDouble();
        // sumahaber=query.value(1).toDouble();
        if (saldoin>0) sumadebe=saldoin;
        if (saldoin<0) sumahaber=saldoin*-1;
      }

    // obtenemos lista de apuntes con vencimiento y suma de importe con pendiente=false
   QSqlQuery q;
   QList<int> apuntes_vencimiento;
   QList<double> importes_vencimiento;
   QList<int> apuntes_venci_realizados;

   if (!intervalo)
    {

     q=basedatos::instancia()->pases_diario_operacion_vencimiento(ui.subcuentalineEdit->text(),
                                                                    ui.inicialdateEdit->date(),
                                                                    ui.finaldateEdit->date());
     if (q.isActive())
         while (q.next())
            {
             apuntes_vencimiento << q.value(0).toInt();
             importes_vencimiento << q.value(1).toDouble();
            }
    }
    // obtenemos lista de pase_diario_vencimiento y pase_diario_operación para puntear
   if (!intervalo)
    {
       q=basedatos::instancia()->pases_diario_vencimiento(ui.subcuentalineEdit->text(),
                                                          ui.inicialdateEdit->date(),
                                                          ui.finaldateEdit->date());
       if (q.isActive())
           while (q.next())
              {
               apuntes_venci_realizados << q.value(0).toInt();
              }
    }
    fila=0;
    while (fila<ui.mayortable->rowCount())
       {
          if (ui.mayortable->item(fila,4)->text().length()>0) 
              {
              sumadebe+=convapunto(ui.mayortable->item(fila,4)->text()).toDouble();
              saldoin+=convapunto(ui.mayortable->item(fila,4)->text()).toDouble();
              }
          if (ui.mayortable->item(fila,5)->text().length()>0)
             {
              sumahaber+=convapunto(ui.mayortable->item(fila,5)->text()).toDouble();
              saldoin-=convapunto(ui.mayortable->item(fila,5)->text()).toDouble();
             }
        QTableWidgetItem *newItemx = new QTableWidgetItem(formateanumero(saldoin,comadecimal,decimales));
        newItemx->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.mayortable->setItem(fila,6,newItemx); // saldo
        int apunte=ui.mayortable->item(fila,11)->text().toInt();
        if (!intervalo)
          {
           for (int veces=0; veces<apuntes_vencimiento.count(); veces++)
             {
              if (apuntes_vencimiento.at(veces)==apunte)
                {
                  // cogemos valor de debe + haber
                 double sumadebehaber=convapunto(ui.mayortable->item(fila,4)->text()).toDouble()
                               + convapunto(ui.mayortable->item(fila,5)->text()).toDouble();
                 if (sumadebehaber<0) sumadebehaber=sumadebehaber*-1;
                 double compa=importes_vencimiento.at(veces) >0 ? importes_vencimiento.at(veces):
                                importes_vencimiento.at(veces)*-1;
                 QString colocar="*";
                 if (compa-sumadebehaber>0.005 || compa-sumadebehaber<-0.005) colocar="P";
                 QTableWidgetItem *newItemx = new QTableWidgetItem(colocar);
                 newItemx->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
                 //QBrush brocha; brocha.setColor(Qt::green);
                 //newItemx->setBackground(brocha);
                 ui.mayortable->setItem(fila,7,newItemx); // conciliado
                }
             }
           for (int veces=0; veces< apuntes_venci_realizados.count(); veces++)
              {
                if (apuntes_venci_realizados.at(veces)==apunte)
                  {
                    QTableWidgetItem *newItemx = new QTableWidgetItem("*");
                    newItemx->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
                    ui.mayortable->setItem(fila,7,newItemx); // conciliado
                  }
              }
           }
        fila++;
       }
    for (int v=0; v<filas_borrador.count(); v++) {
           for (int column = 0; column < ui.mayortable->columnCount(); ++column)
           {
            QTableWidgetItem* item = ui.mayortable->item(filas_borrador.at(v), column);
            if (!item)
               {
                item = new QTableWidgetItem();
                ui.mayortable->setItem(filas_borrador.at(v), column, item);
               }
            item->setBackground(QColor(255, 200, 200));
           }
       }
   ui.debelineEdit->setText(formateanumero(sumadebe,comadecimal,decimales));
   ui.haberlineEdit->setText(formateanumero(sumahaber,comadecimal,decimales));
   ui.saldolineEdit->setText(formateanumero(saldoin,comadecimal,decimales));
   ui.mayortable->resizeColumnToContents(0);
   ui.mayortable->resizeColumnToContents(1);
   ui.mayortable->resizeColumnToContents(2);
   ui.mayortable->resizeColumnToContents(4);
   ui.mayortable->resizeColumnToContents(5);
   ui.mayortable->resizeColumnToContents(6);

   int currentRow=ui.mayortable->currentRow();
   if (filas_borrador.contains(currentRow)) {
       ui.editarpushButton->setEnabled(true);
       ui.borrarpushButton->setEnabled(true);
   }
   else {
       ui.editarpushButton->setEnabled(false);
       ui.borrarpushButton->setEnabled(false);
   }

}


void consmayor::refrescar()
{
 QPixmap foto;
 ui.fotolabel->setPixmap(foto);

   if (ui.intervalogroupBox->isChecked()) 
      {
       cargadatos();
       ui.fotolabel->setFixedHeight(0);
       ui.fotolabel->setFixedWidth(0);
       return;
      }

   ctaexpandepunto();
   /* if (ejerciciodelafecha(ui.inicialdateEdit->date())!=ejerciciodelafecha(ui.finaldateEdit->date())
	&&(escuentadegasto(ui.subcuentalineEdit->text()) || 
	   escuentadeingreso(ui.subcuentalineEdit->text())))
	{
	   QMessageBox::warning( this, tr("Consultas de Mayor"),
				 tr("En consultas de cuentas de ingresos/gastos\n"
				      "las fechas han de pertenecer al mismo período"));
	   QString ejercicio=ejerciciodelafecha(ui.inicialdateEdit->date());
	   // QMessageBox::warning( this, tr("Consultas de Mayor"),ejercicio);
	   if (ejercicio.length()!=0) ui.finaldateEdit->setDate(finejercicio(ejercicio));
	      else ui.finaldateEdit->setDate(ui.inicialdateEdit->date());
                }
*/
   if ( existesubcuenta(ui.subcuentalineEdit->text())) {
     QString cadena="Consultas de Mayor: ";
     cadena+=descripcioncuenta(ui.subcuentalineEdit->text());
     setWindowTitle(cadena);
     ui.nombrelabel->setText(descripcioncuenta(ui.subcuentalineEdit->text()).left(35));
   }
   else {
       setWindowTitle(tr("Consulta de Mayor"));
       ui.nombrelabel->setText("");
   }
 cargadatos();

}


void consmayor::ctaexpandepunto()
{
  ui.subcuentalineEdit->setText(expandepunto(ui.subcuentalineEdit->text(),anchocuentas()));
}



void consmayor::finedicsubcuenta()
{
 ctaexpandepunto();
 if (ui.subcuentalineEdit->text().length()<anchocuentas() && cod_longitud_variable()) botonsubcuentapulsado();
}


void consmayor::pasadatos(QString qsubcuenta, QDate qfecha , QString externo)
{
   ui.externolineEdit->setText(externo);
   ui.subcuentalineEdit->setText(qsubcuenta);
   QString ejercicio=ejerciciodelafecha(qfecha);
   QDate fechaini=inicioejercicio(ejercicio);
   QDate fechafin=finejercicio(ejercicio);
   ui.inicialdateEdit->setDate(fechaini);
   ui.finaldateEdit->setDate(fechafin);
   QString cadena="Consultas de Mayor: ";
   cadena+=descripcioncuenta(qsubcuenta);
   setWindowTitle(cadena);
   ui.nombrelabel->setText(descripcioncuenta(ui.subcuentalineEdit->text()).left(35));
   if (existesubcuenta(ui.subcuentalineEdit->text())) refrescar();
}


void consmayor::subcuentaprevia()
{
    QString cadena=subcuentaanterior(ui.subcuentalineEdit->text());
    if (cadena=="") return;
    ui.subcuentalineEdit->setText(cadena);
    refrescar();
}


void consmayor::subcuentasiguiente()
{
    QString cadena=subcuentaposterior(ui.subcuentalineEdit->text());
    if (cadena=="") return;
    ui.subcuentalineEdit->setText(cadena);
    refrescar();
}

void consmayor::imprimir() {
    informe();
    return;
}

void consmayor::imprimir_latex()
{

  if (ui.intervalogroupBox->isChecked())
     {
       latexintervalo();
       int valor=imprimelatex2(tr("mayor_int"));
       if (valor==1)
         QMessageBox::warning( this, tr("Imprimir plan"),tr("PROBLEMAS al llamar a Latex"));
       if (valor==2)
         QMessageBox::warning( this, tr("Imprimir plan"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
       if (valor==3)
         QMessageBox::warning( this, tr("Imprimir plan"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());
       return;
     }

  cuadimprimayor *cuadimpri=new cuadimprimayor(comadecimal,decimales);
  cuadimpri->generalatexmayor( ui.subcuentalineEdit->text(),ui.subcuentalineEdit->text(),
 			       ui.inicialdateEdit->date(), ui.finaldateEdit->date() );
  delete cuadimpri;

   int valor=imprimelatex(tr("mayor"));
   if (valor==1)
       QMessageBox::warning( this, tr("IMPRIMIR MAYOR"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("IMPRIMIR MAYOR"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("IMPRIMIR MAYOR"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void consmayor::subcuentacambiada()
{
  if (existesubcuenta(ui.subcuentalineEdit->text()))
      refrescar();
}




void consmayor::editarpulsado()
{
    QString asiento;

    if (ui.mayortable->currentItem()==0) return;
    int dia=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().left(2).toInt();
    int mes=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().mid(3,2).toInt();
    int anyo=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().right(4).toInt();
    QDate fecha(anyo,mes,dia);
    asiento=ui.mayortable->item(ui.mayortable->currentRow(),2)->text();
       
    QString ejercicio=ejerciciodelafecha(fecha);
    QString qdiario;
    qdiario=diariodeasiento(asiento,ejercicio);
    
    if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
       {
         QMessageBox::warning( this, tr("Consultas de Mayor"),
			       tr("ERROR: No se pueden editar asientos del diario de apertura, "
				  "regularización o cierre\n"));
         return;
       }

    if (!existeasiento(asiento,ejercicio))
       {
         QMessageBox::warning( this, tr("Consultas de Mayor"),
			       tr("ERROR: El asiento seleccionado ya no existe"));
         return;
       }

    editarasiento(asiento,usuario,ejercicio);

    refrescar();

}


void consmayor::origenpercambiado()
{
 if (ui.origencheckBox->isChecked())
     {
      ui.inicialdateEdit->setDate(inicioejercicio(ejerciciodelafecha(ui.inicialdateEdit->date())));
      ui.inicialdateEdit->setEnabled(false);
     }
  else
      ui.inicialdateEdit->setEnabled(true);
}

void consmayor::finalpercambiado()
{
 if (ui.fincheckBox->isChecked())
   {
    ui.finaldateEdit->setDate(finejercicio(ejerciciodelafecha(ui.inicialdateEdit->date())));
    ui.finaldateEdit->setEnabled(false);
   }
 else
    ui.finaldateEdit->setEnabled(true);
}


void consmayor::intervalogroupcambiado()
{
  if (ui.intervalogroupBox->isChecked())
     {
      ui.nombrelabel->clear();
      ui.subcuentalineEdit->setEnabled(false);
      ui.subcuentalineEdit->clear();
      ui.buscapushButton->setEnabled(false);
      ui.previapushButton->setEnabled(false);
      ui.siguientepushButton->setEnabled(false);
      QString cadena="Consultas de Mayor";
      setWindowTitle(cadena);
      ui.mayortable->showColumn(0);
     }
   else
       {
        ui.nombrelabel->clear();
        ui.subcuentalineEdit->setEnabled(true);
        ui.buscapushButton->setEnabled(true);
        ui.previapushButton->setEnabled(true);
        ui.siguientepushButton->setEnabled(true);
        ui.iniciallineEdit->clear();
        ui.finallineEdit->clear();
        QString cadena="Consultas de Mayor";
        setWindowTitle(cadena);
        ui.mayortable->hideColumn(0);
       }
}


void consmayor::latexintervalo()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("mayor_int.tex");
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero(pasa);

    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
    stream << "\\end{center}" << "\n";
    stream << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("MAYOR ") << filtracad(ui.nombrelabel->text()) <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";

    // cuerpo de listado (tabla)
    stream << "\\begin{center}\n";
    stream << "\\begin{longtable}{|l|l|r|p{6cm}|r|r|r|}\n";
    stream << "\\hline\n";
    QDate fechainicial=ui.inicialdateEdit->date();
    QDate fechafinal=ui.finaldateEdit->date();
    stream << "\\multicolumn{7}{|c|} {\\textbf{";
    stream << tr("Extracto de ") << fechainicial.toString("dd.MM.yyyy") <<
                         tr(" a ") << fechafinal.toString("dd.MM.yyyy");
    stream <<  "}} \\\\";
    stream << "\\hline" << "\n";

    stream << "{\\scriptsize {" << tr("CUENTA") << "}} & {\\scriptsize {" << tr("FECHA") << 
             "}} & {\\scriptsize {" << tr("ASTO.") << "}} & {\\scriptsize {" <<
            tr("CONCEPTO") << "}} & {\\scriptsize {" << tr("DEBE") << "}} & {\\scriptsize {" << tr("HABER") << 
            "}} & {\\scriptsize {" << tr("SALDO") << "}} \\\\\n";
    stream << "\\hline\n";
    stream << "\\endfirsthead";
    // --------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\scriptsize {" << tr("CUENTA") << "}} & {\\scriptsize {" << tr("FECHA") << 
               "}} & {\\scriptsize {" << tr("ASTO.") << "}} & {\\scriptsize {" <<
             tr("CONCEPTO") << "}} & {\\scriptsize {" << tr("DEBE") << "}} & {\\scriptsize {" << tr("HABER") << 
             "}} & {\\scriptsize {" << tr("SALDO") << "}} \\\\\n";
    stream << "\\hline\n";
    stream << "\\endhead";
    // --------------------------------------------------------------------------------------------------

    double importe=0;
    int linactu=0;
    while (linactu<ui.mayortable->rowCount())
          {
           stream << "{\\scriptsize " << filtracad(ui.mayortable->item(linactu,0)->text()) << "} & {\\scriptsize ";
           stream << filtracad(ui.mayortable->item(linactu,1)->text()) << "} & {\\scriptsize ";
           stream << filtracad(ui.mayortable->item(linactu,2)->text()) << "} & {\\scriptsize ";
           stream << filtracad(ui.mayortable->item(linactu,3)->text()) << "} & {\\scriptsize ";
           importe=convapunto(ui.mayortable->item(linactu,4)->text()).toDouble();
           stream << formateanumerosep(importe,comadecimal,decimales) << "} & {\\scriptsize ";
           importe=convapunto(ui.mayortable->item(linactu,5)->text()).toDouble();
           stream << formateanumerosep(importe,comadecimal,decimales) << "} & {\\scriptsize ";
           importe=convapunto(ui.mayortable->item(linactu,6)->text()).toDouble();
           stream << formateanumerosep(importe,comadecimal,decimales) << "} ";
           stream << " \\\\ \n ";
           stream << "\\hline\n";
           linactu++;
          }
      importe=convapunto(ui.debelineEdit->text()).toDouble();
      stream  << "\\multicolumn{4}{|r|}{" << tr("Sumas ...") << "} & {\\scriptsize " <<
              formateanumerosep(importe,comadecimal,decimales);
      importe=convapunto(ui.haberlineEdit->text()).toDouble();
      stream << "} & {\\scriptsize " << formateanumerosep(importe,comadecimal,decimales) << "} & \\\\";
      stream << "\\hline\n";
      // final de la tabla
      stream << "\\end{longtable}\n";
      stream << "\\end{center}\n";

    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();


}


void consmayor::copiar()
{
   QClipboard *cb = QApplication::clipboard();
   QString global;

   global=nombreempresa();
   global+="\n";
   global+=ui.nombrelabel->text();
   global+="\n";
   global+=tr("INICIAL:");
   global+="\t";
   global+=ui.inicialdateEdit->date().toString("dd/MM/yyyy");
   global+="\n";
   global+=tr("FINAL:");
   global+="\t";
   global+=ui.finaldateEdit->date().toString("dd/MM/yyyy");
   global+="\n\n";
   if (ui.intervalogroupBox->isChecked())
      {
       global+=tr("CUENTA") +"\t";
      }
   global+=tr("FECHA") +"\t"+ tr("ASTO.") +"\t"+ tr("CONCEPTO") +
           "\t"+ tr("DEBE") +"\t"+ tr("HABER") +"\t"+ tr("SALDO")+
           "\t"+ tr("PUNTEO")+"\t"+ tr("DOCUMENTO")+"\t"+ tr("DIARIO");
   if (conanalitica()) global+="\t"+ tr("CI");

   global+="\n";

    int linactu=0;
    while (linactu<ui.mayortable->rowCount())
          {
           if (ui.intervalogroupBox->isChecked())
              {
                global+=ui.mayortable->item(linactu,0)->text();
                global+="\t";
              }
           global+=ui.mayortable->item(linactu,1)->text();
           global+="\t";
           global+=ui.mayortable->item(linactu,2)->text();
           global+="\t";
           global+=ui.mayortable->item(linactu,3)->text();
           global+="\t";
           global+=ui.mayortable->item(linactu,4)->text();
           global+="\t";
           global+=ui.mayortable->item(linactu,5)->text();
           global+="\t";
           global+=ui.mayortable->item(linactu,6)->text();
           global+="\t";
           if (ui.mayortable->item(linactu,7)!=NULL)
              global+=ui.mayortable->item(linactu,7)->text();
           global+="\t";
           global+=ui.mayortable->item(linactu,8)->text();
           global+="\t";
           global+=ui.mayortable->item(linactu,9)->text();
           if (conanalitica())
              {
               global+="\t";
               global+=ui.mayortable->item(linactu,10)->text();
              }
           global+="\n";
           linactu++;
          }

   cb->setText(global);
   QMessageBox::information( this, tr("Consulta de mayor"),
                             tr("Se ha pasado el contenido al portapapeles") );

}



void consmayor::genxml()
{
  if (ui.intervalogroupBox->isChecked())
     {
       xmlintervalo();
       QMessageBox::information( this, tr("IMPRIMIR MAYOR"),
                                tr("El fichero XML se ha generado en el directorio de trabajo"));
       return;
     }

  // cuadimprimayor *cuadimpri=new cuadimprimayor(comadecimal,decimales);
  cuadimprimayor cuadimpri(comadecimal,decimales);
  cuadimpri.generaxmlmayor( ui.subcuentalineEdit->text(),ui.subcuentalineEdit->text(),
                               ui.inicialdateEdit->date(), ui.finaldateEdit->date() );
  //delete cuadimpri;

  QMessageBox::information( this, tr("IMPRIMIR MAYOR"),
                                tr("El fichero XML se ha generado en el directorio de trabajo"));

}



void consmayor::xmlintervalo()
{
 QDomDocument doc("Mayor");
 QDomElement root = doc.createElement("Mayor");
 doc.appendChild(root);

 QDomElement tag = doc.createElement("Cabecera");
 root.appendChild(tag);

 addElementoTextoDom(doc,tag,"NombreEmpresa",filtracadxml(nombreempresa()));
 addElementoTextoDom(doc,tag,"Seleccion",filtracadxml(ui.nombrelabel->text()));

 QDate fechainicial=ui.inicialdateEdit->date();
 QDate fechafinal=ui.finaldateEdit->date();
 addElementoTextoDom(doc,tag,"FechaInicial",filtracadxml(fechainicial.toString("yyyy/MM/dd")));
 addElementoTextoDom(doc,tag,"FechaFinal",filtracadxml(fechafinal.toString("yyyy/MM/dd")));

 QDomElement tag2 = doc.createElement("Detalle");
 root.appendChild(tag2);

 double importe=0;
 int linactu=0;
 // sacamos línea de saldo inicial a través de la primera línea de la cuenta
 double saldoinicial=convapunto(ui.mayortable->item(linactu,6)->text()).toDouble()
                     -convapunto(ui.mayortable->item(linactu,4)->text()).toDouble()
                     +convapunto(ui.mayortable->item(linactu,5)->text()).toDouble();
 if (saldoinicial>0.0001 || saldoinicial<-0.0001)
 {
   QDomElement tag3 = doc.createElement("Apunte");
   tag2.appendChild(tag3);
   addElementoTextoDom(doc,tag3,"Cuenta","");
   addElementoTextoDom(doc,tag3,"Fecha",filtracadxml(fechainicial.toString("yyyy/MM/dd")));
   addElementoTextoDom(doc,tag3,"Asiento","");
   addElementoTextoDom(doc,tag3,"Concepto",tr("Saldo inicial"));
   if (saldoinicial>0)
     addElementoTextoDom(doc,tag3,"Debe",filtracadxml(formateanumero(saldoinicial,comadecimal,decimales)));
   else
     addElementoTextoDom(doc,tag3,"Debe","");
   if (saldoinicial<0)
     addElementoTextoDom(doc,tag3,"Haber",filtracadxml(formateanumero(saldoinicial,comadecimal,decimales)));
   else
    addElementoTextoDom(doc,tag3,"Haber","");
   addElementoTextoDom(doc,tag3,"Saldo",filtracadxml(formateanumero(saldoinicial,comadecimal,decimales)));
}
 while (linactu<ui.mayortable->rowCount())
          {
           QDomElement tag3 = doc.createElement("Apunte");
           tag2.appendChild(tag3);
           addElementoTextoDom(doc,tag3,"Cuenta",filtracadxml(ui.mayortable->item(linactu,0)->text()));
           // corregir aquí fecha
           QDate fecha=fecha.fromString(ui.mayortable->item(linactu,1)->text(),"dd.MM.yyyy");
           addElementoTextoDom(doc,tag3,"Fecha",filtracadxml(fecha.toString("yyyy/MM/dd")));
           addElementoTextoDom(doc,tag3,"Asiento",filtracadxml(ui.mayortable->item(linactu,2)->text()));
           addElementoTextoDom(doc,tag3,"Concepto",filtracadxml(ui.mayortable->item(linactu,3)->text()));

           importe=convapunto(ui.mayortable->item(linactu,4)->text()).toDouble();
           addElementoTextoDom(doc,tag3,"Debe",filtracadxml(formateanumero(importe,comadecimal,decimales)));

           importe=convapunto(ui.mayortable->item(linactu,5)->text()).toDouble();
           addElementoTextoDom(doc,tag3,"Haber",filtracadxml(formateanumero(importe,comadecimal,decimales)));

           importe=convapunto(ui.mayortable->item(linactu,6)->text()).toDouble();
           addElementoTextoDom(doc,tag3,"Saldo",filtracadxml(formateanumero(importe,comadecimal,decimales)));

           linactu++;
          }

    QString xml = doc.toString();

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("mayor_int.xml");
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero(pasa);
   if (! fichero.open( QIODevice::WriteOnly ) ) return;
   QTextStream stream( &fichero );
   stream.setEncoding(QStringConverter::Utf8);

   stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  // bssstream << "<?xml-stylesheet type=\"text/xsl\" href=\"plan2html.xslt\"?>\n";

  // ------------------------------------------------------------------------------------
  stream << xml;

  fichero.close();

}

void consmayor::borraasiento()
{
    QString asiento;

    if (ui.mayortable->currentItem()==0) return;
    int dia=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().left(2).toInt();
    int mes=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().mid(3,2).toInt();
    int anyo=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().right(4).toInt();
    QDate fecha(anyo,mes,dia);
    QString ejercicio=ejerciciodelafecha(fecha);
     asiento=ui.mayortable->item(ui.mayortable->currentRow(),2)->text();
     borrarasientofunc(asiento,ejercicio,true);
    refrescar();
}


void consmayor::visdoc()
{
  if (ui.mayortable->currentItem()==0) return;

  QString asiento;

  int dia=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().left(2).toInt();
  int mes=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().mid(3,2).toInt();
  int anyo=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().right(4).toInt();
  QDate fecha(anyo,mes,dia);
  QString ejercicio=ejerciciodelafecha(fecha);
  asiento=ui.mayortable->item(ui.mayortable->currentRow(),2)->text();

  QString fichdoc=basedatos::instancia()->copia_docdiario(asiento,ejercicio);

  if (fichdoc.isEmpty())
  {
    QString qapunte=ui.mayortable->item(ui.mayortable->currentRow(),11)->text();
    QString serie, numero;
    if (qapunte!="")
       {
        if (basedatos::instancia()->pase_en_facturas(qapunte,&serie,&numero))
           {
            QString archivopdf=nombre_fich_factura_apunte(qapunte);

            //QMessageBox::warning( this, tr("Edición de documentos"),archivopdf);

            if (!QFile::exists(archivopdf)) {
               QString fichero=latex_doc(serie,numero);
               if (fichero.isEmpty())
                {
                 QMessageBox::warning( this, tr("Edición de documentos"),
                                   tr("ERROR: No se ha podido generar Latex"));
                 return;
                }

               if (!genera_pdf_latex(fichero))
                  {
                   QMessageBox::information( this, tr("FACTURA EN PDF"),
                                          tr("ERROR: no ha sido posible generar el archivo PDF"));
                   return;
                  }
               archivopdf=fichero;
            }
            archivopdf.truncate(archivopdf.length()-4);
            archivopdf.append(".pdf");
            QString url=archivopdf;
            QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
            /* if (!ejecuta(aplicacionabrirfich(extensionfich(archivopdf)),archivopdf))
                  QMessageBox::warning( this, tr("FACTURA EN PDF"),
                                      tr("No se puede abrir ")+archivopdf+tr(" con ")+
                                      aplicacionabrirfich(extensionfich(archivopdf)) + "\n" +
                                      tr("Verifique el diálogo de preferencias")); */

           }
       }
    return;
  }


  if (basedatos::instancia()->gestor_doc_bd()) {
      QString cadnum;
      if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
          else cadnum=fichdoc.section('-',0,0);
      qlonglong num=cadnum.toLongLong();
      QByteArray outByteArray;
      outByteArray=basedatos::instancia()->documento_bd(num);
      QString url=QStandardPaths::writableLocation(
                  QStandardPaths::TempLocation).append(QDir::separator()).append(fichdoc);
      QFile file(url);
      file.open(QIODevice::WriteOnly);

      //EL BLOB OBTENIDO DE LA CONSULTA LO PASAMOS AL ARCHIVO
      file.write(outByteArray);
      file.close();
      QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
      return;
  }

  fichdoc=expanderutadocfich(fichdoc);

  QDesktopServices::openUrl(QUrl(fichdoc.prepend("file:"),QUrl::TolerantMode));

  /*if (!ejecuta(aplicacionabrirfich(extensionfich(fichdoc)),fichdoc))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+fichdoc+tr(" con ")+
                             aplicacionabrirfich(extensionfich(fichdoc)));*/

}



void consmayor::latex()
{
    int valor=0;
    if (ui.intervalogroupBox->isChecked())
       {
        latexintervalo();
        valor=editalatex(tr("mayor_int"));
       }
      else
       {
        cuadimprimayor *cuadimpri=new cuadimprimayor(comadecimal,decimales);
        cuadimpri->generalatexmayor( ui.subcuentalineEdit->text(),ui.subcuentalineEdit->text(),
                                  ui.inicialdateEdit->date(), ui.finaldateEdit->date() );
        delete cuadimpri;
        valor=editalatex("mayor");
       }
    if (valor==1)
        QMessageBox::warning( this, tr("Consulta de mayor"),tr("PROBLEMAS al llamar al editor Latex"));

}


void consmayor::consultaasiento()
{

    QString asiento;

    if (ui.mayortable->currentItem()==0) return;
    int dia=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().left(2).toInt();
    int mes=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().mid(3,2).toInt();
    int anyo=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().right(4).toInt();
    QDate fecha(anyo,mes,dia);
    asiento=ui.mayortable->item(ui.mayortable->currentRow(),2)->text();

    QString ejercicio=ejerciciodelafecha(fecha);
    QString qdiario;
    qdiario=diariodeasiento(asiento,ejercicio);

    if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
       {
         QMessageBox::warning( this, tr("Consultas de Mayor"),
                               tr("ERROR: No se pueden consultar asientos del diario de apertura, "
                                  "regularización o cierre\n"));
         return;
       }

    if (!existeasiento(asiento,ejercicio))
       {
         QMessageBox::warning( this, tr("Consultas de Mayor"),
                               tr("ERROR: El asiento seleccionado ya no existe"));
         return;
       }

    consultarasiento(asiento,usuario,ejercicio);

    // refrescar();

}


void consmayor::datoscuenta()
{
    QString cuenta=ui.subcuentalineEdit->text();
    if (cuenta.isEmpty()) return;
    if (!existesubcuenta(cuenta)) return;
    datos_accesorios *d = new datos_accesorios();
    d->cargacodigo( cuenta );
    d->exec();
    delete(d);
}


void consmayor::vencimientos()
{

    QString asiento;

    if (ui.mayortable->currentItem()==0) return;

    int dia=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().left(2).toInt();
    int mes=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().mid(3,2).toInt();
    int anyo=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().right(4).toInt();
    QDate fecha(anyo,mes,dia);
    asiento=ui.mayortable->item(ui.mayortable->currentRow(),2)->text();

    QString cadnumpase=ui.mayortable->item(ui.mayortable->currentRow(),11)->text();


    if (espasevtoprocesado(cadnumpase))
       {
         QMessageBox::warning( this, tr("CONSULTA VENCIMIENTOS EN PASE DIARIO"),
         tr("El pase seleccionado es un cobro o pago ya efectuado"));
         return;
       }
    QString caddebe;
    caddebe=convapunto(ui.mayortable->item(ui.mayortable->currentRow(),4)->text());
    QString cadhaber;
    cadhaber=convapunto(ui.mayortable->item(ui.mayortable->currentRow(),5)->text());
    QString subcuenta=ui.mayortable->item(ui.mayortable->currentRow(),0)->text();
    QString externo=basedatos::instancia()->externo_pase(ui.mayortable->item(ui.mayortable->currentRow(),11)->text());
    consultavencipase *losvenci=new consultavencipase(cadnumpase,
                              asiento,
                              caddebe,
                              cadhaber,
                              subcuenta,
                              fecha,externo);
    losvenci->exec();
    delete(losvenci);

}


void consmayor::nuevo_asiento()
{
    introfecha i(QDate::currentDate());
    i.esconde_herramientas();
    i.adjustSize();
    int resultado=i.exec();
    QDate fecha=i.fecha();
    QString diario=i.diario();
    QString doc=i.documento();

    /*introfecha *i = new introfecha(QDate::currentDate());
    int resultado=i->exec();
    QDate fecha=i->fecha();
    QString diario=i->diario();
    delete(i); */
    if (resultado!=QDialog::Accepted) return;
    tabla_asientos *t = new tabla_asientos(comadecimal,decimales,usuario);
    t->pasafechaasiento(fecha);
    t->pasadiario(diario);
    t->pasa_doc(doc);
    t->set_borrador();
    resultado=t->exec();
    delete(t);

    if (resultado!=QDialog::Accepted) return;

    refrescar();

}


void consmayor::procesa_vencimiento()
{
    if (ui.mayortable->currentItem()==0) return;

    QString apunte=ui.mayortable->item(ui.mayortable->currentRow(),11)->text();

    int numvenci = basedatos::instancia()->primer_venci_pte_de_pase(apunte);
    if (numvenci>=0)
      {
        QString cadnum; cadnum.setNum(numvenci);
        procesavencimiento *procvenci=new procesavencimiento(usuario);
        procvenci->cargadatos(cadnum);
        int resultado=procvenci->exec();
        if (resultado!=QDialog::Accepted)
           {
             QMessageBox::information( this, tr("Vencimientos"),tr("El procesado del vencimiento se ha cancelado"));
           }
           else refrescar();
        delete procvenci;
      }

}


void consmayor::acumuladoscta()
{
    acumuladosmes *a = new acumuladosmes();
    a->pasacodigocuenta(ui.subcuentalineEdit->text(),ui.inicialdateEdit->date());
    a->exec();
    delete(a);
}


void consmayor::asigna_ficherodocumento()
{
    if (ui.mayortable->currentItem()==0) return;


    QString elasiento=ui.mayortable->item(ui.mayortable->currentRow(),2)->text();

    QString cadnumpase=ui.mayortable->item(ui.mayortable->currentRow(),11)->text();

    QString documento=ui.mayortable->item(ui.mayortable->currentRow(),8)->text();

    QDate fechapase=basedatos::instancia()->fecha_pase_diario(cadnumpase);

    QString ejercicio=ejerciciodelafecha(fechapase);

    if (ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio))
     {
        QMessageBox::warning( this, tr("ASIGNAR FICHERO A DOCUMENTO"),
                              tr("ERROR: ejercicio cerrado"));
        return;
     }

    // QMessageBox::warning( this, tr("ASIGNAR FICHERO A DOCUMENTO"),
    //                      punterodiario->documentoactual());

    asigna_fichdoc *a = new asigna_fichdoc(elasiento,documento,
                                           ejercicio);
    a->pasafichdoc(basedatos::instancia()->copia_docdiario (elasiento,ejercicio));

    a->exec();
    delete(a);

}


void consmayor::quita_externo()
{
  ui.externolineEdit->clear();
  ui.descrip_externo_lineEdit->clear();
}


void consmayor::buscar_externo()
{
  busca_externo *b = new busca_externo();
  int resultado=b->exec();
  if (resultado==QDialog::Accepted)
     {
      ui.externolineEdit->setText(b->codigo_elec());
      //ui.descrip_externo_lineEdit->setText(b->nombre_elec());
      cod_externo_fin_edic();
     }
  delete(b);
}


void consmayor::cod_externo_fin_edic()
{
    if (!ui.externolineEdit->text().isEmpty())
      {
       ui.descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(ui.externolineEdit->text()));
       QString cod=basedatos::instancia()->cuenta_externo(ui.externolineEdit->text());
       if (!cod.isEmpty())
         ui.subcuentalineEdit->setText(cod);
      }
}

void consmayor::cod_externo_cambiado()
{
    if (!ui.externolineEdit->text().isEmpty())
      {
       ui.descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(ui.externolineEdit->text()));
       QString cod=basedatos::instancia()->cuenta_externo(ui.externolineEdit->text());
       if (!cod.isEmpty())
         ui.subcuentalineEdit->setText(cod);
      }
}


void consmayor::informe_intervalo()
{
    QString fileName = ":/informes/mayor_intervalo.xml";
    QtRPT *report = new QtRPT(this);
    //report->recordCount << ui.mayortable->rowCount();
    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("Imprimir mayor"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }
    QObject::connect(report, SIGNAL(setValueImage(const int, const QString, QImage&, const int)),
                         this, SLOT(setValueImage(const int, const QString, QImage&, const int)));

    QObject::connect(report, SIGNAL(setValue(const int, const QString, QVariant&, const int)),
                     this, SLOT(setValue_i(const int, const QString, QVariant&, const int)));
    QObject::connect(report, &QtRPT::setDSInfo,[&](DataSetInfo &dsInfo){
        dsInfo.recordCount = ui.mayortable->rowCount();
    });

    // report->printExec(true);

    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+tr("mayor.pdf");
           report->printPDF(qfichero);
    }

}

void consmayor::informe()
{
    if (ui.intervalogroupBox->isChecked()) {
        informe_intervalo();
        return;
    }
    QString ejercicio=ejerciciodelafecha(ui.inicialdateEdit->date());
    QDate fechainiejercicio=inicioejercicio(ejercicio);
    QDate fechaejercicioanterior=fechainiejercicio.addDays(-1);
    QString ejercicioanterior=ejerciciodelafecha(fechaejercicioanterior);
    QSqlQuery query;
    QString subctaactual=ui.subcuentalineEdit->text();
    QDate fechainicial=ui.inicialdateEdit->date();
    QDate fechafinal=ui.finaldateEdit->date();
    if (ejerciciocerrado(ejercicioanterior) || escuentadegasto(subctaactual) ||
        escuentadeingreso(subctaactual))
     {
         query = basedatos::instancia()->selectSumdebesumhaberdiariofechascuenta(fechainiejercicio, fechainicial, subctaactual,ui.externolineEdit->text());
     }
    else query = basedatos::instancia()->selectSumdebesumhaberdiariofechacuenta(fechainicial, subctaactual,ui.externolineEdit->text());

    double saldoin=0;
    double sumadebe=0;
    double sumahaber=0;
    double sumadebeextracto=0;
    double sumahaberextracto=0;

    double saldo=0;

    if ( (query.isActive()) && (query.first()) )
       {
        saldoin=query.value(0).toDouble()-query.value(1).toDouble();
        //sumadebe=query.value(0).toDouble();
        //sumahaber=query.value(1).toDouble();
        if (saldoin>0) sumadebe=saldoin;
        if (saldoin<0) sumahaber=-1*saldoin;
       }


  saldo=saldoin;

  QString cadsaldo; cadsaldo.setNum(fabs(saldoin),'f',2);

  if (ui.externolineEdit->text().isEmpty()) query= basedatos::instancia()->select5Diariofechascuentaorderfechapase(fechainicial, fechafinal, subctaactual);
     else query= basedatos::instancia()->selectDiariofechascuentaorderfechapaseExterno(fechainicial, fechafinal, subctaactual, ui.externolineEdit->text());

  fechal.clear();
  asientol.clear();
  conceptol.clear();
  debel.clear();
  haberl.clear();
  documentol.clear();
  externol.clear();
  saldol.clear();

  if (saldoin>0.0001 || saldoin<-0.0001) {
     fechal <<"";
     asientol << "";
     conceptol << tr("Saldo inicial ...........");
     if (saldoin>0) debel << cadsaldo; else debel <<"";
     if (saldoin<0) haberl << cadsaldo; else haberl << "";
     documentol << "";
     externol << "";
     saldol<<formatea_redondeado(saldo,false, 2);
  }

  if (query.isActive())
      while (query.next()) {
        // fecha,asiento,concepto,debe,haber,documento, externo
          fechal<<query.value(0).toDate().toString("dd/MM/yyyy");
          asientol<<query.value(1).toString();
          conceptol<<query.value(2).toString();
          debel<<query.value(3).toString();
          saldo+=query.value(3).toDouble();
          sumadebeextracto+=query.value(3).toDouble();
          haberl<<query.value(4).toString();
          saldo-=query.value(4).toDouble();
          sumahaberextracto+=query.value(4).toDouble();
          documentol<<query.value(5).toString();
          externol<<query.value(6).toString();
          saldol<<formatea_redondeado(saldo,false, 2);
      }

  cadsumadebeextracto=formatea_redondeado(sumadebeextracto,false, 2);
  cadsumahaberextracto=formatea_redondeado(sumahaberextracto,false, 2);

      QString fileName = ":/informes/mayor.xml";
      QtRPT *report = new QtRPT(this);
      //report->recordCount << asientol.count();
      if (report->loadReport(fileName) == false)
      {
          QMessageBox::information( this, tr("Imprimir mayor"),
                                    tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
          return;
      }
      QObject::connect(report, &QtRPT::setDSInfo,
                       [&](DataSetInfo &dsInfo) {
          dsInfo.recordCount=asientol.count();
      });

      QObject::connect(report, SIGNAL(setValueImage(const int, const QString, QImage&, const int)),
                           this, SLOT(setValueImage(const int, const QString, QImage&, const int)));

      QObject::connect(report, SIGNAL(setValue(const int, const QString, QVariant&, const int)),
                       this, SLOT(setValue(const int, const QString, QVariant&, const int)));
      if (hay_visualizador_extendido) report->printExec(true);
      else {
             QString qfichero=dirtrabajo();
             qfichero.append(QDir::separator());
             qfichero=qfichero+tr("mayor.pdf");
             report->printPDF(qfichero);
      }
}

void consmayor::setValueImage(const int recNo, const QString paramName, QImage &paramValue, const int reportPage) {
    //RptFieldObject *fieldObject2 = fieldObject.parentBand->parentReportPage->findFieldObjectByName("field2");
    Q_UNUSED(recNo);
    Q_UNUSED(reportPage);

       if (paramName == "logo") {

           QString imagen=basedatos::instancia()->logo_empresa();
           auto foto= new QImage();
           if (imagen.isEmpty()) imagen = logodefecto();
           if (imagen.length()>0)
              {
               QByteArray byteshexa;
               byteshexa.append ( imagen.toUtf8() );
               QByteArray bytes;
               bytes=bytes.fromHex ( byteshexa );
               foto->loadFromData( bytes, "PNG");
              } else return;

           paramValue = *foto;
       }
}


void consmayor::setValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage)
{
    Q_UNUSED(reportPage);

    if (paramName == "empresa")
        paramValue = basedatos::instancia()->selectEmpresaconfiguracion();

    if (paramName == "cuenta")
        paramValue = ui.subcuentalineEdit->text() + " " +ui.nombrelabel->text();

    if (paramName== "externo")
        paramValue = ui.externolineEdit->text() + " " + ui.descrip_externo_lineEdit->text();

    if (paramName == "fecha")
        paramValue = fechal.at(recNo);

    if (paramName == "asiento")
        paramValue = asientol.at(recNo);

    if (paramName == "concepto")
        paramValue = conceptol.at(recNo);

    if (paramName == "documento")
        paramValue = documentol.at(recNo);

    if (paramName == "debe")
        paramValue = debel.at(recNo);

    if (paramName == "haber")
        paramValue = haberl.at(recNo);

    if (paramName == "saldo")
        paramValue = saldol.at(recNo);

    if (paramName == "extracto_debe")
        paramValue = cadsumadebeextracto;

    if (paramName == "extracto_haber")
        paramValue = cadsumahaberextracto;

    if (paramName == "inicial")
        paramValue = ui.inicialdateEdit->date().toString("dd-MM-yyyy");

    if (paramName == "final")
        paramValue = ui.finaldateEdit->date().toString("dd-MM-yyyy");

}


void consmayor::setValue_i(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage)
{
    Q_UNUSED(reportPage);

    if (paramName == "empresa")
        paramValue = basedatos::instancia()->selectEmpresaconfiguracion();

   if (paramName == "cuenta")
       paramValue = ui.mayortable->item(recNo,0)->text();

    // if (paramName== "externo")
    //    paramValue = ui.externolineEdit->text() + " " + ui.descrip_externo_lineEdit->text();

    if (paramName == "fecha")
        paramValue = ui.mayortable->item(recNo,1)->text();

    if (paramName == "asiento")
        paramValue = ui.mayortable->item(recNo,2)->text();;

    if (paramName == "concepto")
        paramValue = ui.mayortable->item(recNo,3)->text();;

    if (paramName == "debe")
        paramValue = convapunto(ui.mayortable->item(recNo,4)->text());

    if (paramName == "haber")
        paramValue = convapunto(ui.mayortable->item(recNo,5)->text());

    if (paramName == "saldo")
        paramValue = convapunto(ui.mayortable->item(recNo,6)->text());

    if (paramName == "documento")
        paramValue = ui.mayortable->item(recNo,8)->text();;

    if (paramName == "inicial")
        paramValue = ui.inicialdateEdit->date().toString("dd-MM-yyyy");

    if (paramName == "final")
        paramValue = ui.finaldateEdit->date().toString("dd-MM-yyyy");

    if (paramName == "intervalo")
        paramValue = ui.nombrelabel->text();

}

void consmayor::concilia_ext() {
    concilia *c=new concilia(true,ui.subcuentalineEdit->text(),ui.inicialdateEdit->date(), ui.finaldateEdit->date(), usuario);
    c->exec();
    delete(c);
}

void consmayor::importa_c43 () {
  QString cuenta=ui.subcuentalineEdit->text();

 if (cuenta.length()==0) return;

  if (QMessageBox::question(
            this, tr("Importar fichero para conciliación"),
            tr("Este proceso añade información para la conciliación\n"
               "de cuentas bancarias. Sólo se añadirán fechas no\n"
               "importadas anteriormente en la cuenta %1.\n"
               "¿ Desea continuar ?").arg(cuenta)) ==QMessageBox::No )
                          return;

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  // dialogofich.setFilter("*.txt *.pln");
  dialogofich.setDirectory(adapta(dirtrabajobd()));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR CUADERNO 43"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
           QString cadfich=fileNames.at(0);
           bool correcto=false;
           correcto = f_importarfichc43(cadfich,cuenta);
           if (correcto)
               {
                QMessageBox::information( this, tr("FICHERO CONCILIACIÓN"),
                             tr("El archivo seleccionado se ha importado correctamente"));
               }
          }
     }

}

void consmayor::importa_csv () {

 QString cuenta=ui.subcuentalineEdit->text();
 if (cuenta.length()==0) return;

  if (QMessageBox::question(
            this, tr("Importar fichero para conciliación"),
            tr("Este proceso añade información para la conciliación\n"
               "de cuentas bancarias. Sólo se añadirán fechas no\n"
               "importadas anteriormente en la cuenta %1.\n"
               "¿ Desea continuar ?").arg(cuenta)) ==QMessageBox::No )
                          return;

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  // dialogofich.setFilter("*.txt *.pln");
  dialogofich.setDirectory(adapta(dirtrabajobd()));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR CSV"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
           QString cadfich=fileNames.at(0);
           bool correcto=false;
           correcto = f_importa_csv_cta_conci(cadfich,cuenta);
           if (correcto)
               {
                QMessageBox::information( this, tr("FICHERO CONCILIACIÓN"),
                             tr("El archivo seleccionado se ha importado correctamente"));
               }
          }
     }


}

void consmayor::elimina_cuaderno() {
 QString cuenta=ui.subcuentalineEdit->text();
 if (cuenta.length()==0) return;
 pidefecha *p=new pidefecha();
 p->setWindowTitle(tr("Eliminación importado a partir de fecha"));
 p->esconde_concepto_documento();
 int resultado=p->exec();
 QDate fecha=p->fecha();
 delete(p);

 if (resultado != QDialog::Accepted) return;

 if (QMessageBox::question(
          this,
          tr("Eliminación de cuenta externa"),
          tr("Se van a borrar todos los apuntes de la cuenta importada\n"
             "para conciliación, a partir de la fecha %1,\n"
             "la operación es irreversible ¿ Desea continuar ?").arg(fecha.toString("dd.MM.yyyy"))) == QMessageBox::Yes )
         {
            basedatos::instancia()->deleteCuenta_ext_concifechacuenta(fecha, cuenta);
            QMessageBox::information( this, tr("Borrar registros"),
                tr("SE HA REALIZADO LA OPERACIÓN"));
             }
}


void consmayor::editafechaasien()
{

  QString asiento;

  if (ui.mayortable->currentItem()==0) return;
  int dia=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().left(2).toInt();
  int mes=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().mid(3,2).toInt();
  int anyo=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().right(4).toInt();
  QDate fecha(anyo,mes,dia);
  asiento=ui.mayortable->item(ui.mayortable->currentRow(),2)->text();

  editafechaasiento *e = new editafechaasiento();

  e->pasadatos( asiento, fecha );

  e->exec();
  delete(e);
  refrescar();
}


void consmayor::edcondoc()
{
  if (ui.mayortable->currentItem()==0) return;
  QString qapunte=ui.mayortable->item(ui.mayortable->currentRow(),11)->text();
  int dia=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().left(2).toInt();
  int mes=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().mid(3,2).toInt();
  int anyo=ui.mayortable->item(ui.mayortable->currentRow(),1)->text().right(4).toInt();
  QDate qfecha(anyo,mes,dia);

  edit_conc_doc *e = new edit_conc_doc();
  e->pasadatos( qapunte, qfecha, ui.mayortable->item(ui.mayortable->currentRow(),3)->text(),
                ui.mayortable->item(ui.mayortable->currentRow(),8)->text() );
  e->todo_el_asiento();
  e->exec();
  delete(e);

 refrescar();
}


void consmayor::ed_registro_iva() {
    if (ui.mayortable->currentItem()==0) return;
    if (! esauxiliar(ui.subcuentalineEdit->text())) return;
    QString qapunte=ui.mayortable->item(ui.mayortable->currentRow(),11)->text();

    if (basedatos::instancia()->paseenlibroiva_isp(qapunte)) {
        QMessageBox::warning( this, tr("EDITAR REGISTRO DE IVA"),
        tr("Éste tipo de apuntes solo es editable en tabla de asientos"));
        return;
    }

    QString codigo=ui.subcuentalineEdit->text();

// ---------------------------------------------------------------------------------------------------

QString cadrectif,cadautofact,cadagrario;
    QSqlQuery query2 = basedatos::instancia()->select14Libroivapase(qapunte);
    if (query2.isActive())
      if (query2.next()) {
        QString cadsoportado="0";
        if (query2.value(10).toBool()) cadsoportado="1";
        if (query2.value(12).toBool()==true )
           cadrectif="1";
        if (query2.value(13).toBool()==true )
           cadautofact="1";
        if (query2.value(24).toBool()==true )
            cadagrario="1";
        if (escuentadeivasoportado(codigo)) {
            ivasoportado *i = new ivasoportado(comadecimal,decimales);

            i->pasadatos2( query2.value(0).toString(), formateanumero(query2.value(1).toDouble(),comadecimal,decimales),
                                 query2.value(2).toString(),
                                 formateanumero(query2.value(3).toDouble(),comadecimal,decimales), codigo,
                                 formateanumero(query2.value(5).toDouble(),comadecimal,decimales),
                                 query2.value(6).toString(),query2.value(14).toDate(),
                                 formateanumero(query2.value(11).toDouble()*100,comadecimal,decimales),
                                 cadrectif,
                                 query2.value(15).toDate(),query2.value(16).toString(),
                                 formateanumero(query2.value(17).toDouble(),comadecimal,decimales),
                                 query2.value(18).toString(), query2.value(19).toString(),
                                 query2.value(20).toString(), query2.value(21).toString(),
                                 query2.value(22).toBool() ? "1":"",
                                 formateanumero(query2.value(23).toDouble()*100,comadecimal,decimales), cadagrario,
                                 query2.value(25).toString(), query2.value(26).toString(),
                                 query2.value(29).toBool() ? "1" : "", query2.value(31).toBool() ? "1" : "",
                                 query2.value(32).toBool(),
                                 query2.value(35).toBool(), query2.value(36).toBool(),
                                 query2.value(37).toString());
            i->desactiva_base_tipo();

            if (i->exec()==QDialog::Accepted) {
                QString cuentabase, baseimponible, qclaveiva, qtipoiva, cuentaiva, cuotaiva;
                QString ctafra;
                QDate qfechafra;
                QString soportado, prorrata, qrectificativa; QDate fechaop;
                QString claveop, bicoste, frectif, numfact;
                QString facini, facfinal, bieninversion, afectacion, qagrario, qnombre;
                QString qcif, import, cajaiva;
                bool qno347, arrto_ret, arrto_sin_ret;
                QString dua;
                i->recuperadatos( &cuentabase, &baseimponible, &qclaveiva,
                                    &qtipoiva, &cuentaiva, &cuotaiva,
                                    &ctafra, &qfechafra, &soportado,
                                    &prorrata, &qrectificativa, &fechaop,
                                    &claveop,
                                    &bicoste,
                                    &frectif, &numfact,
                                    &facini, &facfinal, &bieninversion,
                                    &afectacion, &qagrario, &qnombre,
                                    &qcif, &import, &cajaiva, &qno347,
                                    &arrto_ret, &arrto_sin_ret, &dua);

                 int numero_facturas=numfact.toInt();
                 basedatos::instancia()->updateLibroiva(qapunte, cuentabase, baseimponible, qclaveiva,
                                qtipoiva, "", cuotaiva, ctafra,
                                qfechafra, soportado=="1", false, false, prorrata,
                                qrectificativa=="1", false,
                                fechaop, claveop, bicoste,
                                frectif, numero_facturas, facini, facfinal,
                                false, false, bieninversion=="1",
                                false, afectacion, qagrario=="1",
                                qnombre, qcif,
                                import=="1", false, false,
                                false, cajaiva=="1", qno347, false,
                                false, arrto_ret, arrto_sin_ret,dua,"");



            }
            delete(i);
        }
        else {
              if (escuentadeivarepercutido(codigo)) {
                  ivarepercutido *i = new ivarepercutido(comadecimal,decimales);
                  i->pasadatos2( query2.value(0).toString(),
                                 formateanumero(query2.value(1).toDouble(),comadecimal,decimales),
                                 query2.value(2).toString(),
                                 formateanumero(query2.value(3).toDouble(),comadecimal,decimales),
                                 formateanumero(query2.value(4).toDouble(),comadecimal,decimales), codigo,
                                 formateanumero(query2.value(5).toDouble(),comadecimal,decimales),
                                 query2.value(6).toString(),
                                 query2.value(14).toDate(), cadrectif,
                                 query2.value(15).toDate(),query2.value(16).toString(),
                                 formateanumero(query2.value(17).toString().toDouble(),comadecimal,decimales),
                                 query2.value(18).toString(), query2.value(19).toString(),
                                 query2.value(20).toString(), query2.value(21).toString(),
                                 query2.value(25).toString(), query2.value(26).toString(),
                                 query2.value(31).toString(),
                                 query2.value(35).toBool() ? "1" : "",query2.value(36).toBool() ? "1" : "");
                  if (i->exec()==QDialog::Accepted) {
                      QString cuentabase, baseimponible, qclaveiva;
                      QString qtipoiva, qtipore, cuentaiva, cuotaiva, ctafra;
                      QDate qfechafra;
                      QString soportado, qrectif;
                      QDate fechaop;
                      QString claveop, bicoste, frectif, numfact, facini, facfinal, nombre, cif;
                      QString caja, arrto_ret, arrto_sin_ret;
                      i->recuperadatos( &cuentabase, &baseimponible, &qclaveiva,
                                        &qtipoiva, &qtipore, &cuentaiva,
                                        &cuotaiva, &ctafra, &qfechafra,
                                        &soportado, &qrectif,
                                        &fechaop, &claveop,
                                        &bicoste,
                                        &frectif, &numfact,
                                        &facini, &facfinal, &nombre,
                                        &cif, &caja, &arrto_ret, &arrto_sin_ret);
                      int numero_facturas=numfact.toInt();
                      basedatos::instancia()->updateLibroiva(qapunte, cuentabase, baseimponible, qclaveiva,
                                     qtipoiva, qtipore, cuotaiva, ctafra,
                                     qfechafra, soportado=="1", false, false, "",
                                     qrectif=="1", false,
                                     fechaop, claveop, bicoste,
                                     frectif, numero_facturas, facini, facfinal,
                                     false, false, false,
                                     false, "", false,
                                     nombre, cif,
                                     false, false, false,
                                     false, caja=="1", false, false,
                                     false, arrto_ret=="1", arrto_sin_ret=="1","","");
                  }
                  delete(i);

              }
               else {
                  if (query2.value(39).toBool() || query2.value(40).toBool()) {
                  // eib ó eis
                  eib *e = new eib(comadecimal,decimales);
                  //pasadatos( QString cuentabase, QString baseimponible, QString ctafra,
                  //          QDate qfechafra, QDate qfechaop, QString claveop )
                  e->pasadatos(query2.value(0).toString(),formateanumero(query2.value(1).toDouble(),comadecimal,decimales),
                               query2.value(6).toString(),query2.value(14).toDate(),query2.value(15).toDate(), query2.value(16).toString());
                  e->esconde_externo();
                  if (query2.value(40).toBool()) e->selec_prservicios();

                  if (e->exec() ==QDialog::Accepted) {;
                      QString cuentabase, baseimponible, ctafra;
                      QDate qfechafra, qfechaop;
                      QString claveop, prservicios;
                      e->recuperadatos( &cuentabase, &baseimponible,  &ctafra,
                                  &qfechafra, &qfechaop, &claveop,
                                  &prservicios);
                      basedatos::instancia()->updateLibroiva(qapunte, cuentabase, baseimponible, "",
                                     "0", "0", "0", ctafra,
                                     qfechafra, false, false, prservicios!="1", "",
                                     "0", false,
                                     qfechaop, claveop, "",
                                     "",0 , "", "",
                                     false, prservicios=="1", false,
                                     false, "", false,
                                     "", "",
                                     false, false, false,
                                     false, "", false, false,
                                     false, false, false,"","");
                  }
                  delete(e);
                  }
                   else {
                   // exento
                      QString registro_donacion;
                      if (query2.value(41).toBool()) {
                         QJsonObject reg_donacion;
                         reg_donacion.insert("clave",query2.value(42).toString());
                         reg_donacion.insert("especie",query2.value(43).toBool());
                         reg_donacion.insert("recurrente",query2.value(44).toBool());
                         reg_donacion.insert("ca",query2.value(45).toString());
                         reg_donacion.insert("deduccion_ca",query2.value(46).toDouble());
                         QJsonDocument doc(reg_donacion);
                         registro_donacion=doc.toJson();
                        }
                   exento *e =new exento(comadecimal,decimales);
                   e->pasadatos(query2.value(0).toString(),formateanumero(query2.value(1).toDouble(),comadecimal,decimales),
                               query2.value(6).toString(),
                               query2.value(14).toDate(),query2.value(15).toDate(), query2.value(16).toString(),
                               cadrectif,query2.value(18).toString(),query2.value(19).toString(),
                               query2.value(20).toString(), query2.value(21).toString(),
                               query2.value(22).toBool() ? "1":"",query2.value(38).toBool() ? "1" : "",
                               query2.value(25).toString(), query2.value(26).toString(), query2.value(29).toString(),
                               query2.value(30).toString(),!query2.value(27).toBool() ? "1" : "",
                               query2.value(33).toBool() ? "1" : "",
                               (query2.value(28).toBool() && !query2.value(10).toBool()) ? "1" : "",registro_donacion, query2.value(37).toString());
                   if (cadsoportado!="1") e->fuerzaemitidas();
                    if (e->exec() ==QDialog::Accepted) {
                        QString cuentabase, baseimponible,  ctafra;
                        QDate qfechafra, fechaop;
                        QString claveop, rectificativa, frectif;
                        QString numfact;
                        QString facini, facfinal, bieninversion;
                        QString op_no_sujeta, nombre, cif;
                        QString importacion, exportacion;
                        QString exenta_no_deduc, vta_fuera_tac, emitida_isp_int;
                        QString dua;

                        e->recuperadatos(&cuentabase, &baseimponible,  &ctafra,
                                         &qfechafra, &fechaop,
                                         &claveop, &rectificativa, &frectif,
                                         &numfact,
                                         &facini, &facfinal, &bieninversion,
                                         &op_no_sujeta, &nombre, &cif,
                                         &importacion, &exportacion,
                                         &exenta_no_deduc, &vta_fuera_tac, &emitida_isp_int, &registro_donacion, &dua);
                        int numero_facturas=numfact.toInt();
                        basedatos::instancia()->updateLibroiva(qapunte, cuentabase, baseimponible, "",
                                       "0", "0", "0", ctafra,
                                       qfechafra, !e->eslibroemitidas(), false, false, "",
                                       rectificativa=="1", false,
                                       fechaop, claveop, "",
                                       frectif, numero_facturas, facini, facfinal,
                                       false, false, bieninversion=="1",
                                       op_no_sujeta=="1", "", false,
                                       nombre, cif,
                                       importacion=="1", exportacion=="1", exenta_no_deduc=="1",
                                       emitida_isp_int=="1", false, false, vta_fuera_tac=="1",
                                       false, false, false,dua, registro_donacion);
                    }
                    delete(e);
                  }
              }
        }
    }

}

void consmayor::ed_registro_ret() {
    if (ui.mayortable->currentItem()==0) return;
    if (! esauxiliar(ui.subcuentalineEdit->text())) return;
    QString qapunte=ui.mayortable->item(ui.mayortable->currentRow(),11)->text();
    QString codigo=ui.subcuentalineEdit->text();

    QSqlQuery query2 = basedatos::instancia()->datos_ret_pase (qapunte);
    if (query2.isActive())
        if (query2.next())
         {
            // SELECT pase, cta_retenido, arrendamiento, clave_ret,
            // base_ret, tipo_ret, retencion, ing_cta, ing_cta_rep,
            // nombre, cif, provincia
            QString cta_retenido=query2.value(1).toString();
            bool ret_arrendamiento=query2.value(2).toBool();
            QString clave=query2.value(3).toString();
            QString base_percepciones=query2.value(4).toString();
            QString tipo_ret=query2.value(5).toString();
            QString qretencion=query2.value(6).toString();
            QString ing_cta=query2.value(7).toString();
            QString ing_cta_repercutido=query2.value(8).toString();
            QString nombre_ret=query2.value(9).toString();
            QString cif_ret= query2.value(10).toString();
            QString provincia=query2.value(11).toString();

            retencion *r = new retencion(comadecimal,decimales);
            r->pasadatos_all(codigo,
                               cta_retenido, ret_arrendamiento,
                               clave, base_percepciones,
                               tipo_ret, qretencion,
                               ing_cta, ing_cta_repercutido,
                               nombre_ret, cif_ret, provincia);
            if (r->exec() ==QDialog::Accepted) {
                r->recuperadatos(&cta_retenido, &ret_arrendamiento,
                                 &clave, &base_percepciones,
                                 &tipo_ret, &qretencion,
                                 &ing_cta, &ing_cta_repercutido,
                                 &nombre_ret, &cif_ret, &provincia);

                basedatos::instancia()->modifica_retencion (qapunte,
                                                  cta_retenido, ret_arrendamiento,
                                                  clave, base_percepciones,
                                                  tipo_ret, qretencion,
                                                  ing_cta, ing_cta_repercutido,
                                                  nombre_ret, cif_ret, provincia);
            }
            delete(r);
        }
}

void consmayor::actu_conci_punteo() {
    for(int veces=0; veces<ui.mayortable->rowCount(); veces++) {
        if (ui.mayortable->item(veces,7)!=0)
            if (!ui.mayortable->item(veces,7)->text().isEmpty())
                basedatos::instancia()->updateDiarioconciliadodif_conciliacionpase (true, "", ui.mayortable->item(veces,11)->text());
    }
    QMessageBox::information( this, tr("Consulta de mayor"),
     tr("Se ha trasladado el punteo de vencimientos a conciliación"));
}

void consmayor::on_mayortable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn);
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);

    if (filas_borrador.contains(currentRow)) {
        ui.editarpushButton->setEnabled(true);
        ui.borrarpushButton->setEnabled(true);
    }
    else {
        ui.editarpushButton->setEnabled(false);
        ui.borrarpushButton->setEnabled(false);
    }
}

