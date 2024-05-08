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

#include "bss.h"
#include "funciones.h"
#include "pidenombre.h"
#include "introci.h"
#include "basedatos.h"
#include "consmayor.h"
#include "externos_cuenta.h"
#include <QMessageBox>
#include <QProgressDialog>
#include <qtrpt.h>


bss::bss(QString qusuario) : QDialog() {
    ui.setupUi(this);

    comadecimal=haycomadecimal();
    decimales=haydecimales();
    usuario=qusuario;
    QString ejercicio;
    ejercicio=ejerciciodelafecha(QDate::currentDate());
    if (ejercicio.length()==0) ejercicio = ejercicio_igual_o_maxigual(QDate::currentDate());
    if (ejercicio.length()>0)
        {
         ui.desdedateEdit->setDate(inicioejercicio(ejercicio));
         ui.hastadateEdit->setDate(finejercicio(ejercicio));
         QDate fecha_ant=inicioejercicio(ejercicio);
         fecha_ant=fecha_ant.addDays(-1);
         QString ejercicio2=ejerciciodelafecha(fecha_ant);
         if (ejercicio2.length()==0) ejercicio2=ejercicio;
         ui.desde_c_dateEdit->setDate(inicioejercicio(ejercicio2));
         ui.hasta_c_dateEdit->setDate(finejercicio(ejercicio2));
        }
        else
             {
              ui.desdedateEdit->setDate(QDate::currentDate());
              ui.hastadateEdit->setDate(QDate::currentDate());
              ui.desde_c_dateEdit->setDate(QDate::currentDate());
              ui.hasta_c_dateEdit->setDate(QDate::currentDate());

             }
    ui.desdelineEdit->setText("0");
    QString cadena;
    if (!cod_longitud_variable()) for (int veces=0;veces<anchocuentas();veces++) cadena=cadena+"9";
       else cadena="999";
    ui.hastalineEdit->setText(cadena);
    fichero=tr("bss");
    QStringList columnas;
    columnas << tr("CUENTA") << tr("DESCRIPCIÓN") << tr("SALDO INICIAL");
    columnas  << tr("SUMA DEBE") << tr("SUMA HABER") << tr("SALDO") ;

    columnas << tr("SALDO INICIAL 2");
    columnas  << tr("SUMA DEBE 2") << tr("SUMA HABER 2") << tr("SALDO 2") ;
    columnas << tr("DIF.SALDOS");

    ui.table->hideColumn(2); // columna de saldo inicial

    ui.table->hideColumn(6); // columna de saldo inicial comparación
    ui.table->hideColumn(7); // columna de suma debe comparación
    ui.table->hideColumn(8); // columna de suma haber comparación
    ui.table->hideColumn(9); // columna de saldo comparación
    ui.table->hideColumn(10); // columna de diferencia saldo entre periodos

    ui.si1_label->hide();
    ui.si1_lineEdit->hide();
    ui.si2_label->hide();
    ui.si2_lineEdit->hide();

    ui.sd2_label->hide();
    ui.sd2_lineEdit->hide();
    ui.sh2_label->hide();
    ui.sh2_lineEdit->hide();
    ui.ss2_label->hide();
    ui.ss2_lineEdit->hide();

    hay_analitica_tabla=basedatos::instancia()->analitica_tabla();

    ui.table->setHorizontalHeaderLabels(columnas);
    
    ui.diariostableWidget->verticalHeader()->hide();
    ui.diariostableWidget->horizontalHeader()->hide();
    ui.diariostableWidget->setColumnWidth(0,150);

    QCheckBox *checkbox0 = new QCheckBox(diario_apertura(), this);
    checkbox0->setChecked(true);
    ui.diariostableWidget->insertRow(0);
    ui.diariostableWidget->setCellWidget ( 0, 0, checkbox0 );
    connect((QCheckBox*)ui.diariostableWidget->cellWidget(0,0),
         SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
    QCheckBox *checkbox1 = new QCheckBox(diario_no_asignado(), this);
    checkbox1->setChecked(true);
    ui.diariostableWidget->insertRow(1);
    ui.diariostableWidget->setCellWidget ( 1, 0, checkbox1 );
    connect((QCheckBox*)ui.diariostableWidget->cellWidget(1,0),
         SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
    QCheckBox *checkbox2 = new QCheckBox(diario_regularizacion(), this);
    ui.diariostableWidget->insertRow(2);
    ui.diariostableWidget->setCellWidget ( 2, 0, checkbox2 );
    connect((QCheckBox*)ui.diariostableWidget->cellWidget(2,0),
         SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));

    QCheckBox *checkbox3 = new QCheckBox(diario_cierre(), this);
    ui.diariostableWidget->insertRow(3);
    ui.diariostableWidget->setCellWidget ( 3, 0, checkbox3 );
    connect((QCheckBox*)ui.diariostableWidget->cellWidget(3,0),
         SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));

    numdiarios=4;
    QSqlQuery query = basedatos::instancia()->selectCodigoDiariosordercodigo();
    while (query.next())
       {
           QCheckBox *checkbox = new QCheckBox(query.value(0).toString(), this);
           ui.diariostableWidget->insertRow(numdiarios);
           ui.diariostableWidget->setCellWidget ( numdiarios, 0, checkbox );
           connect((QCheckBox*)ui.diariostableWidget->cellWidget(numdiarios,0),
             SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
	   numdiarios++;

       }

   if (!basedatos::instancia()->hay_externos()) ui.externos_pushButton->hide();

    if (!conanalitica() && !conanalitica_parc()) ui.analiticagroupBox->setEnabled(false);

    connect(ui.actualizapushButton,SIGNAL(clicked()),this,SLOT(actualizar()));
    connect(ui.conmovcheckBox,SIGNAL(stateChanged(int)),this,SLOT(varsinmovs()));
    connect(ui.fichpushButton,SIGNAL(clicked()),this,SLOT(txtexport()));
    connect(ui.ImprimirpushButton,SIGNAL(clicked()),this,SLOT(imprimir()));
    connect(ui.gentexpushButton,SIGNAL(clicked()),this,SLOT(gentex()));

  connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(fijadescripciones()));
  connect(ui.buscapushButton,SIGNAL(clicked()),this,SLOT(buscapulsado()));

  connect(ui.cuentascheckBox,SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
  connect(ui.gruposcheckBox,SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
  connect(ui.subcuentascheckBox,SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
  connect(ui.subgruposcheckBox,SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
  connect(ui.c3digcheckBox,SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
  // connect(ui.aperturacheckBox,SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
  // connect(ui.generalcheckBox,SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));
  // connect(ui.regularizacioncheckBox,SIGNAL(stateChanged(int)),this,SLOT(activaactualiza()));

  connect(ui.desdedateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(activaactualiza()));
  connect(ui.hastadateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(activaactualiza()));

  connect(ui.desde_c_dateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(activaactualiza()));
  connect(ui.hasta_c_dateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(activaactualiza()));


  connect(ui.desdelineEdit,SIGNAL(textChanged(QString)),this,SLOT(activaactualiza()));
  connect(ui.hastalineEdit,SIGNAL(textChanged(QString)),this,SLOT(activaactualiza()));

  connect(ui.inicialescheckBox,SIGNAL(stateChanged(int)),this,SLOT(var_saldos_iniciales()));

  connect(ui.compa_groupBox,SIGNAL(clicked(bool)),this,SLOT(comparativo_check()));

  connect(ui.XMLpushButton,SIGNAL(clicked()),this,SLOT(genxml()));
  connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));

  connect(ui.mayorpushButton,SIGNAL(clicked()),SLOT(consultamayor()));

#ifdef NOEDITTEX
 ui.latexpushButton->hide();
#endif

}

double bss::saldo_inicial_ej1(QString aux)
{
  for (int veces=0;veces<aux_saldos_iniciales_ej1.count();veces++)
     {
      if (aux_saldos_iniciales_ej1.at(veces)==aux)
          return saldos_iniciales_aux_ej1.at(veces);
     }
  return 0;
}

double bss::saldo_inicial_ej2(QString aux)
{
  for (int veces=0;veces<aux_saldos_iniciales_ej2.count();veces++)
     {
      if (aux_saldos_iniciales_ej2.at(veces)==aux)
          return saldos_iniciales_aux_ej2.at(veces);
     }
  return 0;
}


double bss::saldo_inicial_princi_ej1(QString cod)
{
  double resultado=0;
  for (int veces=0;veces<aux_saldos_iniciales_ej1.count();veces++)
     {
      if (aux_saldos_iniciales_ej1.at(veces).startsWith(cod))
          resultado+=saldos_iniciales_aux_ej1.at(veces);
     }
  return resultado;
}

double bss::saldo_inicial_princi_ej2(QString cod)
{
  double resultado=0;
  for (int veces=0;veces<aux_saldos_iniciales_ej2.count();veces++)
     {
      if (aux_saldos_iniciales_ej2.at(veces).startsWith(cod))
          resultado+=saldos_iniciales_aux_ej2.at(veces);
     }
  return resultado;
}

double bss::saldo_hasta_fecha_ej1(QString aux)
{
  for (int veces=0; veces<aux_saldos_hasta_fecha_ej1.count();veces++)
     {
       if (aux_saldos_hasta_fecha_ej1.at(veces)==aux)
           return saldos_hasta_fecha_ej1.at(veces);
     }
  return 0;
}

double bss::saldo_hasta_fecha_ej2(QString aux)
{
    for (int veces=0; veces<aux_saldos_hasta_fecha_ej2.count();veces++)
       {
         if (aux_saldos_hasta_fecha_ej2.at(veces)==aux)
             return saldos_hasta_fecha_ej2.at(veces);
       }
    return 0;

}


double bss::saldo_hasta_fecha_princi_ej1(QString cod)
{
  double resultado=0;
  for (int veces=0; veces<aux_saldos_hasta_fecha_ej1.count();veces++)
     {
       if (aux_saldos_hasta_fecha_ej1.at(veces).startsWith(cod))
           resultado+=saldos_hasta_fecha_ej1.at(veces);
     }
  return resultado;
}

double bss::saldo_hasta_fecha_princi_ej2(QString cod)
{
    double resultado=0;
    for (int veces=0; veces<aux_saldos_hasta_fecha_ej2.count();veces++)
       {
         if (aux_saldos_hasta_fecha_ej2.at(veces).startsWith(cod))
             resultado+=saldos_hasta_fecha_ej2.at(veces);
       }
    return resultado;
}



void bss::actualizar()
{
   if (!ui.actualizapushButton->isEnabled()) return;
   ui.table->setRowCount(0);

   if (!ui.gruposcheckBox->isChecked() && !ui.subgruposcheckBox->isChecked() &&
       !ui.cuentascheckBox->isChecked() && !ui.subcuentascheckBox->isChecked()
       && !ui.c3digcheckBox->isChecked())
      return;

   // borramos listas
   aux_saldos_iniciales_ej1.clear();
   aux_saldos_iniciales_ej2.clear();
   saldos_iniciales_aux_ej1.clear();
   saldos_iniciales_aux_ej2.clear();

   aux_saldos_hasta_fecha_ej1.clear();
   saldos_hasta_fecha_ej1.clear();
   aux_saldos_hasta_fecha_ej2.clear();
   saldos_hasta_fecha_ej2.clear();


   aux_sumas_ej1.clear();
   sumasdebe_ej1.clear();
   sumashaber_ej1.clear();
   aux_sumas_ej2.clear();
   sumasdebe_ej2.clear();
   sumashaber_ej2.clear();

   codigo_cuenta.clear();
   es_auxiliar.clear();

   QString filtro;

     filtro = basedatos::instancia()->filtrobss( ui.gruposcheckBox->isChecked() ,
       ui.subgruposcheckBox->isChecked() , ui.c3digcheckBox->isChecked(),
       ui.cuentascheckBox->isChecked() ,
       ui.subcuentascheckBox->isChecked() , ui.cuentasgroupBox->isChecked(),
       ui.desdelineEdit->text() , ui.hastalineEdit->text() );

    if (filtro.length() == 0) { return; }

    // QMessageBox::warning( this, tr("BSS"),filtro);
    int filas = basedatos::instancia()->selectCountcodigoplancontablefiltro(filtro);
        ui.table->setRowCount(filas);
        for (int veces=0; veces<filas; veces++)
	    {
              for (int veces2=0;veces2<=10;veces2++)
               {
                QTableWidgetItem *newItem = new QTableWidgetItem("");
                if (veces2>=2 && veces2<=10) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                ui.table->setItem(veces,veces2,newItem);
               }
	    }

   QSqlQuery query = basedatos::instancia()->selectCodigodescripcionplancontablefiltro(filtro);
   int fila=0;
    if (query.isActive())
        while (query.next())
        {
            ui.table->item(fila,0)->setText(query.value(0).toString());
            codigo_cuenta << query.value(0).toString();
            es_auxiliar << query.value(2).toBool();
            ui.table->item(fila,1)->setText(query.value(1).toString());
            fila++;
        }

  ui.progressBar->setMaximum(6);
  bool comparativo=ui.compa_groupBox->isChecked();

  // cargamos listas de acumulados para cuentas auxiliares if (ui.desdedateEdit->date()==fecha_inicio_ejercicio
  // && ui.inicialescheckBox->isChecked() && qsindiarioapertura )

  int qqanchocuentas=anchocuentas();
  bool qcod_longitud_variable=cod_longitud_variable();
  QDate fecha_inicio_ejercicio=inicioejercicio(ejerciciodelafecha(ui.desdedateEdit->date()));
  bool qsindiarioapertura=sindiarioapertura();

  if (ui.desdedateEdit->date()==fecha_inicio_ejercicio
           && qsindiarioapertura)
     {
      QSqlQuery q=basedatos::instancia()->saldo_auxs_endiarioejercicio (ejerciciodelafecha(ui.desdedateEdit->date()),
                                                                        diario_apertura(),
                                                                        ui.cuentasgroupBox->isChecked(),
                                                                        ui.desdelineEdit->text(),
                                                                        ui.hastalineEdit->text());
      if (q.isActive())
              while (q.next())
                {
                  aux_saldos_iniciales_ej1 << q.value(0).toString();
                  saldos_iniciales_aux_ej1 << q.value(1).toDouble();
                }
      if (comparativo)
         {
           q=basedatos::instancia()->saldo_auxs_endiarioejercicio (ejerciciodelafecha(ui.desde_c_dateEdit->date()),
                                                                                      diario_apertura(),
                                                                                      ui.cuentasgroupBox->isChecked(),
                                                                                      ui.desdelineEdit->text(),
                                                                                      ui.hastalineEdit->text());
           if (q.isActive())
                  while (q.next())
                        {
                          aux_saldos_iniciales_ej2 << q.value(0).toString();
                          saldos_iniciales_aux_ej2 << q.value(1).toDouble();
                        }

         }
     }
     else
         {
           QSqlQuery q=basedatos::instancia()->saldo_auxs_hasta_fecha(ui.desdedateEdit->date(),ui.cuentasgroupBox->isChecked(),
                                                                      ui.desdelineEdit->text(),ui.hastalineEdit->text());
           if (q.isActive())
               while (q.next())
                  {
                   aux_saldos_hasta_fecha_ej1 << q.value(0).toString();
                   saldos_hasta_fecha_ej1 << q.value(1).toDouble();
                  }
           if (comparativo)
             {
               q=basedatos::instancia()->saldo_auxs_hasta_fecha(ui.desde_c_dateEdit->date(),ui.cuentasgroupBox->isChecked(),
                                                                ui.desdelineEdit->text(),ui.hastalineEdit->text());
               if (q.isActive())
                    while (q.next())
                        {
                         aux_saldos_hasta_fecha_ej2 << q.value(0).toString();
                         saldos_hasta_fecha_ej2 << q.value(1).toDouble();
                        }
             }
         }

  // cargamos sumas debe y haber
  /*
                QStringList aux_sumas_ej1;
                QList<double> sumasdebe_ej1;
                QList<double> sumashaber_ej1;
                QStringList aux_sumas_ej2;
                QList<double> sumasdebe_ej2;
                QList<double> sumashaber_ej2;

  */
  carga_sumas();
  ui.progressBar->setValue(1);
  // QMessageBox::information(this,"PRUEBAS",tr("PRIMERA FASE CARGA SUMAS"));
  // ----------------------------------------------------------

   // averiguamos información de saldos y sumas para auxiliares
   for (int veces=0;veces<filas;veces++)
        {
           bool cod_variable_auxiliar=false;
           if (qcod_longitud_variable) {
              int lugar=codigo_cuenta.indexOf(ui.table->item(veces,0)->text());
              if (lugar>-1) cod_variable_auxiliar=es_auxiliar.at(lugar);
           }

           if ((ui.table->item(veces,0)->text().length()==qqanchocuentas && !qcod_longitud_variable)
                   || (qcod_longitud_variable && cod_variable_auxiliar))
               //esauxiliar(ui.table->item(veces,0)->text())
             {
               // -----------------------------------------------------------------------------------------
               QString cadnum, cadnum2;
               if (ui.desdedateEdit->date()==fecha_inicio_ejercicio
                    && qsindiarioapertura )
                  {
                   cadnum=formateanumero(saldo_inicial_ej1(ui.table->item(veces,0)->text()),
                         comadecimal,decimales);
                   // QMessageBox::warning( 0, QObject::tr("bss"),cadnum);
                   if (comparativo)
                       cadnum2=formateanumero(saldo_inicial_ej2(ui.table->item(veces,0)->text()),
                             comadecimal,decimales);
                  }
                 else
                    // -----------------------------------------------------------------------------------------
                    {
                     cadnum=formateanumero(saldo_hasta_fecha_ej1(ui.table->item(veces,0)->text()),
                                           comadecimal,decimales);
                     if (comparativo) cadnum2=formateanumero(saldo_hasta_fecha_ej2 (ui.table->item(veces,0)->text()),
                                           comadecimal,decimales);
                    }
               ui.table->item(veces,2)->setText(cadnum);
               if (comparativo) ui.table->item(veces,6)->setText(cadnum2);

               // void sumas_debe_haber(QString aux, QString *debe, QString *haber);
               // void sumas_debe_haber_compa(QString aux, QString *debe, QString *haber);
               QString sdebe, shaber;
               sumas_debe_haber(ui.table->item(veces,0)->text(), &sdebe, &shaber);
               ui.table->item(veces,3)->setText(sdebe);
               ui.table->item(veces,4)->setText(shaber);
               if (comparativo)
                  {
                   sumas_debe_haber_compa(ui.table->item(veces,0)->text(), &sdebe, &shaber);
                   ui.table->item(veces,7)->setText(sdebe);
                   ui.table->item(veces,8)->setText(shaber);

                  }

            }
        }
   // QMessageBox::information(this,"PRUEBAS",tr("SEGUNDA FASE SUMAS TERMINADA"));
   ui.progressBar->setValue(2);

   // ui.progressBar->reset();
   // cargamos información de cuentas,subgrupos y grupos si hace falta
   // -----------------------------------------------------------------------------------
   if (!ui.subcuentascheckBox->isChecked())
    for (int veces=0;veces<filas;veces++)
	 {
          QString cadnum,cadnum2;
          if (ui.desdedateEdit->date()==fecha_inicio_ejercicio
            && ui.inicialescheckBox->isChecked() && sindiarioapertura() )
           {
            cadnum=formateanumero(saldo_inicial_princi_ej1(ui.table->item(veces,0)->text()),comadecimal,decimales);
            if (comparativo)
               cadnum2=formateanumero(saldo_inicial_princi_ej2(ui.table->item(veces,0)->text()),comadecimal,decimales);
           }
          else
             {
              cadnum=formateanumero(saldo_hasta_fecha_princi_ej1(ui.table->item(veces,0)->text()),comadecimal,decimales);
              if (comparativo)
                cadnum2=formateanumero(saldo_hasta_fecha_princi_ej2(ui.table->item(veces,0)->text()),comadecimal,decimales);
             }
            ui.table->item(veces,2)->setText(cadnum);
            if (comparativo) ui.table->item(veces,6)->setText(cadnum2);
            QString caddebe, cadhaber;

            sumas_debe_haber_princi(ui.table->item(veces,0)->text(), &caddebe, &cadhaber);
            ui.table->item(veces,3)->setText(caddebe);
            ui.table->item(veces,4)->setText(cadhaber);
            if (comparativo)
              {
               sumas_debe_haber_princi_compa(ui.table->item(veces,0)->text(), &caddebe, &cadhaber);
               ui.table->item(veces,7)->setText(caddebe);
               ui.table->item(veces,8)->setText(cadhaber);
              }
           // ui.progressBar->setValue(veces+1);
          }

   ui.progressBar->setValue(3);

   // llenamos celdas en blanco sólo para el caso de que hayan subcuentas
 double suma=0;
 int topecol=5;
 if (comparativo) topecol=9;
 int qanchocuentas=anchocuentas();
 bool qlong_variable=cod_longitud_variable();
 for (int columna=2;columna<topecol;columna++)
   {
     for (int veces=0;veces<filas;veces++)
        {
           if (ui.table->item(veces,columna)->text().length()==0)
	   {
	       suma=0;
	       for (int veces2=0;veces2<filas;veces2++)
	            {
                        bool cod_variable_auxiliar=false;
                          if (qlong_variable) {
                          int lugar=codigo_cuenta.indexOf(ui.table->item(veces2,0)->text());
                          if (lugar>-1) cod_variable_auxiliar=es_auxiliar.at(lugar);
                        }
                        if (ui.table->item(veces2,columna)->text().length()>0)
		          if (ui.table->item(veces2,0)->text().startsWith(ui.table->item(veces,0)->text()) &&
                             ((ui.table->item(veces2,0)->text().length()==qanchocuentas &&
                                 !qlong_variable) || cod_variable_auxiliar ))
		             suma+=convapunto(ui.table->item(veces2,columna)->text()).toDouble();
	            }
	       ui.table->item(veces,columna)->setText(formateanumero(suma,comadecimal,decimales));
	   }
        }
    }
 ui.progressBar->setValue(4);
 
 //QMessageBox::information(this,"PRUEBAS",tr("TERCERA FASE SUMAS TERMINADA"));

 // queda calcular la diferencia entre sumas para calcular los saldos
 double saldo=0;
 for (int veces=0;veces<filas;veces++)
      {
        saldo=convapunto(ui.table->item(veces,2)->text()).toDouble() + 
              convapunto(ui.table->item(veces,3)->text()).toDouble() - 
              convapunto(ui.table->item(veces,4)->text()).toDouble();
        ui.table->item(veces,5)->setText(formateanumero(saldo,comadecimal,decimales));
        if (comparativo)
          {
           double saldo2=convapunto(ui.table->item(veces,6)->text()).toDouble() +
              convapunto(ui.table->item(veces,7)->text()).toDouble() -
              convapunto(ui.table->item(veces,8)->text()).toDouble();
            ui.table->item(veces,9)->setText(formateanumero(saldo2,comadecimal,decimales));
            ui.table->item(veces,10)->setText(formateanumero(saldo2-saldo,comadecimal,decimales));
          }
      }
 // QMessageBox::information(this,"PRUEBAS",tr("cuarta FASE Saldos terminada"));
 ui.progressBar->setValue(5);
 varsinmovs();
 ui.table->resizeColumnsToContents();
 ui.timeEdit->setTime(QTime::currentTime());
 //ui.actualizapushButton->setEnabled(false);

 if (ui.inicialescheckBox->isChecked())
     actualiza_totales_si();
   else
     actualiza_totales();
 ui.progressBar->setValue(6);

}

void bss::carga_sumas()
{
    QString filtro2;

    for (int veces=0;veces<numdiarios;veces++)
    {
        if (((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->isChecked())
        {
            if (filtro2.length()>0) filtro2+=" or ";
            if (((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->text().remove('&')==diario_no_asignado())
            {
                filtro2+="diario=''";
            }
            else
            {
                filtro2+="diario='";
                filtro2+=((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->text().remove('&');
                filtro2+="'";
            }
        }
    }
    /*
                  QStringList aux_sumas_ej1;
                  QList<double> sumasdebe_ej1;
                  QList<double> sumashaber_ej1;
                  QStringList aux_sumas_ej2;
                  QList<double> sumasdebe_ej2;
                  QList<double> sumashaber_ej2;

    */
   // QMessageBox::warning( this, tr("BSS"),filtro2);
   QSqlQuery q;
   if (!(hay_analitica_tabla && ui.analiticagroupBox->isChecked()))
    {
     q=basedatos::instancia()->sumas_aux_dh(ui.desdedateEdit->date(),ui.hastadateEdit->date(),filtro2,
                                                  ui.cuentasgroupBox->isChecked(),
                                                  ui.desdelineEdit->text(),
                                                  ui.hastalineEdit->text(),
                                                  ui.analiticagroupBox->isChecked() && !hay_analitica_tabla,
                                                  ui.cilineEdit->text()
                                                  );
   }
    else
        {
          // analítica tabla
          q=basedatos::instancia()->sumas_aux_dh_analitica_tabla (ui.desdedateEdit->date(),ui.hastadateEdit->date(),
                                                                  ui.cilineEdit->text(),filtro2,ui.cuentasgroupBox->isChecked(),
                                                                  ui.desdelineEdit->text(),ui.hastalineEdit->text());
        }
   if (q.isActive())
     while (q.next())
       {
        aux_sumas_ej1 <<  q.value(0).toString();
        sumasdebe_ej1 << q.value(1).toDouble();
        sumashaber_ej1 << q.value(2).toDouble();
       }

   if (ui.compa_groupBox->isChecked())
    {
     if (!(hay_analitica_tabla && ui.analiticagroupBox->isChecked()))
       {
        q=basedatos::instancia()->sumas_aux_dh(ui.desde_c_dateEdit->date(),ui.hasta_c_dateEdit->date(),filtro2,
                                                      ui.cuentasgroupBox->isChecked(),
                                                      ui.desdelineEdit->text(),
                                                      ui.hastalineEdit->text(),
                                                      ui.analiticagroupBox->isChecked() && !hay_analitica_tabla,
                                                      ui.cilineEdit->text()
                                                      );
      }
     if (q.isActive())
         while (q.next())
           {
            aux_sumas_ej2 <<  q.value(0).toString();
            sumasdebe_ej2 << q.value(1).toDouble();
            sumashaber_ej2 << q.value(2).toDouble();
           }
    }
}


void bss::sumas_debe_haber(QString aux, QString *debe, QString *haber)
{
  for(int veces=0; veces<aux_sumas_ej1.count();veces++)
    {
      if (aux_sumas_ej1.at(veces)==aux)
        {
          *debe=formateanumero(sumasdebe_ej1.at(veces),comadecimal,decimales);
          *haber=formateanumero(sumashaber_ej1.at(veces),comadecimal,decimales);
          return;
        }
    }
  QString vacio;
  *debe=vacio;
  *haber=vacio;
}

void bss::sumas_debe_haber_compa(QString aux, QString *debe, QString *haber)
{
    for(int veces=0; veces<aux_sumas_ej2.count();veces++)
      {
        if (aux_sumas_ej2.at(veces)==aux)
          {
            *debe=formateanumero(sumasdebe_ej2.at(veces),comadecimal,decimales);
            *haber=formateanumero(sumashaber_ej2.at(veces),comadecimal,decimales);
            return;
          }
      }
    QString vacio;
    *debe=vacio;
    *haber=vacio;
}


void bss::sumas_debe_haber_princi(QString cod, QString *debe, QString *haber)
{
  double sumadebe=0; double sumahaber=0;
  for(int veces=0; veces<aux_sumas_ej1.count();veces++)
    {
      if (aux_sumas_ej1.at(veces).startsWith(cod))
        {
          sumadebe+=sumasdebe_ej1.at(veces);
          sumahaber+=sumashaber_ej1.at(veces);
          // *debe=formateanumero(sumasdebe_ej1.at(veces),comadecimal,decimales);
          // *haber=formateanumero(sumashaber_ej1.at(veces),comadecimal,decimales);
        }
    }
  QString caddebe, cadhaber;
  caddebe=formateanumero(sumadebe,comadecimal,decimales);
  cadhaber=formateanumero(sumahaber,comadecimal,decimales);
  *debe=caddebe;
  *haber=cadhaber;
}


void bss::sumas_debe_haber_princi_compa(QString cod, QString *debe, QString *haber)
{
    double sumadebe=0; double sumahaber=0;
    for(int veces=0; veces<aux_sumas_ej2.count();veces++)
      {
        if (aux_sumas_ej2.at(veces).startsWith(cod))
          {
            sumadebe+=sumasdebe_ej2.at(veces);
            sumahaber+=sumashaber_ej2.at(veces);
            // *debe=formateanumero(sumasdebe_ej1.at(veces),comadecimal,decimales);
            // *haber=formateanumero(sumashaber_ej1.at(veces),comadecimal,decimales);
          }
      }
    QString caddebe, cadhaber;
    caddebe=formateanumero(sumadebe,comadecimal,decimales);
    cadhaber=formateanumero(sumahaber,comadecimal,decimales);
    *debe=caddebe;
    *haber=cadhaber;
}



QString bss::sumadh(bool subcuenta, bool debe, QString codigo, QDate desde, QDate hasta)
{
    QString filtro2;

    for (int veces=0;veces<numdiarios;veces++)
    {
        if (((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->isChecked())
        {
            if (filtro2.length()>0) filtro2+=" or ";
            if (((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->text().remove('&')==diario_no_asignado())
            {
                filtro2+="diario=''";
            }
            else
            {
                filtro2+="diario='";
                filtro2+=((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->text().remove('&');
                filtro2+="'";
            }
        }
    }

    // QMessageBox::warning( this, tr("BSS"),filtro2);
    // QMessageBox::warning( this, tr("BSS"),cadena);

    if (filtro2.length()==0) return "0";

    // si no analítica tabla o analítica tabla y cuenta no es para analítica
    bool cuenta_para_analitica=es_cuenta_para_analitica(codigo);
    if (!ui.analiticagroupBox->isChecked())
    //if (!hay_analitica_tabla || (hay_analitica_tabla && !cuenta_para_analitica))
      return basedatos::instancia()->sumadh(subcuenta,debe,codigo,desde,hasta,filtro2,
                                       cuenta_para_analitica && ui.analiticagroupBox->isChecked() ,
                                       ui.cilineEdit->text() , comadecimal, decimales);
     else
          {
           if (!hay_analitica_tabla)
                 return basedatos::instancia()->sumadh(subcuenta,debe,codigo,desde,hasta,filtro2,
                                            cuenta_para_analitica && ui.analiticagroupBox->isChecked() ,
                                            ui.cilineEdit->text() , comadecimal, decimales);
               else
                 return basedatos::instancia()->sumadh_analitica_tabla (debe,codigo,
                                        desde,
                                        hasta, ui.cilineEdit->text(), filtro2,
                                        comadecimal,decimales);
          }
}




void bss::varsinmovs()
{
 // eliminamos, si procede las cuentas sin movimientos
 if  (ui.conmovcheckBox->isChecked())
    {
       int veces=0;
       while (veces<ui.table->rowCount())
          {
            double saldoini=convapunto(ui.table->item(veces,2)->text()).toDouble();
	    double sumadebe=convapunto(ui.table->item(veces,3)->text()).toDouble();
	    double sumahaber=convapunto(ui.table->item(veces,4)->text()).toDouble();
            if (ui.compa_groupBox->isChecked())
              {
                double saldoini2=convapunto(ui.table->item(veces,6)->text()).toDouble();
                double sumadebe2=convapunto(ui.table->item(veces,7)->text()).toDouble();
                double sumahaber2=convapunto(ui.table->item(veces,8)->text()).toDouble();
                if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                  && (saldoini>-0.001 && saldoini<0.001) &&
                  (sumadebe2>-0.001 && sumadebe<20.001) && (sumahaber2>-0.001 && sumahaber2<0.001)
                   && (saldoini2>-0.001 && saldoini2<0.001))
		ui.table->setRowHidden ( veces, true );
              }
              else
                  {
                   if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                    && (saldoini>-0.001 && saldoini<0.001))
                   ui.table->setRowHidden ( veces, true );
                  }
            veces++;
          }
    }
      else
             {
               int veces=0;
               while (veces<ui.table->rowCount())
                {
                 // double saldoini=convapunto(ui.table->item(veces,2)->text()).toDouble();
                 // double sumadebe=convapunto(ui.table->item(veces,3)->text()).toDouble();
                 // double sumahaber=convapunto(ui.table->item(veces,4)->text()).toDouble();
                 // if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                 // && (saldoini>-0.001 && saldoini<0.001))
                 ui.table->setRowHidden(veces,false);
                 veces++;
                }
            }

}




// ------------------------------------------------------------------------------------------------------


void bss::txtexport()
{
    if (ui.table->rowCount()==0) {
       QMessageBox::warning( this, tr("Balance de sumas y saldos"),tr("No hay registros, operación cancelada"));
    } else {
   QClipboard *cb = QApplication::clipboard();
   QString global;
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("bss.txt");
    QFile fichero(qfichero);
    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    // ---------------------------------------------------------------------------------------------
    if (ui.analiticagroupBox->isChecked() && ui.cilineEdit->text().length()>0)
        {
         global+=tr("CÓDIGO DE IMPUTACIÓN\t");
         global+=ui.cilineEdit->text();
         global+="\n";
         stream << tr("CÓDIGO DE IMPUTACIÓN\t");
         stream << ui.cilineEdit->text();
         stream << "\n";
         QString codigo=ui.cilineEdit->text();
         QString cadena,descripcion;
         QString qnivel=0;


         if (codigo.startsWith("???"))
            {
             global+= tr("CUALQUIERA");
             stream << tr("CUALQUIERA");
            }
         else
           {
            bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
            if (encontrada && qnivel.toInt()==1)
               {
                global+= descripcion;
                stream << descripcion;
               }
           }
         if (codigo.length()>3) { global+="\t"; stream << "\t";}
         if (codigo.length()>3 && codigo.mid(3,3)==QString("???")) 
             { global+= tr("CUALQUIERA"); stream << tr("CUALQUIERA");}
         else
          {
           bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
           int elnivel=qnivel.toInt();
           if (encontrada && elnivel==2)
              { global += descripcion; stream << descripcion; }
          }

         if (codigo.length()>6) { global+="\t"; stream << "\t";}
         if (codigo.length()==7 && codigo[6]=='*')
             { global+= tr("CUALQUIERA"); stream << tr("CUALQUIERA");}
         else
           {
             bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
             if (encontrada && qnivel.toInt()==3)
               {global+=descripcion; stream << descripcion;}
           }
         global+= "\n";
         stream << "\n";
        }

    // -----------------------------------------------------------------------------------------------------


    stream << "DESDE:\t" << ui.desdedateEdit->date().toString("dd.MM.yyyy") << "\t";
    global+="DESDE:\t";
    global+=ui.desdedateEdit->date().toString("dd.MM.yyyy");
    global+="\t";
    stream << "HASTA: \t" << ui.hastadateEdit->date().toString("dd.MM.yyyy") << "\n";
    global+="HASTA: \t";
    global+=ui.hastadateEdit->date().toString("dd.MM.yyyy");
    global+="\n";

    if (ui.compa_groupBox->isChecked())
       {
         stream << "DESDE 2:\t" << ui.desde_c_dateEdit->date().toString("dd.MM.yyyy") << "\t";
         global+="DESDE 2:\t";
         global+=ui.desde_c_dateEdit->date().toString("dd.MM.yyyy");
         global+="\t";
         stream << "HASTA 2: \t" << ui.hasta_c_dateEdit->date().toString("dd.MM.yyyy") << "\n";
         global+="HASTA 2: \t";
         global+=ui.hasta_c_dateEdit->date().toString("dd.MM.yyyy");
         global+="\n";
       }


    stream << "DESDE CUENTA:\t" << ui.desdelineEdit->text() << "\t";
    global+="DESDE CUENTA:\t";
    global+=ui.desdelineEdit->text();
    global+="\t";
    stream << "HASTA CUENTA: \t" << ui.hastalineEdit->text() << "\n";
    global+="HASTA CUENTA: \t";
    global+=ui.hastalineEdit->text();
    global+="\n";
    for (int veces2=0;veces2<10;veces2++)
             {
               stream << ui.table->horizontalHeaderItem ( veces2 )->text();
               global+=ui.table->horizontalHeaderItem ( veces2 )->text();
               if (veces2!=9) { stream << "\t"; global+="\t"; }
               if (veces2==9)  { stream << "\n"; global+="\n"; }
              }
    for (int veces=0;veces<ui.table->rowCount();veces++)
     {
      if (ui.table->isRowHidden(veces)) continue;
      for (int veces2=0;veces2<10;veces2++)
	{
	   QString cad=ui.table->item(veces,veces2)->text();
	   // if (cad[cad.length()-3]=='.') cad[cad.length()-3]=',';
	   stream << cad;
	   global+=cad;
           if (veces2!=9) { stream << "\t"; global+="\t"; }
           if (veces2==9) { stream << "\n"; global+="\n"; }
        }
     }
    
    fichero.close();
    cb->setText(global);
    QMessageBox::information( this, tr("Balance de sumas y saldos"),
			      tr("Se ha pasado el contenido al portapales"));
    }
}

void bss::informe_bss() {
    bool haysubcuentas=false;
    // bool solocuentasauxiliares=false;
    bool compa=ui.compa_groupBox->isChecked();
    int veces=0;
    while (veces<ui.table->rowCount())
      {
        if ((ui.table->item(veces,0)->text().length()==anchocuentas() && !cod_longitud_variable())
            || (cod_longitud_variable() && esauxiliar(ui.table->item(veces,0)->text())))
          {
           haysubcuentas=true;
           break;
          }
        veces++;
      }
    // construimos listas con contenido
    QStringList cuentas;
    QStringList titulos;
    QStringList sumasdebe;
    QStringList sumashaber;
    QStringList saldosdeudores;
    QStringList saldosacreedores;
    QStringList sumasdebe2;
    QStringList sumashaber2;
    QStringList saldosdeudores2;
    QStringList saldosacreedores2;
    double totaldebe=0;
    double totalhaber=0;
    double totalsaldodeudor=0;
    double totalsaldoacreedor=0;
    double totaldebe2=0;
    double totalhaber2=0;
    double totalsaldodeudor2=0;
    double totalsaldoacreedor2=0;

    QString guardacta, guardacta2; // esto es para control de sumas cuando no hay auxiliares

    int pos=0;
    while (pos<ui.table->rowCount()) {
        if (ui.conmovcheckBox->isChecked())
           {
              double saldoinicial=convapunto(ui.table->item(pos,2)->text()).toDouble();
              double sumadebe=convapunto(ui.table->item(pos,3)->text()).toDouble();
              double sumahaber=convapunto(ui.table->item(pos,4)->text()).toDouble();
              if (!compa)
                {
                 if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                    && (saldoinicial>-0.001 && saldoinicial<0.001))
                   {pos++; continue;}
                }
              else
                  {
                   double saldoinicial2=convapunto(ui.table->item(pos,6)->text()).toDouble();
                   double sumadebe2=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   double sumahaber2=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   if ((sumadebe>-0.001 && sumadebe<0.001)
                       && (sumahaber>-0.001 && sumahaber<0.001)
                      && (saldoinicial>-0.001 && saldoinicial<0.001) &&
                         (sumadebe2>-0.001 && sumadebe2<0.001)
                      && (sumahaber2>-0.001 && sumahaber2<0.001)
                      && (saldoinicial2>-0.001 && saldoinicial2<0.001))
                       {pos++; continue;}
                  }
           }
        cuentas <<  ui.table->item(pos,0)->text();
        titulos <<  ui.table->item(pos,1)->text();
        sumasdebe << convapunto(ui.table->item(pos,3)->text());
        sumashaber << convapunto(ui.table->item(pos,4)->text());
        double saldocalc=convapunto(ui.table->item(pos,5)->text()).toDouble();
        saldosdeudores << (saldocalc > 0 ? formateanumero(saldocalc,false,decimales): "0");
        saldosacreedores << (saldocalc < 0 ? formateanumero(-1*saldocalc,false,decimales): "0");
        // tenemos que arrastrar totales en función de si la cuenta es auxiliar
        if (ui.table->item(pos,0)->text().length()==anchocuentas() || esauxiliar(ui.table->item(pos,0)->text()))
          {
           totaldebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
           totalhaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
           totalsaldodeudor+=(saldocalc > 0 ? saldocalc: 0);
           totalsaldoacreedor+=(saldocalc < 0 ? -1*saldocalc : 0);
          }
        if (!haysubcuentas)
          {
           if (pos==0)
              {
               guardacta=ui.table->item(pos,0)->text();
               totaldebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
               totalhaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
               totalsaldodeudor+=(saldocalc > 0 ? saldocalc: 0);
               totalsaldoacreedor+=(saldocalc < 0 ? -1*saldocalc : 0);
              }
           if (pos>0)
              if(!ui.table->item(pos,0)->text().startsWith(guardacta ))
                {
                 totaldebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
                 totalhaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
                 totalsaldodeudor+=(saldocalc > 0 ? saldocalc: 0);
                 totalsaldoacreedor+=(saldocalc < 0 ? -1*saldocalc : 0);
                 guardacta=ui.table->item(pos,0)->text();
                }
          }
        if (compa) {
            sumasdebe2 << convapunto(ui.table->item(pos,7)->text());
            sumashaber2 << convapunto(ui.table->item(pos,8)->text());
            double saldocalc=convapunto(ui.table->item(pos,9)->text()).toDouble();
            saldosdeudores2 << (saldocalc > 0 ? formateanumero(saldocalc,false,decimales): "0");
            saldosacreedores2 << (saldocalc < 0 ? formateanumero(-1*saldocalc,false,decimales): "0");
            // tenemos que arrastrar totales en función de si la cuenta es auxiliar
            if (ui.table->item(pos,0)->text().length()==anchocuentas() || esauxiliar(ui.table->item(pos,0)->text()))
              {
               totaldebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
               totalhaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
               totalsaldodeudor2+=(saldocalc > 0 ? saldocalc: 0);
               totalsaldoacreedor2+=(saldocalc < 0 ? -1*saldocalc : 0);
              }
            if (!haysubcuentas)
              {
               if (pos==0)
                  {
                   guardacta2=ui.table->item(pos,0)->text();
                   totaldebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   totalhaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   totalsaldodeudor2+=(saldocalc > 0 ? saldocalc: 0);
                   totalsaldoacreedor2+=(saldocalc < 0 ? -1*saldocalc : 0);
                  }
               if (pos>0)
                  if(!ui.table->item(pos,0)->text().startsWith(guardacta2 ))
                    {
                     totaldebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                     totalhaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                     totalsaldodeudor2+=(saldocalc > 0 ? saldocalc: 0);
                     totalsaldoacreedor2+=(saldocalc < 0 ? -1*saldocalc : 0);
                     guardacta2=ui.table->item(pos,0)->text();
                    }
              }
        }

        pos++;
    } // del while

    QString fileName;
    fileName =  ":/informes/bss.xml" ;
    if (compa) fileName =  ":/informes/bss_compa.xml" ;
    QtRPT *report = new QtRPT(this);
    // report->recordCount << cuentas.count();
    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("Balance de sumas y saldos"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }

    QObject::connect(report, &QtRPT::setDSInfo,
                     [&](DataSetInfo &dsInfo) {
        dsInfo.recordCount=cuentas.count();;
    });

    QString imagen=basedatos::instancia()->logo_empresa();

      connect(report, &QtRPT::setValueImage,[&](const int recNo, const QString paramName, QImage &paramValue, const int reportPage) {
        Q_UNUSED(recNo);
        Q_UNUSED(reportPage);
        if (paramName == "logo") {

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
    });


    connect(report, &QtRPT::setValue, [&](const int recNo, const QString paramName, QVariant &paramValue, const int reportPage) {
        // campos: descrip, nota, cifra1, cifra2
        // recordCount es una lista, añadir también segunda página

        // QMessageBox::warning( this, tr("Estados Contables"),tr("Num %1").arg(recNo));
        Q_UNUSED(reportPage);
        if (paramName == "cuenta") paramValue = cuentas.at(recNo);

        if (paramName == "descripcion") paramValue = titulos.at(recNo);

        if (paramName == "empresa") paramValue = basedatos::instancia()->selectEmpresaconfiguracion();
        if (paramName == "ANALITICA" ) paramValue = ui.cilineEdit->text();

        if (paramName == "FECHAS1") paramValue = ui.desdedateEdit->date().toString("dd/MM/yyyy") + " - " +
                    ui.hastadateEdit->date().toString("dd/MM/yyyy");

        if (paramName == "FECHAS2") paramValue = ui.desde_c_dateEdit->date().toString("dd/MM/yyyy") + " - " +
                    ui.hasta_c_dateEdit->date().toString("dd/MM/yyyy");

        if (paramName == "sumasdebe") paramValue = sumasdebe.at(recNo);
        if (paramName == "sumashaber") paramValue = sumashaber.at(recNo);
        if (paramName == "saldodeudor") paramValue = saldosdeudores.at(recNo);
        if (paramName == "saldoacreedor") paramValue = saldosacreedores.at(recNo);        

        if (paramName == "totaldebe") paramValue = formateanumero(totaldebe,false,decimales);
        if (paramName == "totalhaber") paramValue = formateanumero(totalhaber,false,decimales);
        if (paramName == "totalsaldodeudor") paramValue = formateanumero(totalsaldodeudor,false,decimales);
        if (paramName == "totalsaldoacreedor") paramValue = formateanumero(totalsaldoacreedor,false,decimales);

        if (totaldebe>totalhaber)
         if (paramName == "difdebe") paramValue = formateanumerosep(totaldebe-totalhaber,comadecimal,decimales);

        if (totalhaber>totaldebe)
            if (paramName == "difhaber") paramValue = formateanumerosep(totalhaber-totaldebe,comadecimal,decimales);

        if (paramName == "sumasdebe2") paramValue = sumasdebe2.at(recNo);
        if (paramName == "sumashaber2") paramValue = sumashaber2.at(recNo);
        if (paramName == "saldodeudor2") paramValue = saldosdeudores2.at(recNo);
        if (paramName == "saldoacreedor2") paramValue = saldosacreedores2.at(recNo);

        if (paramName == "totaldebe2") paramValue = formateanumero(totaldebe2,false,decimales);
        if (paramName == "totalhaber2") paramValue = formateanumero(totalhaber2,false,decimales);
        if (paramName == "totalsaldodeudor2") paramValue = formateanumero(totalsaldodeudor2,false,decimales);
        if (paramName == "totalsaldoacreedor2") paramValue = formateanumero(totalsaldoacreedor2,false,decimales);

        /*
        if (ui.fechas_ej2_groupBox->isChecked()) {
            if (paramName == "FECHAS2") paramValue = ui.ini_ej2_dateEdit->date().toString("dd/MM/yyyy") + " - " +
                    ui.fin_ej2_dateEdit->date().toString("dd/MM/yyyy");
        }
        if (!ui.cilineEdit->text().isEmpty()) {
            if (paramName == "ANALITICA") paramValue = ui.cilineEdit->text();
                    //+ " " + ui.nivel1lineEdit->text()+ "-" +
                    //ui.nivel2lineEdit->text() + "-" + ui.nivel3lineEdit->text();
        }*/

    });


    //  report->printExec(true);

    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+tr("bss.pdf");
           report->printPDF(qfichero);
    }


}

void bss::informe_bcomp() {
    bool haysubcuentas=false;
    // bool solocuentasauxiliares=false;
    bool compa=ui.compa_groupBox->isChecked();
    int veces=0;
    while (veces<ui.table->rowCount())
      {
        if ((ui.table->item(veces,0)->text().length()==anchocuentas() && !cod_longitud_variable())
            || (cod_longitud_variable() && esauxiliar(ui.table->item(veces,0)->text())))
          {
           haysubcuentas=true;
           break;
          }
        veces++;
      }
    // construimos listas con contenido
    QStringList cuentas;
    QStringList titulos;
    QStringList saldosiniciales;
    QStringList sumasdebe;
    QStringList sumashaber;
    QStringList saldos;
    QStringList saldosiniciales2;
    QStringList sumasdebe2;
    QStringList sumashaber2;
    QStringList saldos2;
    double totalsaldoinicial=0;
    double totaldebe=0;
    double totalhaber=0;
    double totalsaldo=0;
    double totalsaldoinicial2=0;
    double totaldebe2=0;
    double totalhaber2=0;
    double totalsaldo2=0;

    QString guardacta, guardacta2; // esto es para control de sumas cuando no hay auxiliares

    int pos=0;
    while (pos<ui.table->rowCount()) {
        if (ui.conmovcheckBox->isChecked())
           {
              double saldoinicial=convapunto(ui.table->item(pos,2)->text()).toDouble();
              double sumadebe=convapunto(ui.table->item(pos,3)->text()).toDouble();
              double sumahaber=convapunto(ui.table->item(pos,4)->text()).toDouble();
              if (!compa)
                {
                 if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                    && (saldoinicial>-0.001 && saldoinicial<0.001))
                   {pos++; continue;}
                }
              else
                  {
                   double saldoinicial2=convapunto(ui.table->item(pos,6)->text()).toDouble();
                   double sumadebe2=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   double sumahaber2=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   if ((sumadebe>-0.001 && sumadebe<0.001)
                       && (sumahaber>-0.001 && sumahaber<0.001)
                      && (saldoinicial>-0.001 && saldoinicial<0.001) &&
                         (sumadebe2>-0.001 && sumadebe2<0.001)
                      && (sumahaber2>-0.001 && sumahaber2<0.001)
                      && (saldoinicial2>-0.001 && saldoinicial2<0.001))
                       {pos++; continue;}
                  }
           }
        cuentas <<  ui.table->item(pos,0)->text();
        titulos <<  ui.table->item(pos,1)->text();
        saldosiniciales << convapunto(ui.table->item(pos,2)->text());
        sumasdebe << convapunto(ui.table->item(pos,3)->text());
        sumashaber << convapunto(ui.table->item(pos,4)->text());
        saldos << convapunto(ui.table->item(pos,5)->text());
        // tenemos que arrastrar totales en función de si la cuenta es auxiliar
        if (ui.table->item(pos,0)->text().length()==anchocuentas() || esauxiliar(ui.table->item(pos,0)->text()))
          {
           totaldebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
           totalhaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
           totalsaldoinicial+=convapunto(ui.table->item(pos,2)->text()).toDouble();;
           totalsaldo+=convapunto(ui.table->item(pos,5)->text()).toDouble();
          }
        if (!haysubcuentas)
          {
           if (pos==0)
              {
               guardacta=ui.table->item(pos,0)->text();
               totaldebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
               totalhaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
               totalsaldoinicial+=convapunto(ui.table->item(pos,2)->text()).toDouble();;
               totalsaldo+=convapunto(ui.table->item(pos,5)->text()).toDouble();
              }
           if (pos>0)
              if(!ui.table->item(pos,0)->text().startsWith(guardacta ))
                {
                 totaldebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
                 totalhaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
                 totalsaldoinicial+=convapunto(ui.table->item(pos,2)->text()).toDouble();;
                 totalsaldo+=convapunto(ui.table->item(pos,5)->text()).toDouble();
                 guardacta=ui.table->item(pos,0)->text();
                }
          }
        if (compa) {
            saldosiniciales2 << convapunto(ui.table->item(pos,6)->text());
            sumasdebe2 << convapunto(ui.table->item(pos,7)->text());
            sumashaber2 << convapunto(ui.table->item(pos,8)->text());
            saldos2 << convapunto(ui.table->item(pos,9)->text());
            // tenemos que arrastrar totales en función de si la cuenta es auxiliar
            if (ui.table->item(pos,0)->text().length()==anchocuentas() || esauxiliar(ui.table->item(pos,0)->text()))
              {
               totalsaldoinicial2 += convapunto(ui.table->item(pos,6)->text()).toDouble();
               totaldebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
               totalhaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
               totalsaldo2+=convapunto(ui.table->item(pos,9)->text()).toDouble();
              }
            if (!haysubcuentas)
              {
               if (pos==0)
                  {
                   totalsaldoinicial2 += convapunto(ui.table->item(pos,6)->text()).toDouble();
                   guardacta2=ui.table->item(pos,0)->text();
                   totaldebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   totalhaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   totalsaldo2+=convapunto(ui.table->item(pos,9)->text()).toDouble();
                  }
               if (pos>0)
                  if(!ui.table->item(pos,0)->text().startsWith(guardacta2 ))
                    {
                     totalsaldoinicial2 += convapunto(ui.table->item(pos,6)->text()).toDouble();
                     totaldebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                     totalhaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                     saldos2 << convapunto(ui.table->item(pos,9)->text());
                     totalsaldo2+=convapunto(ui.table->item(pos,9)->text()).toDouble();
                    }
              }
        }

        pos++;
    } // del while

    QString fileName;
    fileName =  ":/informes/bcomp.xml" ;
    if (compa) fileName =  ":/informes/bcomp_compa.xml" ;
    QtRPT *report = new QtRPT(this);
    // report->recordCount << cuentas.count();
    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("Balance de sumas y saldos"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }

    QObject::connect(report, &QtRPT::setDSInfo,
                     [&](DataSetInfo &dsInfo) {
        dsInfo.recordCount=cuentas.count();;
    });

    QString imagen=basedatos::instancia()->logo_empresa();

      connect(report, &QtRPT::setValueImage,[&](const int recNo, const QString paramName, QImage &paramValue, const int reportPage) {
        Q_UNUSED(recNo);
        Q_UNUSED(reportPage);
        if (paramName == "logo") {

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
    });


    connect(report, &QtRPT::setValue, [&](const int recNo, const QString paramName, QVariant &paramValue, const int reportPage) {
        // campos: descrip, nota, cifra1, cifra2
        // recordCount es una lista, añadir también segunda página

        // QMessageBox::warning( this, tr("Estados Contables"),tr("Num %1").arg(recNo));
        Q_UNUSED(reportPage);
        if (paramName == "cuenta") paramValue = cuentas.at(recNo);

        if (paramName == "descripcion") paramValue = titulos.at(recNo);

        if (paramName == "empresa") paramValue = basedatos::instancia()->selectEmpresaconfiguracion();
        if (paramName == "ANALITICA" ) paramValue = ui.cilineEdit->text();

        if (paramName == "FECHAS1") paramValue = ui.desdedateEdit->date().toString("dd/MM/yyyy") + " - " +
                    ui.hastadateEdit->date().toString("dd/MM/yyyy");

        if (paramName == "FECHAS2") paramValue = ui.desde_c_dateEdit->date().toString("dd/MM/yyyy") + " - " +
                    ui.hasta_c_dateEdit->date().toString("dd/MM/yyyy");

        if (paramName == "sumasdebe") paramValue = sumasdebe.at(recNo);
        if (paramName == "sumashaber") paramValue = sumashaber.at(recNo);
        if (paramName == "saldoinicial") paramValue = saldosiniciales.at(recNo);
        if (paramName == "saldo") paramValue = saldos.at(recNo);

        if (paramName == "totaldebe") paramValue = formateanumero(totaldebe,false,decimales);
        if (paramName == "totalhaber") paramValue = formateanumero(totalhaber,false,decimales);
        if (paramName == "totalsaldoini") paramValue = formateanumero(totalsaldoinicial,false,decimales);
        if (paramName == "totalsaldo") paramValue = formateanumero(totalsaldo,false,decimales);

        if (paramName == "sumasdebe2") paramValue = sumasdebe2.at(recNo);
        if (paramName == "sumashaber2") paramValue = sumashaber2.at(recNo);
        if (paramName == "saldoinicial2") paramValue = saldosiniciales2.at(recNo);
        if (paramName == "saldo2") paramValue = saldos2.at(recNo);

        if (paramName == "totaldebe2") paramValue = formateanumero(totaldebe2,false,decimales);
        if (paramName == "totalhaber2") paramValue = formateanumero(totalhaber2,false,decimales);
        if (paramName == "totalsaldoini2") paramValue = formateanumero(totalsaldoinicial2,false,decimales);
        if (paramName == "totalsaldo2") paramValue = formateanumero(totalsaldo2,false,decimales);

        /*
        if (ui.fechas_ej2_groupBox->isChecked()) {
            if (paramName == "FECHAS2") paramValue = ui.ini_ej2_dateEdit->date().toString("dd/MM/yyyy") + " - " +
                    ui.fin_ej2_dateEdit->date().toString("dd/MM/yyyy");
        }
        if (!ui.cilineEdit->text().isEmpty()) {
            if (paramName == "ANALITICA") paramValue = ui.cilineEdit->text();
                    //+ " " + ui.nivel1lineEdit->text()+ "-" +
                    //ui.nivel2lineEdit->text() + "-" + ui.nivel3lineEdit->text();
        }*/

    });


    //report->printExec(true);
    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+tr("bcomp.pdf");
           report->printPDF(qfichero);
    }

}


void bss::generalatex()
{
    int maxlin=38;
    if (ui.compa_groupBox->isChecked()) maxlin=25;
    if (ui.cilineEdit->text().length()>0 && ui.analiticagroupBox->isChecked())
        {
          maxlin-=4;
        }

    if (ui.compa_groupBox->isChecked())
        {
          maxlin-=3;
        }

    bool haysubcuentas=false;
    // bool solocuentasauxiliares=false;
    bool compa=ui.compa_groupBox->isChecked();
    int veces=0;
    while (veces<ui.table->rowCount())
      {
        if ((ui.table->item(veces,0)->text().length()==anchocuentas() && !cod_longitud_variable())
            || (cod_longitud_variable() && esauxiliar(ui.table->item(veces,0)->text())))
	{
	    haysubcuentas=true;
	    break;
	}
        veces++;
      }
   //if (!ui.cuentascheckBox->isChecked() && !ui.subcuentascheckBox->isChecked() &&
   //    !ui.gruposcheckBox->isChecked() && !ui.subgruposcheckBox->isChecked())
   //    solocuentasauxiliares=true;
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=fichero;
   qfichero+=".tex";
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero2(pasa);
    if (! fichero2.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero2 );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    if (compa) stream << "\\begin{landscape}" << "\n";

    int pos=0;
    int lineapagina=0;
    double sumadebe=0;
    double sumahaber=0;
    double sumasaldodebe=0;
    double sumasaldohaber=0;
    double sumadebe2=0;
    double sumahaber2=0;
    double sumasaldodebe2=0;
    double sumasaldohaber2=0;
    QString guardacta, guardacta2;
    while (pos<=ui.table->rowCount())
     {
        if (ui.conmovcheckBox->isChecked() && pos<ui.table->rowCount())
           {
              double saldoinicial=convapunto(ui.table->item(pos,2)->text()).toDouble();
              double sumadebe=convapunto(ui.table->item(pos,3)->text()).toDouble();
              double sumahaber=convapunto(ui.table->item(pos,4)->text()).toDouble();
              if (!compa)
                {
                 if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                    && (saldoinicial>-0.001 && saldoinicial<0.001))
                   {pos++; continue;}
                }
              else
                  {
                   double saldoinicial2=convapunto(ui.table->item(pos,6)->text()).toDouble();
                   double sumadebe2=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   double sumahaber2=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   if ((sumadebe>-0.001 && sumadebe<0.001)
                       && (sumahaber>-0.001 && sumahaber<0.001)
                      && (saldoinicial>-0.001 && saldoinicial<0.001) &&
                         (sumadebe2>-0.001 && sumadebe2<0.001)
                      && (sumahaber2>-0.001 && sumahaber2<0.001)
                      && (saldoinicial2>-0.001 && saldoinicial2<0.001))
                       {pos++; continue;}
                  }
           }
        if (lineapagina==0)
          {	
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
           stream << "\\end{center}" << "\n";
           stream << "\n";
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << tr("BALANCE DE SUMAS Y SALDOS") <<  "}}" << "\n";
           stream << "\\end{center}" << "\n";
           if (ui.cilineEdit->text().length()>0 && ui.analiticagroupBox->isChecked())
              {
               stream << "\\begin{center}" << "\n";
               stream << "{\\normalsize \\textbf {" << tr("CÓDIGO DE IMPUTACIÓN:") << " " <<
                         ui.cilineEdit->text() <<  "}}" << "\n";
               stream << "\\end{center}" << "\n";

               QString codigo=ui.cilineEdit->text();
               QString descripcion;
               QString qnivel=0;

               stream << "\\begin{center}" << "\n";
               stream << "{\\normalsize {" ;

               if (codigo.startsWith("???"))
                   {
                    stream << tr("CUALQUIERA");
                   }
               else
                   {
                     bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
                     if (encontrada && qnivel.toInt()==1)
                     stream << descripcion;
                   }
               if (codigo.length()>3) stream << " - ";
               if (codigo.length()>3 && codigo.mid(3,3)==QString("???")) stream << tr("CUALQUIERA");
               else
                    {
                     bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
                     int elnivel=qnivel.toInt();
                     if (encontrada && elnivel==2)
                     stream << descripcion;
                    }

               if (codigo.length()>6) stream << " - ";
               if (codigo.length()==7 && codigo[6]=='*')  stream << tr("CUALQUIERA");
                else
                   {
                     bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
                     if (encontrada && qnivel.toInt()==3)
                     stream << descripcion;
                   }
               stream << "}}" << "\n";
               stream << "\\end{center}" << "\n";
            }

           stream << "{\\textbf {" << tr("Período de") << " " << ui.desdedateEdit->date().toString("dd.MM.yyyy") <<
	                         tr(" a ") << ui.hastadateEdit->date().toString("dd.MM.yyyy") << "}}" << "\n";
           if (compa)
            {
             stream << "\\\\\n";
             stream << "{\\textbf {" << tr("Período 2 de") << " " << ui.desde_c_dateEdit->date().toString("dd.MM.yyyy") <<
                     tr(" a ") << ui.hasta_c_dateEdit->date().toString("dd.MM.yyyy") << "}}" << "\n";
            }
           // cuerpo de listado (tablas)
           stream << "\\begin{center}\n";
           if (!compa)
             stream << "\\begin{tabular}{|l|p{7.2cm}|r|r|r|r|}\n";
            else stream << "\\begin{tabular}{|l|p{6cm}|r|r|r|r|r|r|r|r|}\n";
           stream << "\\hline\n";
           stream << "\\rowcolor{gray!30}\n";

           if (!compa)
            {
             stream << "{\\textbf {\\scriptsize " << tr("CUENTA") << "}} & {\\textbf {\\scriptsize " << tr("TITULO") <<
              "}} & {\\textbf{\\scriptsize " << tr("Sumas Debe") << "}} & {\\textbf {\\scriptsize " << tr("Sumas Haber") <<
              "}} & {\\textbf {\\scriptsize " << tr("Saldo Deudor") << "}} & {\\textbf{\\scriptsize " << tr("Saldo Acreedor");
             }
           if (compa)
             {
               stream << "{\\textbf {\\scriptsize " << tr("CUENTA") << "}} & {\\textbf {\\scriptsize " << tr("TITULO") <<
                "}} & {\\textbf {\\scriptsize " << tr("Sum. Debe") << "}} & {\\textbf {\\scriptsize " << tr("Sum. Haber") <<
                "}} & {\\textbf {\\scriptsize " << tr("S. Deudor") << "}} & {\\textbf {\\scriptsize " << tr("S. Acreedor");
               stream <<
               "}} & {\\textbf {\\scriptsize " << tr("Sum. Debe2") << "}} & {\\textbf {\\scriptsize " << tr("Sum. Haber2") <<
               "}} & {\\textbf {\\scriptsize " << tr("S. Deudor2") << "}} & {\\textbf {\\scriptsize " << tr("S. Acreedor2");
             }
           stream << "}} \\\\\n";
           stream << "\\hline\n";
          }
        if (pos==ui.table->rowCount()) break;

       QString descrip;
       int tope=50;
       if (compa) tope=40;
        descrip=mcortaa(filtracad(ui.table->item(pos,1)->text()),tope);
       if (ui.table->item(pos,1)->text().length()>descrip.length()) descrip=descrip+"\\#";
       if (ui.table->item(pos,0)->text().length()!=anchocuentas()) 
         {
           descrip=mcortaa(filtracad(ui.table->item(pos,1)->text()),tope);
           if (ui.table->item(pos,1)->text().length()>descrip.length()) descrip=descrip+"\\#";
         }
       // descrip.remove('&');
       stream << "{\\scriptsize " << ui.table->item(pos,0)->text() 
              << "} & {\\scriptsize " << descrip << "} & {\\scriptsize ";
       double numero1=convapunto(ui.table->item(pos,3)->text()).toDouble();
       double numero2=convapunto(ui.table->item(pos,4)->text()).toDouble();
       stream << formateanumerosep(numero1,comadecimal,decimales) << "} & {\\scriptsize "
              << formateanumerosep(numero2,comadecimal,decimales) << "} & {\\scriptsize ";

       if (ui.table->item(pos,0)->text().length()==anchocuentas() || esauxiliar(ui.table->item(pos,0)->text()))
         {
          sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
          sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
          double saldocalc = convapunto(ui.table->item(pos,5)->text()).toDouble();
          sumasaldodebe+=(saldocalc > 0 ? saldocalc: 0);
          sumasaldohaber+=(saldocalc < 0 ? -1*saldocalc : 0);
         }
       if (!haysubcuentas)
         {
              if (pos==0)
                 {
                  guardacta=ui.table->item(pos,0)->text();
                  sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
                  sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
                  double saldocalc = convapunto(ui.table->item(pos,5)->text()).toDouble();
                  sumasaldodebe+=(saldocalc > 0 ? saldocalc: 0);
                  sumasaldohaber+=(saldocalc < 0 ? -1*saldocalc : 0);
                 }
              if (pos>0)
                 if(!ui.table->item(pos,0)->text().startsWith(guardacta ))
                   {
                    sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
                    sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
                    double saldocalc = convapunto(ui.table->item(pos,5)->text()).toDouble();
                    sumasaldodebe+=(saldocalc > 0 ? saldocalc: 0);
                    sumasaldohaber+=(saldocalc < 0 ? -1*saldocalc : 0);
                    guardacta=ui.table->item(pos,0)->text();
                   }

         }
      if (convapunto(ui.table->item(pos,5)->text()).toDouble()>0.001) 
         { 
            double num=convapunto(ui.table->item(pos,5)->text()).toDouble();
            stream << formateanumerosep(num,comadecimal,decimales);
         }
          else { stream << formateanumerosep(0,comadecimal,decimales);}
       stream << "} & {\\scriptsize ";
       if (convapunto(ui.table->item(pos,5)->text()).toDouble()<0.001)
          {
             double num=convapunto(ui.table->item(pos,5)->text()).toDouble();
             stream << formateanumerosep(num,comadecimal,decimales).remove('-');
          }
           else { stream << formateanumerosep(0,comadecimal,decimales);}

      if (compa)
        {
          stream << "} & {\\scriptsize ";
         //-------------------------------------------------------------------------------
          double numero11=convapunto(ui.table->item(pos,7)->text()).toDouble();
          double numero22=convapunto(ui.table->item(pos,8)->text()).toDouble();
          stream << formateanumerosep(numero11,comadecimal,decimales) << "} & {\\scriptsize "
                 << formateanumerosep(numero22,comadecimal,decimales) << "} & {\\scriptsize ";

          if (ui.table->item(pos,0)->text().length()==anchocuentas() ||
              esauxiliar(ui.table->item(pos,0)->text()))
            {
             sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
             sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
             double saldocalc = convapunto(ui.table->item(pos,9)->text()).toDouble();
             sumasaldodebe2+=(saldocalc > 0 ? saldocalc: 0);
             sumasaldohaber2+=(saldocalc < 0 ? -1*saldocalc : 0);
            }
          if (!haysubcuentas)
            {              
              if (pos==0)
                 {
                  guardacta2=ui.table->item(pos,0)->text();
                  sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                  sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                  double saldocalc = convapunto(ui.table->item(pos,9)->text()).toDouble();
                  sumasaldodebe2+=(saldocalc > 0 ? saldocalc: 0);
                  sumasaldohaber2+=(saldocalc < 0 ? -1*saldocalc : 0);
                 }
              if (pos>0)
                 if(!ui.table->item(pos,0)->text().startsWith(guardacta2 ))
                   {
                    sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                    sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                    double saldocalc = convapunto(ui.table->item(pos,9)->text()).toDouble();
                    sumasaldodebe2+=(saldocalc > 0 ? saldocalc: 0);
                    sumasaldohaber2+=(saldocalc < 0 ? -1*saldocalc : 0);
                    guardacta2=ui.table->item(pos,0)->text();
                   }

            }
         if (convapunto(ui.table->item(pos,9)->text()).toDouble()>0.001)
            {
               double num=convapunto(ui.table->item(pos,9)->text()).toDouble();
               stream << formateanumerosep(num,comadecimal,decimales);
            }
             else { stream << formateanumerosep(0,comadecimal,decimales);}
          stream << "} & {\\scriptsize ";
          if (convapunto(ui.table->item(pos,9)->text()).toDouble()<0.001)
             {
                double num=convapunto(ui.table->item(pos,9)->text()).toDouble();
                stream << formateanumerosep(num,comadecimal,decimales).remove('-');
             }
              else { stream << formateanumerosep(0,comadecimal,decimales);}
         //-------------------------------------------------------------------------------
        }



      stream << "} \\\\ \n " << "\\hline\n";

       if (lineapagina>=maxlin && pos<ui.table->rowCount()-1)
          {
            /*   if (ui.conmovcheckBox->isChecked() && pos<ui.table->rowCount()-1)
                {
                 double sumadebe2=convapunto(ui.table->item(pos+1,2)->text()).toDouble();
                 double sumahaber2=convapunto(ui.table->item(pos+1,3)->text()).toDouble();
                 if ((sumadebe2>-0.001 && sumadebe2<0.001) && (sumahaber2>-0.001 && sumahaber2<0.001))
	            {pos++;  continue ;}
                } */
	   lineapagina=0;
           // if (haysubcuentas || solocuentasauxiliares)
             {
              stream << "\\multicolumn{2}{|r|}{" << tr("Sumas y siguen ...");
              stream << "} & {\\scriptsize " << formateanumerosep(sumadebe,comadecimal,decimales);
	      stream << "} & {\\scriptsize " << formateanumerosep(sumahaber,comadecimal,decimales);
	      stream << "} & {\\scriptsize " << formateanumerosep(sumasaldodebe,comadecimal,decimales);
	      stream << "} & {\\scriptsize " << formateanumerosep(sumasaldohaber,comadecimal,decimales);
              if (compa)
                {
                  stream << "} & {\\scriptsize " << formateanumerosep(sumadebe2,comadecimal,decimales);
                  stream << "} & {\\scriptsize " << formateanumerosep(sumahaber2,comadecimal,decimales);
                  stream << "} & {\\scriptsize " << formateanumerosep(sumasaldodebe2,comadecimal,decimales);
                  stream << "} & {\\scriptsize " << formateanumerosep(sumasaldohaber2,comadecimal,decimales);
                }
	      stream << "} \\\\";
              stream << "\\hline\n";
             }
     	   stream << "\\end{tabular}\n";
           stream << "\\end{center}\n";
           stream << "\\newpage\n";
	   pos++;
           continue;
          }
       pos++; lineapagina++;
     }
    // if (haysubcuentas || solocuentasauxiliares)
     {
      stream << "\\multicolumn{2}{|r|}{" << tr("Sumas ...");
      stream << "} & {\\scriptsize " << formateanumerosep(sumadebe,comadecimal,decimales);
      stream << "} & {\\scriptsize " << formateanumerosep(sumahaber,comadecimal,decimales);
      stream << "} & {\\scriptsize " << formateanumerosep(sumasaldodebe,comadecimal,decimales);
      stream << "} & {\\scriptsize " << formateanumerosep(sumasaldohaber,comadecimal,decimales);
      if (compa)
        {
          stream << "} & {\\scriptsize " << formateanumerosep(sumadebe2,comadecimal,decimales);
          stream << "} & {\\scriptsize " << formateanumerosep(sumahaber2,comadecimal,decimales);
          stream << "} & {\\scriptsize " << formateanumerosep(sumasaldodebe2,comadecimal,decimales);
          stream << "} & {\\scriptsize " << formateanumerosep(sumasaldohaber2,comadecimal,decimales);
        }
      stream << "} \\\\";
      stream << "\\multicolumn{2}{|r|}{" << tr("Diferencias ...");
      stream << "} & {\\scriptsize " <<
              (sumadebe-sumahaber>0.0001 ? formateanumerosep(sumadebe-sumahaber,comadecimal,decimales)
                  : "");
      stream << "} & {\\scriptsize " <<
              (sumahaber-sumadebe>0.0001 ? formateanumerosep(sumahaber-sumadebe,comadecimal,decimales):
              "");

      stream << "} & {\\scriptsize " <<
              (sumasaldodebe-sumasaldohaber>0.0001 ? formateanumerosep(sumasaldodebe-sumasaldohaber,comadecimal,decimales):
              "") ;
      stream << "} & {\\scriptsize " <<
              (sumasaldohaber-sumasaldodebe>0.0001 ? formateanumerosep(sumasaldohaber-sumasaldodebe,comadecimal,decimales)
                  : "");
      if (compa)
        {
          stream << "} & {\\scriptsize " <<
                  (sumadebe2-sumahaber2>0.0001 ? formateanumerosep(sumadebe2-sumahaber2,comadecimal,decimales)
                      : "");
          stream << "} & {\\scriptsize " <<
                  (sumahaber2-sumadebe2>0.0001 ? formateanumerosep(sumahaber2-sumadebe2,comadecimal,decimales)
                      : "");
          stream << "} & {\\scriptsize " <<
                  (sumasaldodebe2-sumasaldohaber2 >0.0001 ? formateanumerosep(sumasaldodebe2-sumasaldohaber2,comadecimal,decimales)
                      : "");
          stream << "} & {\\scriptsize " <<
                  (sumasaldohaber2-sumasaldodebe2 > 0.0001 ? formateanumerosep(sumasaldohaber2-sumasaldodebe2,comadecimal,decimales)
                      : "");
        }
      stream << "} \\\\";
     stream << "\\hline\n";
    }
   stream << "\\end{tabular}\n";
   stream << "\\end{center}\n";
    
   if (compa) stream << "\\end{landscape}\n";

    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero2.close();


}

bool bss::noinfovisible()
{
  for (int veces=0; veces<ui.table->rowCount(); veces++)
     {
      if (!ui.table->isRowHidden(veces)) return false;
     }
  return true;
}

void bss::imprimir()
{
   if (ui.table->rowCount()==0 || noinfovisible())
    {
       QMessageBox::warning( this, tr("IMPRIMIR BALANCE SUMAS Y SALDOS"),tr("ERROR: Consulta vacía"));
       return;
    }

   if (!ui.inicialescheckBox->isChecked())
     informe_bss();
    else
       informe_bcomp();

}


void bss::gentex()
{
  if (ui.table->rowCount()==0 || noinfovisible())
    {
       QMessageBox::warning( this, tr("IMPRIMIR BALANCE SUMAS Y SALDOS"),tr("ERROR: Consulta vacía"));
       return;
    }

   pidenombre *p=new pidenombre();
   int resultado=0;
   p->asignanombreventana(tr("Balance sumas y saldos"));
   p->asignaetiqueta(tr("NOMBRE FICHERO (sin extensión):"));
   resultado=p->exec();
   if (resultado==QDialog::Accepted)
      {
       if (p->contenido().length()>0) fichero=p->contenido();
       fichero.remove('*');
       fichero.remove("..");
       fichero.remove('/');
       fichero.replace(' ','_');
       delete p;
       if (!ui.inicialescheckBox->isChecked())
           generalatex();
        else
           generalatex2();

       int valor=consultalatex(fichero);
       if (valor==1)
           QMessageBox::warning( this, tr("VISTA PREVIA BALANCE SUMAS Y SALDOS"),
                                 tr("PROBLEMAS al llamar a Latex"));
       if (valor==2)
           QMessageBox::warning( this, tr("VISTA PREVIA MAYOR BALANCE SUMAS Y SALDOS"),
                                 tr("PROBLEMAS al llamar a ")+visordvi());
      }
}


void bss::fijadescripciones()
{
  activaactualiza();
  QString codigo=ui.cilineEdit->text();
  QString cadena,descripcion;
  QString qnivel=0;
  ui.nivel1lineEdit->setText("");
  ui.nivel2lineEdit->setText("");
  ui.nivel3lineEdit->setText("");

 if (codigo.length()==0) return;

  if (codigo.startsWith("???"))
      {
       ui.nivel1lineEdit->setText(tr("CUALQUIERA"));
      }
      else
           {
            bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
            if (encontrada && qnivel.toInt()==1)
                ui.nivel1lineEdit->setText(descripcion);
           }

 if (codigo.length()<=3) return;

 if (codigo.mid(3,3)==QString("???")) ui.nivel2lineEdit->setText(tr("CUALQUIERA"));
     else
          {
           bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
           int elnivel=qnivel.toInt();
           if (encontrada && elnivel==2)
              ui.nivel2lineEdit->setText(descripcion);
          }

 if (codigo.length()<=6) return;

 if (codigo.length()==7 && codigo[6]=='*')  ui.nivel3lineEdit->setText(tr("CUALQUIERA"));
    else
         {
          bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
          if (encontrada && qnivel.toInt()==3)
             ui.nivel3lineEdit->setText(descripcion);
         }

}

void bss::buscapulsado()
{
 introci *c = new introci();
 c->hazparafiltro();
 c->pasaci(ui.cilineEdit->text());
 c->exec();
 ui.cilineEdit->setText(c->cadenaci());
 delete c;
}

void bss::activaactualiza()
{
 ui.actualizapushButton->setEnabled(true);
 ui.table->clearContents();

}


void bss::var_saldos_iniciales()
{
 activaactualiza();
 if (ui.inicialescheckBox->isChecked())
    ui.table->showColumn(2); // columna de saldo inicial
  else
    ui.table->hideColumn(2); // columna de saldo inicial

  if (ui.inicialescheckBox->isChecked() && ui.compa_groupBox->isChecked())
      ui.table->showColumn(6);
    else
        ui.table->hideColumn(6);


  if (ui.inicialescheckBox->isChecked()) {
      ui.si1_label->show();
      ui.si1_lineEdit->show();
      if (ui.compa_groupBox->isChecked()) {
         ui.si2_label->show();
         ui.si2_lineEdit->show();
      }
  }
  else {
      ui.si1_label->hide();
      ui.si1_lineEdit->hide();
      if (ui.compa_groupBox->isChecked()) {
         ui.si2_label->hide();
         ui.si2_lineEdit->hide();
      }
  }

}

void bss::comparativo_check()
{
  activaactualiza();
  if (ui.compa_groupBox->isChecked())
    {
      if (ui.inicialescheckBox->isChecked()) ui.table->showColumn(6);
      ui.table->showColumn(7);
      ui.table->showColumn(8);
      ui.table->showColumn(9);
      ui.table->showColumn(10);
      if (ui.inicialescheckBox->isChecked()) {
          ui.si2_label->show();
          ui.si2_lineEdit->show();
      }

      ui.sd2_label->show();
      ui.sd2_lineEdit->show();
      ui.sh2_label->show();
      ui.sh2_lineEdit->show();
      ui.ss2_label->show();
      ui.ss2_lineEdit->show();
    }
    else
        {
         ui.table->hideColumn(6);
         ui.table->hideColumn(7);
         ui.table->hideColumn(8);
         ui.table->hideColumn(9);
         ui.table->hideColumn(10);
         ui.si2_label->hide();
         ui.si2_lineEdit->hide();

         ui.sd2_label->hide();
         ui.sd2_lineEdit->hide();
         ui.sh2_label->hide();
         ui.sh2_lineEdit->hide();
         ui.ss2_label->hide();
         ui.ss2_lineEdit->hide();
        }
}

void bss::actualiza_totales() {
    bool haysubcuentas=false;
    bool compa=ui.compa_groupBox->isChecked();
    int qanchocuentas=anchocuentas();
    bool qcod_longitud_variable=cod_longitud_variable();
    int veces=0;
    while (veces<ui.table->rowCount())
      {
        bool cod_variable_auxiliar=false;
        if (qcod_longitud_variable) {
           int lugar=codigo_cuenta.indexOf(ui.table->item(veces,0)->text());
           if (lugar>-1) cod_variable_auxiliar=es_auxiliar.at(lugar);
        }

        if ((ui.table->item(veces,0)->text().length()==qanchocuentas && !qcod_longitud_variable)
            || (cod_variable_auxiliar))
        {
          haysubcuentas=true;
          break;
        }
        veces++;
      }
    double sumadebe=0;
    double sumahaber=0;
    double sumasaldodebe=0;
    double sumasaldohaber=0;
    double sumadebe2=0;
    double sumahaber2=0;
    double sumasaldodebe2=0;
    double sumasaldohaber2=0;
    int pos=0;
    QString guardacta, guardacta2;
    while (pos<ui.table->rowCount())
     {
        bool cod_variable_auxiliar=false;
        if (qcod_longitud_variable) {
           int lugar=codigo_cuenta.indexOf(ui.table->item(veces,0)->text());
           if (lugar>-1) cod_variable_auxiliar=es_auxiliar.at(lugar);
        }

       if (ui.table->item(pos,0)->text().length()==qanchocuentas || cod_variable_auxiliar)
         {
          sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
          sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
          double saldocalc = convapunto(ui.table->item(pos,5)->text()).toDouble();
          sumasaldodebe+=(saldocalc > 0 ? saldocalc: 0);
          sumasaldohaber+=(saldocalc < 0 ? -1*saldocalc : 0);
         }
       if (!haysubcuentas)
         {
           if (pos==0)
              {
               guardacta=ui.table->item(pos,0)->text();
               sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
               sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
               double saldocalc = convapunto(ui.table->item(pos,5)->text()).toDouble();
               sumasaldodebe+=(saldocalc > 0 ? saldocalc: 0);
               sumasaldohaber+=(saldocalc < 0 ? -1*saldocalc : 0);
              }
           if (pos>0)
              if(!ui.table->item(pos,0)->text().startsWith(guardacta ))
                {
                 sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
                 sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
                 double saldocalc = convapunto(ui.table->item(pos,5)->text()).toDouble();
                 sumasaldodebe+=(saldocalc > 0 ? saldocalc: 0);
                 sumasaldohaber+=(saldocalc < 0 ? -1*saldocalc : 0);
                 guardacta=ui.table->item(pos,0)->text();
                }
         }

      if (compa)
        {
         //-------------------------------------------------------------------------------

          if (ui.table->item(pos,0)->text().length()==qanchocuentas ||
              cod_variable_auxiliar)
            {
             sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
             sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
             double saldocalc = convapunto(ui.table->item(pos,9)->text()).toDouble();
             sumasaldodebe2+=(saldocalc > 0 ? saldocalc: 0);
             sumasaldohaber2+=(saldocalc < 0 ? -1*saldocalc : 0);
            }
          if (!haysubcuentas)
            {
              if (pos==0)
                 {
                  guardacta2=ui.table->item(pos,0)->text();
                  sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                  sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                  double saldocalc = convapunto(ui.table->item(pos,9)->text()).toDouble();
                  sumasaldodebe2+=(saldocalc > 0 ? saldocalc: 0);
                  sumasaldohaber2+=(saldocalc < 0 ? -1*saldocalc : 0);
                 }
              if (pos>0)
                 if(!ui.table->item(pos,0)->text().startsWith(guardacta2 ))
                   {
                    sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                    sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                    double saldocalc = convapunto(ui.table->item(pos,9)->text()).toDouble();
                    sumasaldodebe2+=(saldocalc > 0 ? saldocalc: 0);
                    sumasaldohaber2+=(saldocalc < 0 ? -1*saldocalc : 0);
                    guardacta2=ui.table->item(pos,0)->text();
                   }
            }
         } // if compa

      pos++;
     }
    ui.sd1_lineEdit->setText(formatea_redondeado_sep(sumadebe,comadecimal, decimales));
    ui.sh1_lineEdit->setText(formatea_redondeado_sep(sumahaber,comadecimal, decimales));
    ui.ss1_lineEdit->setText(formatea_redondeado_sep(sumasaldodebe-sumasaldohaber,comadecimal, decimales));

    ui.sd2_lineEdit->setText(formatea_redondeado_sep(sumadebe2,comadecimal, decimales));
    ui.sh2_lineEdit->setText(formatea_redondeado_sep(sumahaber2,comadecimal, decimales));
    ui.ss2_lineEdit->setText(formatea_redondeado_sep(sumasaldodebe2-sumasaldohaber2,comadecimal, decimales));

}

void bss::actualiza_totales_si() {
    bool compa=ui.compa_groupBox->isChecked();
    bool haysubcuentas=false;
    int veces=0;
    int qanchocuentas=anchocuentas();
    bool qcod_longitud_variable=cod_longitud_variable();
    while (veces<ui.table->rowCount())
      {
        bool cod_variable_auxiliar=false;
        if (qcod_longitud_variable) {
           int lugar=codigo_cuenta.indexOf(ui.table->item(veces,0)->text());
           if (lugar>-1) cod_variable_auxiliar=es_auxiliar.at(lugar);
        }
        if ((ui.table->item(veces,0)->text().length()==qanchocuentas && !qcod_longitud_variable)
            || (cod_variable_auxiliar))
         {
          haysubcuentas=true;
          break;
         }
        veces++;
      }
    double sumadebe=0;
    double sumahaber=0;
    double sumadebe2=0;
    double sumahaber2=0;
    double sumsaldoini=0;
    double sumsaldofinal=0;
    double sumsaldoini2=0;
    double sumsaldofinal2=0;

    int pos=0;
    QString guardacta, guardacta2;
    while (pos<ui.table->rowCount())
     {
        bool cod_variable_auxiliar=false;
        if (qcod_longitud_variable) {
           int lugar=codigo_cuenta.indexOf(ui.table->item(pos,0)->text());
           if (lugar>-1) cod_variable_auxiliar=es_auxiliar.at(lugar);
        }
       double saldoini=convapunto(ui.table->item(pos,2)->text()).toDouble();
       double num=convapunto(ui.table->item(pos,5)->text()).toDouble();
       if (ui.table->item(pos,0)->text().length()==qanchocuentas || cod_variable_auxiliar)
         {
          sumsaldoini+=saldoini;
          sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
          sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
          sumsaldofinal+=num;
         }

       if (!haysubcuentas)
         {
           if (pos==0)
              {
               guardacta=ui.table->item(pos,0)->text();
               sumsaldoini+=saldoini;
               sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
               sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
               sumsaldofinal+=num;
              }
           if (pos>0)
              if(!ui.table->item(pos,0)->text().startsWith(guardacta ))
                {
                  sumsaldoini+=saldoini;
                  sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
                  sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
                  sumsaldofinal+=num;
                  guardacta=ui.table->item(pos,0)->text();
                }

          }

       if (compa)
          {
           double saldoini2=convapunto(ui.table->item(pos,6)->text()).toDouble();
           double num2=convapunto(ui.table->item(pos,9)->text()).toDouble();
           if (ui.table->item(pos,0)->text().length()==qanchocuentas ||
               cod_variable_auxiliar)
             {
              sumsaldoini2+=saldoini2;
              sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
              sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
              sumsaldofinal2+=num2;
             }
           if (!haysubcuentas)
             {
               if (pos==0)
                  {
                   guardacta2=ui.table->item(pos,0)->text();
                   sumsaldoini2+=saldoini2;
                   sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   sumsaldofinal2+=num2;
                  }
               if (pos>0)
                  if(!ui.table->item(pos,0)->text().startsWith(guardacta2 ))
                    {
                      sumsaldoini2+=saldoini2;
                      sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                      sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                      sumsaldofinal2+=num2;
                      guardacta2=ui.table->item(pos,0)->text();
                    }

             }
          }
       pos++;
     }
    ui.si1_lineEdit->setText(formatea_redondeado_sep(sumsaldoini,comadecimal, decimales));
    ui.sd1_lineEdit->setText(formatea_redondeado_sep(sumadebe,comadecimal, decimales));
    ui.sh1_lineEdit->setText(formatea_redondeado_sep(sumahaber,comadecimal, decimales));
    ui.ss1_lineEdit->setText(formatea_redondeado_sep(sumsaldofinal,comadecimal, decimales));

    ui.si2_lineEdit->setText(formatea_redondeado_sep(sumsaldoini2,comadecimal, decimales));
    ui.sd2_lineEdit->setText(formatea_redondeado_sep(sumadebe2,comadecimal, decimales));
    ui.sh2_lineEdit->setText(formatea_redondeado_sep(sumahaber2,comadecimal, decimales));
    ui.ss2_lineEdit->setText(formatea_redondeado_sep(sumsaldofinal2,comadecimal, decimales));

}

// latex para el balance de comprobación con saldos iniciales de cuentas
void bss::generalatex2()
{
    int maxlin=38;
    if (ui.compa_groupBox->isChecked()) maxlin=25;
    if (ui.cilineEdit->text().length()>0 && ui.analiticagroupBox->isChecked())
        {
          maxlin-=4;
        }

    if (ui.compa_groupBox->isChecked())
        {
          maxlin-=3;
        }

    bool compa=ui.compa_groupBox->isChecked();

    bool haysubcuentas=false;
    int veces=0;
    while (veces<ui.table->rowCount())
      {
        if ((ui.table->item(veces,0)->text().length()==anchocuentas() && !cod_longitud_variable())
            || (cod_longitud_variable() && esauxiliar(ui.table->item(veces,0)->text())))
         {
          haysubcuentas=true;
          break;
         }
        veces++;
      }
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=fichero;
   qfichero+=".tex";
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero2(pasa);
    if (! fichero2.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero2 );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();
    if (compa) stream << "\\begin{landscape}" << "\n";
    int pos=0;
    int lineapagina=0;
    double sumadebe=0;
    double sumahaber=0;
    double sumadebe2=0;
    double sumahaber2=0;
    double sumsaldoini=0;
    double sumsaldofinal=0;
    double sumsaldoini2=0;
    double sumsaldofinal2=0;
    QString guardacta, guardacta2;

    while (pos<ui.table->rowCount())
     {
        if (ui.conmovcheckBox->isChecked())
           {
              double saldoinicial=convapunto(ui.table->item(pos,2)->text()).toDouble();
              double sumadebe=convapunto(ui.table->item(pos,3)->text()).toDouble();
              double sumahaber=convapunto(ui.table->item(pos,4)->text()).toDouble();
              if (!compa)
                {
                 if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                    && (saldoinicial>-0.001 && saldoinicial<0.001))
                   {pos++; continue;}
                }
              else
                  {
                   double saldoinicial2=convapunto(ui.table->item(pos,6)->text()).toDouble();
                   double sumadebe2=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   double sumahaber2=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   if ((sumadebe>-0.001 && sumadebe<0.001)
                       && (sumahaber>-0.001 && sumahaber<0.001)
                      && (saldoinicial>-0.001 && saldoinicial<0.001) &&
                         (sumadebe2>-0.001 && sumadebe2<0.001)
                      && (sumahaber2>-0.001 && sumahaber2<0.001)
                      && (saldoinicial2>-0.001 && saldoinicial2<0.001))
                       {pos++; continue;}
                  }
           }
        if (lineapagina==0)
          {	
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
           stream << "\\end{center}" << "\n";
           stream << "\n";
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << tr("BALANCE DE COMPROBACIÓN") <<  "}}" << "\n";
           stream << "\\end{center}" << "\n";
           if (ui.cilineEdit->text().length()>0 && ui.analiticagroupBox->isChecked())
              {
               stream << "\\begin{center}" << "\n";
               stream << "{\\normalsize \\textbf {" << tr("CÓDIGO DE IMPUTACIÓN:") << " " <<
                         ui.cilineEdit->text() <<  "}}" << "\n";
               stream << "\\end{center}" << "\n";

               QString codigo=ui.cilineEdit->text();
               QString descripcion;
               QString qnivel=0;

               stream << "\\begin{center}" << "\n";
               stream << "{\\normalsize {" ;

               if (codigo.startsWith("???"))
                   {
                    stream << tr("CUALQUIERA");
                   }
               else
                   {
                     bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
                     if (encontrada && qnivel.toInt()==1)
                     stream << descripcion;
                   }
               if (codigo.length()>3) stream << " - ";
               if (codigo.length()>3 && codigo.mid(3,3)==QString("???")) stream << tr("CUALQUIERA");
               else
                    {
                     bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
                     int elnivel=qnivel.toInt();
                     if (encontrada && elnivel==2)
                     stream << descripcion;
                    }

               if (codigo.length()>6) stream << " - ";
               if (codigo.length()==7 && codigo[6]=='*')  stream << tr("CUALQUIERA");
                else
                   {
                     bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
                     if (encontrada && qnivel.toInt()==3)
                     stream << descripcion;
                   }
               stream << "}}" << "\n";
               stream << "\\end{center}" << "\n";
            }

           stream << "{\\textbf {" << tr("Período de") << " " << ui.desdedateEdit->date().toString("dd.MM.yyyy") <<
	                         tr(" a ") << ui.hastadateEdit->date().toString("dd.MM.yyyy") << "}}" << "\n";
           if (compa)
            {
             stream << "\\\\\n";
             stream << "{\\textbf {" << tr("Período 2 de") << " " << ui.desde_c_dateEdit->date().toString("dd.MM.yyyy") <<
                                 tr(" a ") << ui.hasta_c_dateEdit->date().toString("dd.MM.yyyy") << "}}" << "\n";
            }
           // cuerpo de listado (tablas)
           stream << "\\begin{center}\n";
           if (!compa)
              stream << "\\begin{tabular}{|l|p{7.2cm}|r|r|r|r|}\n";
            else
                stream << "\\begin{tabular}{|l|p{6cm}|r|r|r|r|r|r|r|r|}\n";
           stream << "\\hline\n";
           if (!compa)
            {
             stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("TITULO") <<
              "} & {\\scriptsize " << tr("Saldo Inicial") << "} & {\\scriptsize " << tr("Sumas Debe") <<
              "} & {\\scriptsize " << tr("Sumas Haber") << "} & {\\scriptsize " << tr("Saldo Final");
            }
             else
              {
                 stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("TITULO") <<
                  "} & {\\scriptsize " << tr("S. Inicial") << "} & {\\scriptsize " << tr("Sum. Debe") <<
                  "} & {\\scriptsize " << tr("Sum. Haber") << "} & {\\scriptsize " << tr("S. Final");
                 stream << "} & {\\scriptsize " << tr("S. Inicial 2") << "} & {\\scriptsize " << tr("Sum. Debe 2") <<
                      "} & {\\scriptsize " << tr("Sum. Haber 2") << "} & {\\scriptsize " << tr("S. Final 2");
              }
           stream << "} \\\\\n";
           stream << "\\hline\n";
          }
        int tope=50;
        if (compa) tope=40;
        QString descrip;
        descrip=mcortaa(filtracad(ui.table->item(pos,1)->text()),tope);
       if (ui.table->item(pos,1)->text().length()>descrip.length()) descrip=descrip+"\\#";
       if (ui.table->item(pos,0)->text().length()!=anchocuentas()) 
         {
           descrip=mcortaa(filtracad(ui.table->item(pos,1)->text()),tope);
           if (ui.table->item(pos,1)->text().length()>descrip.length()) descrip=descrip+"\\#";
         }
       // descrip.remove('&');
       stream << "{\\scriptsize " << ui.table->item(pos,0)->text() 
              << "} & {\\scriptsize " << descrip << "} & {\\scriptsize ";
       double saldoini=convapunto(ui.table->item(pos,2)->text()).toDouble();
       double numero1=convapunto(ui.table->item(pos,3)->text()).toDouble();
       double numero2=convapunto(ui.table->item(pos,4)->text()).toDouble();
       stream << formateanumerosep(saldoini,comadecimal,decimales) << "} & {\\scriptsize ";
       stream << formateanumerosep(numero1,comadecimal,decimales) << "} & {\\scriptsize "
              << formateanumerosep(numero2,comadecimal,decimales) << "} & {\\scriptsize ";
       double num=convapunto(ui.table->item(pos,5)->text()).toDouble();
       if (ui.table->item(pos,0)->text().length()==anchocuentas() || esauxiliar(ui.table->item(pos,0)->text()))
         {
          sumsaldoini+=saldoini;
          sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
          sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
          sumsaldofinal+=num;
         }

       if (!haysubcuentas)
         {
           if (pos==0)
              {
               guardacta=ui.table->item(pos,0)->text();
               sumsaldoini+=saldoini;
               sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
               sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
               sumsaldofinal+=num;
              }
           if (pos>0)
              if(!ui.table->item(pos,0)->text().startsWith(guardacta ))
                {
                 guardacta=ui.table->item(pos,0)->text();
                 sumsaldoini+=saldoini;
                 sumadebe+=convapunto(ui.table->item(pos,3)->text()).toDouble();
                 sumahaber+=convapunto(ui.table->item(pos,4)->text()).toDouble();
                 sumsaldofinal+=num;
                }
         }

       stream << formateanumerosep(num,comadecimal,decimales);
       if (compa)
          {
           stream << "} & {\\scriptsize";
           double saldoini2=convapunto(ui.table->item(pos,6)->text()).toDouble();
           double numero11=convapunto(ui.table->item(pos,7)->text()).toDouble();
           double numero22=convapunto(ui.table->item(pos,8)->text()).toDouble();
           stream << formateanumerosep(saldoini2,comadecimal,decimales) << "} & {\\scriptsize ";
           stream << formateanumerosep(numero11,comadecimal,decimales) << "} & {\\scriptsize "
                  << formateanumerosep(numero22,comadecimal,decimales) << "} & {\\scriptsize ";
           double num2=convapunto(ui.table->item(pos,9)->text()).toDouble();
           if (ui.table->item(pos,0)->text().length()==anchocuentas() ||
               esauxiliar(ui.table->item(pos,0)->text()))
             {
              sumsaldoini2+=saldoini2;
              sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
              sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
              sumsaldofinal2+=num2;
             }
           if (!haysubcuentas)
             {
               if (pos==0)
                  {
                   sumsaldoini2+=saldoini2;
                   sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   sumsaldofinal2+=num2;
                   guardacta2=ui.table->item(pos,0)->text();
                  }
               if (pos>0)
                  if(!ui.table->item(pos,0)->text().startsWith(guardacta2 ))
                    {
                      sumsaldoini2+=saldoini2;
                      sumadebe2+=convapunto(ui.table->item(pos,7)->text()).toDouble();
                      sumahaber2+=convapunto(ui.table->item(pos,8)->text()).toDouble();
                      sumsaldofinal2+=num2;
                      guardacta2=ui.table->item(pos,0)->text();
                    }
             }
           stream << formateanumerosep(num2,comadecimal,decimales);
          }
       stream << "} \\\\ \n " << "\\hline\n";
       if (lineapagina>=maxlin && pos<ui.table->rowCount()-1)
          {
            /*   if (ui.conmovcheckBox->isChecked() && pos<ui.table->rowCount()-1)
                {
                 double sumadebe2=convapunto(ui.table->item(pos+1,2)->text()).toDouble();
                 double sumahaber2=convapunto(ui.table->item(pos+1,3)->text()).toDouble();
                 if ((sumadebe2>-0.001 && sumadebe2<0.001) && (sumahaber2>-0.001 && sumahaber2<0.001))
	            {pos++;  continue ;}
                } */
	   lineapagina=0;
           // if (haysubcuentas)
             {
              stream  << "\\multicolumn{2}{|r|}{" << tr("Sumas y siguen ...") << "} & ";
	      stream << "& {\\scriptsize " << formateanumerosep(sumadebe,comadecimal,decimales);
	      stream << "} & {\\scriptsize " << formateanumerosep(sumahaber,comadecimal,decimales);
	      stream << "} & ";
              if (compa)
                {
                 stream << "& & {\\scriptsize " << formateanumerosep(sumadebe2,comadecimal,decimales);
                 stream << "} & {\\scriptsize " << formateanumerosep(sumahaber2,comadecimal,decimales);
                 stream << "} & ";
                }
              stream << " \\\\";
              stream << "\\hline\n";
             }
     	   stream << "\\end{tabular}\n";
           stream << "\\end{center}\n";
           stream << "\\newpage";
	   pos++;
           continue;
          }
       pos++; lineapagina++;
     }
    // QString cadnum; cadnum.setNum(lineapagina);
    // QMessageBox::warning( this, tr("BSS"),cadnum);

    if (lineapagina==0)
      {
       stream << "\\begin{center}" << "\n";
       stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
       stream << "\\end{center}" << "\n";
       stream << "\n";
       stream << "\\begin{center}" << "\n";
       stream << "{\\Large \\textbf {" << tr("BALANCE DE COMPROBACIÓN") <<  "}}" << "\n";
       stream << "\\end{center}" << "\n";
       if (ui.cilineEdit->text().length()>0 && ui.analiticagroupBox->isChecked())
          {
           stream << "\\begin{center}" << "\n";
           stream << "{\\normalsize \\textbf {" << tr("CÓDIGO DE IMPUTACIÓN:") << " " <<
                     ui.cilineEdit->text() <<  "}}" << "\n";
           stream << "\\end{center}" << "\n";

           QString codigo=ui.cilineEdit->text();
           QString descripcion;
           QString qnivel=0;

           stream << "\\begin{center}" << "\n";
           stream << "{\\normalsize {" ;

           if (codigo.startsWith("???"))
               {
                stream << tr("CUALQUIERA");
               }
           else
               {
                 bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
                 if (encontrada && qnivel.toInt()==1)
                 stream << descripcion;
               }
           if (codigo.length()>3) stream << " - ";
           if (codigo.length()>3 && codigo.mid(3,3)==QString("???")) stream << tr("CUALQUIERA");
           else
                {
                 bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
                 int elnivel=qnivel.toInt();
                 if (encontrada && elnivel==2)
                 stream << descripcion;
                }

           if (codigo.length()>6) stream << " - ";
           if (codigo.length()==7 && codigo[6]=='*')  stream << tr("CUALQUIERA");
            else
               {
                 bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
                 if (encontrada && qnivel.toInt()==3)
                 stream << descripcion;
               }
           stream << "}}" << "\n";
           stream << "\\end{center}" << "\n";
        }

       stream << "{\\textbf {" << tr("Período de") << " " << ui.desdedateEdit->date().toString("dd.MM.yyyy") <<
                             tr(" a ") << ui.hastadateEdit->date().toString("dd.MM.yyyy") << "}}" << "\n";
       if (compa)
        {
         stream << "\\\\\n";
         stream << "{\\textbf {" << tr("Período 2 de") << " " << ui.desde_c_dateEdit->date().toString("dd.MM.yyyy") <<
                             tr(" a ") << ui.hasta_c_dateEdit->date().toString("dd.MM.yyyy") << "}}" << "\n";
        }
       // cuerpo de listado (tablas)
       stream << "\\begin{center}\n";
       if (!compa)
          stream << "\\begin{tabular}{|l|p{7.2cm}|r|r|r|r|}\n";
        else
            stream << "\\begin{tabular}{|l|p{6cm}|r|r|r|r|r|r|r|r|}\n";
       stream << "\\hline\n";
       if (!compa)
        {
         stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("TITULO") <<
          "} & {\\scriptsize " << tr("Saldo Inicial") << "} & {\\scriptsize " << tr("Sumas Debe") <<
          "} & {\\scriptsize " << tr("Sumas Haber") << "} & {\\scriptsize " << tr("Saldo Final");
        }
         else
          {
             stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("TITULO") <<
              "} & {\\scriptsize " << tr("S. Inicial") << "} & {\\scriptsize " << tr("Sum. Debe") <<
              "} & {\\scriptsize " << tr("Sum. Haber") << "} & {\\scriptsize " << tr("S. Final");
             stream << "} & {\\scriptsize " << tr("S. Inicial 2") << "} & {\\scriptsize " << tr("Sum. Debe 2") <<
                  "} & {\\scriptsize " << tr("Sum. Haber 2") << "} & {\\scriptsize " << tr("S. Final 2");
          }
       stream << "} \\\\\n";
       stream << "\\hline\n";
      } // lineapagina ==0
    // if (haysubcuentas)
     {
      stream  << tr("\\multicolumn{2}{|r|}{Sumas ...} ");
      stream << " & {\\scriptsize " << formateanumerosep(sumsaldoini,comadecimal,decimales);
      stream << "} & {\\scriptsize " << formateanumerosep(sumadebe,comadecimal,decimales);
      stream << "} & {\\scriptsize " << formateanumerosep(sumahaber,comadecimal,decimales);
      stream << "} & {\\scriptsize " << formateanumerosep(sumsaldofinal,comadecimal,decimales);
      stream << "} ";
      if (compa)
        {
         stream << " & {\\scriptsize " << formateanumerosep(sumsaldoini2,comadecimal,decimales);
         stream << "} & {\\scriptsize " << formateanumerosep(sumadebe2,comadecimal,decimales);
         stream << "} & {\\scriptsize " << formateanumerosep(sumahaber2,comadecimal,decimales);
         stream << "} & {\\scriptsize " << formateanumerosep(sumsaldofinal2,comadecimal,decimales);
         stream << "}  ";
        }
      stream << " \\\\";
     stream << "\\hline\n";
    }
   stream << "\\end{tabular}\n";
   stream << "\\end{center}\n";

   if (compa) stream << "\\end{landscape}\n";

    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero2.close();


}


void bss::genxml()
{
   pidenombre *p=new pidenombre();
   int resultado=0;
   p->asignanombreventana(tr("Balance sumas y saldos (XML)"));
   p->asignaetiqueta(tr("NOMBRE FICHERO (sin extensión):"));
   p->asignacontenido(tr("bss"));
   resultado=p->exec();
   QString contenido=p->contenido();
   delete p;
   if (resultado==QDialog::Accepted)
      {
       if (contenido.length()>0) fichero=contenido;
       fichero.remove('*');
       fichero.remove("..");
       fichero.remove('/');
       fichero.replace(' ','_');
       if (!ui.inicialescheckBox->isChecked())
            generaXML();
        else
            generaXML2();
        QMessageBox::information( this, tr("BALANCE SUMAS Y SALDOS"),
                                 tr("Se ha generado el archivo XML"));
      }
}


void bss::generaXML()
{

    bool haysubcuentas=false;
    int veces=0;
    while (veces<ui.table->rowCount())
      {
        if ((ui.table->item(veces,0)->text().length()==anchocuentas() && !cod_longitud_variable())
            || (cod_longitud_variable() && esauxiliar(ui.table->item(veces,0)->text())))
        {
            haysubcuentas=true;
            break;
        }
        veces++;
      }

 bool compa = ui.compa_groupBox->isChecked();

 QProgressDialog progreso(tr("Cargando información ..."), 0, 0, 3);
  progreso.setMinimumDuration(100);
  progreso.setWindowModality(Qt::WindowModal);
  progreso.setValue(1);
  QCoreApplication::processEvents();


 QDomDocument doc("BalanceSumasYSaldos");
 QDomElement root = doc.createElement("BalanceSumasYSaldos");
 doc.appendChild(root);

 QDomElement tag = doc.createElement("Cabecera");
 root.appendChild(tag);
 addElementoTextoDom(doc,tag,"NombreEmpresa",filtracadxml(nombreempresa()));
 QString cadci;
 QString caddescripci;
 if (ui.cilineEdit->text().length()>0 && ui.analiticagroupBox->isChecked())
    {
      cadci=ui.cilineEdit->text();
      caddescripci= cadena_descrip_ci(cadci);
    }
 addElementoTextoDom(doc,tag,"CI",cadci);
 addElementoTextoDom(doc,tag,"CIDescrip",caddescripci);
 addElementoTextoDom(doc,tag,"Inicio",ui.desdedateEdit->date().toString("yyyy/MM/dd"));
 addElementoTextoDom(doc,tag,"Fin",ui.hastadateEdit->date().toString("yyyy/MM/dd"));

 if (compa)
   {
     addElementoTextoDom(doc,tag,"Inicio2",ui.desde_c_dateEdit->date().toString("yyyy/MM/dd"));
     addElementoTextoDom(doc,tag,"Fin2",ui.hasta_c_dateEdit->date().toString("yyyy/MM/dd"));
   }

 addElementoTextoDom(doc,tag,"ConAuxiliares",haysubcuentas ? "true" : "false");

 QDomElement tag2 = doc.createElement("Detalle");
 root.appendChild(tag2);
 int pos=0;
 while (pos<ui.table->rowCount())
   {
     if (ui.conmovcheckBox->isChecked())
           {
              double saldoinicial=convapunto(ui.table->item(pos,2)->text()).toDouble();
              double sumadebe=convapunto(ui.table->item(pos,3)->text()).toDouble();
              double sumahaber=convapunto(ui.table->item(pos,4)->text()).toDouble();
              if (!compa)
                {
                 if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                    && (saldoinicial>-0.001 && saldoinicial<0.001))
                   {pos++; continue;}
                }
              else
                  {
                   double saldoinicial2=convapunto(ui.table->item(pos,6)->text()).toDouble();
                   double sumadebe2=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   double sumahaber2=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   if ((sumadebe>-0.001 && sumadebe<0.001)
                       && (sumahaber>-0.001 && sumahaber<0.001)
                      && (saldoinicial>-0.001 && saldoinicial<0.001) &&
                         (sumadebe2>-0.001 && sumadebe2<0.001)
                      && (sumahaber2>-0.001 && sumahaber2<0.001)
                      && (saldoinicial2>-0.001 && saldoinicial2<0.001))
                       {pos++; continue;}
                  }
           }
    QDomElement tag3 = doc.createElement("Linea");
    tag2.appendChild(tag3);
    addElementoTextoDom(doc,tag3,"Cuenta",ui.table->item(pos,0)->text());
    addElementoTextoDom(doc,tag3,"Descrip",filtracadxml(ui.table->item(pos,1)->text()));
    bool auxiliar=ui.table->item(pos,0)->text().length()==anchocuentas() || esauxiliar(ui.table->item(pos,0)->text());
    addElementoTextoDom(doc,tag3,"Auxiliar",auxiliar ? "true" : "false");
    double numero1=convapunto(ui.table->item(pos,3)->text()).toDouble();
    double numero2=convapunto(ui.table->item(pos,4)->text()).toDouble();
    addElementoTextoDom(doc,tag3,"SumaDebe",formateanumero(numero1,comadecimal,decimales));
    addElementoTextoDom(doc,tag3,"SumaHaber",formateanumero(numero2,comadecimal,decimales));
    if (compa)
      {
        double numero11=convapunto(ui.table->item(pos,7)->text()).toDouble();
        double numero22=convapunto(ui.table->item(pos,8)->text()).toDouble();
        addElementoTextoDom(doc,tag3,"SumaDebe2",formateanumero(numero11,comadecimal,decimales));
        addElementoTextoDom(doc,tag3,"SumaHaber2",formateanumero(numero22,comadecimal,decimales));
      }
    QString cadsaldodebe;
    if (convapunto(ui.table->item(pos,5)->text()).toDouble()>0.001)
         {
            double num=convapunto(ui.table->item(pos,5)->text()).toDouble();
            cadsaldodebe=formateanumero(num,comadecimal,decimales);
         }
    addElementoTextoDom(doc,tag3,"SaldoDebe",cadsaldodebe);
    QString cadsaldohaber;
    if (convapunto(ui.table->item(pos,5)->text()).toDouble()<0.001)
          {
            double num=convapunto(ui.table->item(pos,5)->text()).toDouble();
            cadsaldohaber=formateanumero(num,comadecimal,decimales).remove('-');
          }
    addElementoTextoDom(doc,tag3,"SaldoHaber",cadsaldohaber);

    if (compa)
      {
        QString cadsaldodebe2;
        if (convapunto(ui.table->item(pos,9)->text()).toDouble()>0.001)
             {
                double num=convapunto(ui.table->item(pos,9)->text()).toDouble();
                cadsaldodebe2=formateanumero(num,comadecimal,decimales);
             }
        addElementoTextoDom(doc,tag3,"SaldoDebe2",cadsaldodebe2);
        QString cadsaldohaber2;
        if (convapunto(ui.table->item(pos,9)->text()).toDouble()<0.001)
              {
                double num=convapunto(ui.table->item(pos,9)->text()).toDouble();
                cadsaldohaber2=formateanumero(num,comadecimal,decimales).remove('-');
              }
        addElementoTextoDom(doc,tag3,"SaldoHaber2",cadsaldohaber);
      }

    pos++;
   }

  QString xml = doc.toString();

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=fichero;
   qfichero+=".xml";

  QFile bss( adapta(qfichero)  );

  if ( !bss.open( QIODevice::WriteOnly ) )
       {
        QMessageBox::warning( this, tr("Balance de sumas y saldos (XML)"),tr("Error: Imposible grabar fichero"));
        return;
       }
  QTextStream bssstream( &bss );
  bssstream.setEncoding(QStringConverter::Utf8);
  bssstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  // bssstream << "<?xml-stylesheet type=\"text/xsl\" href=\"plan2html.xslt\"?>\n";

  // ------------------------------------------------------------------------------------
  bssstream << xml;

  bss.close();

  /*
  QString qficherologo=dirtrabajo();
  qficherologo.append(QDir::separator());
  QString rutagraf=qficherologo+"logo.png";
  //if (eswindows()) rutagraf=QFile::encodeName(rutagraf);
  // generamos imagen del logo
  QString imagen=basedatos::instancia()->logo_empresa();
  QPixmap foto;
  if (imagen.isEmpty()) imagen = logodefecto();
  if (imagen.length()>0)
     {
      QByteArray byteshexa;
      byteshexa.append ( imagen );
      QByteArray bytes;
      bytes=bytes.fromHex ( byteshexa );
      foto.loadFromData ( bytes, "PNG");
      foto.save(rutagraf,"PNG");
     }

  progreso.setValue(2);
  QCoreApplication::processEvents();


  QString cadfichjasper=trayreport();
  cadfichjasper.append(QDir::separator());
  cadfichjasper+= (!compa ? "bss.jasper" : "bss-comp.jasper");

  QString cadpdf=dirtrabajo();
  cadpdf.append(QDir::separator());
  cadpdf+=fichero+".pdf";



  informe_jasper_xml(cadfichjasper, qfichero,
                     "/BalanceSumasYSaldos/Detalle/Linea", cadpdf,
                     rutagraf);

  progreso.setValue(3);
  */
  QCoreApplication::processEvents();



}



// xml para el balance de comprobación con saldos iniciales de cuentas
void bss::generaXML2()
{
    bool haysubcuentas=false;
    bool compa=ui.compa_groupBox->isChecked();
    int veces=0;
    while (veces<ui.table->rowCount())
      {
        if ((ui.table->item(veces,0)->text().length()==anchocuentas() && !cod_longitud_variable())
            || (cod_longitud_variable() && esauxiliar(ui.table->item(veces,0)->text())))
        {
            haysubcuentas=true;
            break;
        }
        veces++;
      }

    QProgressDialog progreso(tr("Cargando información ..."), 0, 0, 3);
     progreso.setMinimumDuration(100);
     progreso.setWindowModality(Qt::WindowModal);
     progreso.setValue(1);
     QCoreApplication::processEvents();


 QDomDocument doc("BalanceCombrobacion");
 QDomElement root = doc.createElement("BalanceComprobacion");
 doc.appendChild(root);

 QDomElement tag = doc.createElement("Cabecera");
 root.appendChild(tag);
 addElementoTextoDom(doc,tag,"NombreEmpresa",filtracadxml(nombreempresa()));
 QString cadci;
 QString caddescripci;
 if (ui.cilineEdit->text().length()>0 && ui.analiticagroupBox->isChecked())
    {
      cadci=ui.cilineEdit->text();
      caddescripci= cadena_descrip_ci(cadci);
    }
 addElementoTextoDom(doc,tag,"CI",cadci);
 addElementoTextoDom(doc,tag,"CIDescrip",caddescripci);
 addElementoTextoDom(doc,tag,"Inicio",ui.desdedateEdit->date().toString("yyyy/MM/dd"));
 addElementoTextoDom(doc,tag,"Fin",ui.hastadateEdit->date().toString("yyyy/MM/dd"));

 if (compa)
   {
     addElementoTextoDom(doc,tag,"Inicio2",ui.desde_c_dateEdit->date().toString("yyyy/MM/dd"));
     addElementoTextoDom(doc,tag,"Fin2",ui.hasta_c_dateEdit->date().toString("yyyy/MM/dd"));
   }


 addElementoTextoDom(doc,tag,"ConAuxiliares",haysubcuentas ? "true" : "false");

 QDomElement tag2 = doc.createElement("Detalle");
 root.appendChild(tag2);
 int pos=0;
 while (pos<ui.table->rowCount())
   {
     if (ui.conmovcheckBox->isChecked())
           {
              double saldoinicial=convapunto(ui.table->item(pos,2)->text()).toDouble();
              double sumadebe=convapunto(ui.table->item(pos,3)->text()).toDouble();
              double sumahaber=convapunto(ui.table->item(pos,4)->text()).toDouble();
              if (!compa)
                {
                 if ((sumadebe>-0.001 && sumadebe<0.001) && (sumahaber>-0.001 && sumahaber<0.001)
                    && (saldoinicial>-0.001 && saldoinicial<0.001))
                   {pos++; continue;}
                }
              else
                  {
                   double saldoinicial2=convapunto(ui.table->item(pos,6)->text()).toDouble();
                   double sumadebe2=convapunto(ui.table->item(pos,7)->text()).toDouble();
                   double sumahaber2=convapunto(ui.table->item(pos,8)->text()).toDouble();
                   if ((sumadebe>-0.001 && sumadebe<0.001)
                       && (sumahaber>-0.001 && sumahaber<0.001)
                      && (saldoinicial>-0.001 && saldoinicial<0.001) &&
                         (sumadebe2>-0.001 && sumadebe2<0.001)
                      && (sumahaber2>-0.001 && sumahaber2<0.001)
                      && (saldoinicial2>-0.001 && saldoinicial2<0.001))
                       {pos++; continue;}
                  }
           }
    QDomElement tag3 = doc.createElement("Linea");
    tag2.appendChild(tag3);
    addElementoTextoDom(doc,tag3,"Cuenta",ui.table->item(pos,0)->text());
    addElementoTextoDom(doc,tag3,"Descrip",filtracadxml(ui.table->item(pos,1)->text()));
    bool auxiliar=ui.table->item(pos,0)->text().length()==anchocuentas() || esauxiliar(ui.table->item(pos,0)->text());
    addElementoTextoDom(doc,tag3,"Auxiliar",auxiliar ? "true" : "false");
    double saldoini=convapunto(ui.table->item(pos,2)->text()).toDouble();
    double numero1=convapunto(ui.table->item(pos,3)->text()).toDouble();
    double numero2=convapunto(ui.table->item(pos,4)->text()).toDouble();
    // Saldo inicial
    addElementoTextoDom(doc,tag3,"SaldoInicial",formateanumero(saldoini,comadecimal,decimales));
    // Sumas Debe
    addElementoTextoDom(doc,tag3,"SumaDebe",formateanumero(numero1,comadecimal,decimales));
    // Sumas Haber
    addElementoTextoDom(doc,tag3,"SumaHaber",formateanumero(numero2,comadecimal,decimales));
    // Saldo final
    double num=convapunto(ui.table->item(pos,5)->text()).toDouble();
    addElementoTextoDom(doc,tag3,"SaldoFinal",formateanumero(num,comadecimal,decimales));
    if (compa)
       {
        double saldoini2=convapunto(ui.table->item(pos,6)->text()).toDouble();
        double numero11=convapunto(ui.table->item(pos,7)->text()).toDouble();
        double numero22=convapunto(ui.table->item(pos,8)->text()).toDouble();
        // Saldo inicial
        addElementoTextoDom(doc,tag3,"SaldoInicial2",formateanumero(saldoini2,comadecimal,decimales));
        // Sumas Debe
        addElementoTextoDom(doc,tag3,"SumaDebe2",formateanumero(numero11,comadecimal,decimales));
        // Sumas Haber
        addElementoTextoDom(doc,tag3,"SumaHaber2",formateanumero(numero22,comadecimal,decimales));
        // Saldo final
        double num=convapunto(ui.table->item(pos,9)->text()).toDouble();
        addElementoTextoDom(doc,tag3,"SaldoFinal2",formateanumero(num,comadecimal,decimales));
       }
    pos++;
   }

  QString xml = doc.toString();

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=fichero;
   qfichero+=".xml";


  QFile bss( adapta(qfichero)  );

  if ( !bss.open( QIODevice::WriteOnly ) )
       {
        QMessageBox::warning( this, tr("Balance de sumas y saldos (XML)"),tr("Error: Imposible grabar fichero"));
        return;
       }
  QTextStream bssstream( &bss );
  bssstream.setEncoding(QStringConverter::Utf8);

  bssstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  // bssstream << "<?xml-stylesheet type=\"text/xsl\" href=\"plan2html.xslt\"?>\n";

  // ------------------------------------------------------------------------------------
  bssstream << xml;

  bss.close();


  QString qficherologo=dirtrabajo();
  qficherologo.append(QDir::separator());
  QString rutagraf=qficherologo+"logo.png";
  // if (eswindows()) rutagraf=QFile::encodeName(rutagraf);
  // generamos imagen del logo
  QString imagen=basedatos::instancia()->logo_empresa();
  QPixmap foto;
  if (imagen.isEmpty()) imagen = logodefecto();
  if (imagen.length()>0)
     {
      QByteArray byteshexa;
      byteshexa.append ( imagen.toUtf8() );
      QByteArray bytes;
      bytes=bytes.fromHex ( byteshexa );
      foto.loadFromData ( bytes, "PNG");
      foto.save(rutagraf,"PNG");
     }

  progreso.setValue(2);
  QCoreApplication::processEvents();


  QString cadfichjasper=trayreport();
  cadfichjasper.append(QDir::separator());
  if (!compa) cadfichjasper+="bcomp.jasper";
   else cadfichjasper+="bcompro-comp.jasper";

  QString cadpdf=dirtrabajo();
  cadpdf.append(QDir::separator());
  cadpdf+=fichero+".pdf";


  if (!compa)
    informe_jasper_xml(cadfichjasper, qfichero,
                     "/BalanceComprobacion/Detalle/Linea", cadpdf,
                     rutagraf);
    else
        informe_jasper_xml(cadfichjasper, qfichero,
                         "/BalanceComprobacion/Detalle/Linea", cadpdf,
                         rutagraf);

  progreso.setValue(3);
  QCoreApplication::processEvents();



}



bool bss::sindiarioapertura()
{
    bool conapertura=false;
    for (int veces=0;veces<numdiarios;veces++)
    {
        if (((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->isChecked())
        {
                if (((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->text().remove('&')==diario_apertura())
                    conapertura=true;
        }
    }
    return !conapertura;
}


void bss::latex()
{
    if (ui.table->rowCount()==0)
     {
        QMessageBox::warning( this, tr("BALANCE SUMAS Y SALDOS"),tr("ERROR: Consulta vacía"));
        return;
     }
    fichero=tr("bss");
    if (!ui.inicialescheckBox->isChecked())
        generalatex();
     else
        generalatex2();

    int valor=editalatex("bss");
    if (valor==1)
        QMessageBox::warning( this, tr("BALANCE SUMAS Y SALDOS"),tr("PROBLEMAS al llamar al editor Latex"));

}


void bss::consultamayor()
{
    if (ui.table->rowCount()<=0) return;
    if (ui.table->item(ui.table->currentRow(),0)==0) return;
    QString cuenta=ui.table->item(ui.table->currentRow(),0)->text();
    if (cuenta.length()>0)
    {
        QDate fecha;
        consmayor *elmayor=new consmayor(comadecimal,decimales,usuario);
        fecha=ui.desdedateEdit->date();
        elmayor->pasadatos(cuenta,fecha);
        elmayor->exec();
        delete(elmayor);
    }
    ui.actualizapushButton->setEnabled(true);

}


void bss::on_externos_pushButton_clicked()
{
    if (ui.table->rowCount()<=0) return;
    if (ui.table->item(ui.table->currentRow(),0)==0) return;
    QString cuenta=ui.table->item(ui.table->currentRow(),0)->text();
    if (cuenta.length()>0)
    {
        externos_cuenta *e = new externos_cuenta();
        e->pasa_parametros(cuenta,ui.desdedateEdit->date(),ui.hastadateEdit->date());
        e->exec();
        delete(e);
    }
    ui.actualizapushButton->setEnabled(true);
}
