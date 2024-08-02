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

#include "calcestado.h"
#include <QSqlQuery>
#include "introci.h"
#include "funciones.h"
#include "referencias.h"
#include "aritmetica.h"
#include "basedatos.h"
// #include "grafico.h"
#include <QMessageBox>
#include <QProgressDialog>
#include <qtrpt.h>
#include "estado_grafico.h"

calcestado::calcestado() : QDialog() {
  ui.setupUi(this);

  comadecimal=haycomadecimal();
  decimales=haydecimales();
  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1,ej2;
  ej2 << noejercicio();
  if ( query.isActive() ) {
          while ( query.next() )
              {
                ej1 << query.value(0).toString();
                ej2 << query.value(0).toString();
	      }
	  }
  ui.Ejercicio1comboBox->addItems(ej1);
  ui.Ejercicio2comboBox->addItems(ej2);

  QDate fechaini=inicioejercicio(ui.Ejercicio1comboBox->currentText());
  QDate fechafin=finejercicio(ui.Ejercicio1comboBox->currentText());

  ui.ini_ej1_dateEdit->setDate(fechaini);
  ui.fin_ej1_dateEdit->setDate(fechafin);

  ui.fechas_ej2_groupBox->setEnabled(false);

  condesglose=false;
  condesglosectas=false;
  hay_analitica_tabla=basedatos::instancia()->analitica_tabla();

  connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(fijadescripciones()));
  connect(ui.buscapushButton,SIGNAL(clicked()),this,SLOT(buscapulsado()));
  connect(ui.referenciaspushButton,SIGNAL(clicked()), this, SLOT(procesareferencias()));
  connect(ui.CalcularpushButton,SIGNAL(clicked()), this, SLOT(calculaestado()));
  connect(ui.ConsultapushButton,SIGNAL(clicked()), this, SLOT (consulta( void )));
  connect(ui.ImprimirpushButton,SIGNAL(clicked()),this,SLOT(imprime()));
  connect(ui.copiarpushButton,SIGNAL(clicked()),this,SLOT(copia()));
  connect(ui.graficopushButton,SIGNAL(clicked()),this,SLOT(generagrafico()));

  connect(ui.Ejercicio1comboBox,SIGNAL(currentIndexChanged (int)),this,SLOT(actualizacomboej1()));
  connect(ui.Ejercicio2comboBox,SIGNAL(currentIndexChanged (int)),this,SLOT(actualizacomboej2()));

  connect(ui.ini_ej1_dateEdit,SIGNAL(dateChanged (QDate)),this,SLOT(fecha_ej1_ini_cambiada()));
  connect(ui.fin_ej1_dateEdit,SIGNAL(dateChanged (QDate)),this,SLOT(fecha_ej1_fin_cambiada()));
  connect(ui.ini_ej2_dateEdit,SIGNAL(dateChanged (QDate)),this,SLOT(fecha_ej2_ini_cambiada()));
  connect(ui.fin_ej2_dateEdit,SIGNAL(dateChanged (QDate)),this,SLOT(fecha_ej2_fin_cambiada()));
  connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));
  connect(ui.xmlpushButton,SIGNAL(clicked()),SLOT(genera_xml()));

  msj_calculado=true;

#ifdef NOEDITTEX
 ui.latexpushButton->hide();
#endif

}


void calcestado::no_msj_calculado()
{
  msj_calculado=false;
}

void calcestado::pasaejercicio1(QString ejercicio)
{
    if (!ejercicio.isEmpty())
       {
        int indice=0;
        while (indice<ui.Ejercicio1comboBox->count())
           {
             if (ui.Ejercicio1comboBox->itemText(indice)==ejercicio) break;
             indice++;
           }
        if (indice>ui.Ejercicio1comboBox->count()) indice--;
        ui.Ejercicio1comboBox->setCurrentIndex(indice);
       }

}

void calcestado::pasaejercicio2(QString ejercicio)
{
    if (!ejercicio.isEmpty())
       {
        int indice=0;
        while (indice<ui.Ejercicio2comboBox->count())
           {
             if (ui.Ejercicio2comboBox->itemText(indice)==ejercicio) break;
             indice++;
           }
        if (indice>ui.Ejercicio2comboBox->count()) indice--;
        ui.Ejercicio2comboBox->setCurrentIndex(indice);
       }

}

void calcestado::fecha_ej1_ini_cambiada()
{
 QDate fecha=ui.ini_ej1_dateEdit->date();
 if (fecha < inicioejercicio(ui.Ejercicio1comboBox->currentText()))
    ui.ini_ej1_dateEdit->setDate(inicioejercicio(ui.Ejercicio1comboBox->currentText()));

 if (fecha > finejercicio(ui.Ejercicio1comboBox->currentText()))
    ui.ini_ej1_dateEdit->setDate(inicioejercicio(ui.Ejercicio1comboBox->currentText()));

}

void calcestado::fecha_ej1_fin_cambiada()
{
 QDate fecha=ui.fin_ej1_dateEdit->date();
 if (fecha < inicioejercicio(ui.Ejercicio1comboBox->currentText()))
    ui.fin_ej1_dateEdit->setDate(finejercicio(ui.Ejercicio1comboBox->currentText()));

 if (fecha > finejercicio(ui.Ejercicio1comboBox->currentText()))
    ui.fin_ej1_dateEdit->setDate(finejercicio(ui.Ejercicio1comboBox->currentText()));

}

void calcestado::fecha_ej2_ini_cambiada()
{
 QDate fecha=ui.ini_ej2_dateEdit->date();
 if (fecha < inicioejercicio(ui.Ejercicio2comboBox->currentText()))
    ui.ini_ej2_dateEdit->setDate(inicioejercicio(ui.Ejercicio2comboBox->currentText()));

 if (fecha > finejercicio(ui.Ejercicio2comboBox->currentText()))
    ui.ini_ej2_dateEdit->setDate(inicioejercicio(ui.Ejercicio2comboBox->currentText()));

}

void calcestado::fecha_ej2_fin_cambiada()
{
 QDate fecha=ui.fin_ej2_dateEdit->date();
 if (fecha < inicioejercicio(ui.Ejercicio2comboBox->currentText()))
    ui.fin_ej2_dateEdit->setDate(finejercicio(ui.Ejercicio2comboBox->currentText()));

 if (fecha > finejercicio(ui.Ejercicio2comboBox->currentText()))
    ui.fin_ej2_dateEdit->setDate(finejercicio(ui.Ejercicio2comboBox->currentText()));
}


void calcestado::actualizacomboej1()
{
  QDate fechaini=inicioejercicio(ui.Ejercicio1comboBox->currentText());
  QDate fechafin=finejercicio(ui.Ejercicio1comboBox->currentText());

  ui.ini_ej1_dateEdit->setDate(fechaini);
  ui.fin_ej1_dateEdit->setDate(fechafin);
}

void calcestado::actualizacomboej2()
{
  if (ui.Ejercicio2comboBox->currentText()==noejercicio())
    {
     ui.fechas_ej2_groupBox->setEnabled(false);
     return;
    }

  ui.fechas_ej2_groupBox->setEnabled(true);
  QDate fechaini=inicioejercicio(ui.Ejercicio2comboBox->currentText());
  QDate fechafin=finejercicio(ui.Ejercicio2comboBox->currentText());

  ui.ini_ej2_dateEdit->setDate(fechaini);
  ui.fin_ej2_dateEdit->setDate(fechafin);
}


QString calcestado::noejercicio()
{
  return tr("- -");
}


void calcestado::cargaestado( QString titulo )
{
   QDate lafecha;
   QSqlQuery query = basedatos::instancia()->selectCabeceraestadostitulo(titulo);
   if ( (query.isActive()) &&  (query.first()) ){
       ui.titulolabel->setText(query.value(0).toString());
	   QString ejercicio1,ejercicio2;
	   ejercicio1 = query.value(1).toString();
	   if (ejercicio1.length()==0)
	      {
	        QString apertura = basedatos::instancia()->selectMaxaperturaejercicios();
	        ejercicio1 = basedatos::instancia()->selectCodigoejercicioapertura(apertura);;
	      }
	   int indice=0;
	   while (indice<ui.Ejercicio1comboBox->count())
	      {
	        if (ui.Ejercicio1comboBox->itemText(indice)==ejercicio1) break;
	        indice++;
	      }
	   if (indice>ui.Ejercicio1comboBox->count()) indice--;
	   ui.Ejercicio1comboBox->setCurrentIndex(indice);
	   
	   ejercicio2=query.value(2).toString();
           if (ejercicio2.length()==0)
		ejercicio2=noejercicio();
	   indice=0;
	   while (indice<ui.Ejercicio2comboBox->count())
	      {
	        if (ui.Ejercicio2comboBox->itemText(indice)==ejercicio2) break;
	        indice++;
	      }
	   if (indice>ui.Ejercicio2comboBox->count()) indice--;
	   ui.Ejercicio2comboBox->setCurrentIndex(indice);
	   
	   lafecha=query.value(3).toDate();
	   if (lafecha.year()>2000)
	       ui.FechadateEdit->setDate(lafecha);
           QString linea=query.value(4).toString();
           int ndiarios=linea.count(' ')+1;
           for (int veces=0;veces<ndiarios;veces++)
               {
                QString trozo=linea.section(' ',veces,veces);
                ui.diariolistWidget->addItem(trozo);
               }
           // if (query.value(4).toBool()) ui.aperturalabel->setEnabled(true);
           // if (query.value(5).toBool()) ui.generallabel->setEnabled(true);
           // if (query.value(6).toBool()) ui.regularizacionlabel->setEnabled(true);
           if (query.value(5).toBool()) 
              {
               ui.medioslabel->setText("SALDOS MEDIOS");
               consaldosmedios=true;
               ui.fechas_ej1_groupBox->setEnabled(false);
               ui.fechas_ej2_groupBox->setEnabled(false);
              }
              else {ui.medioslabel->setText("SALDOS NORMALES");consaldosmedios=false;}
           if (query.value(6).toBool()) 
              {
               ui.analiticagroupBox->setEnabled(true);
               ui.cilineEdit->setText(query.value(7).toString());
              }
              else ui.analiticagroupBox->setDisabled(true);
           if (!query.value(8).toBool()) ui.referenciaspushButton->setEnabled(false);
           if (query.value(10).toBool() || query.value(13).toBool())
               {
                if (query.value(10).toBool())
                   {
                    condesglose=true;
                    ui.desgloselabel->setText("DETALLE AUXILIARES");
                   }
                if (query.value(13).toBool())
                   {
                    condesglosectas=true;
                    ui.desgloselabel->setText("DETALLE CUENTAS");
                   }
                ui.Ejercicio2comboBox->setEnabled(false);
                ui.ejercicio2textLabel->setEnabled(false);
                ui.fechas_ej2_groupBox->setEnabled(false);
               }
               else
                    {
                     ui.desgloselabel->setText("SIN DETALLE");
                    }
           if (query.value(5).toBool())
               {
                ui.diariolistWidget->setDisabled(true);
                // ui.aperturalabel->setDisabled(true);
                // ui.generallabel->setDisabled(true);
                // ui.regularizacionlabel->setDisabled(true);
               }
           if (query.value(11).toBool())
              ui.graficopushButton->setEnabled(true);
           ui.horalineEdit->setText(query.value(12).toString());
	  }
}



void calcestado::buscapulsado()
{
 introci *c = new introci();
 c->hazparafiltro();
 c->pasaci(ui.cilineEdit->text());
 c->exec();
 ui.cilineEdit->setText(c->cadenaci());
 delete(c);
}


void calcestado::fijadescripciones()
{

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

void calcestado::procesareferencias()
{
 referencias *r = new referencias();
 r->cargaestado( ui.titulolabel->text() );
 r->exec();
 delete(r);

}


bool calcestado::haymedias()
{
    return basedatos::instancia()->calcestadohaymedias( ui.titulolabel->text() );
}


bool calcestado::saldosmediosbloqueados()
{
    if ( basedatos::instancia()->selectBloqsaldosmedios() )
    {
        QMessageBox::warning( this, tr("Estados Contables"),tr("IMPOSIBLE EL CÁLCULO, OTRO TERMINAL ESTÁ REALIZANDO OPERACIONES CON SALDOS MEDIOS"));
        return true;
    }
    return false;
}

void calcestado::bloqueasaldosmedios()
{
    basedatos::instancia()->updateConfiguracionbloqsaldosmedios(true);
}


void calcestado::desbloqueasaldosmedios()
{
    basedatos::instancia()->updateConfiguracionbloqsaldosmedios(false);
}


void calcestado::calculasaldosmediosperiodo(bool ejercicio1)
{
  QString cadquery;
  QString cadsubquery;
  QSqlQuery subquery;
  QSqlQuery query3;
  QString cadquery3;
  QString ejercicio="";
  QDate fechaini,fechafin;
  // asignamos ejercicio ...
  if (ejercicio1) ejercicio=ui.Ejercicio1comboBox->currentText();
     else ejercicio=ui.Ejercicio2comboBox->currentText();

    fechaini = basedatos::instancia()->selectAperturaejercicios(ejercicio);
    fechafin = basedatos::instancia()->selectCierreejercicios(ejercicio);
  
  QDate fechainicial;
  double saldoinicial=0;
  double numerador=0;
  double resultado=0;
  int denominador=0;
  int dias=0;
  QString cadnum;
  int cuentas = basedatos::instancia()->selectCountcodigosaldossubcuenta();

  ui.progressBar->reset();
  ui.progressBar->setMaximum(cuentas);
  QSqlQuery query = basedatos::instancia()->selectCodigosaldossubcuenta();
  int proceso=0;
  if ( query.isActive() ) {
          while ( query.next() ) {
            subquery = basedatos::instancia()->selectFechadebehaberdiariocuentafechasnocierreorderfecha( query.value(0).toString() , fechaini,fechafin);

	    if (subquery.isActive())
	       {
	         fechainicial=fechaini;
		 saldoinicial=0; numerador=0; denominador=0;
	         while (subquery.next())
		   {
		    QString cc;
		    dias=fechainicial.daysTo(subquery.value(0).toDate());
		    cc.setNum(dias);
		    cc.setNum(saldoinicial,'f');
		    fechainicial=subquery.value(0).toDate();
		    numerador+=saldoinicial*dias;
		    denominador+=dias;
		    saldoinicial+=subquery.value(1).toDouble()-subquery.value(2).toDouble();
		   }
		 dias=fechainicial.daysTo(fechafin);
		 numerador+=saldoinicial*dias;
		 denominador+=dias;
		 if (denominador>0) resultado=numerador/denominador;
		    else resultado=0;
		 cadnum.setNum(resultado,'f',2);

		 if (ejercicio1) basedatos::instancia()->updateSaldossubcuentasaldomedio1codigo(cadnum, query.value(0).toString() );
		    else basedatos::instancia()->updateSaldossubcuentasaldomedio2codigo(cadnum, query.value(0).toString() );

	       }
              proceso++;
              ui.progressBar->setValue(proceso);
              QApplication::processEvents();
          }
     }
   ui.progressBar->reset();
   ui.progressBar->setMaximum(100);
}



void calcestado::calculaestado()
{
   cargarinfo();
   if ( !cifiltrook(ui.cilineEdit->text()) && ui.cilineEdit->text().length()>0)
       {
	 QMessageBox::warning( this, tr("Estados Contables"),
          tr("ERROR: CÓDIGO DE IMPUTACIÓN INCORRECTO"));
	 return;
       }
   ui.progressBar->reset();
   ui.progressBar->setMaximum(100);
   if (consaldosmedios || haymedias())
      { 
        // calcestadomedio();
        // return;
       if (saldosmediosbloqueados()) return;
       bloqueasaldosmedios();
       calculasaldosmediosperiodo(true);
       if (ui.Ejercicio2comboBox->currentText()!=noejercicio()) calculasaldosmediosperiodo(false);
      }
   ui.progressBar->setValue(10);
   QApplication::processEvents();
   bool estadosmedios=consaldosmedios;
   //  ------------------------------------------------------------------------------------------------
    basedatos::instancia()->updateEstadoscalculadotitulo(false,ui.titulolabel->text());

  ui.progressBar->setValue(20);
  QApplication::processEvents();

 QSqlQuery query = basedatos::instancia()->select5Estadostituloordernodo(ui.titulolabel->text());
 QString qnodo,qformula;
 double resultado1=0;
 double resultado2=0;
 bool parte1;
 bool yacalculado=false;

 if (ui.desglose_mes_checkBox->isChecked())
 {
     if ( query.isActive() ) {
        while ( query.next() )
              {
               resultado2=0;
               parte1=query.value(0).toBool();
               qnodo=query.value(2).toString();
               qformula=query.value(3).toString();
               // lo hacemos en dos pasadas, primero evitamos los puntos con
               // totales de apartados con llaves
               if (qformula.contains('{')) continue;
               resultado1=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),&yacalculado,estadosmedios,
                                        ui.fechas_ej1_groupBox->isChecked(),
                                        ui.ini_ej1_dateEdit->date(),
                                        ui.fin_ej1_dateEdit->date());
               resultado1=redond(resultado1, 2); //---------------------
               basedatos::instancia()->updateEstadoscalcularestado( resultado1, resultado2, true, parte1, qnodo, ui.titulolabel->text() );

               QDate inicio;
               QDate fin;
               double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
               int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();
               inicio.setDate(anyo,1,1);
               fin.setDate(anyo,1,31);

               m1=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,1);

               inicio.setDate(anyo,2,1);
               fin.setDate(anyo,3,1);
               fin=fin.addDays(-1);
               m2=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,2);

               inicio.setDate(anyo,3,1);
               fin.setDate(anyo,3,31);
               m3=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,3);

               inicio.setDate(anyo,4,1);
               fin.setDate(anyo,4,30);
               m4=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,4);
               // ------------------------------------------------------------------
               inicio.setDate(anyo,5,1);
               fin.setDate(anyo,5,31);
               m5=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,5);
               // ------------------------------------------------------------------
               inicio.setDate(anyo,6,1);
               fin.setDate(anyo,6,30);
               m6=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,6);
               // ------------------------------------------------------------------
               inicio.setDate(anyo,7,1);
               fin.setDate(anyo,7,31);
               m7=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,7);
               // ------------------------------------------------------------------
               inicio.setDate(anyo,8,1);
               fin.setDate(anyo,8,31);
               m8=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,8);
               // ------------------------------------------------------------------
               inicio.setDate(anyo,9,1);
               fin.setDate(anyo,9,30);
               m9=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,9);
               // ------------------------------------------------------------------
               inicio.setDate(anyo,10,1);
               fin.setDate(anyo,10,31);
               m10=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,10);
               // ------------------------------------------------------------------
               inicio.setDate(anyo,11,1);
               fin.setDate(anyo,11,30);
               m11=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,11);
               // ------------------------------------------------------------------
               inicio.setDate(anyo,12,1);
               fin.setDate(anyo,12,31);
               m12=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                         &yacalculado,estadosmedios,
                                          true, // acotación
                                          inicio,
                                          fin,12);
               // ------------------------------------------------------------------

                      // queda grabar los resultados
               basedatos::instancia()->actu_rdos_mes( m1, m2, m3, m4, m5, m6, m7, m8, m9, m10,
                                                      m11, m12,
                                                      true, parte1, qnodo,
                                                      ui.titulolabel->text() );
              }

     }
 }

 if (!ui.desglose_mes_checkBox->isChecked())
  if ( query.isActive() ) {
     while ( query.next() ) 
           {
    	    resultado2=0;
    	    parte1=query.value(0).toBool();
    	    qnodo=query.value(2).toString();
    	    qformula=query.value(3).toString();
    	    // lo hacemos en dos pasadas, primero evitamos los puntos con
    	    // totales de apartados con llaves
    	    if (qformula.contains('{')) continue;
    	    resultado1=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),&yacalculado,estadosmedios,
                                     ui.fechas_ej1_groupBox->isChecked(),
                                     ui.ini_ej1_dateEdit->date(),
                                     ui.fin_ej1_dateEdit->date());
    	    if (ui.Ejercicio2comboBox->currentText()!=noejercicio())
    	     resultado2=calculapartado(qformula,ui.Ejercicio2comboBox->currentText(),&yacalculado,estadosmedios,
                                     ui.fechas_ej2_groupBox->isChecked(),
                                     ui.ini_ej2_dateEdit->date(),
                                     ui.fin_ej2_dateEdit->date());
                 // queda grabar los resultados
            resultado1=redond(resultado1, 2); //---------------------
            resultado2=redond(resultado2, 2); //---------------------
            basedatos::instancia()->updateEstadoscalcularestado( resultado1, resultado2, true, parte1, qnodo, ui.titulolabel->text() );
           }
 }


 ui.progressBar->setValue(50);
 QApplication::processEvents();
 //  QMessageBox::warning( this, tr("Estados Contables"),tr("AQUÍ ESTAMOS"));
 // hacemos otro barrido para calcular las funciones
int barridos=0; 
bool todoscalculados=true;

// hacer esto especial si hay desglose por mes
// solo para ejercicio1
if (ui.desglose_mes_checkBox->isChecked())
  {
    while (barridos<=20)
    {
    todoscalculados=true;
    query = basedatos::instancia()->select5Estadostituloordernodo(ui.titulolabel->text());
    if ( query.isActive() ) {
        while ( query.next() )
           {
             resultado2=0;
             parte1=query.value(0).toBool();
             qnodo=query.value(2).toString();
             qformula=query.value(3).toString();
             // lo hacemos en segunda pasada,
             // para totales de apartados con llaves
             if (qformula.contains('{')==0) continue;

             resultado1=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                      ui.fechas_ej1_groupBox->isChecked(),
                                      ui.ini_ej1_dateEdit->date(),
                                      ui.fin_ej1_dateEdit->date());
             if (!yacalculado) todoscalculados=false;
                  // queda grabar los resultados
             resultado1=redond(resultado1, 2); //---------------------
             basedatos::instancia()->updateEstadoscalcularestado( resultado1, resultado2,
                                                                  yacalculado, parte1,
                                                                  qnodo,
                                                                  ui.titulolabel->text() );


             QDate inicio;
             QDate fin;
             double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
             int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();
             inicio.setDate(anyo,1,1);
             fin.setDate(anyo,1,31);

             m1=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,1);
             if (!yacalculado) todoscalculados=false;

             inicio.setDate(anyo,2,1);
             fin.setDate(anyo,3,1);
             fin=fin.addDays(-1);
             m2=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,2);
             if (!yacalculado) todoscalculados=false;

             inicio.setDate(anyo,3,1);
             fin.setDate(anyo,3,31);
             m3=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,3);
             if (!yacalculado) todoscalculados=false;

             inicio.setDate(anyo,4,1);
             fin.setDate(anyo,4,30);
             m4=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,4);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------
             inicio.setDate(anyo,5,1);
             fin.setDate(anyo,5,31);
             m5=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,5);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------
             inicio.setDate(anyo,6,1);
             fin.setDate(anyo,6,30);
             m6=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,6);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------
             inicio.setDate(anyo,7,1);
             fin.setDate(anyo,7,31);
             m7=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,7);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------
             inicio.setDate(anyo,8,1);
             fin.setDate(anyo,8,31);
             m8=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,8);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------
             inicio.setDate(anyo,9,1);
             fin.setDate(anyo,9,30);
             m9=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,9);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------
             inicio.setDate(anyo,10,1);
             fin.setDate(anyo,10,31);
             m10=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,10);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------
             inicio.setDate(anyo,11,1);
             fin.setDate(anyo,11,30);
             m11=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,11);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------
             inicio.setDate(anyo,12,1);
             fin.setDate(anyo,12,31);
             m12=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),
                                       &yacalculado,estadosmedios,
                                        true, // acotación
                                        inicio,
                                        fin,12);
             if (!yacalculado) todoscalculados=false;
             // ------------------------------------------------------------------

                    // queda grabar los resultados
             basedatos::instancia()->actu_rdos_mes( m1, m2, m3, m4, m5, m6, m7, m8, m9, m10,
                                                    m11, m12,
                                                    yacalculado, parte1, qnodo,
                                                    ui.titulolabel->text() );
           }
    }
       barridos++;
       if (todoscalculados) break;
    }


  }

if (!ui.desglose_mes_checkBox->isChecked())
{
 while (barridos<=20)
 {
 todoscalculados=true;
 query = basedatos::instancia()->select5Estadostituloordernodo(ui.titulolabel->text());
 if ( query.isActive() ) {
     while ( query.next() ) 
        {
	    resultado2=0;
	    parte1=query.value(0).toBool();
	    qnodo=query.value(2).toString();
	    qformula=query.value(3).toString();
	    // lo hacemos en segunda pasada,
	    // para totales de apartados con llaves
	    if (qformula.contains('{')==0) continue;
        resultado1=redond(resultado1, 2); //---------------------
        resultado2=redond(resultado2, 2); //---------------------

	    resultado1=calculapartado(qformula,ui.Ejercicio1comboBox->currentText(),&yacalculado,estadosmedios,
                                     ui.fechas_ej1_groupBox->isChecked(),
                                     ui.ini_ej1_dateEdit->date(),
                                     ui.fin_ej1_dateEdit->date());
        if (!yacalculado) todoscalculados=false;
	    if (ui.Ejercicio2comboBox->currentText()!=noejercicio())
	     resultado2=calculapartado(qformula,ui.Ejercicio2comboBox->currentText(),&yacalculado,estadosmedios,
                                     ui.fechas_ej2_groupBox->isChecked(),
                                     ui.ini_ej2_dateEdit->date(),
                                     ui.fin_ej2_dateEdit->date());
        if (!yacalculado) todoscalculados=false;
                 // queda grabar los resultados
            basedatos::instancia()->updateEstadoscalcularestado( resultado1, resultado2, yacalculado, parte1, qnodo, ui.titulolabel->text() );
        }
 }
    barridos++;
    if (todoscalculados) break;
 }
}


 ui.progressBar->setValue(90);
 QApplication::processEvents();
 if (!todoscalculados)
   {
     QMessageBox::warning( this, tr("Estados Contables"),tr("ERROR en el cálculo, ¿referencia circular?"));
     desbloqueasaldosmedios();
     ui.progressBar->reset();
     return;
   }

 // Queda grabar la cabecera del estado contable

 grabacabeceraestado();
 
 // calculamos fórmula base  porcentaje
 // campo valorbasepor
 QString formulabasepor = basedatos::instancia()->selectFormulabaseporcabeceraestadostitulo(ui.titulolabel->text());
     if ( formulabasepor.length()>0 )
           {
             resultado1=calculapartado(formulabasepor,ui.Ejercicio1comboBox->currentText(),
                        &yacalculado,estadosmedios,
                        ui.fechas_ej1_groupBox->isChecked(),
                        ui.ini_ej1_dateEdit->date(),
                        ui.fin_ej1_dateEdit->date());
             if (ui.Ejercicio2comboBox->currentText()!=noejercicio())
                resultado2=calculapartado(formulabasepor,ui.Ejercicio2comboBox->currentText(),
                      &yacalculado,estadosmedios,
                      ui.fechas_ej2_groupBox->isChecked(),
                      ui.ini_ej2_dateEdit->date(),
                      ui.fin_ej2_dateEdit->date());
    
             basedatos::instancia()->updateCabeceraestadosvalorbaseportitulo(resultado1,resultado2, ui.titulolabel->text() );
            }
   desbloqueasaldosmedios();
   ui.progressBar->setValue(100);
   QApplication::processEvents();
   if (msj_calculado)
      QMessageBox::information( this, tr("Cálculo de Estado Contable"),
       tr("Se han realizado todos los cálculos para el Estado" ));
   ui.progressBar->reset();
   return;
}


double calcestado::calculapartado( QString formula, QString ejercicio, bool *calculado, bool estadosmedios,
                                   bool acotacion, QDate fecha1, QDate fecha2, int mes )
{
  //empezamos por reemplazar corchetes de cuentas
  if (formula.length()==0) return 0;
  QString cadpru;
  QString extract;
  QString cadval;
  bool solopositivo=false;
  cadpru=formula;
  // eliminamos espacios de cadpru
  cadpru.remove(' ');
  if (cadpru.contains(cadpositivo()))
     {
       solopositivo=true;
       cadpru.remove(cadpositivo());
     }
  double valor;
  int final;
  int inicio;
  int indice=0;
  while (indice<cadpru.length())
      {
         *calculado=true;
         if (cadpru[indice]=='[')
           {
              inicio=indice;
              final=inicio;
              while (final<cadpru.length() && cadpru[final]!=']') final++;
              // extraemos la cadena de la cuenta sin corchetes
              extract=cadpru.mid(inicio+1,final-inicio-1);
              // habría que calcular el valor de la cuenta
              // QMessageBox::warning( this, tr("Estados Contables"),extract);
              // -------------------------------------------------------------------------------------
              if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotacion,fecha1,fecha2);
               else
                if (extract.contains(';')==1) valor=calculacuentaci(extract,ejercicio,acotacion,fecha1,fecha2);
                else
	            {
                      if (!estadosmedios)
                         {
                          if (mes==0)
                            {
                             if (ejercicio==ui.Ejercicio1comboBox->currentText())
                               valor=saldo_cuenta_lista_ej1(extract);
                             else
                               valor=saldo_cuenta_lista_ej2(extract);
                            }
                            else valor=saldo_cuenta_lista_mes(extract, mes);
                         }
	                 else
	                     {
	                       if (ui.Ejercicio1comboBox->currentText()==ejercicio)
                             valor=calculamediacuenta(extract,true);
                           else
                             valor=calculamediacuenta(extract,false);
	                     }
	            }
	     if (valor>-0.004 && valor < 0.005) valor=0;
             cadval.setNum(valor,'f',2);
             if (valor<0 && inicio>=1 && cadpru[inicio-1]=='+' ) inicio--;
             if (valor<0 && inicio>=1 && cadpru[inicio-1]=='-' )
               {cadpru[inicio-1]='+'; cadval.setNum(valor*-1,'f',2); }
             cadpru.remove(inicio,final-inicio+1);
             cadpru.insert(inicio,cadval);
             continue;
           }
      if (cadpru[indice]=='{')
           {
              inicio=indice;
              final=inicio;
              while (final<cadpru.length() && cadpru[final]!='}') final++;
              // extraemos la cadena de la cuenta sin llaves
              extract=cadpru.mid(inicio+1,final-inicio-1);
              // habría que calcular el valor de la fórmula
              // QMessageBox::warning( this, tr("Estados Contables"),extract);
              int ejercicio1=0;
              if (ejercicio==ui.Ejercicio1comboBox->currentText()) ejercicio1=1;
               // QMessageBox::warning( this, tr("Estados Contables"),ejercicio+extract);
              valor=calculaformula(extract,ejercicio1,calculado,mes);
               // QMessageBox::warning( this, tr("Estados Contables"),ejercicio);
              if (!*calculado) return 0;
              cadval.setNum(valor,'f',2);
               if (valor<0 && inicio>=1 && cadpru[inicio-1]=='+' ) inicio--;
               if (valor<0 && inicio>=1 && cadpru[inicio-1]=='-' ) 
	        {
	         cadpru[inicio-1]='+';
	         cadval.setNum(valor*-1,'f',2);
	        }
              cadpru.remove(inicio,final-inicio+1);
              cadpru.insert(inicio,cadval);
              continue;
           }
       indice++;
      }
  int okeval;
  exparitmetica expresion;
  if (cadpru[0]=='+') { cadpru[0]=' '; cadpru.remove(' ');}
  if (cadpru[0]=='-' && cadpru[1]=='(') cadpru=cadpru.insert(1,"1*");
  cadpru.left(-1).replace("(+","(");
  // QMessageBox::warning( this, tr("Estados Contables"),cadpru);
  expresion.inicializa(cadpru);
  valor=expresion.resultado(&okeval);
  if (solopositivo) if (valor<0) valor=0;
  if (!okeval) valor=0;
  valor=redond(valor, 2); //---------------------
 cadval.setNum(valor,'f',2);
  // QMessageBox::warning( this, tr("Estados Contables"),cadval);
  // QMessageBox::warning( this, tr("Estados Contables"),/*formula*/cadval+" "+ cadpru);
 return valor; // ojo hay que cambiar
}

void calcestado::cargarinfo()
{
    // cargamos samadebe y sumahaber
    // ejercicio1
    auxiliares.clear();
    saldos.clear();
    auxiliares2.clear();
    saldos2.clear();

    QDate fechaini,fechafin;

    fechaini = basedatos::instancia()->selectAperturaejercicios(ui.Ejercicio1comboBox->currentText());
    fechafin = basedatos::instancia()->selectCierreejercicios(ui.Ejercicio1comboBox->currentText());

    if (ui.fechas_ej1_groupBox->isChecked())
       {
        fechaini=ui.ini_ej1_dateEdit->date();
        fechafin=ui.fin_ej1_dateEdit->date();
       }

    QString subcadena="";
    // ESPECIFICAMOS DIARIOS SEGÚN CONTENIDO DE LA LISTA
    int numdiarios=ui.diariolistWidget->count();
    QString primer_diario;
    for (int veces=0;veces<numdiarios;veces++)
       {
        if (subcadena=="") subcadena=subcadena+" and (";
          else subcadena=subcadena+" or ";
        subcadena+="diario='";
        ui.diariolistWidget->setCurrentRow(veces);
        QString caddia=ui.diariolistWidget->currentItem()->text();
        if (veces==0) primer_diario=ui.diariolistWidget->currentItem()->text();
        if (caddia!=diario_no_asignado())
           subcadena+=caddia;
        subcadena+="'";
       }
    if (subcadena.length()>0) subcadena+=")";

    QSqlQuery q;
    if (!ui.analiticagroupBox->isEnabled())
       {
        if (numdiarios==1 && primer_diario==diario_apertura())
            q=basedatos::instancia()->cuentas_saldo_estados_apertura(fechaini, fechafin, subcadena, "");
          else
            q=basedatos::instancia()->cuentas_saldo_estados (fechaini, fechafin, subcadena, "");
       }
       else
           {
             if (!hay_analitica_tabla) q=basedatos::instancia()->cuentas_saldo_estados (fechaini, fechafin, subcadena, ui.cilineEdit->text());
               else q=basedatos::instancia()->cuentas_saldo_estados_ci (fechaini, fechafin, subcadena, ui.cilineEdit->text());
           }

    if (q.isActive())
        while (q.next())
          {
            auxiliares << q.value(0).toString();
            saldos << q.value(1).toDouble();
          }

    // ejercicio2
   if (ui.Ejercicio2comboBox->currentText()!=noejercicio())
    {
    // cargamos listas de segundo ejercicio
    fechaini = basedatos::instancia()->selectAperturaejercicios(ui.Ejercicio2comboBox->currentText());
    fechafin = basedatos::instancia()->selectCierreejercicios(ui.Ejercicio2comboBox->currentText());

    if (ui.fechas_ej2_groupBox->isChecked())
       {
        fechaini=ui.ini_ej2_dateEdit->date();
        fechafin=ui.fin_ej2_dateEdit->date();
       }

    if (!ui.analiticagroupBox->isEnabled())
       {
        q=basedatos::instancia()->cuentas_saldo_estados (fechaini, fechafin, subcadena, "");
       }
       else
           {
             if (!hay_analitica_tabla) q=basedatos::instancia()->cuentas_saldo_estados (fechaini, fechafin, subcadena, ui.cilineEdit->text());
               else q=basedatos::instancia()->cuentas_saldo_estados_ci (fechaini, fechafin, subcadena, ui.cilineEdit->text());
           }

    if (q.isActive())
        while (q.next())
          {
            auxiliares2 << q.value(0).toString();
            saldos2 << q.value(1).toDouble();
          }
   } // fin ejercicio 2

    // cargamos saldos mensuales si procede
    if (!ui.desglose_mes_checkBox->isChecked()) return;

    // auxm1.clear(); auxm2.clear(); auxm3.clear(); auxm4.clear(); auxm5.clear(); auxm6.clear(); auxm7.clear();
    // auxm8.clear(); auxm9.clear(); auxm10.clear(); auxm11.clear(); auxm12.clear();

    // saldosm1.clear(); saldosm2.clear(); saldosm3.clear(); saldosm4.clear(); saldosm5.clear(); saldosm6.clear();
    //saldosm7.clear(); saldosm8.clear(); saldosm9.clear(); saldosm10.clear(); saldosm11.clear(); saldosm12.clear();
    for (int veces=0;veces<12;veces++)
       {
        auxm[veces].clear();
        saldosm[veces].clear();
       }
    int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();
    fechaini.setDate(anyo,1,1);
    fechafin.setDate(anyo,1,31);

    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[0] << q.value(0).toString();
            saldosm[0] << q.value(1).toDouble();
          }

    fechaini.setDate(anyo,2,1);
    fechafin.setDate(anyo,3,1);
    fechafin=fechafin.addDays(-1);

    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[1] << q.value(0).toString();
            saldosm[1] << q.value(1).toDouble();
            //QMessageBox::warning( this, tr("Estados Contables"),q.value(0).toString() + " "+ q.value(1).toString()+" ");
          }
    //QMessageBox::warning(this,tr("estados"),auxm[1].at(2));

    fechaini.setDate(anyo,3,1);
    fechafin.setDate(anyo,3,31);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[2] << q.value(0).toString();
            saldosm[2] << q.value(1).toDouble();
          }

    fechaini.setDate(anyo,4,1);
    fechafin.setDate(anyo,4,30);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[3] << q.value(0).toString();
            saldosm[3] << q.value(1).toDouble();
          }
    // ------------------------------------------------------------------
    fechaini.setDate(anyo,5,1);
    fechafin.setDate(anyo,5,31);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[4] << q.value(0).toString();
            saldosm[4] << q.value(1).toDouble();
          }
    // ------------------------------------------------------------------
    fechaini.setDate(anyo,6,1);
    fechafin.setDate(anyo,6,30);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[5] << q.value(0).toString();
            saldosm[5] << q.value(1).toDouble();
          }
    // ------------------------------------------------------------------
    fechaini.setDate(anyo,7,1);
    fechafin.setDate(anyo,7,31);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[6] << q.value(0).toString();
            saldosm[6] << q.value(1).toDouble();
          }
    // ------------------------------------------------------------------
    fechaini.setDate(anyo,8,1);
    fechafin.setDate(anyo,8,31);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[7] << q.value(0).toString();
            saldosm[7] << q.value(1).toDouble();
          }
    // ------------------------------------------------------------------
    fechaini.setDate(anyo,9,1);
    fechafin.setDate(anyo,9,30);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[8] << q.value(0).toString();
            saldosm[8] << q.value(1).toDouble();
          }
    // ------------------------------------------------------------------
    fechaini.setDate(anyo,10,1);
    fechafin.setDate(anyo,10,31);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[9] << q.value(0).toString();
            saldosm[9] << q.value(1).toDouble();
          }
    // ------------------------------------------------------------------
    fechaini.setDate(anyo,11,1);
    fechafin.setDate(anyo,11,30);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[10] << q.value(0).toString();
            saldosm[10] << q.value(1).toDouble();
          }
    // ------------------------------------------------------------------
    fechaini.setDate(anyo,12,1);
    fechafin.setDate(anyo,12,31);
    q=consulta_aux_saldo(fechaini, fechafin, subcadena);

    if (q.isActive())
        while (q.next())
          {
            auxm[11] << q.value(0).toString();
            saldosm[11] << q.value(1).toDouble();
          }

}


QSqlQuery calcestado::consulta_aux_saldo(QDate fechaini, QDate fechafin, QString subcadena)
{
    QSqlQuery q;
    if (!ui.analiticagroupBox->isEnabled())
       {
        q=basedatos::instancia()->cuentas_saldo_estados (fechaini, fechafin, subcadena, "");
       }
       else
           {
             if (!hay_analitica_tabla) q=basedatos::instancia()->cuentas_saldo_estados (fechaini, fechafin, subcadena, ui.cilineEdit->text());
               else q=basedatos::instancia()->cuentas_saldo_estados_ci (fechaini, fechafin, subcadena, ui.cilineEdit->text());
           }
     return q;
}

double calcestado::saldo_cuenta_lista_mes(QString codigo, int mes)
{
    bool positivo=false;
    bool negativo=false;
    positivo=codigo.contains('+');
    negativo=codigo.contains('-');
    if (positivo && negativo) return 0;
    if (positivo) codigo.remove('+');
    if (negativo) codigo.remove('-');
    double resultado=0;
    for (int veces=0; veces<auxm[mes-1].count(); veces++)
        {
         if (auxm[mes-1].at(veces).startsWith(codigo))
            {
             if (!positivo && !negativo) resultado+=saldosm[mes-1].at(veces);
             if (positivo && saldosm[mes-1].at(veces)>0) resultado+=saldosm[mes-1].at(veces);
             if (negativo && saldosm[mes-1].at(veces)<0) resultado+=saldosm[mes-1].at(veces);
            }
        }
    return resultado;

}

double calcestado::saldo_cuenta_lista_ej1(QString codigo)
{
    bool positivo=false;
    bool negativo=false;
    positivo=codigo.contains('+');
    negativo=codigo.contains('-');
    if (positivo && negativo) return 0;
    if (positivo) codigo.remove('+');
    if (negativo) codigo.remove('-');
    double resultado=0;
    for (int veces=0; veces<auxiliares.count(); veces++)
        {
         if (auxiliares.at(veces).startsWith(codigo))
            {
             if (!positivo && !negativo) resultado+=saldos.at(veces);
             if (positivo && saldos.at(veces)>0) resultado+=saldos.at(veces);
             if (negativo && saldos.at(veces)<0) resultado+=saldos.at(veces);
            }
        }
    return resultado;
}

double calcestado::saldo_cuenta_lista_ej2(QString codigo)
{
    bool positivo=false;
    bool negativo=false;
    positivo=codigo.contains('+');
    negativo=codigo.contains('-');
    if (positivo && negativo) return 0;
    if (positivo) codigo.remove('+');
    if (negativo) codigo.remove('-');
    double resultado=0;
    for (int veces=0; veces<auxiliares2.count(); veces++)
        {
         if (auxiliares2.at(veces).startsWith(codigo))
            {
             if (!positivo && !negativo) resultado+=saldos2.at(veces);
             if (positivo && saldos2.at(veces)>0) resultado+=saldos2.at(veces);
             if (negativo && saldos2.at(veces)<0) resultado+=saldos2.at(veces);
            }
        }
    return resultado;

}

double calcestado::calculacuenta(QString codigo, QString ejercicio, bool acotacion,
                                 QDate fecha1, QDate fecha2, QString const & ciAdd)
{
  bool positivo=false;
  bool negativo=false;
  positivo=codigo.contains('+');
  negativo=codigo.contains('-');
  if (positivo && negativo) return 0;
  if (positivo) codigo.remove('+');
  if (negativo) codigo.remove('-');
  if (ui.previsiones->isChecked())
     {
      double valor=prevision(codigo,ejercicio);
      if (positivo && valor<0) return 0;
      if (negativo && valor>0) return 0;
      return valor;
     }
  if (!ui.diariolistWidget->isEnabled() ) return 0;
  QDate fechaini,fechafin;

  fechaini = basedatos::instancia()->selectAperturaejercicios(ejercicio);
  fechafin = basedatos::instancia()->selectCierreejercicios(ejercicio);

  if (acotacion)
     {
      fechaini=fecha1;
      fechafin=fecha2;
     }

  double resultado=0;

  QString subcadena="";
  // ESPECIFICAMOS DIARIOS SEGÚN CONTENIDO DE LA LISTA
  int numdiarios=ui.diariolistWidget->count();
  for (int veces=0;veces<numdiarios;veces++)
     {
      if (subcadena=="") subcadena=subcadena+" and (";
        else subcadena=subcadena+" or ";
      subcadena+="diario='";
      ui.diariolistWidget->setCurrentRow(veces);
      QString caddia=ui.diariolistWidget->currentItem()->text();
      if (caddia!=diario_no_asignado())
         subcadena+=caddia;
      subcadena+="'";
     }
  if (subcadena.length()>0) subcadena+=")";
  //jsala
  // subcadena += ciWhere;
  //jsala
  // vemos donde puede casar ciAdd
  QString pasarci;
  if (!ciAdd.isEmpty()) pasarci=ciAdd;
     else pasarci=ui.cilineEdit->text();
  QSqlQuery query;
  if (!ui.analiticagroupBox->isEnabled())
  // if (!hay_analitica_tabla || (hay_analitica_tabla && !es_cuenta_para_analitica(codigo)))
      query = basedatos::instancia()->selectCuentadebehaberdiariocuentafechasgroupcuenta(codigo,
                                       fechaini, fechafin, subcadena, pasarci );
   else
     {
       if (!hay_analitica_tabla)
           query = basedatos::instancia()->selectCuentadebehaberdiariocuentafechasgroupcuenta(codigo,
                                            fechaini, fechafin, subcadena, pasarci );
         else
          query = basedatos::instancia()->debehaber_ci(codigo, fechaini, fechafin, subcadena,
                                                   pasarci );
     }

  if ( query.isActive() ) {
     while ( query.next() ) 
           {
            if (positivo || negativo)
               {
                if (positivo && query.value(1).toDouble()>0) resultado+=query.value(1).toDouble();
                if (negativo && query.value(1).toDouble()<0) resultado+=query.value(1).toDouble();
               }
               else
	           resultado+=query.value(1).toDouble();
           }
     }
  return resultado;
}




double calcestado::calculaformula( QString codigo, int ejercicio1,bool *calculado, int mes=0)
{
  // tenemos un código de nodo con indicación de parte del estado
 // 1 ó 2 entre paréntesis
 // intentamos conocer la parte del estado (1 derecha, 2 izquierda)
    int pos;
    int izquierdo;
    QString extract="";
    *calculado=0;
    if (codigo.indexOf('(')<0) return 0;
    pos=codigo.indexOf('(');
    if (pos+1>=codigo.length()) return 0;
    if (codigo[pos+1]=='1') izquierdo=1; 
       else { if (codigo[pos+1]=='2') izquierdo=0; else return 0;   }
   // aislamos código de nodo
    extract=codigo.left(pos);
    // consultamos estados en busca del nodo extract 

 QSqlQuery query;
 if (mes==0)
  {
   if (ejercicio1)
    query= basedatos::instancia()->selectImporte1calculadonodoparte1titulo(extract,
        izquierdo,ui.titulolabel->text());
   else
    query=basedatos::instancia()->selectImporte2calculadonodoparte1titulo(extract,izquierdo,
        ui.titulolabel->text());
  }
  else
      {
       // total de mes X de ejercicio1
      query= basedatos::instancia()->selectImporte1calculadonodoparte1titulo(extract,
          izquierdo,ui.titulolabel->text(),mes);
      }

 double resultado=0;
 if ( query.isActive() ) {
     while ( query.next() ) 
           {
	    resultado=query.value(0).toDouble();
	    *calculado=query.value(1).toBool();
           }
    }
 return resultado;
}


void calcestado::grabacabeceraestado()
{
 QDate fechaactual;
 QString cadena;
 QSqlQuery query;
 fechaactual=QDate::currentDate();
 ui.FechadateEdit->setDate(fechaactual);
 QTime hora=QTime::currentTime();
 QString cadhora=hora.toString("hh:mm:ss");
 ui.horalineEdit->setText(cadhora);
 basedatos::instancia()->updateCabeceraestadosfechacalculoejercicio1ciejercicio2titulo( fechaactual, ui.Ejercicio1comboBox->currentText() , ui.cilineEdit->text() , ui.Ejercicio2comboBox->currentText() , ui.titulolabel->text() );

}


double calcestado::calculamediacuenta(QString codigo,bool ejercicio1)
{
  bool positivo=false;
  bool negativo=false;
  positivo=codigo.contains('+');
  negativo=codigo.contains('-');
  if (positivo && negativo) return 0;
  if (positivo) codigo.remove('+');
  if (negativo) codigo.remove('-');

    return basedatos::instancia()->selectSumsaldomediosaldossubcuentacodigo(ejercicio1,codigo,positivo,negativo);
}

double calcestado::cuentaespec( QString extract, QString ejercicio, bool acotacion, QDate fecha1, QDate fecha2 )
{
   double resultado=0;
  // separamos código de cuenta y especificador
   QString codigo=extract.section(':',0,0);
   QString espec=extract.section(':',1,1);
   if (espec==cadenamedia()) 
      {
        if (ui.Ejercicio1comboBox->currentText()==ejercicio)  resultado=calculamediacuenta(codigo,true);
	else resultado=calculamediacuenta(codigo,false);
      }
      else
          {
           resultado=calculacuentadiario(codigo,ejercicio,espec,acotacion,fecha1,fecha2);
          }
   return resultado;
}


double calcestado::calculacuentadiario( QString codigo, QString ejercicio, QString qdiario,
                                        bool acotacion,
                                        QDate fecha1, QDate fecha2 )
{
    double resultado=0;
    bool positivo=false;
    bool negativo=false;
    positivo=codigo.contains('+');
    negativo=codigo.contains('-');
    if (positivo && negativo) return 0;
    if (positivo) codigo.remove('+');
    if (negativo) codigo.remove('-');
    if (qdiario==previsiones())
    {
        if (positivo) return previsionsegunsaldo(codigo,ejercicio,true);
        if (negativo) return previsionsegunsaldo(codigo,ejercicio,false);
        return prevision(codigo,ejercicio);;
    }
    if (!ui.diariolistWidget->isEnabled() ) return 0;
    QDate fechaini,fechafin;

    fechaini = basedatos::instancia()->selectAperturaejercicios(ejercicio);
    fechafin = basedatos::instancia()->selectCierreejercicios(ejercicio);

    if (acotacion)
       {
        fechaini=fecha1;
        fechafin=fecha2;
       }

    QString subcadena = " and diario='";
    if (qdiario != diario_no_asignado())
    subcadena += qdiario.left(-1).replace("'","''");
    subcadena += "' ";

    QSqlQuery query = basedatos::instancia()->selectCuentadebehaberdiariocuentafechasgroupcuenta(codigo, fechaini, fechafin, subcadena, "");

    resultado = 0;
    if ( query.isActive() ) {
        while ( query.next() ) 
        {
            if (positivo || negativo)
            {
                if (positivo && query.value(1).toDouble()>0) resultado += query.value(1).toDouble();
                if (negativo && query.value(1).toDouble()<0) resultado += query.value(1).toDouble();
            }
            else
	            resultado += query.value(1).toDouble();
        }
    }
    return resultado;
}


void calcestado::generalatex()
{
    if (ui.desglose_mes_checkBox->isChecked())
       { generalatex_meses(); return;}
    if (condesglose || condesglosectas) { generalatex2(); return; }
    QString ejercicio1="";
    QString ejercicio2="";
    QString parte1="";
    QString parte2="";
    QString formulabasepor;
    QString observaciones;
    QString cabecera;
    double valorbasepor1=0;
    double valorbasepor2=0;
    // bool estadosmedios=false;
    bool analitica,haycolref;
    QString ci,colref;
    QSqlQuery query = basedatos::instancia()->select14Cabeceraestadostitulo( ui.titulolabel->text() );

    if ( (query.isActive()) && (query.first()) ) {
       parte1=query.value(0).toString();
	   parte2=query.value(1).toString();
	   ejercicio1=query.value(2).toString();
	   ejercicio2=query.value(3).toString();
       //estadosmedios=query.value(4).toBool();
	   formulabasepor=query.value(5).toString();
	   valorbasepor1=query.value(6).toDouble();
	   valorbasepor2=query.value(7).toDouble();
	   observaciones=query.value(8).toString();
           analitica=query.value(9).toBool();
           ci=query.value(10).toString();
           haycolref=query.value(11).toBool();
           colref=query.value(12).toString();
           cabecera=query.value(13).toString();
       } else return;
    bool hayporcentajes=false;
    double porcentaje=0;
    QString cadpor;
    if (formulabasepor.length()>0 && (valorbasepor1>0.001 || valorbasepor1<0.001)
    && (valorbasepor2>0.001 || valorbasepor2<0.001)) hayporcentajes=true;
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   if (ui.texlineEdit->text().length()==0)
      qfichero=qfichero+tr("estadocontable.tex");
   else
         {
            qfichero=qfichero+ui.texlineEdit->text();
            if (ui.texlineEdit->text().right(4)!=".tex") qfichero=qfichero+".tex";
         }
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

    // Generamos primera parte del Estado Contable
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    if (ui.previsiones->isChecked())
      {
       stream << "\\begin{center}" << "\n";
       stream << "{\\Large \\textbf {";
       stream << " ** PREVISIONES ** }}";
       stream << "\\end{center}";
      }

    if (analitica && ci.length()>0)
        {
         stream << "\\begin{center}" << "\n";
         stream << "{\\normalsize \\textbf {" << tr("CÓDIGO DE IMPUTACIÓN: ") << ci <<  "}}" << "\n";
         stream << "\\end{center}" << "\n";

         QString codigo=ci;
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

    if (ui.fechas_ej1_groupBox->isChecked())
       {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Ejercicio: ") << filtracad(ui.Ejercicio1comboBox->currentText()) << tr(" - FECHAS: ");
        stream << ui.ini_ej1_dateEdit->date().toString("dd/MM/yyyy") << tr(" a ");
        stream << ui.fin_ej1_dateEdit->date().toString("dd/MM/yyyy");
        stream << "}}";
        stream << "\\end{center}";
       }

    if (ui.fechas_ej2_groupBox->isEnabled() && ui.fechas_ej2_groupBox->isChecked())
       {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Ejercicio: ") << filtracad(ui.Ejercicio2comboBox->currentText()) << tr(" - FECHAS: ");
        stream << ui.ini_ej2_dateEdit->date().toString("dd/MM/yyyy") << tr(" a ");
        stream << ui.fin_ej2_dateEdit->date().toString("dd/MM/yyyy");
        stream << "}}";
        stream << "\\end{center}";
       }

    stream << tr("\\begin{center}") << "\n";
    if (ejercicio2.length()>0)
       {
          if (hayporcentajes)
             {
              if (haycolref) stream << "\\begin{longtable}{|p{8cm}|p{1.3cm}|r|r|r|r|}" << "\n";
              else stream << "\\begin{longtable}{|p{10cm}|r|r|r|r|}" << "\n";
             }
	  else
               {
                 if (haycolref) stream << "\\begin{longtable}{|p{11cm}|p{1.3cm}|r|r|}" << "\n";
                  else stream << "\\begin{longtable}{|p{12cm}|r|r|}" << "\n";
               }
       }
    else 
           {
	     if (!hayporcentajes) 
               {
                if (haycolref) stream << "\\begin{longtable}{|p{10cm}|p{1.3cm}|r|}" << "\n";
                 else stream << "\\begin{longtable}{|p{12cm}|r|}" << "\n";
               }
	       else
                {
	         if (haycolref) stream << "\\begin{longtable}{|p{10cm}|p{1.3cm}|r|r|}" << "\n";
                  else stream << "\\begin{longtable}{|p{12cm}|r|r|}" << "\n";
                }
           }
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
   stream << "\\multicolumn{";
   if (ejercicio2.length()>0) 
      { 
        if(hayporcentajes) 
           {
            if (haycolref) stream <<  tr("6");
            else stream <<  tr("5");
           }
        else 
           { 
            if (haycolref) stream <<  tr("4");
            else stream <<  tr("3");
           }
      }
   else
        { 
          if (hayporcentajes)
            { 
             if (haycolref) stream << tr("4");
             else stream <<  tr("3"); 
            }
           else 
             {
              if (haycolref) stream <<  tr("3") ;
              else stream <<  tr("2") ;
             }
         }

    stream << "}{|c|} {\\textbf{ ";
    stream << cabecera <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // ---------------------------------------------------------------------------------------
    stream << "\\rowcolor{gray!30}\n";
    stream << "{\\textbf{" << parte1 << "}} & ";
    if (haycolref)
        stream << "{\\scriptsize{ " << colref << "}} & ";
    if (hayporcentajes) stream << "{\\textbf{ \\% }} & ";
    stream << "{\\textbf {" << filtracad(ejercicio1) <<"}";
    // ----------------------------------------------------------------------------------------
    if (ejercicio2.length()>0)
      {
         stream << "} & ";
         if (hayporcentajes) stream << "{\\textbf{ \\% }} & ";
         stream <<  "{\\textbf {" << filtracad(ejercicio2)  <<"}";
      }
    stream << "} \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // -------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    stream << "{\\textbf{ " << filtracad(parte1) << "}} & ";
    if (haycolref)
        stream << "{\\scriptsize{ " << filtracad(colref) << "}} & ";
    if (hayporcentajes) stream << "{\\textbf{ \\% }} & ";
    stream << "{\\textbf{ " << filtracad(ejercicio1) << "}";
    if (ejercicio2.length()>0)
       {
         stream <<  "} & ";
         if (hayporcentajes) stream << "{\\textbf{ \\% }} & ";
         stream << " {\\textbf{ " << filtracad(ejercicio2) << "}" ;
       }
    stream << "} \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    stream << "\\hline" << "\n";
    
    QSqlQuery query2 = basedatos::instancia()->select5Estadostituloparte1ordernodo( ui.titulolabel->text() , true );
    int veces;
    if ( query2.isActive() ) {
             while ( query2.next() ) {
	       if (query2.value(0).toString().contains("LINEA")>0)
		 {
		   stream << "\\hline\n";
		   continue;
	         }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001 
                    && query2.value(2).toDouble()>-0.0001
                    && ejercicio2.length()==0) continue;
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001 
                    && query2.value(2).toDouble()>-0.0001
                    && query2.value(3).toDouble()<0.0001 
                    && query2.value(3).toDouble()>-0.0001
                    && ejercicio2.length()>0) continue;
               // ----------------------------------------------------------------------
	       for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
		   stream << "\\hspace{0.5cm} ";
	       // if (query2.value(0).toString().contains("TOTAL")==0) 
	      //	   stream << query2.value(0).toString() << " " ;
	       stream << "{\\scriptsize {" << filtracad(query2.value(1).toString()) << "}} & ";
               if (haycolref) 
                  stream << "{\\scriptsize {" << filtracad(query2.value(4).toString()) << "}} & ";
	       if (hayporcentajes)
	                 {
   	                   if (query2.value(2).toDouble()>0.001 ||
                                query2.value(2).toDouble()<-0.001)
		              {
	                          porcentaje=query2.value(2).toDouble()/valorbasepor1*100;
		                  stream << "{\\scriptsize {" << 
                                     formateanumerosep(porcentaje,comadecimal,decimales) << "}}";
	                      }
                                   stream << " & ";
	                 }
                       //---------------------------------------------
	       if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
	             stream << "{\\scriptsize {" << 
                        formateanumerosep(query2.value(2).toDouble(),comadecimal,decimales) << "}}";
	       if (ejercicio2.length()>0) 
	            {
		     stream << " & ";
		     if (hayporcentajes)
		      {
 		         if (query2.value(3).toDouble()>0.001 || query2.value(3).toDouble()<-0.001)
		              {
	                         porcentaje=query2.value(3).toDouble()/valorbasepor2*100;
		                 cadpor.setNum(porcentaje,'f',2);
		                 stream << "{\\scriptsize {" << 
                                    formateanumerosep(cadpor.toDouble(),comadecimal,decimales) << "}}";
		              }
                         stream << " & ";
		      }
		   if (query2.value(3).toDouble()>0.001 || query2.value(3).toDouble()<-0.001)
		       stream << "{\\scriptsize {" << 
                         formateanumerosep(query2.value(3).toDouble(),comadecimal,decimales) << "}}";
	            }
	       stream << " \\\\" << "\n";
	     }
           }
    stream << "\\hline" << "\n";
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";
 if (parte2.length()>0)
    {
    stream << "\\newpage" << "\n";
    
    // Generamos segunda parte
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    if (ui.previsiones->isChecked())
      {
       stream << "\\begin{center}" << "\n";
       stream << "{\\Large \\textbf {";
       stream << " ** PREVISIONES ** }}";
       stream << "\\end{center}";
      }

    stream << "\\begin{center}"  << "\n";  // vamos por aquí

    if (ejercicio2.length()>0)
       {
          if (hayporcentajes)
             {
              if (haycolref) stream << "\\begin{longtable}{|p{8cm}|p{1.3cm}|r|r|r|r|}" << "\n";
              else stream << "\\begin{longtable}{|p{10cm}|r|r|r|r|}" << "\n";
             }
	  else
               {
                 if (haycolref) stream << "\\begin{longtable}{|p{11cm}|p{1.3cm}|r|r|}" << "\n";
                  else stream << "\\begin{longtable}{|p{12cm}|r|r|}" << "\n";
               }
       }
    else 
           {
	     if (!hayporcentajes) 
               {
                if (haycolref) stream << "\\begin{longtable}{|p{10cm}|p{1.3cm}|r|}" << "\n";
                 else stream << "\\begin{longtable}{|p{12cm}|r|}" << "\n";
               }
	       else
                {
	         if (haycolref) stream << "\\begin{longtable}{|p{10cm}|p{1.3cm}|r|r|}" << "\n";
                  else stream << "\\begin{longtable}{|p{12cm}|r|r|}" << "\n";
                }
           }


// -------------------------------------------------------------------------------------------------------    
   stream << "\\hline" << "\n";
   stream << "\\rowcolor{gray!30}\n";
   stream << "\\multicolumn{";
   if (ejercicio2.length()>0) 
      { 
        if(hayporcentajes) 
           {
            if (haycolref) stream <<  tr("6");
            else stream <<  tr("5");
           }
        else 
           { 
            if (haycolref) stream <<  tr("4");
            else stream <<  tr("3");
           }
      }
   else
        { 
          if (hayporcentajes)
            { 
             if (haycolref) stream << tr("4");
             else stream <<  tr("3"); 
            }
           else 
             {
              if (haycolref) stream <<  tr("3") ;
              else stream <<  tr("2") ;
             }
         }


    stream << "}{|c|} {\\textbf{";
    stream << cabecera <<  "}} \\\\";
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    // -----------------------------------------------------------------------------------------------------
    stream << "{\\textbf{" << parte2 << "}} & ";
    if (haycolref)
        stream << "{\\scriptsize{ " << colref << "}} & ";
    if (hayporcentajes) stream << "{\\textbf{ \\% }} & ";
    stream << "{\\textbf {" << ejercicio1 <<"}";
    // -----------------------------------------------------------------------------------------------------
    if (ejercicio2.length()>0)
      {
         stream << "} & ";
         if (hayporcentajes) stream << "{\\textbf{ \\% }} & ";
         stream <<  "{\\textbf {" << ejercicio2 << "}";
      }
        
    stream << "} \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    stream << "\\hline" << "\n";
    
    // ------------------------------------------------------------------------------------------------------
    stream << "{\\textbf{" << parte2 << "}} & ";
    if (haycolref)
        stream << "{\\scriptsize{ " << colref << "}} & ";
    if (hayporcentajes) stream << "{\\textbf{ \\% }} & ";
    stream << "{\\textbf {" << ejercicio1 << "}";
    if (ejercicio2.length()>0)
      {
         stream << "} & ";
         if (hayporcentajes) stream << "{\\textbf{ \\% }} & ";
	 stream <<  "{\\textbf {" << ejercicio2 << "}";
      }
    stream << "} \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    stream << "\\hline" << "\n";
    
    query2 = basedatos::instancia()->select5Estadostituloparte1ordernodo( ui.titulolabel->text() , false );
    if ( query2.isActive() ) {
             while ( query2.next() ) {
	       if (query2.value(0).toString().contains("LINEA")>0)
		 {
		   stream << "\\hline\n";
		   continue;
	         }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001 
                    && query2.value(2).toDouble()>-0.0001
                    && ejercicio2.length()==0) continue;
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001 
                    && query2.value(2).toDouble()>-0.0001
                    && query2.value(3).toDouble()<0.0001 
                    && query2.value(3).toDouble()>-0.0001
                    && ejercicio2.length()>0) continue;
               // ----------------------------------------------------------------------
	       for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
		   stream << "\\hspace{0.5cm} ";
	       // if (query2.value(0).toString().contains("TOTAL")==0) 
		//   stream << query2.value(0).toString() << " " ;
	       stream << "{\\scriptsize {" << filtracad(query2.value(1).toString()) << "}} & ";
               if (haycolref) 
                  stream << "{\\scriptsize {" << filtracad(query2.value(4).toString()) << "}} & ";
	       if (hayporcentajes)
	                 {
   	                   if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
		                {
	                          porcentaje=query2.value(2).toDouble()/valorbasepor1*100;
		                  stream << "{\\scriptsize {" << 
                                     formateanumero(porcentaje,comadecimal,decimales) << "}}";
	                        }
                           stream << " & ";
	                 }
	       if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
		   stream << "{\\scriptsize {" <<
                      formateanumerosep(query2.value(2).toDouble(),comadecimal,decimales) << "}}";
	       if (ejercicio2.length()>0) 
	            {
		     stream << " & ";
		     if (hayporcentajes)
		       {
 		         if (query2.value(3).toDouble()>0.001 || query2.value(3).toDouble()<-0.001)
		              {
	                         porcentaje=query2.value(3).toDouble()/valorbasepor2*100;
		                 stream << "{\\scriptsize {" << 
                                   formateanumerosep(porcentaje,comadecimal,decimales) << "}}";
		              }
                         stream << " & ";
		       }
		     if (query2.value(3).toDouble()>0.001 || query2.value(3).toDouble()<-0.001)
		       stream << "{\\scriptsize {" << 
                          formateanumerosep(query2.value(3).toDouble(),comadecimal,decimales) << "}}";
	            }
	       stream << " \\\\" << "\n";
	     }
           }
    stream << "\\hline" << "\n";
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";
}
    stream << observaciones << "\n";
    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";
    
    fichero.close();
    
}


void calcestado::consulta( void )
{
    QStringList textolineas1, textolineas2;
    QStringList notas1, notas2;
    QStringList porcentajes1_1, porcentajes2_1;
    QStringList porcentajes1_2, porcentajes2_2;
    QStringList importes1_1, importes2_1;
    QStringList importes1_2, importes2_2;
    QStringList importes_detalle1, importes_detalle2;
    QStringList importes_cuentas1, importes_cuentas2;
    QString r_ejercicio1;
    QString r_ejercicio2;
    QString r_parte1;
    QString r_parte2;

    bool no_segundo_ejercicio;


    if (auxiliares.count()==0)
       if (condesglose || condesglosectas) { calculaestado(); }
    textolineas1.clear();
    textolineas2.clear();
    notas1.clear();
    notas2.clear();
    porcentajes1_1.clear();
    porcentajes2_1.clear();
    porcentajes1_2.clear();
    porcentajes2_2.clear();
    importes1_1.clear();
    importes2_1.clear();
    importes1_2.clear();
    importes2_2.clear();



    QString formulabasepor;
    QString observaciones;
    QString cabecera;
    double valorbasepor1=0;
    double valorbasepor2=0;
    // bool estadosmedios=false;
    bool analitica,haycolref;
    QString ci,colref;
    QSqlQuery query = basedatos::instancia()->select14Cabeceraestadostitulo( ui.titulolabel->text() );

    if ( (query.isActive()) && (query.first()) ) {
           r_parte1=query.value(0).toString();
           r_parte2=query.value(1).toString();
           r_ejercicio1=query.value(2).toString();
           r_ejercicio2=query.value(3).toString();
           // estadosmedios=query.value(4).toBool();
           formulabasepor=query.value(5).toString();
           valorbasepor1=query.value(6).toDouble();
           valorbasepor2=query.value(7).toDouble();
           observaciones=query.value(8).toString();
           analitica=query.value(9).toBool();
           ci=query.value(10).toString();
           haycolref=query.value(11).toBool();
           colref=query.value(12).toString();
           cabecera=query.value(13).toString();
       } else return;
    bool hayporcentajes=false;
    double porcentaje=0;

    if (formulabasepor.length()>0 && (valorbasepor1>0.001 || valorbasepor1<0.001)
        && (valorbasepor2>0.001 || valorbasepor2<0.001)) hayporcentajes=true;


    QString fechaini1,fechaini2, fechafin1, fechafin2;
    if (ui.fechas_ej1_groupBox->isChecked())
      {
       fechaini1=ui.ini_ej1_dateEdit->date().toString("yyyy/MM/dd");
       fechafin1=ui.fin_ej1_dateEdit->date().toString("yyyy/MM/dd");
      }
    if (ui.fechas_ej2_groupBox->isChecked())
      {
        fechaini2=ui.ini_ej2_dateEdit->date().toString("yyyy/MM/dd");
        fechafin2=ui.fin_ej2_dateEdit->date().toString("yyyy/MM/dd");
      }
    QString previsiones;
    if (ui.previsiones->isChecked()) previsiones=tr("Previsiones");

    bool laparte1=true;

  while (true) {
    QSqlQuery query2 = basedatos::instancia()->select6Estadostituloparte1ordernodo( ui.titulolabel->text() , laparte1 );
    int veces;
    if ( query2.isActive() ) {
             while ( query2.next() ) {
               if (query2.value(0).toString().contains("LINEA")>0)
                 {
                   continue;
                 }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001
                    && query2.value(2).toDouble()>-0.0001
                    && r_ejercicio2.length()==0) continue;
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001
                    && query2.value(2).toDouble()>-0.0001
                    && query2.value(3).toDouble()<0.0001
                    && query2.value(3).toDouble()>-0.0001
                    && r_ejercicio2.length()>0) continue;
               // ----------------------------------------------------------------------
               //addElementoTextoDom(doc,tag2,"Parte",parte1);
               QString espacios;
               for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
                   espacios += "    ";
               if (laparte1) {
                   textolineas1 << espacios + query2.value(1).toString();
                   if (haycolref) notas1 << query2.value(4).toString();
                   if (hayporcentajes)
                             {
                                porcentaje=query2.value(2).toDouble()/valorbasepor1*100;
                                QString cadpor= formateanumero(porcentaje,false,decimales);
                                porcentajes1_1 << cadpor;
                             }
                   QString importe1;
                   if (query2.value(2).toDouble()>0.0001 ||
                       query2.value(2).toDouble()<-0.0001)
                       importe1=formateanumero(
                               query2.value(2).toDouble(),false,decimales);
                   importes1_1<<importe1;
                   // IMPORTE 2
                   if (r_ejercicio2.length()>0)
                      {
                         if (hayporcentajes)
                          {
                             porcentaje=query2.value(3).toDouble()/valorbasepor2*100;
                             QString cadpor= formateanumero(porcentaje,false,decimales);
                             porcentajes2_1 << cadpor;

                          }
                         QString importe2;
                         if (query2.value(3).toDouble()>0.0001 ||
                             query2.value(3).toDouble()<-0.0001)
                             importe2=formateanumero(
                                     query2.value(3).toDouble(),false,decimales);
                         importes2_1 << importe2;
                      }
               } else {
                   textolineas2  << espacios + query2.value(1).toString();
                   if (haycolref) notas2 << query2.value(4).toString();
                   if (hayporcentajes)
                             {
                                porcentaje=query2.value(2).toDouble()/valorbasepor1*100;
                                QString cadpor= formateanumero(porcentaje,false,decimales);
                                porcentajes1_2 << cadpor;
                             }
                   QString importe1;
                   if (query2.value(2).toDouble()>0.0001 ||
                       query2.value(2).toDouble()<-0.0001)
                       importe1=formateanumero(
                               query2.value(2).toDouble(),false,decimales);
                   importes1_2<<importe1;
                   // IMPORTE 2
                   if (r_ejercicio2.length()>0)
                      {
                         if (hayporcentajes)
                          {
                             porcentaje=query2.value(3).toDouble()/valorbasepor2*100;
                             QString cadpor= formateanumero(porcentaje,false,decimales);
                             porcentajes2_2 << cadpor;

                          }
                         QString importe2;
                         if (query2.value(3).toDouble()>0.0001 ||
                             query2.value(3).toDouble()<-0.0001)
                             importe2=formateanumero(
                                     query2.value(3).toDouble(),false,decimales);
                         importes2_2 << importe2;
                      }
               }

               no_segundo_ejercicio=false;
               if (condesglose || condesglosectas) no_segundo_ejercicio=true;
               if (ui.Ejercicio2comboBox->currentText()==noejercicio()) no_segundo_ejercicio=true;
               //
               if (laparte1) importes_detalle1 << ""; else importes_detalle2 << "";
               if (laparte1) importes_cuentas1 << ""; else importes_cuentas2 << "";

               if (!condesglose && !condesglosectas) continue;

               // desglosamos el campo fórmula que no contenga {
               QString cadpru=query2.value(5).toString();
               if (cadpru.contains('{')) continue;

               int indice=0;
               int inicio,final;
               QString ejercicio=ui.Ejercicio1comboBox->currentText();
               bool acotado=ui.fechas_ej1_groupBox->isChecked();
               QDate fecha1=ui.ini_ej1_dateEdit->date();
               QDate fecha2=ui.fin_ej1_dateEdit->date();
               bool estadosmedios=consaldosmedios;
               double valor;
               QString extract;
               while (indice<cadpru.length())
                   {
                    if (cadpru[indice]=='[')
                        {
                         inicio=indice;
                         final=inicio;
                         while (final<cadpru.length() && cadpru[final]!=']') final++;
                         // extraemos la cadena de la cuenta sin corchetes
                         extract=cadpru.mid(inicio+1,final-inicio-1);
                        // habría que calcular el valor de la cuenta
                        // QMessageBox::warning( this, tr("Estados Contables"),extract);
                        // -------------------------------------------------------------------------------------
                        if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotado,fecha1,fecha2);
                           else
                              {
                               if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract);
                                 else
                                      valor=calculamediacuenta(extract,true);
                              }
                        bool positivo=false;
                        bool negativo=false;
                        positivo=extract.contains('+');
                        negativo=extract.contains('-');
                        extract.remove('+');
                        extract.remove('-');
                           if (valor>0.001 || valor<-0.001) // ----------------------------------------------------------------------
                              {
                                 if (inicio-1>=0 && cadpru[inicio-1]=='-')
                                   {
                                    valor=-1*valor;
                                   }
                                 if (laparte1) {
                                    textolineas1 << "    " +extract+ " "+descripcioncuenta(extract);
                                    notas1 << "";
                                    porcentajes1_1 << "";
                                    porcentajes2_1 << "";
                                    importes1_1 << "";
                                    importes2_1 << "";
                                    importes_cuentas1 << formateanumero(valor,false,decimales);
                                    importes_detalle1 <<"";
                                 } else {
                                   textolineas2 << "    " +extract+ " "+descripcioncuenta(extract);
                                   notas2 << "";
                                   porcentajes1_2 << "";
                                   porcentajes2_2 << "";
                                   importes1_2 << "";
                                   importes2_2 << "";
                                   importes_cuentas2 << formateanumero(valor,false,decimales);
                                   importes_detalle2 << "";
                                 }

                                if (condesglose)
                                   {
                                    QSqlQuery q;
                                    if (!cod_longitud_variable())
                                       {
                                        q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                                       }
                                     else
                                        {
                                         q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                        }
                                     while (q.next())
                                         {
                                          QString extract2=q.value(0).toString();
                                          QString extract2sig=extract2;
                                          if (positivo) extract2sig.append('+');
                                          if (negativo) extract2sig.append('-');
                                          if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                       acotado,fecha1,fecha2);
                                             else
                                                {
                                                  if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract2sig);
                                                    else
                                                       valor=calculamediacuenta_aux(extract2sig,true);
                                                }
                                          if (valor>0.001 || valor <-0.001)
                                               {
                                                 if (inicio-1>=0 && cadpru[inicio-1]=='-')
                                                   {
                                                    valor=-1*valor;
                                                   }
                                                 if (laparte1) {
                                                   textolineas1 << "     "+extract2+" "+descripcioncuenta(extract2);
                                                   notas1 << "";
                                                   importes1_1 << "";
                                                   importes2_1 << "";
                                                   porcentajes1_1 << "";
                                                   porcentajes2_1 << "";
                                                   importes_detalle1 << formateanumero(valor,false,decimales);
                                                   importes_cuentas1 <<"";
                                                   //importes2_1 << "";
                                                 } else {
                                                   textolineas2 << "     "+extract2+" "+descripcioncuenta(extract2);
                                                   notas2 << "";
                                                   importes1_2 << "";
                                                   importes2_2 << "";
                                                   porcentajes1_2 << "";
                                                   porcentajes2_2 << "";
                                                   importes_detalle2 << formateanumero(valor,false,decimales);
                                                   importes_cuentas2 <<"";
                                                   //importes2_2 << "";
                                                 }
                                                }
                                            } // while
                                     } // fin del condesglose
                                } // if valor no es cero
                    } // del cadpru índice
                  indice++;
                } // del while índice

               // -----------------------------------------------------------

             } // del while query next
           }
     if (!laparte1) break;
     if (r_parte2.length()>0) laparte1=false;
        else break;
    } // fin del while (true)

  // elaboramos el informe

  /*QString num1; num1.setNum(textolineas1.count()) ;
  //QString num2=porcentajes1_1.count();
  //QString num3=porcentajes2_1.count();
  QString num4; num4.setNum(importes1_1.count());
  QString num5; num5.setNum(importes2_1.count());
  QString num6; num6.setNum(notas1.count());
  QMessageBox::information( this, tr("Imprimir"),num1);
  QMessageBox::information( this, tr("Imprimir"),num4);
  QMessageBox::information( this, tr("Imprimir"),num5);
  QMessageBox::information( this, tr("Imprimir"),num6);*/

  QString fileName;
  fileName = r_parte2.length()>0 ? ":/informes/estado2p.xml" : ":/informes/estado1p.xml";
  if (hayporcentajes) fileName = r_parte2.length()>0 ? ":/informes/estado2p_porc.xml" : ":/informes/estado1p_porc.xml";
  if (condesglosectas) {
      if (r_parte2.length()>0) fileName = ":/informes/estado2p_desglose_principales.xml";
         else fileName = ":/informes/estado1p_desglose_principales.xml";
  }
  if (condesglose) {
      if (r_parte2.length()>0) fileName = ":/informes/estado2p_desglose.xml";
         else fileName = ":/informes/estado1p_desglose.xml";
  }
  QtRPT *report = new QtRPT(this);
  //report->recordCount << textolineas1.count();
  //if (r_parte2.length()>0) report->recordCount << textolineas2.count();
  if (report->loadReport(fileName) == false)
  {
      QMessageBox::information( this, tr("Imprimir Estado Contable"),
                                tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
      return;
  }

  QObject::connect(report, &QtRPT::setDSInfo,
                   [&](DataSetInfo &dsInfo) {
      if (dsInfo.reportPage == 0)
        dsInfo.recordCount=textolineas1.count();
      if (dsInfo.reportPage == 1)
          dsInfo.recordCount=textolineas2.count();
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

      if (reportPage ==0 && recNo>textolineas1.count()-1) return;
      if (reportPage ==1 && recNo>textolineas2.count()-1) return;
      if (paramName == "descrip") paramValue = (reportPage ==0 ? textolineas1.at(recNo) : textolineas2.at(recNo));
      if (paramName == "nota") {
          if (!notas1.isEmpty() && reportPage==0) paramValue = notas1.at(recNo);
          if (!notas2.isEmpty() && reportPage==1) paramValue = notas2.at(recNo);
          // paramValue = (reportPage ==0 ? notas1.at(recNo) : notas2.at(recNo));
      }
      if (paramName == "cifra1") paramValue = (reportPage ==0 ? importes1_1.at(recNo) : importes1_2.at(recNo));
      if (!no_segundo_ejercicio) {
         if (paramName == "cifra2") paramValue = (reportPage ==0 ? importes2_1.at(recNo) : importes2_2.at(recNo));
      }
      if (paramName == "porcentaje1") paramValue = (reportPage ==0 ? porcentajes1_1.at(recNo) : porcentajes1_2.at(recNo));
      if (!no_segundo_ejercicio) {
         if (paramName == "porcentaje2") paramValue = (reportPage ==0 ? porcentajes2_1.at(recNo) : porcentajes2_2.at(recNo));
      }
      if (paramName == "cuentas") paramValue = (reportPage ==0 ? importes_cuentas1.at(recNo) : importes_cuentas2.at(recNo));
      if (paramName == "detalle") paramValue = (reportPage ==0 ? importes_detalle1.at(recNo) : importes_detalle2.at(recNo));
      if (paramName == "empresa") paramValue = basedatos::instancia()->selectEmpresaconfiguracion();
      if (paramName == "estado") paramValue = cabecera;
      if (paramName == "parte1") paramValue = r_parte1;
      if (paramName == "parte2") paramValue = r_parte2;
      if (paramName == "ej1") paramValue = r_ejercicio1;
      if (paramName == "ej2") paramValue = r_ejercicio2;

      if (ui.fechas_ej1_groupBox->isChecked()) {
          if (paramName == "FECHAS1") paramValue = ui.ini_ej1_dateEdit->date().toString("dd/MM/yyyy") + " - " +
                  ui.fin_ej1_dateEdit->date().toString("dd/MM/yyyy");
      }
      if (ui.fechas_ej2_groupBox->isChecked()) {
          if (paramName == "FECHAS2") paramValue = ui.ini_ej2_dateEdit->date().toString("dd/MM/yyyy") + " - " +
                  ui.fin_ej2_dateEdit->date().toString("dd/MM/yyyy");
      }
      if (!ui.cilineEdit->text().isEmpty()) {
          if (paramName == "ANALITICA") paramValue = ui.cilineEdit->text();
                  //+ " " + ui.nivel1lineEdit->text()+ "-" +
                  //ui.nivel2lineEdit->text() + "-" + ui.nivel3lineEdit->text();
      }

  });


  // report->printExec(true);
  if (hay_visualizador_extendido) report->printExec(true);
  else {
         QString qfichero=dirtrabajo();
         qfichero.append(QDir::separator());
         qfichero=qfichero+(ui.texlineEdit->text().length()==0 ? "estadocontable.pdf" :
                                              ui.texlineEdit->text());
         report->printPDF(qfichero);
  }


}



void calcestado::imprime()
{

   QString qfichero;
   if (ui.texlineEdit->text().length()==0)
      qfichero=tr("estadocontable");
   else
         {
            qfichero=ui.texlineEdit->text();
            if (ui.texlineEdit->text().right(4)==".tex") qfichero=qfichero.left(qfichero.length()-4);
         }

    generalatex();


   int valor=imprimelatex2(qfichero);
   if (valor==1)
       QMessageBox::warning( this, tr("Consulta de estado contable"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Consulta de estado contable"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Consulta de estado contable"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}



void calcestado::copia()
{
   if (condesglose) { copia2(); return; }

   QClipboard *cb = QApplication::clipboard();
   QString global;

    QString ejercicio1="";
    QString ejercicio2="";
    QString parte1="";
    QString parte2="";
    QString formulabasepor;
    QString observaciones;
    // double valorbasepor1=0;
    // double valorbasepor2=0;
    // bool estadosmedios=false;
    bool analitica,haycolref;
    QString ci,colref;
    QSqlQuery query = basedatos::instancia()->select14Cabeceraestadostitulo( ui.titulolabel->text() );

    if ( query.isActive() ) {
           query.first();
           parte1=query.value(0).toString();
	   parte2=query.value(1).toString();
	   ejercicio1=query.value(2).toString();
	   ejercicio2=query.value(3).toString();
       //estadosmedios=query.value(4).toBool();
	   formulabasepor=query.value(5).toString();
       //valorbasepor1=query.value(6).toDouble();
       // valorbasepor2=query.value(7).toDouble();
	   observaciones=query.value(8).toString();
           analitica=query.value(9).toBool();
	   ci=query.value(10).toString();
           haycolref=query.value(11).toBool();
           colref=query.value(12).toString();
       } else return;
    
    // Generamos primera parte del Estado Contable
    global=nombreempresa();
    global+="\n\n";
    if (ui.previsiones->isChecked())
      {
       global+="** PREVISIONES **\n\n";
      }


    // ---------------------------------------------------------------------------------------------
    if (analitica && ci.length()>0)
        {
         global+=tr("CÓDIGO DE IMPUTACIÓN\t");
         global+=ci;
         global+="\n";
         QString codigo=ci;
         QString descripcion;
         QString qnivel=0;


         if (codigo.startsWith("???"))
            {
             global+= tr("CUALQUIERA");
            }
         else
           {
            bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
            if (encontrada && qnivel.toInt()==1)
                global+= descripcion;
           }
         if (codigo.length()>3) global+="\t";
         if (codigo.length()>3 && codigo.mid(3,3)==QString("???")) global+= tr("CUALQUIERA");
         else
          {
           bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
           int elnivel=qnivel.toInt();
           if (encontrada && elnivel==2)
              global += descripcion;
          }

         if (codigo.length()>6) global+="\t";
         if (codigo.length()==7 && codigo[6]=='*')  global+= tr("CUALQUIERA");
         else
           {
             bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
             if (encontrada && qnivel.toInt()==3)
             global+=descripcion;
           }
         global+= "\n";
        }

    // -----------------------------------------------------------------------------------------------------
    global+= parte1;
    global+="\t";
    if (haycolref) { global+=colref; global+="\t"; }
    global+= ejercicio1;
    // -----------------------------------------------------------------------------------------------------
    if (ejercicio2.length()>0 && !ui.desglose_mes_checkBox->isChecked())
         {
            global+="\t";
            global+=ejercicio2;
         }
    if (ui.desglose_mes_checkBox->isChecked())
        global+="\tM1\tM2\tM3\tM4\tM5\tM6\tM7\tM8\tM9\tM10\tM11\tM12";
    global += "\n";
    // ------------------------------------------------------------------------------------------------------
    
    QSqlQuery query2 = basedatos::instancia()->select5Estadostituloparte1ordernodo( ui.titulolabel->text() , true);
     if ( query2.isActive() ) {
             while ( query2.next() ) {
	       if (query2.value(0).toString().contains("LINEA")>0)
		 {
		   continue;
	                 }
	       //for (veces=1;veces<=query2.value(0).toString().contains('.');veces++)
		//   stream << "\\hspace{0.5cm} ";
	       // if (query2.value(0).toString().contains("TOTAL")==0) 
	      //	   stream << query2.value(0).toString() << " " ;
	       global+=  query2.value(1).toString() ;
	       global+= "\t";
               if (haycolref)
                   {
	            global+=  query2.value(4).toString() ;
	            global+= "\t";
                   }
               //---------------------------------------------
	       if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
                  global+=formateanumero(query2.value(2).toDouble(),comadecimal,decimales);
           if (ejercicio2.length()>0 && !ui.desglose_mes_checkBox->isChecked())
	            {
                  global += "\t";
                  if (query2.value(3).toDouble()>0.001 || query2.value(3).toDouble()<-0.001)
                        global+=formateanumero(query2.value(3).toDouble(),comadecimal,decimales);
	            }
           if (ui.desglose_mes_checkBox->isChecked())
             {
               global += "\t";
               global+=formateanumero(query2.value(5).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(6).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(7).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(8).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(9).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(10).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(11).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(12).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(13).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(14).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(15).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(16).toDouble(),comadecimal,decimales);
             }
	       global+="\n";
	     }
       }
    global+= "\n\n\n";
    
 if (parte2.length()>0)
    {
     global+=ui.titulolabel->text();
     global+= "\n\n";
    // -------------------------------------------------------------------------------------------------------
    global+= parte2 ;
    global+="\t";
    if (haycolref) { global+=colref; global+="\t"; }
    global+=ejercicio1;
    // -------------------------------------------------------------------------------------------------------
    if (ejercicio2.length()>0 && !ui.desglose_mes_checkBox->isChecked())
      {
         global+="\t";
         global+= ejercicio2 ;
      }

    global+="\n";

    query2 = basedatos::instancia()->select5Estadostituloparte1ordernodo(ui.titulolabel->text(),false);
    if ( query2.isActive() ) {
             while ( query2.next() ) {
	       if (query2.value(0).toString().contains("LINEA")>0)
		 {
		   continue;
	         }
	       // for (veces=1;veces<=query2.value(0).toString().contains('.');veces++)
		   // stream << "\\hspace{0.5cm} ";
	       // if (query2.value(0).toString().contains("TOTAL")==0) 
		//   stream << query2.value(0).toString() << " " ;
	       global+= query2.value(1).toString();
	       global+="\t";
               if (haycolref)
                   {
	            global+=  query2.value(4).toString() ;
	            global+= "\t";
                   }
	       if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
                  global+=formateanumero(query2.value(2).toDouble(),comadecimal,decimales);
           if (ejercicio2.length()>0 && !ui.desglose_mes_checkBox->isChecked())
	            {
                 global+= "\t";
                 if (query2.value(3).toDouble()>0.001 || query2.value(3).toDouble()<-0.001)
                        global+=formateanumero(query2.value(3).toDouble(),comadecimal,decimales);
	            }
           if (ui.desglose_mes_checkBox->isChecked())
             {
               global += "\t";
               global+=formateanumero(query2.value(5).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(6).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(7).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(8).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(9).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(10).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(11).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(12).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(13).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(14).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(15).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(16).toDouble(),comadecimal,decimales);
             }

	       global+= "\n";
	     }
           }
 }
    global+="\n";
    global+=observaciones;
    global+="\n";

    cb->setText(global);
    QMessageBox::information( this, tr("Cálculo de Estado Contable"),
                             tr("Se ha pasado el contenido al portapapeles") );

}


void calcestado::generalatex2()
{
    // con desglose
    // forzamos cálculo de estado
    // ya que los saldos de las cuentas se cogerán del diario
    if (auxiliares.count()==0) calculaestado();
    QString ejercicio1="";
    QString parte1="";
    QString parte2="";
    QString observaciones;
    QString cabecera;
    QString descabecera;
    QString despie;
    // bool estadosmedios=false;
    bool analitica;
    QString ci;

    QSqlQuery query = basedatos::instancia()->select16Cabeceraestadostitulo( ui.titulolabel->text() );
    if ( query.isActive() ) {
           query.first();
           parte1=query.value(0).toString();
	   parte2=query.value(1).toString();
	   ejercicio1=query.value(2).toString();
       // estadosmedios=query.value(4).toBool();
	   observaciones=query.value(8).toString();
           analitica=query.value(9).toBool();
           ci=query.value(10).toString();
           cabecera=query.value(13).toString();
           descabecera=query.value(14).toString();
           despie=query.value(15).toString();
       } else return;
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   if (ui.texlineEdit->text().length()==0)
      qfichero=qfichero+tr("estadocontable.tex");
   else
         {
            qfichero=qfichero+ui.texlineEdit->text();
            if (ui.texlineEdit->text().right(4)!=".tex") qfichero=qfichero+".tex";
         }
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QFile fichero(qfichero);
    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();
    
    // Generamos primera parte del Estado Contable
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";
    if (ui.previsiones->isChecked())
      {
       stream << "\\begin{center}" << "\n";
       stream << "{\\Large \\textbf {";
       stream << " ** PREVISIONES ** }}";
       stream << "\\end{center}";
      }

    if (analitica && ci.length()>0)
        {
         stream << "\\begin{center}" << "\n";
         stream << "{\\normalsize \\textbf {" << tr("CÓDIGO DE IMPUTACIÓN: ") << ci <<  "}}" << "\n";
         stream << "\\end{center}" << "\n";

         QString codigo=ci;
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

    if (ui.fechas_ej1_groupBox->isChecked())
       {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Ejercicio: ") << ui.Ejercicio1comboBox->currentText() << tr(" - FECHAS: ");
        stream << ui.ini_ej1_dateEdit->date().toString("dd/MM/yyyy") << tr(" a ");
        stream << ui.fin_ej1_dateEdit->date().toString("dd/MM/yyyy");
        stream << "}}";
        stream << "\\end{center}";
       }

    if (ui.fechas_ej2_groupBox->isEnabled() && ui.fechas_ej2_groupBox->isChecked())
       {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Ejercicio: ") << ui.Ejercicio2comboBox->currentText() << tr(" - FECHAS: ");
        stream << ui.ini_ej2_dateEdit->date().toString("dd/MM/yyyy") << tr(" a ");
        stream << ui.fin_ej2_dateEdit->date().toString("dd/MM/yyyy");
        stream << "}}";
        stream << "\\end{center}";
       }

    stream << "\\begin{center}" << "\n";

    stream << "\\begin{longtable}{|p{10cm}|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    stream << "\\multicolumn{4";
    stream << "}{|c|} {\\textbf{ ";
    stream << cabecera <<  "}} \\\\";
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    // ---------------------------------------------------------------------------------------

    stream << "{\\textbf {" << parte1 << "}} & ";
    stream << "{\\scriptsize {" << tr("DETALLE") << "}} &";
    stream << "{\\scriptsize {" << tr("CUENTAS") <<"}} &";
    stream << "{\\scriptsize {" << ejercicio1 <<"}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // -------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    stream << "{\\textbf{" << parte1 << "}} & ";
    stream << "{\\scriptsize {" << tr("DETALLE") <<"}} &";
    stream << "{\\scriptsize {" << tr("CUENTAS") <<"}} &";
    stream << "{\\scriptsize {" << ejercicio1 <<"}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    stream << "\\hline" << "\n";
    
    QSqlQuery query2 = basedatos::instancia()->select6Estadostituloparte1ordernodo( ui.titulolabel->text() , true );
    int veces;
    if ( query2.isActive() ) {
             while ( query2.next() ) {
	       if (query2.value(0).toString().contains("LINEA")>0)
		 {
		   stream << "\\hline\n";
		   continue;
	         }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001 
                    && query2.value(2).toDouble()>-0.0001
                    ) continue;
	       for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
		   stream << "\\hspace{0.5cm} ";
	       // if (query2.value(0).toString().contains("TOTAL")==0) 
	      //	   stream << query2.value(0).toString() << " " ;
	       stream << "{\\scriptsize {" << filtracad(query2.value(1).toString()) << "}} & ";
 	       stream << " & ";
 	       stream << " & ";
	       if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
	             stream << "{\\scriptsize {" << 
                        formateanumerosep(query2.value(2).toDouble(),comadecimal,decimales) << "}}";
	       stream << " \\\\" << "\n";
               // desglosamos el campo fórmula que no contenga {
               QString cadpru=query2.value(5).toString();
               if (cadpru.contains('{')) continue;

               int indice=0;
               int inicio,final;
               QString ejercicio=ui.Ejercicio1comboBox->currentText();
               bool acotado=ui.fechas_ej1_groupBox->isChecked();
               QDate fecha1=ui.ini_ej1_dateEdit->date();
               QDate fecha2=ui.fin_ej1_dateEdit->date();
               bool estadosmedios=consaldosmedios;
               double valor;
               QString extract;
               while (indice<cadpru.length())
                   {
                    if (cadpru[indice]=='[')
                        {
                         inicio=indice;
                         final=inicio;
                         while (final<cadpru.length() && cadpru[final]!=']') final++;
                         // extraemos la cadena de la cuenta sin corchetes
                         extract=cadpru.mid(inicio+1,final-inicio-1);
                        // habría que calcular el valor de la cuenta
                        // QMessageBox::warning( this, tr("Estados Contables"),extract);
                        // -------------------------------------------------------------------------------------
                        if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotado,fecha1,fecha2);
                           else
	                      {
                             if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract);
                              else
		                      valor=calculamediacuenta(extract,true);
	                      }
	                // if (valor>-0.004 && valor < 0.005) continue;
                        if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                        // imprimimos cadena formada por código de cuenta y descripción
                        bool positivo=false;
                        bool negativo=false;
                        positivo=extract.contains('+');
                        negativo=extract.contains('-');
                        extract.remove('+');
                        extract.remove('-');
     	                if (valor>0.001 || valor<-0.001)
                           {
                            stream << "{\\tiny {" << extract << " " << filtracad(descripcioncuenta(extract))
                                      << "}} & &";
                            stream << "{\\tiny {" << formateanumerosep(valor,comadecimal,decimales) << "}} & ";
                            stream << " \\\\" << "\n";
                            // QMessageBox::warning( this, tr("Estados Contables"),extract);
                            // seguimos aquí con el desglose de todas las cuentas
                            // ojo si aparece el signo + ó menos en extract
                            // habría que quitarlo y añadirlo a cada selección
                            if (condesglose)
                            {
                             QSqlQuery q;
                             if (!cod_longitud_variable())
                               {
                                q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                               }
                               else
                                   {
                                    q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                   }
                              while (q.next())
                                 {
                                   QString extract2=q.value(0).toString();
                                   QString extract2sig=extract2;
                                   if (positivo) extract2sig.append('+');
                                   if (negativo) extract2sig.append('-');
                                   if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                       acotado,fecha1,fecha2);
                                   else
                                       {
                                         if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract2sig);
                                         else
                                           valor=calculamediacuenta_aux(extract2sig,true);
                                       }
                                   if (valor>0.001 || valor <-0.001)
                                       {
                                         stream << "\\hspace{0.5cm} ";
                                         stream << "{\\tiny {" << extract2 << " " << 
                                             filtracad(descripcioncuenta(extract2)) << "}} &";
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                                         stream << "{\\tiny {" << formateanumerosep(valor,comadecimal,decimales)
                                              << "}} & &";
                                         stream << " \\\\" << "\n";
                                       }
                                  }
                               } // fin del condesglose
                           }
                       }
                    indice++;
                   }
	     } // final del while query
           }
    stream << "\\hline" << "\n";
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

 if (parte2.length()>0)
    {
    stream << "\\newpage" << "\n";
    stream << "\\begin{center}" << "\n";

    stream << "\\begin{longtable}{|p{10cm}|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    stream << "\\multicolumn{4";
    stream << "}{|c|} {\\textbf{ ";
    stream << cabecera <<  "}} \\\\";
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    // ---------------------------------------------------------------------------------------

    stream << "{\\textbf{" << parte2 << "}} & ";
    stream << "{\\scriptsize {" << tr("DETALLE") <<"}} &";
    stream << "{\\scriptsize {" << tr("CUENTAS") <<"}} &";
    stream << "{\\scriptsize {" << ejercicio1 <<"}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // -------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    stream << "{\\textbf{" << parte2 << "}} & ";
    stream << "{\\scriptsize {" << tr("DETALLE") << "}} &";
    stream << "{\\scriptsize {" << tr("CUENTAS") << "}} &";
    stream << "{\\scriptsize {" << ejercicio1 << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    stream << "\\hline" << "\n";
    
    QSqlQuery query2 = basedatos::instancia()->select6Estadostituloparte1ordernodo( ui.titulolabel->text() , false);
    int veces;
    if ( query2.isActive() ) {
             while ( query2.next() ) {
	       if (query2.value(0).toString().contains("LINEA")>0)
		 {
		   stream << "\\hline\n";
		   continue;
	         }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001 
                    && query2.value(2).toDouble()>-0.0001
                   ) continue;
	       for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
		   stream << "\\hspace{0.5cm} ";
	       // if (query2.value(0).toString().contains("TOTAL")==0) 
	      //	   stream << query2.value(0).toString() << " " ;
	       stream << "{\\scriptsize {" << filtracad(query2.value(1).toString()) << "}} & ";
 	       stream << " & ";
 	       stream << " & ";
	       if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
	             stream << "{\\scriptsize {" << 
                        formateanumerosep(query2.value(2).toDouble(),comadecimal,decimales) << "}}";
	       stream << " \\\\" << "\n";
               // tendríamos que desglosar las fórmulas (las que no contengan {)
               // desglosamos el campo fórmula que no contenga {
               QString cadpru=query2.value(5).toString();
               if (cadpru.contains('{')) continue;

               int indice=0;
               int inicio,final;
               QString ejercicio=ui.Ejercicio1comboBox->currentText();
               bool acotado=ui.fechas_ej1_groupBox->isChecked();
               QDate fecha1=ui.ini_ej1_dateEdit->date();
               QDate fecha2=ui.fin_ej1_dateEdit->date();
               bool estadosmedios=consaldosmedios;
               double valor;
               QString extract;
               while (indice<cadpru.length())
                   {
                    if (cadpru[indice]=='[')
                        {
                         inicio=indice;
                         final=inicio;
                         while (final<cadpru.length() && cadpru[final]!=']') final++;
                         // extraemos la cadena de la cuenta sin corchetes
                         extract=cadpru.mid(inicio+1,final-inicio-1);
                        // habría que calcular el valor de la cuenta
                        // QMessageBox::warning( this, tr("Estados Contables"),extract);
                        // -------------------------------------------------------------------------------------
                        if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotado,fecha1,fecha2);
                           else
	                      {
                               if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract);
	                         else
		                      valor=calculamediacuenta(extract,true);
	                      }
	                // if (valor>-0.004 && valor < 0.005) continue;
                        if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                        // imprimimos cadena formada por código de cuenta y descripción
                        bool positivo=false;
                        bool negativo=false;
                        positivo=extract.contains('+');
                        negativo=extract.contains('-');
                        extract.remove('+');
                        extract.remove('-');
     	                if (valor>0.001 || valor<-0.001)
                           {
                            stream << "{\\tiny {" << extract << " " << filtracad(descripcioncuenta(extract))
                                                 << "}} & &";
                            stream << "{\\tiny {" << formateanumerosep(valor,comadecimal,decimales) << "}} & ";
                            stream << " \\\\" << "\n";
                            // seguimos aquí con el desglose de todas las cuentas
                            if (condesglose)
                            {
                             QSqlQuery q;
                             if (!cod_longitud_variable())
                               {
                                q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                               }
                               else
                                   {
                                    q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                   }
                             while (q.next())
                                 {
                                   QString extract2=q.value(0).toString();
                                   QString extract2sig=extract2;
                                   if (positivo) extract2sig.append('+');
                                   if (negativo) extract2sig.append('-');
                                   if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                           acotado,fecha1,fecha2);
                                   else
                                      {
                                       if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract2sig);
                                         else
                                       valor=calculamediacuenta_aux(extract2sig,true);
                                      }
                                   if (valor>0.001 || valor <-0.001)
                                       {
                                         stream << "\\hspace{0.5cm} ";
                                         stream << "{\\tiny {" << extract2 << " " << 
                                             filtracad(descripcioncuenta(extract2)) << "}} &";
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                                         stream << "{\\tiny {" << formateanumerosep(valor,comadecimal,decimales)
                                              << "}} & &";
                                         stream << " \\\\" << "\n";
                                       }
                                  }
                              }
                           }
                       }
                    indice++;
                   }
	     }
           }
     stream << "\\hline" << "\n";
     stream << "\\end{longtable}" << "\n";
     stream << "\\end{center}" << "\n";
    }

    stream << observaciones << "\n";
    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}




void calcestado::copia2()
{
    if (auxiliares.count()==0) calculaestado();
    QString ejercicio1="";
    QString parte1="";
    QString parte2="";
    QString observaciones;
    QString cabecera;
    QString descabecera;
    QString despie;
    // bool estadosmedios=false;
    bool analitica;
    QString ci;

    QSqlQuery query = basedatos::instancia()->select16Cabeceraestadostitulo( ui.titulolabel->text() );
    if ( query.isActive() ) {
           query.first();
           parte1=query.value(0).toString();
	   parte2=query.value(1).toString();
	   ejercicio1=query.value(2).toString();
       // estadosmedios=query.value(4).toBool();
	   observaciones=query.value(8).toString();
           analitica=query.value(9).toBool();
           ci=query.value(10).toString();
           cabecera=query.value(13).toString();
           descabecera=query.value(14).toString();
           despie=query.value(15).toString();
       } else return;


    QProgressDialog progreso(tr("Generando Informe ..."), 0, 0, 3);
    progreso.setWindowTitle("Estados Contables");

    progreso.setValue(1);
    QApplication::processEvents();

   QClipboard *cb = QApplication::clipboard();
   QString global;

    // Generamos primera parte del Estado Contable
    global=nombreempresa();
    global+="\n\n";
    if (ui.previsiones->isChecked())
      {
       global+="** PREVISIONES **\n\n";
      }

    // ---------------------------------------------------------------------------------------------
    if (analitica && ci.length()>0)
        {
         global+=tr("CÓDIGO DE IMPUTACIÓN\t");
         global+=ci;
         global+="\n";
         QString codigo=ci;
         QString cadena,descripcion;
         QString qnivel=0;


         if (codigo.startsWith("???"))
            {
             global+= tr("CUALQUIERA");
            }
         else
           {
            bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
            if (encontrada && qnivel.toInt()==1)
                global+= descripcion;
           }
         if (codigo.length()>3) global+="\t";
         if (codigo.length()>3 && codigo.mid(3,3)==QString("???")) global+= tr("CUALQUIERA");
         else
          {
           bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
           int elnivel=qnivel.toInt();
           if (encontrada && elnivel==2)
              global += descripcion;
          }

         if (codigo.length()>6) global+="\t";
         if (codigo.length()==7 && codigo[6]=='*')  global+= tr("CUALQUIERA");
         else
           {
             bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
             if (encontrada && qnivel.toInt()==3)
             global+=descripcion;
           }
         global+= "\n";
        }

    global+="\n";
    global+= cabecera;
    global+= "\n";
    global+= "\n";
    // ---------------------------------------------------------------------------------------

    global+= parte1;
    global+= "\t";;
    global+=tr("DETALLE");
    global+= "\t";
    global+= tr("CUENTAS");
    global+= "\t";
    global+=ejercicio1;
    global+="\n";
    
    QSqlQuery query2 = basedatos::instancia()->select6Estadostituloparte1ordernodo( ui.titulolabel->text() , true);
    int veces;
    if ( query2.isActive() ) {
             while ( query2.next() ) {
	       if (query2.value(0).toString().contains("LINEA")>0)
		 {
		   continue;
	         }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001 
                    && query2.value(2).toDouble()>-0.0001
                    ) continue;
	       for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
		   global+= "  ";
	       global+=filtracad(query2.value(1).toString());
               global+= "\t";
 	       global+= "\t";
 	       global+= "\t";
	       if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
	             global+=formateanumero(query2.value(2).toDouble(),comadecimal,decimales);
           // -------------------------------------------------------------------------------------------
           if (ui.desglose_mes_checkBox->isChecked())
             {
               global += "\t";
               global+=formateanumero(query2.value(5).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(6).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(7).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(8).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(9).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(10).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(11).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(12).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(13).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(14).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(15).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(16).toDouble(),comadecimal,decimales);
             }
           global+="\n";
               // desglosamos el campo fórmula que no contenga {
               QString cadpru=query2.value(5).toString();
               if (cadpru.contains('{')) continue;

               int indice=0;
               int inicio,final;
               QString ejercicio=ui.Ejercicio1comboBox->currentText();
               bool acotado=ui.fechas_ej1_groupBox->isChecked();
               QDate fecha1=ui.ini_ej1_dateEdit->date();
               QDate fecha2=ui.fin_ej1_dateEdit->date();
               bool estadosmedios=consaldosmedios;
               double valor;
               QString extract;
               while (indice<cadpru.length())
                   {
                    QApplication::processEvents();
                    if (cadpru[indice]=='[')
                        {
                         inicio=indice;
                         final=inicio;
                         while (final<cadpru.length() && cadpru[final]!=']') final++;
                         // extraemos la cadena de la cuenta sin corchetes
                         extract=cadpru.mid(inicio+1,final-inicio-1);
                        // habría que calcular el valor de la cuenta
                        // QMessageBox::warning( this, tr("Estados Contables"),extract);
                        // -------------------------------------------------------------------------------------
                        if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotado,fecha1,fecha2);
                           else
	                      {
                            if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract);
	                         else
		                      valor=calculamediacuenta(extract,true);
	                      }
	                // if (valor>-0.004 && valor < 0.005) continue;
                        if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                        // imprimimos cadena formada por código de cuenta y descripción
                        bool positivo=false;
                        bool negativo=false;
                        positivo=extract.contains('+');
                        negativo=extract.contains('-');
                        //extract.remove('+');
                        //extract.remove('-');
     	                if (valor>0.001 || valor<-0.001)
                           {
                            global+=extract;
                            global+=" ";
                            global+= descripcioncuenta(extract);
                            global+= "\t\t";
                            global+=formateanumero(valor,comadecimal,decimales);
                            global+= "\t";

                            // -----------------------------------------------------------------------------
                            // repetimos esto para cada mes
                            QDate finicio;
                            QDate fin;
                           double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
                           m1=m2=m3=m4=m5=m6=m7=m8=m9=m10=m11=m12=0;
                           int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();
                            if (ui.desglose_mes_checkBox->isChecked())
                            {

                            finicio.setDate(anyo,1,1);
                            fin.setDate(anyo,1,31);
                            if (extract.contains(':')==1) m1=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m1=saldo_cuenta_lista_mes(extract,1);


                            finicio.setDate(anyo,2,1);
                            fin.setDate(anyo,3,1);
                            fin=fin.addDays(-1);
                            if (extract.contains(':')==1) m2=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m2=saldo_cuenta_lista_mes(extract,2);

                            finicio.setDate(anyo,3,1);
                            fin.setDate(anyo,3,31);
                            if (extract.contains(':')==1) m3=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                  m3=saldo_cuenta_lista_mes(extract,3);


                            finicio.setDate(anyo,4,1);
                            fin.setDate(anyo,4,30);
                            if (extract.contains(':')==1) m4=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m4=saldo_cuenta_lista_mes(extract,4);


                            finicio.setDate(anyo,5,1);
                            fin.setDate(anyo,5,31);
                            if (extract.contains(':')==1) m5=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m5=saldo_cuenta_lista_mes(extract,5);

                            finicio.setDate(anyo,6,1);
                            fin.setDate(anyo,6,30);
                            if (extract.contains(':')==1) m6=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m6=saldo_cuenta_lista_mes(extract,6);

                            finicio.setDate(anyo,7,1);
                            fin.setDate(anyo,7,31);
                            if (extract.contains(':')==1) m7=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m7=saldo_cuenta_lista_mes(extract,7);

                            finicio.setDate(anyo,8,1);
                            fin.setDate(anyo,8,31);
                            if (extract.contains(':')==1) m8=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m8=saldo_cuenta_lista_mes(extract,8);

                            finicio.setDate(anyo,9,1);
                            fin.setDate(anyo,9,30);
                            if (extract.contains(':')==1) m9=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m9=saldo_cuenta_lista_mes(extract,9);


                            finicio.setDate(anyo,10,1);
                            fin.setDate(anyo,10,31);
                            if (extract.contains(':')==1) m10=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m10=saldo_cuenta_lista_mes(extract,10);


                            finicio.setDate(anyo,11,1);
                            fin.setDate(anyo,11,30);
                            if (extract.contains(':')==1) m11=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m11=saldo_cuenta_lista_mes(extract,11);


                            finicio.setDate(anyo,12,1);
                            fin.setDate(anyo,12,31);
                            if (extract.contains(':')==1) m12=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m12=saldo_cuenta_lista_mes(extract,12);

                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m1=-1*m1;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m2=-1*m2;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m3=-1*m3;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m4=-1*m4;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m5=-1*m5;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m6=-1*m6;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m7=-1*m7;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m8=-1*m8;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m9=-1*m9;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m10=-1*m10;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m11=-1*m11;
                            if (inicio-1>=0 && cadpru[inicio-1]=='-') m12=-1*m12;
                            // if (valor>-0.004 && valor < 0.005) continue;
                            // if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                            // imprimimos cadena formada por código de cuenta y descripción
                                // ahora todos los meses
                             // global+="\t\t";
                             global+= "\t" + formateanumerosep(m1,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m2,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m3,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m4,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m5,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m6,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m7,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m8,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m9,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m10,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m11,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m12,comadecimal,decimales);
                            }
                            // -----------------------------------------------------------------------------
                            global+= "\n";
                            // seguimos aquí con el desglose de todas las cuentas
                            QSqlQuery q;
                            if (!cod_longitud_variable())
                               {
                                q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                               }
                               else
                                   {
                                    q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                   }
                            while (q.next())
                                 {
                                   QString extract2=q.value(0).toString();
                                   QString extract2sig=extract2;
                                   if (positivo) extract2sig.append('+');
                                   if (negativo) extract2sig.append('-');
                                   if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                                  acotado,fecha1,fecha2);
                                   else
                                     {
                                     if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract2sig);
                                      else
                                        valor=calculamediacuenta_aux(extract2sig,true);
                                     }
                                   // --------------------------------------------------------------------------
                                   if (ui.desglose_mes_checkBox->isChecked())
                                   {
                                    finicio.setDate(anyo,1,1);
                                    fin.setDate(anyo,1,31);
                                    if (extract2sig.contains(':')==1) m1=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m1=saldo_cuenta_lista_mes(extract2sig,1);


                                   finicio.setDate(anyo,2,1);
                                   fin.setDate(anyo,3,1);
                                   fin=fin.addDays(-1);
                                   if (extract2sig.contains(':')==1) m2=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m2=saldo_cuenta_lista_mes(extract2sig,2);


                                   finicio.setDate(anyo,3,1);
                                   fin.setDate(anyo,3,31);
                                   if (extract2sig.contains(':')==1) m3=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m3=saldo_cuenta_lista_mes(extract2sig,3);


                                   finicio.setDate(anyo,4,1);
                                   fin.setDate(anyo,4,30);
                                   if (extract2sig.contains(':')==1) m4=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m4=saldo_cuenta_lista_mes(extract2sig,4);


                                   finicio.setDate(anyo,5,1);
                                   fin.setDate(anyo,5,31);
                                   if (extract2sig.contains(':')==1) m5=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m5=saldo_cuenta_lista_mes(extract2sig,5);

                                   finicio.setDate(anyo,6,1);
                                   fin.setDate(anyo,6,30);
                                   if (extract2sig.contains(':')==1) m6=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m6=saldo_cuenta_lista_mes(extract2sig,6);

                                   finicio.setDate(anyo,7,1);
                                   fin.setDate(anyo,7,31);
                                   if (extract2sig.contains(':')==1) m7=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m7=saldo_cuenta_lista_mes(extract2sig,7);

                                   finicio.setDate(anyo,8,1);
                                   fin.setDate(anyo,8,31);
                                   if (extract2sig.contains(':')==1) m8=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m8=saldo_cuenta_lista_mes(extract2sig,8);


                                   finicio.setDate(anyo,9,1);
                                   fin.setDate(anyo,9,30);
                                   if (extract2sig.contains(':')==1) m9=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m9=saldo_cuenta_lista_mes(extract2sig,9);


                                   finicio.setDate(anyo,10,1);
                                   fin.setDate(anyo,10,31);
                                   if (extract2sig.contains(':')==1) m10=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m10=saldo_cuenta_lista_mes(extract2sig,10);


                                   finicio.setDate(anyo,11,1);
                                   fin.setDate(anyo,11,30);
                                   if (extract2sig.contains(':')==1) m11=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m11=saldo_cuenta_lista_mes(extract2sig,11);


                                   finicio.setDate(anyo,12,1);
                                   fin.setDate(anyo,12,31);
                                   if (extract2sig.contains(':')==1) m12=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m12=saldo_cuenta_lista_mes(extract2sig,12);

                                   }
                                   if (valor>0.001 || valor <-0.001)
                                       {
                                         global += extract2 + " " +
                                             filtracad(descripcioncuenta(extract2)) + "\t";
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                                         global += formateanumerosep(valor,comadecimal,decimales) + "\t ";
                                         // ahora todos los meses
                                         if (ui.desglose_mes_checkBox->isChecked())
                                         {
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m1=-1*m1;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m2=-1*m2;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m3=-1*m3;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m4=-1*m4;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m5=-1*m5;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m6=-1*m6;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m7=-1*m7;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m8=-1*m8;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m9=-1*m9;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m10=-1*m10;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m11=-1*m11;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m12=-1*m12;
                                         global += "\t\t";
                                         global += formateanumerosep(m1,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m2,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m3,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m4,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m5,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m6,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m7,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m8,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m9,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m10,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m11,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m12,comadecimal,decimales);
                                         }
                                         global += "\n";
                                       }
                                   // --------------------------------------------------------------------------
                                  }
                           }
                       }
                    indice++;
                   }
	     } // final del while query
           }
    global+="\n\n";


    progreso.setValue(2);
    QApplication::processEvents();

 if (parte2.length()>0)
   {
    global+=cabecera;
    global+="\n\n";

    global+= parte2;
    global+= "\t";
    global+=tr("DETALLE");
    global+="\t";
    global+=tr("CUENTAS");
    global+="\t";
    global+= ejercicio1;
    global+="\n";
    
    QSqlQuery query2 = basedatos::instancia()->select6Estadostituloparte1ordernodo( ui.titulolabel->text() , false);
    int veces;
    if ( query2.isActive() ) {
             while ( query2.next() ) {
	       if (query2.value(0).toString().contains("LINEA")>0)
             {
              continue;
	         }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001 
                    && query2.value(2).toDouble()>-0.0001
                   ) continue;
	       for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
		   global+="  ";
	       global+=filtracad(query2.value(1).toString());
               global+="\t";
 	       global+="\t";
 	       global+="\t";
	       if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
	             global+=formateanumero(query2.value(2).toDouble(),comadecimal,decimales);
           // -----------------------------------------------------------------------------------------
           if (ui.desglose_mes_checkBox->isChecked())
             {
               global += "\t";
               global+=formateanumero(query2.value(5).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(6).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(7).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(8).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(9).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(10).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(11).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(12).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(13).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(14).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(15).toDouble(),comadecimal,decimales);
               global += "\t";
               global+=formateanumero(query2.value(16).toDouble(),comadecimal,decimales);
             }
	       global+= "\n";
               // tendríamos que desglosar las fórmulas (las que no contengan {)
               // desglosamos el campo fórmula que no contenga {
               QString cadpru=query2.value(5).toString();
               if (cadpru.contains('{')) continue;

               int indice=0;
               int inicio,final;
               QString ejercicio=ui.Ejercicio1comboBox->currentText();
               bool acotado=ui.fechas_ej1_groupBox->isChecked();
               QDate fecha1=ui.ini_ej1_dateEdit->date();
               QDate fecha2=ui.fin_ej1_dateEdit->date();
               bool estadosmedios=consaldosmedios;
               double valor;
               QString extract;
               while (indice<cadpru.length())
                   {
                    if (cadpru[indice]=='[')
                        {
                         inicio=indice;
                         final=inicio;
                         while (final<cadpru.length() && cadpru[final]!=']') final++;
                         // extraemos la cadena de la cuenta sin corchetes
                         extract=cadpru.mid(inicio+1,final-inicio-1);
                        // habría que calcular el valor de la cuenta
                        // QMessageBox::warning( this, tr("Estados Contables"),extract);
                        // -------------------------------------------------------------------------------------
                        if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,
                                                                    acotado,fecha1,fecha2);
                           else
	                      {
                               if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract);
	                         else
                                      valor=calculamediacuenta(extract,true);
	                      }
                        if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                        bool positivo=false;
                        bool negativo=false;
                        positivo=extract.contains('+');
                        negativo=extract.contains('-');
                        //extract.remove('+');
                        //extract.remove('-');
     	                if (valor>0.001 || valor<-0.001)
                           {
                            global+=  extract;
                            global+= " ";
                            global+=  descripcioncuenta(extract);
                            global+= "\t\t";
                            global+= formateanumero(valor,comadecimal,decimales);
                            global+= "\t";
                            // -----------------------------------------------------------------------------

                            // -----------------------------------------------------------------------------
                            // repetimos esto para cada mes
                            QDate finicio;
                            QDate fin;
                            double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
                            int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();
                            m1=m2=m3=m4=m5=m6=m7=m8=m9=m10=m11=m12=0;
                           if (ui.desglose_mes_checkBox->isChecked())
                           {
                            // QDate finicio;
                            // QDate fin;
                            // double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
                            // int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();

                            finicio.setDate(anyo,1,1);
                            fin.setDate(anyo,1,31);
                            if (extract.contains(':')==1) m1=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m1=saldo_cuenta_lista_mes(extract,1);


                            finicio.setDate(anyo,2,1);
                            fin.setDate(anyo,3,1);
                            fin=fin.addDays(-1);
                            if (extract.contains(':')==1) m2=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m2=saldo_cuenta_lista_mes(extract,2);


                            finicio.setDate(anyo,3,1);
                            fin.setDate(anyo,3,31);
                            if (extract.contains(':')==1) m3=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m3=saldo_cuenta_lista_mes(extract,3);


                            finicio.setDate(anyo,4,1);
                            fin.setDate(anyo,4,30);
                            if (extract.contains(':')==1) m4=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m4=saldo_cuenta_lista_mes(extract,4);


                            finicio.setDate(anyo,5,1);
                            fin.setDate(anyo,5,31);
                            if (extract.contains(':')==1) m5=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m5=saldo_cuenta_lista_mes(extract,5);


                            finicio.setDate(anyo,6,1);
                            fin.setDate(anyo,6,30);
                            if (extract.contains(':')==1) m6=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m6=saldo_cuenta_lista_mes(extract,6);


                            finicio.setDate(anyo,7,1);
                            fin.setDate(anyo,7,31);
                            if (extract.contains(':')==1) m7=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m7=saldo_cuenta_lista_mes(extract,7);


                            finicio.setDate(anyo,8,1);
                            fin.setDate(anyo,8,31);
                            if (extract.contains(':')==1) m8=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m8=saldo_cuenta_lista_mes(extract,8);


                            finicio.setDate(anyo,9,1);
                            fin.setDate(anyo,9,30);
                            if (extract.contains(':')==1) m9=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m9=saldo_cuenta_lista_mes(extract,9);


                            finicio.setDate(anyo,10,1);
                            fin.setDate(anyo,10,31);
                            if (extract.contains(':')==1) m10=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m10=saldo_cuenta_lista_mes(extract,10);


                            finicio.setDate(anyo,11,1);
                            fin.setDate(anyo,11,30);
                            if (extract.contains(':')==1) m11=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m11=saldo_cuenta_lista_mes(extract,11);


                            finicio.setDate(anyo,12,1);
                            fin.setDate(anyo,12,31);
                            if (extract.contains(':')==1) m12=cuentaespec(extract,ejercicio,true,
                                                                         finicio,fin);
                               else
                                   m12=saldo_cuenta_lista_mes(extract,12);



                            // if (valor>-0.004 && valor < 0.005) continue;
                            // if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                            // imprimimos cadena formada por código de cuenta y descripción
                                // ahora todos los meses
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m1=-1*m1;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m2=-1*m2;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m3=-1*m3;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m4=-1*m4;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m5=-1*m5;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m6=-1*m6;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m7=-1*m7;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m8=-1*m8;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m9=-1*m9;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m10=-1*m10;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m11=-1*m11;
                             if (inicio-1>=0 && cadpru[inicio-1]=='-') m12=-1*m12;
                             global+= "\t" + formateanumerosep(m1,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m2,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m3,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m4,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m5,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m6,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m7,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m8,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m9,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m10,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m11,comadecimal,decimales);
                             global+= "\t" + formateanumerosep(m12,comadecimal,decimales);
                            }
                            // -----------------------------------------------------------------------------

                            global+= "\n";
                            // seguimos aquí con el desglose de todas las cuentas
                            QSqlQuery q;
                            if (!cod_longitud_variable())
                               {
                                q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                               }
                                else
                                     {
                                   q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                     }
                            while (q.next())
                                 {
                                   QString extract2=q.value(0).toString();
                                   QString extract2sig=extract2;
                                   if (positivo) extract2sig.append('+');
                                   if (negativo) extract2sig.append('-');
                                   if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                        acotado,fecha1,fecha2);
                                   else
                                     {
                                      if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract2sig);
                                       else
                                         valor=calculamediacuenta_aux(extract2sig,true);
                                     }
                                   // --------------------------------------------------------------------------
                                   if (ui.desglose_mes_checkBox->isChecked())
                                   {
                                    finicio.setDate(anyo,1,1);
                                    fin.setDate(anyo,1,31);
                                    if (extract2sig.contains(':')==1) m1=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m1=saldo_cuenta_lista_mes(extract2sig,1);



                                   finicio.setDate(anyo,2,1);
                                   fin.setDate(anyo,3,1);
                                   fin=fin.addDays(-1);
                                   if (extract2sig.contains(':')==1) m2=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m2=saldo_cuenta_lista_mes(extract2sig,2);


                                   finicio.setDate(anyo,3,1);
                                   fin.setDate(anyo,3,31);
                                   if (extract2sig.contains(':')==1) m3=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m3=saldo_cuenta_lista_mes(extract2sig,3);


                                   finicio.setDate(anyo,4,1);
                                   fin.setDate(anyo,4,30);
                                   if (extract2sig.contains(':')==1) m4=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m4=saldo_cuenta_lista_mes(extract2sig,4);


                                   finicio.setDate(anyo,5,1);
                                   fin.setDate(anyo,5,31);
                                   if (extract2sig.contains(':')==1) m5=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m5=saldo_cuenta_lista_mes(extract2sig,5);


                                   finicio.setDate(anyo,6,1);
                                   fin.setDate(anyo,6,30);
                                   if (extract2sig.contains(':')==1) m6=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m6=saldo_cuenta_lista_mes(extract2sig,6);


                                   finicio.setDate(anyo,7,1);
                                   fin.setDate(anyo,7,31);
                                   if (extract2sig.contains(':')==1) m7=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m7=saldo_cuenta_lista_mes(extract2sig,7);


                                   finicio.setDate(anyo,8,1);
                                   fin.setDate(anyo,8,31);
                                   if (extract2sig.contains(':')==1) m8=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m8=saldo_cuenta_lista_mes(extract2sig,8);


                                   finicio.setDate(anyo,9,1);
                                   fin.setDate(anyo,9,30);
                                   if (extract2sig.contains(':')==1) m9=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m9=saldo_cuenta_lista_mes(extract2sig,9);


                                   finicio.setDate(anyo,10,1);
                                   fin.setDate(anyo,10,31);
                                   if (extract2sig.contains(':')==1) m10=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m10=saldo_cuenta_lista_mes(extract2sig,10);


                                   finicio.setDate(anyo,11,1);
                                   fin.setDate(anyo,11,30);
                                   if (extract2sig.contains(':')==1) m11=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m11=saldo_cuenta_lista_mes(extract2sig,11);

                                   finicio.setDate(anyo,12,1);
                                   fin.setDate(anyo,12,31);
                                   if (extract2sig.contains(':')==1) m12=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m12=saldo_cuenta_lista_mes(extract2sig,12);

                                   }
                                   if (valor>0.001 || valor <-0.001)
                                       {
                                         global += extract2 + " " +
                                             filtracad(descripcioncuenta(extract2)) + "\t";
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                                         global += formateanumerosep(valor,comadecimal,decimales) + "\t ";
                                         // ahora todos los meses
                                         if (ui.desglose_mes_checkBox->isChecked())
                                         {
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m1=-1*m1;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m2=-1*m2;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m3=-1*m3;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m4=-1*m4;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m5=-1*m5;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m6=-1*m6;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m7=-1*m7;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m8=-1*m8;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m9=-1*m9;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m10=-1*m10;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m11=-1*m11;
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') m12=-1*m12;
                                         global += "\t\t";
                                         global += formateanumerosep(m1,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m2,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m3,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m4,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m5,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m6,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m7,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m8,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m9,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m10,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m11,comadecimal,decimales) + "\t ";
                                         global += formateanumerosep(m12,comadecimal,decimales);
                                         }
                                         global += "\n";
                                       }
                                   // --------------------------------------------------------------------------
                                  }
                           }
                       }
                    indice++;
                   }
	     }
           }
    }

    global+="\n";
    global+= observaciones;

    cb->setText(global);
    QMessageBox::information( this, tr("Cálculo de Estado Contable"),
                             tr("Se ha pasado el contenido al portapapeles") );
}


void calcestado::generagrafico()
{
 QStringList etiquetas_parte1; // activo
 QStringList etiquetas_parte2; // pasivo
 QList<double> valores_parte1; // activo
 QList<double> valores_parte2; // pasivo
 QList<double> valores_parte1_2; // activo
 QList<double> valores_parte2_2; // pasivo

 QSqlQuery query2 = basedatos::instancia()->select4Estadostituloordernodo( ui.titulolabel->text() , true);

 if (query2.isActive())
 while (query2.next())
    {
     if (query2.value(0).toString().contains('.')) continue;
     etiquetas_parte1 << query2.value(1).toString();
     valores_parte1 << query2.value(2).toDouble();
     valores_parte1_2 << query2.value(3).toDouble();
    }

 query2 = basedatos::instancia()->select5Estadostituloparte1ordernodo( ui.titulolabel->text() , false);
 while (query2.next())
    {
     if (query2.value(0).toString().contains('.')) continue;
     etiquetas_parte2 << query2.value(1).toString();
     valores_parte2 << query2.value(2).toDouble();
     valores_parte2_2 << query2.value(3).toDouble();
    }


 QSqlQuery query = basedatos::instancia()->select4Estadostitulo( ui.titulolabel->text() );

 QString nombreEmpresa;
 QString ejercicio;
 QString ejercicio2;
 QString titulo1;
 QString titulo2;
 if (query.next())
    {
     nombreEmpresa=nombreempresa();
     ejercicio=query.value(2).toString();
     ejercicio2=query.value(3).toString();
     titulo1=query.value(0).toString();
     titulo2=query.value(1).toString();
    }

 estado_grafico *grafico = new estado_grafico();
 grafico->pasa_valores(nombreEmpresa,ejercicio,titulo1,titulo2,etiquetas_parte1,etiquetas_parte2,
         valores_parte1,valores_parte2,ui.titulolabel->text());

 if (ui.Ejercicio2comboBox->currentText()!=noejercicio())
     grafico->pasa_valores(nombreEmpresa,ejercicio2,titulo1,titulo2,etiquetas_parte1,etiquetas_parte2,
             valores_parte1_2,valores_parte2_2,ui.titulolabel->text(),true);

 grafico->exec();
 delete(grafico);

}



double calcestado::cuentaespec_aux( QString extract, QString ejercicio,
                                   bool acotacion, QDate fecha1, QDate fecha2 )
{
   double resultado=0;
  // separamos código de cuenta y especificador
   QString codigo=extract.section(':',0,0);
   QString espec=extract.section(':',1,1);
   if (espec==cadenamedia()) 
      {
        if (ui.Ejercicio1comboBox->currentText()==ejercicio)  resultado=calculamediacuenta_aux(codigo,true);
	else resultado=calculamediacuenta_aux(codigo,false);
      }
      else
          {
           resultado=calculacuentadiario_aux(codigo,ejercicio,espec,acotacion,fecha1,fecha2);
          }
   return resultado;
}


double calcestado::calculacuenta_aux(QString codigo, QString ejercicio,
                                     bool acotacion, QDate fecha1, QDate fecha2)
{
  bool positivo=false;
  bool negativo=false;
  positivo=codigo.contains('+');
  negativo=codigo.contains('-');
  if (positivo && negativo) return 0;
  if (positivo) codigo.remove('+');
  if (negativo) codigo.remove('-');
  if (ui.previsiones->isChecked())
     {
      double valor=prevision(codigo,ejercicio);
      if (positivo && valor<0) return 0;
      if (negativo && valor>0) return 0;
      return valor;
     }
  if (!ui.diariolistWidget->isEnabled() ) return 0;
  QDate fechaini,fechafin;

  fechaini = basedatos::instancia()->selectAperturaejercicios( ejercicio );
  fechafin = basedatos::instancia()->selectCierreejercicios( ejercicio );

  if (acotacion)
     {
      fechaini=fecha1;
      fechafin=fecha2;
     }

  QString cadena;
  double resultado=0;

  QString subcadena="";
  // ESPECIFICAMOS DIARIOS SEGÚN CONTENIDO DE LA LISTA
  int numdiarios=ui.diariolistWidget->count();
  for (int veces=0;veces<numdiarios;veces++)
     {
      if (subcadena=="") subcadena=subcadena+" and (";
        else subcadena=subcadena+" or ";
      subcadena+="diario='";
      ui.diariolistWidget->setCurrentRow(veces);
      QString caddia=ui.diariolistWidget->currentItem()->text();
      if (caddia!=diario_no_asignado())
         subcadena+=caddia.left(-1).replace("'","''");
      subcadena+="'";
     }
  if (subcadena.length()>0) subcadena+=")";

  QSqlQuery query = basedatos::instancia()->selectCuentadebehaberdiariocuentaigualfechasgroupcuenta( codigo, fechaini, fechafin, subcadena, ui.cilineEdit->text() );;
  if ( query.isActive() ) {
     while ( query.next() ) 
           {
            if (positivo || negativo)
               {
                if (positivo && query.value(1).toDouble()>0) resultado+=query.value(1).toDouble();
                if (negativo && query.value(1).toDouble()<0) resultado+=query.value(1).toDouble();
               }
               else
	           resultado+=query.value(1).toDouble();
           }
     }
  return resultado;
}


double calcestado::calculamediacuenta_aux(QString codigo,bool ejercicio1)
{
  bool positivo=false;
  bool negativo=false;
  positivo=codigo.contains('+');
  negativo=codigo.contains('-');
  if (positivo && negativo) return 0;
  if (positivo) codigo.remove('+');
  if (negativo) codigo.remove('-');

    return basedatos::instancia()->selectSumsaldomediosaldossubcuentaigualcodigo(ejercicio1,codigo,positivo,negativo);
}


double calcestado::calculacuentadiario_aux( QString codigo, QString ejercicio, QString qdiario,
                                           bool acotacion, QDate fecha1, QDate fecha2 )
{
  double resultado=0;
  bool positivo=false;
  bool negativo=false;
  positivo=codigo.contains('+');
  negativo=codigo.contains('-');
  if (positivo && negativo) return 0;
  if (positivo) codigo.remove('+');
  if (negativo) codigo.remove('-');
  if (qdiario==previsiones())
     {
      if (positivo) return previsionsegunsaldo_aux(codigo,ejercicio,true);
      if (negativo) return previsionsegunsaldo_aux(codigo,ejercicio,false);
      return prevision(codigo,ejercicio);;
     }
  if (!ui.diariolistWidget->isEnabled() ) return 0;
  QDate fechaini,fechafin;

  fechaini = basedatos::instancia()->selectAperturaejercicios(ejercicio);
  fechafin = basedatos::instancia()->selectCierreejercicios(ejercicio);

  if (acotacion)
     {
      fechaini=fecha1;
      fechafin=fecha2;
     }

    QString subcadena = "' and diario='";
    if (qdiario!=diario_no_asignado())
        subcadena += qdiario.left(-1).replace("'","''");
    subcadena += "'";

    QSqlQuery query = basedatos::instancia()->selectCuentadebehaberdiariocuentaigualfechasgroupcuenta(codigo, fechaini, fechafin, subcadena,"");


  resultado=0;
  if ( query.isActive() ) {
     while ( query.next() ) 
           {
            if (positivo || negativo)
               {
                if (positivo && query.value(1).toDouble()>0) resultado+=query.value(1).toDouble();
                if (negativo && query.value(1).toDouble()<0) resultado+=query.value(1).toDouble();
               }
               else
	           resultado+=query.value(1).toDouble();
           }
     }
  return resultado;
}


void calcestado::latex()
{

    QString qfichero;
    if (ui.texlineEdit->text().length()==0)
       qfichero=tr("estadocontable");
    else
          {
             qfichero=ui.texlineEdit->text();
             if (ui.texlineEdit->text().right(4)==".tex") qfichero=qfichero.left(qfichero.length()-4);
          }

     generalatex();

    int valor=editalatex(qfichero);
    if (valor==1)
        QMessageBox::warning( this, tr("ESTADOS CONTABLES"),tr("PROBLEMAS al llamar al editor Latex"));

}



// jsala
double calcestado::calculacuentaci(QString const & codigo, QString const & ejercicio,
                                   bool acotacion,
                                 QDate const & fecha1, QDate const & fecha2)
{
  // separamos código en cuenta y ci
   QString cuenta=codigo.section(';',0,0);
   QString ci=codigo.section(';',1,1);
   return calculacuenta(cuenta,ejercicio,acotacion,fecha1,fecha2,ci);
}


void calcestado::genera_xml()
{
    QProgressDialog progreso(tr("Generando Informe ..."), 0, 0, 3);
    progreso.setWindowTitle("Estados Contables");

    QApplication::processEvents();

    // if (condesglose || condesglosectas) { generalatex2(); return; }
    if (auxiliares.count()==0)
       if (condesglose || condesglosectas) { calculaestado(); }

    progreso.setValue(1);
    QApplication::processEvents();

    QString ejercicio1="";
    QString ejercicio2="";
    QString parte1="";
    QString parte2="";
    QString formulabasepor;
    QString observaciones;
    QString cabecera;
    double valorbasepor1=0;
    double valorbasepor2=0;
    // bool estadosmedios=false;
    bool analitica,haycolref;
    QString ci,colref;
    QSqlQuery query = basedatos::instancia()->select14Cabeceraestadostitulo( ui.titulolabel->text() );

    if ( (query.isActive()) && (query.first()) ) {
       parte1=query.value(0).toString();
           parte2=query.value(1).toString();
           ejercicio1=query.value(2).toString();
           ejercicio2=query.value(3).toString();
           // estadosmedios=query.value(4).toBool();
           formulabasepor=query.value(5).toString();
           valorbasepor1=query.value(6).toDouble();
           valorbasepor2=query.value(7).toDouble();
           observaciones=query.value(8).toString();
           analitica=query.value(9).toBool();
           ci=query.value(10).toString();
           haycolref=query.value(11).toBool();
           colref=query.value(12).toString();
           cabecera=query.value(13).toString();
       } else return;
    bool hayporcentajes=false;
    double porcentaje=0;

    if (formulabasepor.length()>0 && (valorbasepor1>0.001 || valorbasepor1<0.001)
        && (valorbasepor2>0.001 || valorbasepor2<0.001)) hayporcentajes=true;


    QDomDocument doc("Estado");
    QDomElement root = doc.createElement("Estado");
    doc.appendChild(root);

    addElementoTextoDom(doc,root,"NombreEmpresa",filtracadxml(nombreempresa()));
    QDomElement tag = doc.createElement("Cabecera");
    root.appendChild(tag);
    addElementoTextoDom(doc,tag,"Titulo",filtracadxml(ui.titulolabel->text()));
    addElementoTextoDom(doc,tag,"Parte1",filtracadxml(parte1));
    addElementoTextoDom(doc,tag,"Parte2",filtracadxml(parte2));
    addElementoTextoDom(doc,tag,"Ejercicio1",filtracadxml(ejercicio1));
    addElementoTextoDom(doc,tag,"Ejercicio2",filtracadxml(ejercicio2));
    addElementoTextoDom(doc,tag,"Valorbasepor1",formateanumero(valorbasepor1,comadecimal,decimales));
    addElementoTextoDom(doc,tag,"Valorbasepor2",formateanumero(valorbasepor2,comadecimal,decimales));
    addElementoTextoDom(doc,tag,"Observaciones",filtracadxml(observaciones));
    addElementoTextoDom(doc,tag,"Analitica",analitica ? "true" : "false");
    addElementoTextoDom(doc,tag,"Ci",filtracadxml(ci));
    addElementoTextoDom(doc,tag,"Colref",filtracadxml(colref));
    addElementoTextoDom(doc,tag,"Cabecera",filtracadxml(cabecera));
    QString fechaini1,fechaini2, fechafin1, fechafin2;
    if (ui.fechas_ej1_groupBox->isChecked())
      {
       fechaini1=ui.ini_ej1_dateEdit->date().toString("yyyy/MM/dd");
       fechafin1=ui.fin_ej1_dateEdit->date().toString("yyyy/MM/dd");
      }
    if (ui.fechas_ej2_groupBox->isChecked())
      {
        fechaini2=ui.ini_ej2_dateEdit->date().toString("yyyy/MM/dd");
        fechafin2=ui.fin_ej2_dateEdit->date().toString("yyyy/MM/dd");
      }
    addElementoTextoDom(doc,tag,"FechaIni1",filtracadxml(fechaini1));
    addElementoTextoDom(doc,tag,"FechaFin1",filtracadxml(fechafin1));
    addElementoTextoDom(doc,tag,"FechaIni2",filtracadxml(fechaini2));
    addElementoTextoDom(doc,tag,"FechaFin2",filtracadxml(fechafin2));
    QString previsiones;
    if (ui.previsiones->isChecked()) previsiones=tr("Previsiones");
    addElementoTextoDom(doc,tag,"Previsiones",filtracadxml(previsiones));



    QSqlQuery query2 = basedatos::instancia()->select6Estadostituloparte1ordernodo( ui.titulolabel->text() , true );
    int veces;
    if ( query2.isActive() ) {
             while ( query2.next() ) {
               if (query2.value(0).toString().contains("LINEA")>0)
                 {
                   continue;
                 }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001
                    && query2.value(2).toDouble()>-0.0001
                    && ejercicio2.length()==0) continue;
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001
                    && query2.value(2).toDouble()>-0.0001
                    && query2.value(3).toDouble()<0.0001
                    && query2.value(3).toDouble()>-0.0001
                    && ejercicio2.length()>0) continue;
               // ----------------------------------------------------------------------
               QDomElement tag2 = doc.createElement("Linea");
               root.appendChild(tag2);
               addElementoTextoDom(doc,tag2,"Parte",parte1);
               QString espacios;
               for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
                   espacios += "  ";
               // apartado
               addElementoTextoDom(doc,tag2,"Apartado",
                                   espacios+filtracadxml(query2.value(1).toString()));
               if (haycolref)
                   addElementoTextoDom(doc,tag2,"ColRef",
                                       filtracadxml(query2.value(4).toString()));
               if (hayporcentajes)
                         {
                            porcentaje=query2.value(2).toDouble()/valorbasepor1*100;
                            QString cadpor= formateanumero(porcentaje,comadecimal,decimales);
                            addElementoTextoDom(doc,tag2,"Porcentaje1",
                                                filtracadxml(cadpor));
                         }
                       //---------------------------------------------
               // IMPORTE 1
               QString importe1;
               if (query2.value(2).toDouble()>0.0001 ||
                   query2.value(2).toDouble()<-0.0001)
                   importe1=formateanumero(
                           query2.value(2).toDouble(),comadecimal,decimales);
               addElementoTextoDom(doc,tag2,"Importe1",
                                       filtracadxml(importe1));
               addElementoTextoDom(doc,tag2,"ImporteCuenta","");
               addElementoTextoDom(doc,tag2,"ImporteAuxiliar","");
               // IMPORTE 2
               if (ejercicio2.length()>0)
                  {
                     if (hayporcentajes)
                      {
                         porcentaje=query2.value(3).toDouble()/valorbasepor2*100;
                         QString cadpor= formateanumero(porcentaje,comadecimal,decimales);
                         addElementoTextoDom(doc,tag2,"Porcentaje2",
                                             filtracadxml(cadpor));
                      }
                     QString importe2;
                     if (query2.value(3).toDouble()>0.0001 ||
                         query2.value(3).toDouble()<-0.0001)
                         importe2=formateanumero(
                                 query2.value(3).toDouble(),comadecimal,decimales);
                    addElementoTextoDom(doc,tag2,"Importe2",
                                           filtracadxml(importe2));
                  }
               // verificamos si hay desglose por meses y añadimos m1 a m12
               // ------------------------------------------------------------
               if (ui.desglose_mes_checkBox->isChecked())
                  {
                   double num=0; QString cadnummes;
                   num=query2.value(6).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes1",cadnummes);

                   num=query2.value(7).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes2",cadnummes);

                   num=query2.value(8).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes3",cadnummes);

                   num=query2.value(9).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes4",cadnummes);

                   num=query2.value(10).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes5",cadnummes);

                   num=query2.value(11).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes6",cadnummes);

                   num=query2.value(12).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes7",cadnummes);

                   num=query2.value(13).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes8",cadnummes);

                   num=query2.value(14).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes9",cadnummes);

                   num=query2.value(15).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes10",cadnummes);

                   num=query2.value(16).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes11",cadnummes);

                   num=query2.value(17).toDouble();
                   cadnummes=(num>0.001 || num<-0.001) ?
                        formateanumerosep(num,comadecimal,decimales) : QString();
                   addElementoTextoDom(doc,tag2,"Mes12",cadnummes);

                  } // if desglose por meses
               // ------------------------------------------------------------
               if (!condesglose && !condesglosectas) continue;
               // desglosamos el campo fórmula que no contenga {
               QString cadpru=query2.value(5).toString();
               if (cadpru.contains('{')) continue;

               int indice=0;
               int inicio,final;
               QString ejercicio=ui.Ejercicio1comboBox->currentText();
               bool acotado=ui.fechas_ej1_groupBox->isChecked();
               QDate fecha1=ui.ini_ej1_dateEdit->date();
               QDate fecha2=ui.fin_ej1_dateEdit->date();
               bool estadosmedios=consaldosmedios;
               double valor;
               QString extract;
               while (indice<cadpru.length())
                   {
                    if (cadpru[indice]=='[')
                        {
                         inicio=indice;
                         final=inicio;
                         while (final<cadpru.length() && cadpru[final]!=']') final++;
                         // extraemos la cadena de la cuenta sin corchetes
                         extract=cadpru.mid(inicio+1,final-inicio-1);
                        // habría que calcular el valor de la cuenta
                        // QMessageBox::warning( this, tr("Estados Contables"),extract);
                        // -------------------------------------------------------------------------------------
                        if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotado,fecha1,fecha2);
                           else
                              {
                               if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract);
                                 else
                                      valor=calculamediacuenta(extract,true);
                              }
                        // añadimos desglose por meses aquí
                           // repetimos esto para cada mes
                           QDate finicio;
                           QDate fin;
                           double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
                           m1=m2=m3=m4=m5=m6=m7=m8=m9=m10=m11=m12=0;
                           int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();
                           if (ui.desglose_mes_checkBox->isChecked())
                             {

                               finicio.setDate(anyo,1,1);
                               fin.setDate(anyo,1,31);
                               if (extract.contains(':')==1) m1=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m1=saldo_cuenta_lista_mes(extract,1);


                               finicio.setDate(anyo,2,1);
                               fin.setDate(anyo,3,1);
                               fin=fin.addDays(-1);
                               if (extract.contains(':')==1) m2=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m2=saldo_cuenta_lista_mes(extract,2);

                               finicio.setDate(anyo,3,1);
                               fin.setDate(anyo,3,31);
                               if (extract.contains(':')==1) m3=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m3=saldo_cuenta_lista_mes(extract,3);

                               finicio.setDate(anyo,4,1);
                               fin.setDate(anyo,4,30);
                               if (extract.contains(':')==1) m4=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m4=saldo_cuenta_lista_mes(extract,4);

                               finicio.setDate(anyo,5,1);
                               fin.setDate(anyo,5,31);
                               if (extract.contains(':')==1) m5=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m5=saldo_cuenta_lista_mes(extract,5);

                               finicio.setDate(anyo,6,1);
                               fin.setDate(anyo,6,30);
                               if (extract.contains(':')==1) m6=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m6=saldo_cuenta_lista_mes(extract,6);

                               finicio.setDate(anyo,7,1);
                               fin.setDate(anyo,7,31);
                               if (extract.contains(':')==1) m7=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m7=saldo_cuenta_lista_mes(extract,7);

                               finicio.setDate(anyo,8,1);
                               fin.setDate(anyo,8,31);
                               if (extract.contains(':')==1) m8=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m8=saldo_cuenta_lista_mes(extract,8);

                               finicio.setDate(anyo,9,1);
                               fin.setDate(anyo,9,30);
                               if (extract.contains(':')==1) m9=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m9=saldo_cuenta_lista_mes(extract,9);

                               finicio.setDate(anyo,10,1);
                               fin.setDate(anyo,10,31);
                               if (extract.contains(':')==1) m10=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m10=saldo_cuenta_lista_mes(extract,10);

                               finicio.setDate(anyo,11,1);
                               fin.setDate(anyo,11,30);
                               if (extract.contains(':')==1) m11=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      m11=saldo_cuenta_lista_mes(extract,11);
                               finicio.setDate(anyo,12,1);
                               fin.setDate(anyo,12,31);
                               if (extract.contains(':')==1) m12=cuentaespec(extract,ejercicio,true,
                                                                            finicio,fin);
                                  else
                                      saldo_cuenta_lista_mes(extract,12);
                             }
                        // if (valor>-0.004 && valor < 0.005) continue;
                        if (inicio-1>=0 && cadpru[inicio-1]=='-')
                           {
                            valor=-1*valor;
                            m1=-1*m1;
                            m2=-1*m2;
                            m3=-1*m3;
                            m4=-1*m4;
                            m5=-1*m5;
                            m6=-1*m6;
                            m7=-1*m7;
                            m8=-1*m8;
                            m9=-1*m9;
                            m10=-1*m10;
                            m11=-1*m11;
                            m12=-1*m12;
                           }
                        // imprimimos cadena formada por código de cuenta y descripción
                        bool positivo=false;
                        bool negativo=false;
                        positivo=extract.contains('+');
                        negativo=extract.contains('-');
                        extract.remove('+');
                        extract.remove('-');
                        if (valor>0.001 || valor<-0.001)
                           {
                            QDomElement tag3 = doc.createElement("Linea");
                            root.appendChild(tag3);
                            addElementoTextoDom(doc,tag3,"Parte",parte1);
                            // apartado
                            addElementoTextoDom(doc,tag3,"Apartado",
                                               "    " +extract+ " "+filtracadxml(descripcioncuenta(extract)));
                            addElementoTextoDom(doc,tag3,"ImporteCuenta",
                                  filtracadxml(formateanumerosep(valor,comadecimal,decimales)));
                            addElementoTextoDom(doc,tag3,"ImporteAuxiliar","");
                            addElementoTextoDom(doc,tag3,"Importe1","");
                            addElementoTextoDom(doc,tag3,"Importe2","");
                            if (ui.desglose_mes_checkBox->isChecked())
                              {
                                QString cadnummes;
                                cadnummes=(m1>0.001 || m1<-0.001) ?
                                          formateanumerosep(m1,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes1",cadnummes);
                                cadnummes=(m2>0.001 || m2<-0.001) ?
                                          formateanumerosep(m2,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes2",cadnummes);
                                cadnummes=(m3>0.001 || m3<-0.001) ?
                                          formateanumerosep(m3,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes3",cadnummes);
                                cadnummes=(m4>0.001 || m4<-0.001) ?
                                          formateanumerosep(m4,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes4",cadnummes);
                                cadnummes=(m5>0.001 || m5<-0.001) ?
                                          formateanumerosep(m5,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes5",cadnummes);
                                cadnummes=(m6>0.001 || m6<-0.001) ?
                                          formateanumerosep(m6,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes6",cadnummes);
                                cadnummes=(m7>0.001 || m7<-0.001) ?
                                          formateanumerosep(m7,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes7",cadnummes);
                                cadnummes=(m8>0.001 || m8<-0.001) ?
                                          formateanumerosep(m8,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes8",cadnummes);
                                cadnummes=(m9>0.001 || m9<-0.001) ?
                                          formateanumerosep(m9,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes9",cadnummes);
                                cadnummes=(m10>0.001 || m10<-0.001) ?
                                          formateanumerosep(m10,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes10",cadnummes);
                                cadnummes=(m11>0.001 || m11<-0.001) ?
                                          formateanumerosep(m11,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes11",cadnummes);
                                cadnummes=(m12>0.001 || m12<-0.001) ?
                                          formateanumerosep(m12,comadecimal,decimales) : QString();
                                addElementoTextoDom(doc,tag3,"Mes12",cadnummes);
                              }
                            if (condesglose)
                            {
                             QSqlQuery q;
                             if (!cod_longitud_variable())
                               {
                                q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                               }
                               else
                                   {
                                    q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                   }
                              while (q.next())
                                 {
                                   QString extract2=q.value(0).toString();
                                   QString extract2sig=extract2;
                                   if (positivo) extract2sig.append('+');
                                   if (negativo) extract2sig.append('-');
                                   if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                       acotado,fecha1,fecha2);
                                   else
                                   {
                                    if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract2sig);
                                    else
                                      valor=calculamediacuenta_aux(extract2sig,true);
                                   }
                                   finicio.setDate(anyo,1,1);
                                   fin.setDate(anyo,1,31);
                                   if (extract.contains(':')==1) m1=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m1=saldo_cuenta_lista_mes(extract2sig,1);


                                   finicio.setDate(anyo,2,1);
                                   fin.setDate(anyo,3,1);
                                   fin=fin.addDays(-1);
                                   if (extract.contains(':')==1) m2=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m2=saldo_cuenta_lista_mes(extract2sig,2);

                                   finicio.setDate(anyo,3,1);
                                   fin.setDate(anyo,3,31);
                                   if (extract.contains(':')==1) m3=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m3=saldo_cuenta_lista_mes(extract2sig,3);

                                   finicio.setDate(anyo,4,1);
                                   fin.setDate(anyo,4,30);
                                   if (extract.contains(':')==1) m4=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m4=saldo_cuenta_lista_mes(extract2sig,4);
                                   finicio.setDate(anyo,5,1);
                                   fin.setDate(anyo,5,31);
                                   if (extract.contains(':')==1) m5=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m5=saldo_cuenta_lista_mes(extract2sig,5);

                                   finicio.setDate(anyo,6,1);
                                   fin.setDate(anyo,6,30);
                                   if (extract.contains(':')==1) m6=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m6=saldo_cuenta_lista_mes(extract2sig,6);

                                   finicio.setDate(anyo,7,1);
                                   fin.setDate(anyo,7,31);
                                   if (extract.contains(':')==1) m7=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m7=saldo_cuenta_lista_mes(extract2sig,7);

                                   finicio.setDate(anyo,8,1);
                                   fin.setDate(anyo,8,31);
                                   if (extract.contains(':')==1) m8=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m8=saldo_cuenta_lista_mes(extract2sig,8);

                                   finicio.setDate(anyo,9,1);
                                   fin.setDate(anyo,9,30);
                                   if (extract.contains(':')==1) m9=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m9=saldo_cuenta_lista_mes(extract2sig,9);

                                   finicio.setDate(anyo,10,1);
                                   fin.setDate(anyo,10,31);
                                   if (extract.contains(':')==1) m10=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m10=saldo_cuenta_lista_mes(extract2sig,10);

                                   finicio.setDate(anyo,11,1);
                                   fin.setDate(anyo,11,30);
                                   if (extract.contains(':')==1) m11=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m11=saldo_cuenta_lista_mes(extract2sig,11);

                                   finicio.setDate(anyo,12,1);
                                   fin.setDate(anyo,12,31);
                                   if (extract.contains(':')==1) m12=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m12=saldo_cuenta_lista_mes(extract2sig,12);

                                   if (valor>0.001 || valor <-0.001)
                                       {
                                         QDomElement tag3 = doc.createElement("Linea");
                                         root.appendChild(tag3);
                                         addElementoTextoDom(doc,tag3,"Parte",parte1);
                                         // apartado
                                         addElementoTextoDom(doc,tag3,"Apartado",
                                                    "     "+extract2+" "+
                                                    filtracadxml(descripcioncuenta(extract2)));
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-')
                                            {
                                             valor=-1*valor;
                                             m1=-1*m1;
                                             m2=-1*m2;
                                             m3=-1*m3;
                                             m4=-1*m4;
                                             m5=-1*m5;
                                             m6=-1*m6;
                                             m7=-1*m7;
                                             m8=-1*m8;
                                             m9=-1*m9;
                                             m10=-1*m10;
                                             m11=-1*m11;
                                             m12=-1*m12;
                                            }
                                         addElementoTextoDom(doc,tag3,"ImporteAuxiliar",
                                               filtracadxml(formateanumerosep(valor,comadecimal,decimales)));
                                         addElementoTextoDom(doc,tag3,"ImporteCuenta","");
                                         addElementoTextoDom(doc,tag3,"Importe1","");
                                         addElementoTextoDom(doc,tag3,"Importe2","");
                                         if (ui.desglose_mes_checkBox->isChecked())
                                           {
                                             QString cadnummes;
                                             cadnummes=(m1>0.001 || m1<-0.001) ?
                                                       formateanumerosep(m1,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes1",cadnummes);
                                             cadnummes=(m2>0.001 || m2<-0.001) ?
                                                       formateanumerosep(m2,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes2",cadnummes);
                                             cadnummes=(m3>0.001 || m3<-0.001) ?
                                                       formateanumerosep(m3,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes3",cadnummes);
                                             cadnummes=(m4>0.001 || m4<-0.001) ?
                                                       formateanumerosep(m4,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes4",cadnummes);
                                             cadnummes=(m5>0.001 || m5<-0.001) ?
                                                       formateanumerosep(m5,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes5",cadnummes);
                                             cadnummes=(m6>0.001 || m6<-0.001) ?
                                                       formateanumerosep(m6,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes6",cadnummes);
                                             cadnummes=(m7>0.001 || m7<-0.001) ?
                                                       formateanumerosep(m7,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes7",cadnummes);
                                             cadnummes=(m8>0.001 || m8<-0.001) ?
                                                       formateanumerosep(m8,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes8",cadnummes);
                                             cadnummes=(m9>0.001 || m9<-0.001) ?
                                                       formateanumerosep(m9,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes9",cadnummes);
                                             cadnummes=(m10>0.001 || m10<-0.001) ?
                                                       formateanumerosep(m10,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes10",cadnummes);
                                             cadnummes=(m11>0.001 || m11<-0.001) ?
                                                       formateanumerosep(m11,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes11",cadnummes);
                                             cadnummes=(m12>0.001 || m12<-0.001) ?
                                                       formateanumerosep(m12,comadecimal,decimales) : QString();
                                             addElementoTextoDom(doc,tag3,"Mes12",cadnummes);
                                           }
                                       }
                                  }
                               } // fin del condesglose
                           } // if valor no es cero
                    } // del cadpru índice
                  indice++;
                } // del while índice

               // -----------------------------------------------------------

             } // del while query next
           }

    progreso.setValue(2);
    QApplication::processEvents();

    if (parte2.length()>0)
     {
       query2 = basedatos::instancia()->select6Estadostituloparte1ordernodo(ui.titulolabel->text(),
                                                                            false );
       if ( query2.isActive() ) {
                while ( query2.next() ) {
                  // ----------------------------------------------------------------------
                  if (ui.importescheckBox->isChecked()
                       && query2.value(2).toDouble()<0.0001
                       && query2.value(2).toDouble()>-0.0001
                       && ejercicio2.length()==0) continue;
                  // ----------------------------------------------------------------------
                  if (ui.importescheckBox->isChecked()
                       && query2.value(2).toDouble()<0.0001
                       && query2.value(2).toDouble()>-0.0001
                       && query2.value(3).toDouble()<0.0001
                       && query2.value(3).toDouble()>-0.0001
                       && ejercicio2.length()>0) continue;
                  // ----------------------------------------------------------------------
                  QDomElement tag2 = doc.createElement("Linea");
                  root.appendChild(tag2);
                  addElementoTextoDom(doc,tag2,"Parte", parte2);
                  QString espacios;
                  for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
                      espacios += "  ";
                  // apartado
                  addElementoTextoDom(doc,tag2,"Apartado",
                                      espacios+filtracadxml(query2.value(1).toString()));

                  if (haycolref)
                      addElementoTextoDom(doc,tag2,"ColRef",
                                          filtracadxml(query2.value(4).toString()));


                  if (hayporcentajes)
                            {
                               porcentaje=query2.value(2).toDouble()/valorbasepor1*100;
                               QString cadpor= formateanumero(porcentaje,comadecimal,decimales);
                               addElementoTextoDom(doc,tag2,"Porcentaje1",
                                                  filtracadxml(cadpor));
                            }
                  QString importe1;
                  if (query2.value(2).toDouble()>0.0001 ||
                      query2.value(2).toDouble()<-0.0001)
                      importe1=formateanumero(
                              query2.value(2).toDouble(),comadecimal,decimales);
                  addElementoTextoDom(doc,tag2,"Importe1",
                                      filtracadxml(importe1));
                  addElementoTextoDom(doc,tag2,"ImporteCuenta","");
                  addElementoTextoDom(doc,tag2,"ImporteAuxiliar","");
                  if (ejercicio2.length()>0)
                       {
                        if (hayporcentajes)
                          {
                             porcentaje=query2.value(3).toDouble()/valorbasepor2*100;
                             QString cadpor= formateanumero(porcentaje,comadecimal,decimales);
                             addElementoTextoDom(doc,tag2,"Porcentaje2",
                                                  filtracadxml(cadpor));
                          }
                        QString importe2;
                        if (query2.value(3).toDouble()>0.0001 ||
                            query2.value(3).toDouble()<-0.0001)
                            importe2=formateanumerosep(
                                    query2.value(3).toDouble(),comadecimal,decimales);
                         addElementoTextoDom(doc,tag2,"Importe2",
                                                filtracadxml(importe2));
                       }
                  // verificamos si hay desglose por meses y añadimos m1 a m12
                  // ------------------------------------------------------------
                  if (ui.desglose_mes_checkBox->isChecked())
                     {
                      double num=0; QString cadnummes;
                      num=query2.value(6).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes1",cadnummes);

                      num=query2.value(7).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes2",cadnummes);

                      num=query2.value(8).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes3",cadnummes);

                      num=query2.value(9).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes4",cadnummes);

                      num=query2.value(10).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes5",cadnummes);

                      num=query2.value(11).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes6",cadnummes);

                      num=query2.value(12).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes7",cadnummes);

                      num=query2.value(13).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes8",cadnummes);

                      num=query2.value(14).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes9",cadnummes);

                      num=query2.value(15).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes10",cadnummes);

                      num=query2.value(16).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes11",cadnummes);

                      num=query2.value(17).toDouble();
                      cadnummes=(num>0.001 || num<-0.001) ?
                           formateanumerosep(num,comadecimal,decimales) : QString();
                      addElementoTextoDom(doc,tag2,"Mes12",cadnummes);

                     } // if desglose por meses

                  // -----------------------------------------------------------
                  if (!condesglose && !condesglosectas) continue;
                  // desglosamos el campo fórmula que no contenga {
                  QString cadpru=query2.value(5).toString();
                  if (cadpru.contains('{')) continue;

                  int indice=0;
                  int inicio,final;
                  QString ejercicio=ui.Ejercicio1comboBox->currentText();
                  bool acotado=ui.fechas_ej1_groupBox->isChecked();
                  QDate fecha1=ui.ini_ej1_dateEdit->date();
                  QDate fecha2=ui.fin_ej1_dateEdit->date();
                  bool estadosmedios=consaldosmedios;
                  double valor;
                  QString extract;
                  while (indice<cadpru.length())
                      {
                       if (cadpru[indice]=='[')
                           {
                            inicio=indice;
                            final=inicio;
                            while (final<cadpru.length() && cadpru[final]!=']') final++;
                            // extraemos la cadena de la cuenta sin corchetes
                            extract=cadpru.mid(inicio+1,final-inicio-1);
                           // habría que calcular el valor de la cuenta
                           // QMessageBox::warning( this, tr("Estados Contables"),extract);
                           // -------------------------------------------------------------------------------------
                           if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotado,fecha1,fecha2);
                              else
                                 {
                                  if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract);
                                    else
                                         valor=calculamediacuenta(extract,true);
                                 }
                              // añadimos desglose por meses aquí
                                 // repetimos esto para cada mes
                                 QDate finicio;
                                 QDate fin;
                                 double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
                                 m1=m2=m3=m4=m5=m6=m7=m8=m9=m10=m11=m12=0;
                                 int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();
                                 if (ui.desglose_mes_checkBox->isChecked())
                                   {

                                     finicio.setDate(anyo,1,1);
                                     fin.setDate(anyo,1,31);
                                     if (extract.contains(':')==1) m1=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m1=saldo_cuenta_lista_mes(extract,1);


                                     finicio.setDate(anyo,2,1);
                                     fin.setDate(anyo,3,1);
                                     fin=fin.addDays(-1);
                                     if (extract.contains(':')==1) m2=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m2=saldo_cuenta_lista_mes(extract,2);

                                     finicio.setDate(anyo,3,1);
                                     fin.setDate(anyo,3,31);
                                     if (extract.contains(':')==1) m3=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m3=saldo_cuenta_lista_mes(extract,3);

                                     finicio.setDate(anyo,4,1);
                                     fin.setDate(anyo,4,30);
                                     if (extract.contains(':')==1) m4=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m4=saldo_cuenta_lista_mes(extract,4);

                                     finicio.setDate(anyo,5,1);
                                     fin.setDate(anyo,5,31);
                                     if (extract.contains(':')==1) m5=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m5=saldo_cuenta_lista_mes(extract,5);

                                     finicio.setDate(anyo,6,1);
                                     fin.setDate(anyo,6,30);
                                     if (extract.contains(':')==1) m6=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m6=saldo_cuenta_lista_mes(extract,6);

                                     finicio.setDate(anyo,7,1);
                                     fin.setDate(anyo,7,31);
                                     if (extract.contains(':')==1) m7=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m7=saldo_cuenta_lista_mes(extract,7);

                                     finicio.setDate(anyo,8,1);
                                     fin.setDate(anyo,8,31);
                                     if (extract.contains(':')==1) m8=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m8=saldo_cuenta_lista_mes(extract,8);

                                     finicio.setDate(anyo,9,1);
                                     fin.setDate(anyo,9,30);
                                     if (extract.contains(':')==1) m9=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m9=saldo_cuenta_lista_mes(extract,9);

                                     finicio.setDate(anyo,10,1);
                                     fin.setDate(anyo,10,31);
                                     if (extract.contains(':')==1) m10=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m10=saldo_cuenta_lista_mes(extract,10);

                                     finicio.setDate(anyo,11,1);
                                     fin.setDate(anyo,11,30);
                                     if (extract.contains(':')==1) m11=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m11=saldo_cuenta_lista_mes(extract,11);

                                     finicio.setDate(anyo,12,1);
                                     fin.setDate(anyo,12,31);
                                     if (extract.contains(':')==1) m12=cuentaespec(extract,ejercicio,true,
                                                                                  finicio,fin);
                                        else
                                            m12=saldo_cuenta_lista_mes(extract,12);
                                   }
                              // if (valor>-0.004 && valor < 0.005) continue;
                              if (inicio-1>=0 && cadpru[inicio-1]=='-')
                                 {
                                  valor=-1*valor;
                                  m1=-1*m1;
                                  m2=-1*m2;
                                  m3=-1*m3;
                                  m4=-1*m4;
                                  m5=-1*m5;
                                  m6=-1*m6;
                                  m7=-1*m7;
                                  m8=-1*m8;
                                  m9=-1*m9;
                                  m10=-1*m10;
                                  m11=-1*m11;
                                  m12=-1*m12;
                                 }

                           // if (valor>-0.004 && valor < 0.005) continue;
                           // if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                           // imprimimos cadena formada por código de cuenta y descripción
                           bool positivo=false;
                           bool negativo=false;
                           positivo=extract.contains('+');
                           negativo=extract.contains('-');
                           extract.remove('+');
                           extract.remove('-');
                           if (valor>0.001 || valor<-0.001)
                              {
                               QDomElement tag3 = doc.createElement("Linea");
                               root.appendChild(tag3);
                               addElementoTextoDom(doc,tag3,"Parte",parte2);
                               // apartado
                               addElementoTextoDom(doc,tag3,"Apartado",
                                                 "    " + extract+ " "+filtracadxml(descripcioncuenta(extract)));
                               addElementoTextoDom(doc,tag3,"ImporteCuenta",
                                     filtracadxml(formateanumerosep(valor,comadecimal,decimales)));
                               addElementoTextoDom(doc,tag3,"ImporteAuxiliar","");
                               addElementoTextoDom(doc,tag3,"Importe1","");
                               addElementoTextoDom(doc,tag3,"Importe2","");
                               if (ui.desglose_mes_checkBox->isChecked())
                                 {
                                   QString cadnummes;
                                   cadnummes=(m1>0.001 || m1<-0.001) ?
                                             formateanumerosep(m1,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes1",cadnummes);
                                   cadnummes=(m2>0.001 || m2<-0.001) ?
                                             formateanumerosep(m2,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes2",cadnummes);
                                   cadnummes=(m3>0.001 || m3<-0.001) ?
                                             formateanumerosep(m3,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes3",cadnummes);
                                   cadnummes=(m4>0.001 || m4<-0.001) ?
                                             formateanumerosep(m4,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes4",cadnummes);
                                   cadnummes=(m5>0.001 || m5<-0.001) ?
                                             formateanumerosep(m5,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes5",cadnummes);
                                   cadnummes=(m6>0.001 || m6<-0.001) ?
                                             formateanumerosep(m6,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes6",cadnummes);
                                   cadnummes=(m7>0.001 || m7<-0.001) ?
                                             formateanumerosep(m7,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes7",cadnummes);
                                   cadnummes=(m8>0.001 || m8<-0.001) ?
                                             formateanumerosep(m8,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes8",cadnummes);
                                   cadnummes=(m9>0.001 || m9<-0.001) ?
                                             formateanumerosep(m9,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes9",cadnummes);
                                   cadnummes=(m10>0.001 || m10<-0.001) ?
                                             formateanumerosep(m10,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes10",cadnummes);
                                   cadnummes=(m11>0.001 || m11<-0.001) ?
                                             formateanumerosep(m11,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes11",cadnummes);
                                   cadnummes=(m12>0.001 || m12<-0.001) ?
                                             formateanumerosep(m12,comadecimal,decimales) : QString();
                                   addElementoTextoDom(doc,tag3,"Mes12",cadnummes);
                                 }

                               if (condesglose)
                               {
                                QSqlQuery q;
                                if (!cod_longitud_variable())
                                  {
                                   q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                                  }
                                  else
                                      {
                                       q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                      }
                                 while (q.next())
                                    {
                                      QString extract2=q.value(0).toString();
                                      QString extract2sig=extract2;
                                      if (positivo) extract2sig.append('+');
                                      if (negativo) extract2sig.append('-');
                                      if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                          acotado,fecha1,fecha2);
                                      else
                                      {
                                       if (!estadosmedios) valor=saldo_cuenta_lista_ej1(extract2sig);
                                       else
                                         valor=calculamediacuenta_aux(extract2sig,true);
                                      }
                                      finicio.setDate(anyo,1,1);
                                      fin.setDate(anyo,1,31);
                                      if (extract.contains(':')==1) m1=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m1=saldo_cuenta_lista_mes(extract2sig,1);


                                      finicio.setDate(anyo,2,1);
                                      fin.setDate(anyo,3,1);
                                      fin=fin.addDays(-1);
                                      if (extract.contains(':')==1) m2=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m2=saldo_cuenta_lista_mes(extract2sig,2);

                                      finicio.setDate(anyo,3,1);
                                      fin.setDate(anyo,3,31);
                                      if (extract.contains(':')==1) m3=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m3=saldo_cuenta_lista_mes(extract2sig,3);

                                      finicio.setDate(anyo,4,1);
                                      fin.setDate(anyo,4,30);
                                      if (extract.contains(':')==1) m4=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m4=saldo_cuenta_lista_mes(extract2sig,4);

                                      finicio.setDate(anyo,5,1);
                                      fin.setDate(anyo,5,31);
                                      if (extract.contains(':')==1) m5=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m5=saldo_cuenta_lista_mes(extract2sig,5);

                                      finicio.setDate(anyo,6,1);
                                      fin.setDate(anyo,6,30);
                                      if (extract.contains(':')==1) m6=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m6=saldo_cuenta_lista_mes(extract2sig,6);

                                      finicio.setDate(anyo,7,1);
                                      fin.setDate(anyo,7,31);
                                      if (extract.contains(':')==1) m7=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m7=saldo_cuenta_lista_mes(extract2sig,7);

                                      finicio.setDate(anyo,8,1);
                                      fin.setDate(anyo,8,31);
                                      if (extract.contains(':')==1) m8=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m8=saldo_cuenta_lista_mes(extract2sig,8);

                                      finicio.setDate(anyo,9,1);
                                      fin.setDate(anyo,9,30);
                                      if (extract.contains(':')==1) m9=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m9=saldo_cuenta_lista_mes(extract2sig,9);

                                      finicio.setDate(anyo,10,1);
                                      fin.setDate(anyo,10,31);
                                      if (extract.contains(':')==1) m10=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m10=saldo_cuenta_lista_mes(extract2sig,10);

                                      finicio.setDate(anyo,11,1);
                                      fin.setDate(anyo,11,30);
                                      if (extract.contains(':')==1) m11=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m11=saldo_cuenta_lista_mes(extract2sig,11);

                                      finicio.setDate(anyo,12,1);
                                      fin.setDate(anyo,12,31);
                                      if (extract.contains(':')==1) m12=cuentaespec(extract2sig,ejercicio,true,
                                                                                   finicio,fin);
                                         else
                                             m12=saldo_cuenta_lista_mes(extract2sig,12);

                                      if (valor>0.001 || valor <-0.001)
                                          {
                                            QDomElement tag3 = doc.createElement("Linea");
                                            root.appendChild(tag3);
                                            addElementoTextoDom(doc,tag3,"Parte",parte2);
                                            // apartado
                                            addElementoTextoDom(doc,tag3,"Apartado",
                                                       "     "+extract2+" "+
                                                       filtracadxml(descripcioncuenta(extract2)));
                                            if (inicio-1>=0 && cadpru[inicio-1]=='-')
                                               {
                                                valor=-1*valor;
                                                m1=-1*m1;
                                                m2=-1*m2;
                                                m3=-1*m3;
                                                m4=-1*m4;
                                                m5=-1*m5;
                                                m6=-1*m6;
                                                m7=-1*m7;
                                                m8=-1*m8;
                                                m9=-1*m9;
                                                m10=-1*m10;
                                                m11=-1*m11;
                                                m12=-1*m12;
                                               }

                                            addElementoTextoDom(doc,tag3,"ImporteAuxiliar",
                                                  filtracadxml(formateanumerosep(valor,comadecimal,decimales)));
                                            addElementoTextoDom(doc,tag3,"ImporteCuenta","");
                                            addElementoTextoDom(doc,tag3,"Importe1","");
                                            addElementoTextoDom(doc,tag3,"Importe2","");
                                            if (ui.desglose_mes_checkBox->isChecked())
                                              {
                                                QString cadnummes;
                                                cadnummes=(m1>0.001 || m1<-0.001) ?
                                                          formateanumerosep(m1,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes1",cadnummes);
                                                cadnummes=(m2>0.001 || m2<-0.001) ?
                                                          formateanumerosep(m2,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes2",cadnummes);
                                                cadnummes=(m3>0.001 || m3<-0.001) ?
                                                          formateanumerosep(m3,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes3",cadnummes);
                                                cadnummes=(m4>0.001 || m4<-0.001) ?
                                                          formateanumerosep(m4,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes4",cadnummes);
                                                cadnummes=(m5>0.001 || m5<-0.001) ?
                                                          formateanumerosep(m5,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes5",cadnummes);
                                                cadnummes=(m6>0.001 || m6<-0.001) ?
                                                          formateanumerosep(m6,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes6",cadnummes);
                                                cadnummes=(m7>0.001 || m7<-0.001) ?
                                                          formateanumerosep(m7,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes7",cadnummes);
                                                cadnummes=(m8>0.001 || m8<-0.001) ?
                                                          formateanumerosep(m8,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes8",cadnummes);
                                                cadnummes=(m9>0.001 || m9<-0.001) ?
                                                          formateanumerosep(m9,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes9",cadnummes);
                                                cadnummes=(m10>0.001 || m10<-0.001) ?
                                                          formateanumerosep(m10,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes10",cadnummes);
                                                cadnummes=(m11>0.001 || m11<-0.001) ?
                                                          formateanumerosep(m11,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes11",cadnummes);
                                                cadnummes=(m12>0.001 || m12<-0.001) ?
                                                          formateanumerosep(m12,comadecimal,decimales) : QString();
                                                addElementoTextoDom(doc,tag3,"Mes12",cadnummes);
                                              }
                                          }
                                     }
                                  } // fin del condesglose
                              } // if valor no es cero
                       } // del cadpru índice
                    indice++;
                   } // del while índice

                  // -----------------------------------------------------------

                } // fin del query next
              }
   } // fin de parte2 length > 0


   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   if (ui.texlineEdit->text().length()==0)
      qfichero=qfichero+tr("estadocontable.xml");
   else
         {
            qfichero=qfichero+ui.texlineEdit->text();
            if (ui.texlineEdit->text().right(4)!=".xml") qfichero=qfichero+".xml";
         }
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero(pasa);
    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    QString xml = doc.toString();

    stream.setEncoding(QStringConverter::Utf8);

    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

    // ------------------------------------------------------------------------------------
    stream << xml;

    fichero.close();
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

    progreso.setValue(3);
    QApplication::processEvents();

    QString cj;
    if (haycolref && !ejercicio2.isEmpty()) cj=tr("estado_contable.jasper");
    if (!haycolref && !ejercicio2.isEmpty()) cj=tr("estado_contable_no_colref.jasper");
    if (ejercicio2.isEmpty() && !condesglose && !condesglosectas)
        cj=tr("estado_contable_1ejercicio.jasper");
    if (!formulabasepor.isEmpty())
      {
        if(ejercicio2.isEmpty()) cj=tr("estado_contable_porcent_1col.jasper");
          else cj=tr("estado_contable_porcent_2col.jasper");
      }
    if (condesglose || condesglosectas) cj=tr("estado_contable_desglose.jasper");

    */
    QMessageBox::information( this, tr("Cálculo de Estado Contable"),
                              tr("Se ha generado el archivo XML") );
    /*
    QString cadfichjasper=trayreport();
    cadfichjasper.append(QDir::separator());
    cadfichjasper+=cj;

    QString cadpdf=dirtrabajo();
    cadpdf.append(QDir::separator());
    cadpdf+=cj.remove(".jasper")+".pdf";



    informe_jasper_xml(cadfichjasper, pasa,
                       "/Estado/Linea", cadpdf,
                       rutagraf);
     */

}






void calcestado::generalatex_meses()
{
    if (auxiliares.count()==0)
       if (condesglose || condesglosectas) { calculaestado(); }
    QString ejercicio1="";
    QString ejercicio2="";
    QString parte1="";
    QString parte2="";
    QString formulabasepor;
    QString observaciones;
    QString cabecera;
    //double valorbasepor1=0;
    //double valorbasepor2=0;
    // bool estadosmedios=false;
    bool analitica=false; //,haycolref;
    QString ci,colref;

    QProgressDialog progreso(tr("Generando Informe ..."), 0, 0, 3);
    progreso.setWindowTitle("Estados Contables");

    progreso.setValue(1);
    QApplication::processEvents();

    QSqlQuery query = basedatos::instancia()->select14Cabeceraestadostitulo( ui.titulolabel->text() );

    if ( (query.isActive()) && (query.first()) ) {
       parte1=query.value(0).toString();
           parte2=query.value(1).toString();
           ejercicio1=query.value(2).toString();
           ejercicio2=query.value(3).toString();
           // estadosmedios=query.value(4).toBool();
           formulabasepor=query.value(5).toString();
           //valorbasepor1=query.value(6).toDouble();
           //valorbasepor2=query.value(7).toDouble();
           observaciones=query.value(8).toString();
           // analitica=query.value(9).toBool();
           ci=query.value(10).toString();
           // haycolref=query.value(11).toBool();
           colref=query.value(12).toString();
           cabecera=query.value(13).toString();
       } else return;
    // bool hayporcentajes=false;
    // if (formulabasepor.length()>0 && (valorbasepor1>0.001 || valorbasepor1<0.001)
    //    && (valorbasepor2>0.001 || valorbasepor2<0.001)) hayporcentajes=true;
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   if (ui.texlineEdit->text().length()==0)
      qfichero=qfichero+tr("estadocontable.tex");
   else
         {
            qfichero=qfichero+ui.texlineEdit->text();
            if (ui.texlineEdit->text().right(4)!=".tex") qfichero=qfichero+".tex";
         }
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

    stream << "\\begin{landscape}" << "\n";

    // Generamos primera parte del Estado Contable
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    if (ui.previsiones->isChecked())
      {
       stream << "\\begin{center}" << "\n";
       stream << "{\\Large \\textbf {";
       stream << " ** PREVISIONES ** }}";
       stream << "\\end{center}";
      }

    if (analitica && ci.length()>0)
        {
         stream << "\\begin{center}" << "\n";
         stream << "{\\normalsize \\textbf {" << tr("CÓDIGO DE IMPUTACIÓN: ") << ci <<  "}}" << "\n";
         stream << "\\end{center}" << "\n";

         QString codigo=ci;
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

    QApplication::processEvents();

    stream << tr("\\begin{center}") << "\n";

    stream << "\\begin{longtable}{|p{4.5cm}|r|r|r|r|r|r|r|r|r|r|r|r|r|}" << "\n";

    stream << "\\hline" << "\n";
   stream << "\\multicolumn{14";
    stream << "}{|c|} {\\textbf{ ";
    stream << cabecera <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // ---------------------------------------------------------------------------------------

    stream << "{\\scriptsize{" << filtracad(parte1) << "}} & ";
    stream << "{\\scriptsize {" << filtracad(ejercicio1) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 1")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 2")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 3")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 4")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 5")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 6")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 7")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 8")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 9")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 10")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 11")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 12")) <<"} ";
    stream << "} \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead" << "\n";
    // -------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\scriptsize{" << filtracad(parte1) << "}} & ";
    stream << "{\\scriptsize {" << filtracad(ejercicio1) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 1")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 2")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 3")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 4")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 5")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 6")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 7")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 8")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 9")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 10")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 11")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 12")) <<"}";
    stream << "} \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    stream << "\\hline" << "\n";

    QSqlQuery query2 = basedatos::instancia()->select5Estadostituloparte1ordernodo( ui.titulolabel->text() , true );
    int veces;
    if ( query2.isActive() ) {
             while ( query2.next() ) {

               QApplication::processEvents();

               if (query2.value(0).toString().contains("LINEA")>0)
                 {
                   stream << "\\hline\n";
                   continue;
                 }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001
                    && query2.value(2).toDouble()>-0.0001)
                     continue;
               // ----------------------------------------------------------------------
               // ----------------------------------------------------------------------
               for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
                   stream << "\\hspace{0.5cm} ";
               // if (query2.value(0).toString().contains("TOTAL")==0)
              //	   stream << query2.value(0).toString() << " " ;
               stream << "{\\tiny {" << filtracad(query2.value(1).toString()) << "}} & ";
               //---------------------------------------------
               if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(2).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(5).toDouble()>0.001 || query2.value(5).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(5).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(6).toDouble()>0.001 || query2.value(6).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(6).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(7).toDouble()>0.001 || query2.value(7).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(7).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(8).toDouble()>0.001 || query2.value(8).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(8).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(9).toDouble()>0.001 || query2.value(9).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(9).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(10).toDouble()>0.001 || query2.value(10).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(10).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(11).toDouble()>0.001 || query2.value(11).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(11).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(12).toDouble()>0.001 || query2.value(12).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(12).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(13).toDouble()>0.001 || query2.value(13).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(13).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(14).toDouble()>0.001 || query2.value(14).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(14).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(15).toDouble()>0.001 || query2.value(15).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(15).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(16).toDouble()>0.001 || query2.value(16).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(16).toDouble(),comadecimal,decimales) << "}}";
               stream << " \\\\" << "\n";

// ------------------------------------------------------------------------------------------------
               if (!condesglose && !condesglosectas) continue;

               // desglosamos el campo fórmula que no contenga {
               QString cadpru=query2.value(17).toString();
               if (cadpru.contains('{')) continue;

               int indice=0;
               int inicio,final;
               QString ejercicio=ui.Ejercicio1comboBox->currentText();
               bool acotado=ui.fechas_ej1_groupBox->isChecked();
               QDate fecha1=ui.ini_ej1_dateEdit->date();
               QDate fecha2=ui.fin_ej1_dateEdit->date();
               // bool estadosmedios=consaldosmedios;
               double valor;
               QString extract;
               while (indice<cadpru.length())
                   {
                    if (cadpru[indice]=='[')
                        {
                         inicio=indice;
                         final=inicio;
                         while (final<cadpru.length() && cadpru[final]!=']') final++;
                         // extraemos la cadena de la cuenta sin corchetes
                         extract=cadpru.mid(inicio+1,final-inicio-1);
                        // habría que calcular el valor de la cuenta
                        // QMessageBox::warning( this, tr("Estados Contables"),extract);
                        // -------------------------------------------------------------------------------------
                        if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotado,fecha1,fecha2);
                           else
                               valor=saldo_cuenta_lista_ej1(extract);
                        // repetimos esto para cada mes
                        QDate finicio;
                        QDate fin;
                        double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
                        int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();

                        finicio.setDate(anyo,1,1);
                        fin.setDate(anyo,1,31);
                        if (extract.contains(':')==1) m1=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m1=saldo_cuenta_lista_mes(extract, 1);


                        finicio.setDate(anyo,2,1);
                        fin.setDate(anyo,3,1);
                        fin=fin.addDays(-1);
                        if (extract.contains(':')==1) m2=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m2=saldo_cuenta_lista_mes(extract,2);

                        finicio.setDate(anyo,3,1);
                        fin.setDate(anyo,3,31);
                        if (extract.contains(':')==1) m3=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m3=saldo_cuenta_lista_mes(extract,3);

                        finicio.setDate(anyo,4,1);
                        fin.setDate(anyo,4,30);
                        if (extract.contains(':')==1) m4=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m4=saldo_cuenta_lista_mes(extract,4);

                        finicio.setDate(anyo,5,1);
                        fin.setDate(anyo,5,31);
                        if (extract.contains(':')==1) m5=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m5=saldo_cuenta_lista_mes(extract,5);

                        finicio.setDate(anyo,6,1);
                        fin.setDate(anyo,6,30);
                        if (extract.contains(':')==1) m6=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m6=saldo_cuenta_lista_mes(extract,6);

                        finicio.setDate(anyo,7,1);
                        fin.setDate(anyo,7,31);
                        if (extract.contains(':')==1) m7=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m7=saldo_cuenta_lista_mes(extract,7);

                        finicio.setDate(anyo,8,1);
                        fin.setDate(anyo,8,31);
                        if (extract.contains(':')==1) m8=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m8=saldo_cuenta_lista_mes(extract,8);

                        finicio.setDate(anyo,9,1);
                        fin.setDate(anyo,9,30);
                        if (extract.contains(':')==1) m9=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m9=saldo_cuenta_lista_mes(extract,9);

                        finicio.setDate(anyo,10,1);
                        fin.setDate(anyo,10,31);
                        if (extract.contains(':')==1) m10=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m10=saldo_cuenta_lista_mes(extract,10);

                        finicio.setDate(anyo,11,1);
                        fin.setDate(anyo,11,30);
                        if (extract.contains(':')==1) m11=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m11=saldo_cuenta_lista_mes(extract,11);

                        finicio.setDate(anyo,12,1);
                        fin.setDate(anyo,12,31);
                        if (extract.contains(':')==1) m12=cuentaespec(extract,ejercicio,true,
                                                                     finicio,fin);
                           else
                               m12=saldo_cuenta_lista_mes(extract,12);


                        // if (valor>-0.004 && valor < 0.005) continue;
                        if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                        // imprimimos cadena formada por código de cuenta y descripción
                        bool positivo=false;
                        bool negativo=false;
                        positivo=extract.contains('+');
                        negativo=extract.contains('-');
                        extract.remove('+');
                        extract.remove('-');
                        if (valor>0.001 || valor<-0.001)
                           {
                            stream << "{\\tiny {" << extract << " " << filtracad(descripcioncuenta(extract))
                                      << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(valor,comadecimal,decimales) << "}} & ";
                            // ahora todos los meses
                            stream << "{\\tiny {" << formateanumerosep(m1,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m2,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m3,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m4,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m5,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m6,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m7,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m8,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m9,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m10,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m11,comadecimal,decimales) << "}} & ";
                            stream << "{\\tiny {" << formateanumerosep(m12,comadecimal,decimales) << "}} ";
                            stream << " \\\\" << "\n";

                            // QMessageBox::warning( this, tr("Estados Contables"),extract);
                            // seguimos aquí con el desglose de todas las cuentas
                            // ojo si aparece el signo + ó menos en extract
                            // habría que quitarlo y añadirlo a cada selección
                            if (condesglose)
                            {
                             QSqlQuery q;
                             if (!cod_longitud_variable())
                               {
                                q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                               }
                               else
                                   {
                                    q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                   }
                              while (q.next())
                                 {
                                   QString extract2=q.value(0).toString();
                                   QString extract2sig=extract2;
                                   if (positivo) extract2sig.append('+');
                                   if (negativo) extract2sig.append('-');
                                   if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                       acotado,fecha1,fecha2);
                                   else
                                   {
                                    valor=saldo_cuenta_lista_ej1(extract2sig);
                                   }

                                   finicio.setDate(anyo,1,1);
                                   fin.setDate(anyo,1,31);
                                   if (extract2sig.contains(':')==1) m1=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m1=saldo_cuenta_lista_mes(extract2sig,1);


                                   finicio.setDate(anyo,2,1);
                                   fin.setDate(anyo,3,1);
                                   fin=fin.addDays(-1);
                                   if (extract2sig.contains(':')==1) m2=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m2=saldo_cuenta_lista_mes(extract2sig,2);

                                   finicio.setDate(anyo,3,1);
                                   fin.setDate(anyo,3,31);
                                   if (extract2sig.contains(':')==1) m3=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m3=saldo_cuenta_lista_mes(extract2sig,3);

                                   finicio.setDate(anyo,4,1);
                                   fin.setDate(anyo,4,30);
                                   if (extract2sig.contains(':')==1) m4=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m4=saldo_cuenta_lista_mes(extract2sig,4);

                                   finicio.setDate(anyo,5,1);
                                   fin.setDate(anyo,5,31);
                                   if (extract2sig.contains(':')==1) m5=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m5=saldo_cuenta_lista_mes(extract2sig,5);

                                   finicio.setDate(anyo,6,1);
                                   fin.setDate(anyo,6,30);
                                   if (extract2sig.contains(':')==1) m6=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m6=saldo_cuenta_lista_mes(extract2sig,6);

                                   finicio.setDate(anyo,7,1);
                                   fin.setDate(anyo,7,31);
                                   if (extract2sig.contains(':')==1) m7=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m7=saldo_cuenta_lista_mes(extract2sig,7);

                                   finicio.setDate(anyo,8,1);
                                   fin.setDate(anyo,8,31);
                                   if (extract2sig.contains(':')==1) m8=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m8=saldo_cuenta_lista_mes(extract2sig,8);

                                   finicio.setDate(anyo,9,1);
                                   fin.setDate(anyo,9,30);
                                   if (extract2sig.contains(':')==1) m9=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m9=saldo_cuenta_lista_mes(extract2sig,9);

                                   finicio.setDate(anyo,10,1);
                                   fin.setDate(anyo,10,31);
                                   if (extract2sig.contains(':')==1) m10=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m10=saldo_cuenta_lista_mes(extract2sig,10);

                                   finicio.setDate(anyo,11,1);
                                   fin.setDate(anyo,11,30);
                                   if (extract2sig.contains(':')==1) m11=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m11=saldo_cuenta_lista_mes(extract2sig,11);

                                   finicio.setDate(anyo,12,1);
                                   fin.setDate(anyo,12,31);
                                   if (extract2sig.contains(':')==1) m12=cuentaespec(extract2sig,ejercicio,true,
                                                                                finicio,fin);
                                      else
                                          m12=saldo_cuenta_lista_mes(extract2sig,12);

                                   if (valor>0.001 || valor <-0.001)
                                       {
                                         stream << "\\hspace{0.5cm} ";
                                         stream << "{\\tiny {" << extract2 << " " <<
                                             filtracad(descripcioncuenta(extract2)) << "}} &";
                                         if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                                         stream << "{\\tiny {" << formateanumerosep(valor,comadecimal,decimales) << "}} & ";
                                         // ahora todos los meses
                                         stream << "{\\tiny {" << formateanumerosep(m1,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m2,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m3,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m4,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m5,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m6,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m7,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m8,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m9,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m10,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m11,comadecimal,decimales) << "}} & ";
                                         stream << "{\\tiny {" << formateanumerosep(m12,comadecimal,decimales) << "}} ";
                                         stream << " \\\\" << "\n";
                                       }
                                  }
                                stream << "\\hline\n";
                               } // fin del condesglose
                           }
                    }
                 indice++;
                }
// ------------------------------------------------------------------------------------------------

             }
           }
    stream << "\\hline" << "\n";
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    progreso.setValue(2);
    QApplication::processEvents();

 if (parte2.length()>0)
    {
    stream << "\\newpage" << "\n";

    // Generamos segunda parte
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    if (ui.previsiones->isChecked())
      {
       stream << "\\begin{center}" << "\n";
       stream << "{\\Large \\textbf {";
       stream << " ** PREVISIONES ** }}";
       stream << "\\end{center}";
      }

    stream << tr("\\begin{center}") << "\n";

    stream << "\\begin{longtable}{|p{4.5cm}|r|r|r|r|r|r|r|r|r|r|r|r|r|}" << "\n";

    stream << "\\hline" << "\n";
   stream << "\\multicolumn{14";
    stream << "}{|c|} {\\textbf{ ";
    stream << cabecera <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // ---------------------------------------------------------------------------------------

    stream << "{\\scriptsize{" << filtracad(parte2) << "}} & ";
    stream << "{\\scriptsize {" << filtracad(ejercicio1) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 1")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 2")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 3")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 4")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 5")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 6")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 7")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 8")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 9")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 10")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 11")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 12")) <<"}";
    stream << "} \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // -------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\scriptsize{" << filtracad(parte2) << "}} & ";
    stream << "{\\scriptsize {" << filtracad(ejercicio1) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 1")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 2")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 3")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 4")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 5")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 6")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 7")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 8")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 9")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 10")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 11")) <<"}} &";
    stream << "{\\scriptsize {" << filtracad(tr("MES 12")) <<"}";
    stream << "} \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    stream << "\\hline" << "\n";
// -------------------------------------------------------------------------------------------------------
    query2 = basedatos::instancia()->select5Estadostituloparte1ordernodo( ui.titulolabel->text(),
                                                                          false );
    if ( query2.isActive() ) {
             while ( query2.next() ) {
               QApplication::processEvents();
               if (query2.value(0).toString().contains("LINEA")>0)
                 {
                   stream << "\\hline\n";
                   continue;
                 }
               // ----------------------------------------------------------------------
               if (ui.importescheckBox->isChecked()
                    && query2.value(2).toDouble()<0.0001
                    && query2.value(2).toDouble()>-0.0001)
                     continue;
               // ----------------------------------------------------------------------
               // ----------------------------------------------------------------------
               for (veces=1;veces<=query2.value(0).toString().count('.');veces++)
                   stream << "\\hspace{0.5cm} ";
               // if (query2.value(0).toString().contains("TOTAL")==0)
              //	   stream << query2.value(0).toString() << " " ;
               stream << "{\\tiny {" << filtracad(query2.value(1).toString()) << "}} & ";
               //---------------------------------------------
               if (query2.value(2).toDouble()>0.001 || query2.value(2).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(2).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(5).toDouble()>0.001 || query2.value(5).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(5).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(6).toDouble()>0.001 || query2.value(6).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(6).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(7).toDouble()>0.001 || query2.value(7).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(7).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(8).toDouble()>0.001 || query2.value(8).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(8).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(9).toDouble()>0.001 || query2.value(9).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(9).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(10).toDouble()>0.001 || query2.value(10).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(10).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(11).toDouble()>0.001 || query2.value(11).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(11).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(12).toDouble()>0.001 || query2.value(12).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(12).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(13).toDouble()>0.001 || query2.value(13).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(13).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(14).toDouble()>0.001 || query2.value(14).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(14).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(15).toDouble()>0.001 || query2.value(15).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(15).toDouble(),comadecimal,decimales) << "}}";
               stream << "&";
               if (query2.value(16).toDouble()>0.001 || query2.value(16).toDouble()<-0.001)
                     stream << "{\\tiny {" <<
                        formateanumerosep(query2.value(16).toDouble(),comadecimal,decimales) << "}}";
               stream << " \\\\" << "\n";
               // ------------------------------------------------------------------------------------------------
               if (!condesglose && !condesglosectas) continue;

                              // desglosamos el campo fórmula que no contenga {
                              QString cadpru=query2.value(17).toString();
                              if (cadpru.contains('{')) continue;

                              int indice=0;
                              int inicio,final;
                              QString ejercicio=ui.Ejercicio1comboBox->currentText();
                              bool acotado=ui.fechas_ej1_groupBox->isChecked();
                              QDate fecha1=ui.ini_ej1_dateEdit->date();
                              QDate fecha2=ui.fin_ej1_dateEdit->date();
                              // bool estadosmedios=consaldosmedios;
                              double valor;
                              QString extract;
                              while (indice<cadpru.length())
                                  {
                                   if (cadpru[indice]=='[')
                                       {
                                        inicio=indice;
                                        final=inicio;
                                        while (final<cadpru.length() && cadpru[final]!=']') final++;
                                        // extraemos la cadena de la cuenta sin corchetes
                                        extract=cadpru.mid(inicio+1,final-inicio-1);
                                       // habría que calcular el valor de la cuenta
                                       // QMessageBox::warning( this, tr("Estados Contables"),extract);
                                       // -------------------------------------------------------------------------------------
                                       if (extract.contains(':')==1) valor=cuentaespec(extract,ejercicio,acotado,fecha1,fecha2);
                                          else
                                              valor=saldo_cuenta_lista_ej1(extract);
                                       // repetimos esto para cada mes
                                       QDate finicio;
                                       QDate fin;
                                       double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12;
                                       int anyo=inicioejercicio(ui.Ejercicio1comboBox->currentText()).year();

                                       finicio.setDate(anyo,1,1);
                                       fin.setDate(anyo,1,31);
                                       if (extract.contains(':')==1) m1=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m1=saldo_cuenta_lista_mes(extract,1);


                                       finicio.setDate(anyo,2,1);
                                       fin.setDate(anyo,3,1);
                                       fin=fin.addDays(-1);
                                       if (extract.contains(':')==1) m2=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m2=saldo_cuenta_lista_mes(extract,2);

                                       finicio.setDate(anyo,3,1);
                                       fin.setDate(anyo,3,31);
                                       if (extract.contains(':')==1) m3=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m3=saldo_cuenta_lista_mes(extract,3);

                                       finicio.setDate(anyo,4,1);
                                       fin.setDate(anyo,4,30);
                                       if (extract.contains(':')==1) m4=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m4=saldo_cuenta_lista_mes(extract,4);

                                       finicio.setDate(anyo,5,1);
                                       fin.setDate(anyo,5,31);
                                       if (extract.contains(':')==1) m5=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m5=saldo_cuenta_lista_mes(extract,5);

                                       finicio.setDate(anyo,6,1);
                                       fin.setDate(anyo,6,30);
                                       if (extract.contains(':')==1) m6=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m6=saldo_cuenta_lista_mes(extract,6);

                                       finicio.setDate(anyo,7,1);
                                       fin.setDate(anyo,7,31);
                                       if (extract.contains(':')==1) m7=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m7=saldo_cuenta_lista_mes(extract,7);

                                       finicio.setDate(anyo,8,1);
                                       fin.setDate(anyo,8,31);
                                       if (extract.contains(':')==1) m8=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m8=saldo_cuenta_lista_mes(extract,8);

                                       finicio.setDate(anyo,9,1);
                                       fin.setDate(anyo,9,30);
                                       if (extract.contains(':')==1) m9=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m9=saldo_cuenta_lista_mes(extract,9);

                                       finicio.setDate(anyo,10,1);
                                       fin.setDate(anyo,10,31);
                                       if (extract.contains(':')==1) m10=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m10=saldo_cuenta_lista_mes(extract,10);

                                       finicio.setDate(anyo,11,1);
                                       fin.setDate(anyo,11,30);
                                       if (extract.contains(':')==1) m11=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m11=saldo_cuenta_lista_mes(extract,11);

                                       finicio.setDate(anyo,12,1);
                                       fin.setDate(anyo,12,31);
                                       if (extract.contains(':')==1) m12=cuentaespec(extract,ejercicio,true,
                                                                                    finicio,fin);
                                          else
                                              m12=saldo_cuenta_lista_mes(extract,12);


                                       // if (valor>-0.004 && valor < 0.005) continue;
                                       if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                                       // imprimimos cadena formada por código de cuenta y descripción
                                       bool positivo=false;
                                       bool negativo=false;
                                       positivo=extract.contains('+');
                                       negativo=extract.contains('-');
                                       extract.remove('+');
                                       extract.remove('-');
                                       if (valor>0.001 || valor<-0.001)
                                          {
                                           stream << "{\\tiny {" << extract << " " << filtracad(descripcioncuenta(extract))
                                                     << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(valor,comadecimal,decimales) << "}} & ";
                                           // ahora todos los meses
                                           stream << "{\\tiny {" << formateanumerosep(m1,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m2,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m3,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m4,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m5,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m6,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m7,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m8,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m9,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m10,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m11,comadecimal,decimales) << "}} & ";
                                           stream << "{\\tiny {" << formateanumerosep(m12,comadecimal,decimales) << "}} ";
                                           stream << " \\\\" << "\n";

                                           // QMessageBox::warning( this, tr("Estados Contables"),extract);
                                           // seguimos aquí con el desglose de todas las cuentas
                                           // ojo si aparece el signo + ó menos en extract
                                           // habría que quitarlo y añadirlo a cada selección
                                           if (condesglose)
                                           {
                                            QSqlQuery q;
                                            if (!cod_longitud_variable())
                                              {
                                               q = basedatos::instancia()->selectCodigoplancontablecodigolength(extract);
                                              }
                                              else
                                                  {
                                                   q = basedatos::instancia()->selectCodigoplancontablecodigoauxiliar(extract);
                                                  }
                                             while (q.next())
                                                {
                                                  QString extract2=q.value(0).toString();
                                                  QString extract2sig=extract2;
                                                  if (positivo) extract2sig.append('+');
                                                  if (negativo) extract2sig.append('-');
                                                  if (extract.contains(':')==1) valor=cuentaespec_aux(extract2sig,ejercicio,
                                                                                      acotado,fecha1,fecha2);
                                                  else
                                                  {
                                                   valor=saldo_cuenta_lista_ej1(extract2sig);
                                                  }
                                                  finicio.setDate(anyo,1,1);
                                                  fin.setDate(anyo,1,31);
                                                  if (extract2sig.contains(':')==1) m1=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m1=saldo_cuenta_lista_mes(extract2sig,1);


                                                  finicio.setDate(anyo,2,1);
                                                  fin.setDate(anyo,3,1);
                                                  fin=fin.addDays(-1);
                                                  if (extract2sig.contains(':')==1) m2=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m2=saldo_cuenta_lista_mes(extract2sig,2);
                                                  finicio.setDate(anyo,3,1);
                                                  fin.setDate(anyo,3,31);
                                                  if (extract2sig.contains(':')==1) m3=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m3=saldo_cuenta_lista_mes(extract2sig,3);

                                                  finicio.setDate(anyo,4,1);
                                                  fin.setDate(anyo,4,30);
                                                  if (extract2sig.contains(':')==1) m4=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m4=saldo_cuenta_lista_mes(extract2sig,4);

                                                  finicio.setDate(anyo,5,1);
                                                  fin.setDate(anyo,5,31);
                                                  if (extract2sig.contains(':')==1) m5=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m5=saldo_cuenta_lista_mes(extract2sig,5);

                                                  finicio.setDate(anyo,6,1);
                                                  fin.setDate(anyo,6,30);
                                                  if (extract2sig.contains(':')==1) m6=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m6=saldo_cuenta_lista_mes(extract2sig,6);

                                                  finicio.setDate(anyo,7,1);
                                                  fin.setDate(anyo,7,31);
                                                  if (extract2sig.contains(':')==1) m7=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m7=saldo_cuenta_lista_mes(extract2sig,7);

                                                  finicio.setDate(anyo,8,1);
                                                  fin.setDate(anyo,8,31);
                                                  if (extract2sig.contains(':')==1) m8=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m8=saldo_cuenta_lista_mes(extract2sig,8);

                                                  finicio.setDate(anyo,9,1);
                                                  fin.setDate(anyo,9,30);
                                                  if (extract2sig.contains(':')==1) m9=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m9=saldo_cuenta_lista_mes(extract2sig,9);

                                                  finicio.setDate(anyo,10,1);
                                                  fin.setDate(anyo,10,31);
                                                  if (extract2sig.contains(':')==1) m10=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m10=saldo_cuenta_lista_mes(extract2sig,10);
                                                  finicio.setDate(anyo,11,1);
                                                  fin.setDate(anyo,11,30);
                                                  if (extract2sig.contains(':')==1) m11=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m11=saldo_cuenta_lista_mes(extract2sig,11);
                                                  finicio.setDate(anyo,12,1);
                                                  fin.setDate(anyo,12,31);
                                                  if (extract2sig.contains(':')==1) m12=cuentaespec(extract2sig,ejercicio,true,
                                                                                               finicio,fin);
                                                     else
                                                         m12=saldo_cuenta_lista_mes(extract2sig,12);
                                                  if (valor>0.001 || valor <-0.001)
                                                      {
                                                        stream << "\\hspace{0.5cm} ";
                                                        stream << "{\\tiny {" << extract2 << " " <<
                                                            filtracad(descripcioncuenta(extract2)) << "}} &";
                                                        if (inicio-1>=0 && cadpru[inicio-1]=='-') valor=-1*valor;
                                                        stream << "{\\tiny {" << formateanumerosep(valor,comadecimal,decimales) << "}} & ";
                                                        // ahora todos los meses
                                                        stream << "{\\tiny {" << formateanumerosep(m1,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m2,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m3,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m4,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m5,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m6,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m7,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m8,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m9,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m10,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m11,comadecimal,decimales) << "}} & ";
                                                        stream << "{\\tiny {" << formateanumerosep(m12,comadecimal,decimales) << "}} ";
                                                        stream << " \\\\" << "\n";
                                                      }
                                                 }
                                               stream << "\\hline\n";
                                              } // fin del condesglose
                                          }
                                   }
                                indice++;
                               }
               // ------------------------------------------------------------------------------------------------

             }
           }
    stream << "\\hline" << "\n";
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";
   } // if parte2
    stream << observaciones << "\n";

    stream << "\\end{landscape}\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    progreso.setValue(3);
    QApplication::processEvents();

    fichero.close();

}
