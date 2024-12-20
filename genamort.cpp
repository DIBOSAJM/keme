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

#include "genamort.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>
#include <QProgressDialog>

genamort::genamort(QString qusuario, bool qborrador) : QDialog() {
    ui.setupUi(this);

  // cargar combo de ejercicios
 QStringList ej1;
 QSqlQuery query = basedatos::instancia()->selectCodigoejercicioscerradocerrandoordercodigo(false, false);

 if ( query.isActive() ) {
          while ( query.next() )
              {
                ej1 << query.value(0).toString();
	      }

          }

  ui.ejerciciocomboBox->addItems(ej1);

  vartodos=tr("TODOS");
  varenero=tr("ENERO");
  varfebrero=tr("FEBRERO");
  varmarzo=tr("MARZO");
  varabril=tr("ABRIL");
  varmayo=tr("MAYO");
  varjunio=tr("JUNIO");
  varjulio=tr("JULIO");
  varagosto=tr("AGOSTO");
  varseptiembre=tr("SEPTIEMBRE");
  varoctubre=tr("OCTUBRE");
  varnoviembre=tr("NOVIEMBRE");
  vardiciembre=tr("DICIEMBRE");

  ui.mescomboBox->addItem(vartodos);
  ui.mescomboBox->addItem(varenero);
  ui.mescomboBox->addItem(varfebrero);
  ui.mescomboBox->addItem(varmarzo);
  ui.mescomboBox->addItem(varabril);
  ui.mescomboBox->addItem(varmayo);
  ui.mescomboBox->addItem(varjunio);
  ui.mescomboBox->addItem(varjulio);
  ui.mescomboBox->addItem(varagosto);
  ui.mescomboBox->addItem(varseptiembre);
  ui.mescomboBox->addItem(varoctubre);
  ui.mescomboBox->addItem(varnoviembre);
  ui.mescomboBox->addItem(vardiciembre);

  connect(ui.procesarpushButton,SIGNAL(clicked()),SLOT(procesa()));
  connect(ui.eliminapushButton,SIGNAL(clicked()),SLOT(eliminar()));

  usuario=qusuario;
  borrador=qborrador;
}


void genamort::procesa()
{
  if (ejerciciocerrado(ui.ejerciciocomboBox->currentText()))
     {
          QMessageBox::warning( this, tr("Asiento de amortización"),
              tr("ERROR, el ejercicio seleccionado está cerrado"));
         return;
     }
  // las comprobaciones se realizan según saldos de amortizaciones acumuladas actuales,
  // para el correcto desarrollo del  proceso es necesario que en el ejercicio anterior
  // se haya practicado el asiento de amortizaciones pertinente.
  if (ui.mescomboBox->currentText()==vartodos && practamort(ui.ejerciciocomboBox->currentText()))
     {
          QMessageBox::warning( this, tr("Asiento de amortización"),
		       tr("ERROR, ya se ha practicado el asiento en este ejercicio"));
         return;
     }

  if (ui.mescomboBox->currentText()!=vartodos &&
      basedatos::instancia()->practamort_mes(ui.ejerciciocomboBox->currentText(),
                                             ui.mescomboBox->currentIndex()))
    {
      QMessageBox::warning( this, tr("Asiento de amortización"),
                   tr("ERROR, ya se ha practicado amortización en ")+ui.mescomboBox->currentText());
     return;
    }

   QDate fechacompro; fechacompro=inicioejercicio(ui.ejerciciocomboBox->currentText());
   QDate fechaant; fechaant=fechacompro.addDays(-1);
 /*  QString ejercicioanterior=ejerciciodelafecha(fechaant);
   if (ejercicioanterior.length()>0 && !practamort(ejercicioanterior))
     {
          QMessageBox::warning( this, tr("Asiento de amortización"),
		       tr("ERROR, en el ejercicio anterior no se ha practicado asiento de amortización"));
         return;
     }
  */ 

   // Si analítica_tabla
   // comprobamos que todas las cuentas de gasto se asignan correctamente,
   // es decir 100% de imputación
   bool hay_analitica_tabla=basedatos::instancia()->analitica_tabla();
   QSqlQuery query;
   if (hay_analitica_tabla)
   {
       query = basedatos::instancia()->select10Planamortizacionesordercuenta_gasto();
       if ( query.isActive() )
        {
          while (query.next())
           {
            // ojo, la cuenta de gasto tiene que ser de analítica
            if (es_cuenta_para_analitica(query.value(4).toString()))
              {
               if (!basedatos::instancia()->ci_input_ok (query.value(4).toString()))
                 {
                  QMessageBox::warning( this, tr("Asiento de amortización"),
                             tr("ERROR, los porcentajes de imputación no son correctos"));
                  return;
                 }
              }
          }
       }

       query = basedatos::instancia()->select_cuenta_gasto_amoinv ();
       if ( query.isActive() )
        {
          while (query.next())
           {
            // ojo, la cuenta de gasto tiene que ser de analítica
            if (es_cuenta_para_analitica(query.value(0).toString()))
              {
               if (!basedatos::instancia()->ci_input_ok (query.value(0).toString()))
                 {
                  QMessageBox::warning( this, tr("Asiento de amortización"),
                             tr("ERROR, los porcentajes de imputación no son correctos (amoinv)"));
                  return;
                 }
              }
          }
       }
   }

   if (conanalitica_parc())
     {
       bool hayconanalitica=false;
       bool haysinanalitica=false;
       query = basedatos::instancia()->select_cuenta_gasto_amoinv();
       if ( query.isActive() )
        {
          while (query.next())
           {
              if (es_cuenta_para_analitica(query.value(0).toString()))
                  hayconanalitica=true;
              else
                  haysinanalitica=true;
           }
        }

       query = basedatos::instancia()->select10Planamortizacionesordercuenta_gasto();
       if ( query.isActive() )
        {
          while (query.next())
           {
              if (es_cuenta_para_analitica(query.value(4).toString()))
                  hayconanalitica=true;
              else
                  haysinanalitica=true;
           }
        }

       if (hayconanalitica && haysinanalitica)
       {
        QMessageBox::warning( this, tr("Asiento de amortización"),
                   tr("ERROR, se mezclan cuentas de gastos con y sin analítica"));
        return;
       }
     }


    QString concepto=tr("Amortización del inmovilizado");
    QString cadfecha;
    QDate fechaasiento;
    QDate fechainicio, fechafin;
    fechaasiento=finejercicio(ui.ejerciciocomboBox->currentText());
    fechainicio=inicioejercicio(ui.ejerciciocomboBox->currentText());
    if (ui.mescomboBox->currentText()!=vartodos)
           {
            fechaasiento=fechaasiento.addMonths(-11); // nos vamos a finales de enero
            fechaasiento=fechaasiento.addMonths(ui.mescomboBox->currentIndex()-1);
            int mes=fechaasiento.month();
            int anyo=fechaasiento.year();
            fechainicio.setDate(anyo,mes,1);
           }
    fechafin=fechaasiento;
    cadfecha=fechaasiento.toString("yyyy-MM-dd");

    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();

    basedatos::instancia()->bloquea_para_genamort();

    qlonglong vnum = basedatos::instancia()->proximoasiento(ui.ejerciciocomboBox->currentText());

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum); // número del asiento a realizar

    qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();  // número del  próximo pase

   int totalpasos = basedatos::instancia()->selectCountcuenta_activoplanamortizaciones();
   ui.progressBar->setMaximum(totalpasos);

   query = basedatos::instancia()->select10Planamortizacionesordercuenta_gasto();
   QString cuentagasto="";
   QString cadnumpase;
   double sumagasto=0;
   int fila=0;
   int filafiscal=0;
   int progreso=1;
   QString cadimporte;
   QDate fechafunc;
   if ( query.isActive() )
    {
      while (query.next())
         {
             ui.progressBar->setValue(progreso);
             QCoreApplication::processEvents();

             progreso++;
             if (cuentagasto=="") cuentagasto=query.value(4).toString();
             if (sumagasto<0.001) cuentagasto=query.value(4).toString();
             if (cuentagasto!=query.value(4).toString() && sumagasto>0.001 &&
                 (!es_cuenta_para_analitica(cuentagasto) ||
                  (es_cuenta_para_analitica(cuentagasto) &&
                   hay_analitica_tabla)))
	        {
	         // añadimos apunte de total cuenta de gasto
                 if (es_cuenta_para_analitica(cuentagasto) && hay_analitica_tabla)
                    {
                     QString cadproxci; // averiguamos cadproxci
                     QSqlQuery query = basedatos::instancia()->ci_input(cuentagasto);
                     if (query.isActive())
                       {
                        qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
                        cadproxci.setNum(proxnumci);
                        while (query.next())
                         {
                          double imputacion=sumagasto*query.value(1).toDouble();
                          QString cadimputacion;
                          cadimputacion.setNum(imputacion,'f',4);
                          basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                               query.value(0).toString(),
                                                               cadimputacion);
                         }
                        basedatos::instancia()->incrementa_prox_num_ci();
                       }
                     basedatos::instancia()->insertDiario(cadnumasiento, pase,
                          cadfecha, cuentagasto,
                          sumagasto, 0, concepto, "",
                          "", "",usuario, cadproxci,ui.ejerciciocomboBox->currentText(),"");

                    }
                    else
                      basedatos::instancia()->insert11Diario(cadnumasiento, pase, cadfecha,
                                                             cuentagasto,
                                                             sumagasto, 0, concepto,"",usuario,
                                                             ui.ejerciciocomboBox->currentText(),borrador);

	         pase++;
                 cadimporte.setNum(sumagasto,'f',2);
	         sumagasto=0;
                 actualizasaldo(cuentagasto,cadimporte,true); // cadimporte ???
	         cuentagasto=query.value(4).toString();
	        }
             // insertamos el pase en cuestión
            fechafunc=query.value(1).toDate();
            double saldoctaactivo=saldocuentaendiario(query.value(0).toString());
            if (saldoctaactivo>-0.001 && saldoctaactivo<0.001) continue;
            bool personalizada=query.value(9).toBool();
            double totalamortizado=-saldocuentaendiario(query.value(3).toString());
            double valoramortcontable=0;
            double valoramortfiscal=0;
            double totalamortizadofiscal=totalamortfiscal(query.value(0).toString());

            if (fechafunc>fechafin) continue;
            int diasejercicioamort=365;
            if (fechafunc>=inicioejercicio(ui.ejerciciocomboBox->currentText()))
                diasejercicioamort=fechafunc.daysTo(finejercicio(ui.ejerciciocomboBox->currentText()))+1;
            if (fechafunc>=fechainicio &&
                 fechafunc<=fechafin)
	      {
	        if (personalizada)
                   {
                    if (ui.mescomboBox->currentText()==vartodos)
                        valoramortcontable=valamortpers(ui.ejerciciocomboBox->currentText(),
                                                      query.value(0).toString()).toDouble();
                        else
                            {
                             QString cadvalor=valamortpers(ui.ejerciciocomboBox->currentText(),
                                                        query.value(0).toString());
                             double valor=cadvalor.toDouble();
                             int dias=fechafunc.daysTo(fechafin)+1;
                             valoramortcontable=valor*dias/diasejercicioamort;
                            }
                   }
	         else
	             valoramortcontable=(saldoctaactivo-query.value(5).toDouble())*
                                 query.value(6).toDouble()*
                                 (fechafunc.daysTo(fechafin)+1)/365; // 5 valor residual, 6 coef
	        valoramortfiscal=valoramortcontable;
                // 7 coef amort fiscal, 8 amort_fiscal_1ejer
	        if (query.value(7).toDouble()>0.00001 && !query.value(8).toBool())
		    valoramortfiscal=saldoctaactivo*query.value(7).toDouble()*
                                   (fechafunc.daysTo(fechafin)+1)/365;
	       if (query.value(8).toBool())
                 {
                   if (ui.mescomboBox->currentText()==vartodos) valoramortfiscal=saldoctaactivo;
                     else valoramortfiscal=saldoctaactivo*(fechafunc.daysTo(fechafin)+1)/diasejercicioamort;
                 }
	    }
            else // la fecha de entrada en funcionamiento no está en el periodo actual *******
	       {
                if (personalizada)  // *********************
                   {
                    valoramortcontable=valamortpers(ui.ejerciciocomboBox->currentText(),
                                                   query.value(0).toString()).toDouble();
                    if (!(ui.mescomboBox->currentText()==vartodos))
                          {
                           valoramortcontable=valoramortcontable*(fechainicio.daysTo(fechafin)+1)/
                                              diasejercicioamort;
                          }
                   }
	        else
                    { // ********************
	              valoramortcontable=(saldoctaactivo-query.value(5).toDouble())*
                                      query.value(6).toDouble();
                      if (!(ui.mescomboBox->currentText()==vartodos))
                            {
                             valoramortcontable=valoramortcontable*(fechainicio.daysTo(fechafin)+1)/365;
                            }
                    }
                // ****************
	        valoramortfiscal=valoramortcontable;
	        if (query.value(7).toDouble()>0.00001 && !query.value(8).toBool())
                   {
		    valoramortfiscal=saldoctaactivo*query.value(7).toDouble();
                    if (!(ui.mescomboBox->currentText()==vartodos))
                       valoramortfiscal=valoramortfiscal*(fechainicio.daysTo(fechafin)+1)/365;
                   }
                if (query.value(8).toBool())
                   {
                    if (fechafunc<inicioejercicio(ui.ejerciciocomboBox->currentText()))
                       valoramortfiscal=0;
                     else
                        valoramortfiscal=saldoctaactivo*(
                                fechainicio.daysTo(fechafin)+1)/diasejercicioamort;
                   }
	       }
            if (totalamortizado+valoramortcontable>saldoctaactivo)
	        valoramortcontable=saldoctaactivo-totalamortizado;
            if (totalamortizadofiscal+valoramortfiscal>saldoctaactivo)
	        valoramortfiscal=saldoctaactivo-totalamortizadofiscal;
            if (valoramortfiscal-valoramortcontable>0.001 || valoramortfiscal-valoramortcontable<-0.001)
	     {
               if (ui.mescomboBox->currentIndex()==0)
                  basedatos::instancia()->insert7Amortfiscalycontable( ui.ejerciciocomboBox->currentText() ,
                                                                   query.value(0).toString() ,
                                                                   query.value(2).toString() ,
                                                                   query.value(3).toString() ,
                                                                   query.value(4).toString() ,
                                                                   valoramortcontable ,
                                                                   valoramortfiscal );
               else
                   basedatos::instancia()->insert7Amortfiscalycontable_mes( ui.ejerciciocomboBox->currentText() ,
                                                                    ui.mescomboBox->currentIndex(),
                                                                    query.value(0).toString() ,
                                                                    query.value(2).toString() ,
                                                                    query.value(3).toString() ,
                                                                    query.value(4).toString() ,
                                                                    valoramortcontable ,
                                                                    valoramortfiscal );


	       filafiscal++;
	     }
            if (valoramortcontable<0.001) continue;
            fila++;
            if (!es_cuenta_para_analitica(query.value(4).toString()) ||
                hay_analitica_tabla)
               {
                cadimporte.setNum(valoramortcontable,'f',2);
                sumagasto+=cadimporte.toDouble();

                basedatos::instancia()->insert11Diario (cadnumasiento, pase, cadfecha,
                                                        query.value(3).toString() , 0,
                                                        valoramortcontable,
                                                        concepto,"",usuario,
                                                        ui.ejerciciocomboBox->currentText(),borrador);

                pase++;

                actualizasaldo(query.value(3).toString(),cadimporte,false);
               }
            if (es_cuenta_para_analitica(query.value(4).toString()) &&
                !hay_analitica_tabla)
                    {
                      QSqlQuery query_c = basedatos::instancia()->selectCiasignacionci_amortcuenta(
                              query.value(0).toString() );
                      if (query_c.isActive())
                      while (query_c.next())
                            {
                             double asignacion=query_c.value(1).toDouble();
                             cadimporte.setNum(valoramortcontable*asignacion,'f',2);
                             sumagasto+=cadimporte.toDouble();

                             // insertamos cuenta de gasto con código de imputación (ci)
                             basedatos::instancia()->insert12Diario(cadnumasiento, pase, cadfecha,
                                                                    query.value(4).toString(),
                                                                    valoramortcontable*asignacion, 0,
                                                                    concepto,query_c.value(0).toString(),
                                                                    usuario,
                                                                    ui.ejerciciocomboBox->currentText(),borrador);

                             pase++;

                             actualizasaldo(query.value(3).toString(),cadimporte,true);
                             // cuenta de AA
                             basedatos::instancia()->insert11Diario(cadnumasiento, pase, cadfecha,
                                                                    query.value(3).toString(), 0,
                                                                    valoramortcontable*asignacion,
                                                                    concepto,"",usuario,
                                                                    ui.ejerciciocomboBox->currentText(),borrador);

                             pase++;

                             actualizasaldo(query.value(3).toString(),cadimporte,false);

                            }
                    }
          }  // fin del while
      // añadimos apunte de total cuenta de gasto
      if (sumagasto>0.001 &&
          (!es_cuenta_para_analitica(cuentagasto) ||
           (es_cuenta_para_analitica(cuentagasto) &&
            hay_analitica_tabla)))
      {
       if (es_cuenta_para_analitica(cuentagasto) && hay_analitica_tabla)
          {
           QString cadproxci; // averiguamos cadproxci
           QSqlQuery query = basedatos::instancia()->ci_input(cuentagasto);
           if (query.isActive())
             {
              qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
              cadproxci.setNum(proxnumci);
              while (query.next())
               {
                double imputacion=sumagasto*query.value(1).toDouble();
                QString cadimputacion;
                cadimputacion.setNum(imputacion,'f',4);
                basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                     query.value(0).toString(),
                                                     cadimputacion);
               }
              basedatos::instancia()->incrementa_prox_num_ci();
             }
           basedatos::instancia()->insertDiario(cadnumasiento, pase,
                cadfecha, cuentagasto,
                sumagasto, 0, concepto, "",
                "", "",usuario, cadproxci,ui.ejerciciocomboBox->currentText(),"",borrador);
           actualizasaldo(cuentagasto,cadimporte,true);
           pase++;
          }
          else
               {
                 // añadimos apunte de total cuenta de gasto
                 basedatos::instancia()->insert11Diario(cadnumasiento, pase, cadfecha, cuentagasto,
                                                        sumagasto, 0, concepto,"", usuario,
                                                        ui.ejerciciocomboBox->currentText(), borrador);

	         pase++;
	         actualizasaldo(cuentagasto,cadimporte,true);	
	        }
      }
    } // if query.isactive

// aquí vamos a procesar AMOINV
// --------------------------------------------------------------------------------------

 // "select valor_adq, valor_amortizado, fecha_inicio, coef,";
 // "cta_aa, cta_ag ";
 // "from planamortizaciones order by cta_ag";
   query = basedatos::instancia()->select_amoinv_cuenta_gasto ();
   int numregistros=basedatos::instancia()->registros_amoinv();
   cuentagasto="";

   sumagasto=0;
   if (numregistros>0)
    {
     ui.progressBar->reset();
     ui.progressBar->setMaximum(numregistros);
    }
   //int fila=0;
   // int filafiscal=0;
   // int progreso=1;
   // QString cadimporte;
   // QDate fechafunc;
   if ( query.isActive() )
    {
      while (query.next())
         {
             progreso++;
             ui.progressBar->setValue(progreso);
             QCoreApplication::processEvents();
             if (cuentagasto=="") cuentagasto=query.value(5).toString();
             if (sumagasto<0.001) cuentagasto=query.value(5).toString();
             if (cuentagasto!=query.value(5).toString() && sumagasto>0.001 &&
                 (!es_cuenta_para_analitica(cuentagasto) ||
                  (es_cuenta_para_analitica(cuentagasto) &&
                   hay_analitica_tabla)))
                {
                 // añadimos apunte de total cuenta de gasto
                 if (es_cuenta_para_analitica(cuentagasto) && hay_analitica_tabla)
                    {
                     QString cadproxci; // averiguamos cadproxci
                     QSqlQuery query = basedatos::instancia()->ci_input(cuentagasto);
                     if (query.isActive())
                       {
                        qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
                        cadproxci.setNum(proxnumci);
                        while (query.next())
                         {
                          double imputacion=sumagasto*query.value(1).toDouble();
                          QString cadimputacion;
                          cadimputacion.setNum(imputacion,'f',4);
                          basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                               query.value(0).toString(),
                                                               cadimputacion);
                         }
                        basedatos::instancia()->incrementa_prox_num_ci();
                       }
                     basedatos::instancia()->insertDiario(cadnumasiento, pase,
                          cadfecha, cuentagasto,
                          sumagasto, 0, concepto, "",
                          "", "",usuario, cadproxci,ui.ejerciciocomboBox->currentText(),"",borrador);

                    }
                    else
                      basedatos::instancia()->insert11Diario(cadnumasiento, pase, cadfecha,
                                                             cuentagasto,
                                                             sumagasto, 0, concepto,"",usuario,
                                                             ui.ejerciciocomboBox->currentText(),borrador);

                 pase++;
                 cadimporte.setNum(sumagasto,'f',2);
                 sumagasto=0;
                 actualizasaldo(cuentagasto,cadimporte,true); // cadimporte ???
                 cuentagasto=query.value(5).toString();
                }
             // *********************************************************+++
             // insertamos el pase en cuestión
             // "select valor_adq, valor_amortizado, fecha_inicio, coef,";
             // "cta_aa, cta_ag ";
            fechafunc=query.value(2).toDate();
            int numero=query.value(6).toInt();
            double saldoctaactivo=query.value(0).toString().toDouble();
            if (saldoctaactivo>-0.001 && saldoctaactivo<0.001) continue;
            double totalamortizado=query.value(1).toDouble();
            double valoramortcontable=0;
            if (fechafunc>=fechainicio &&
                 fechafunc<=fechafin)
              {
                valoramortcontable= saldoctaactivo*
                                 query.value(3).toDouble()*
                                 (fechafunc.daysTo(fechafin)+1)/365;
              }
            else // la fecha de entrada en funcionamiento no está en el periodo actual *******
               {
                 if (fechafunc>fechafin) continue;
                 valoramortcontable=(saldoctaactivo)*
                                      query.value(3).toDouble();
                 if (!(ui.mescomboBox->currentText()==vartodos))
                     {
                       valoramortcontable=valoramortcontable*(fechainicio.daysTo(fechafin)+1)/365;
                     }
               }
            if (totalamortizado+valoramortcontable>saldoctaactivo)
                valoramortcontable=saldoctaactivo-totalamortizado;

            if (valoramortcontable<0.001) continue;
            if (!existesubcuenta(query.value(4).toString())) continue;
            fila++;

            if (!es_cuenta_para_analitica(query.value(5).toString()) ||
                hay_analitica_tabla)
               {
                cadimporte.setNum(valoramortcontable,'f',2);
                sumagasto+=cadimporte.toDouble();
                QString cad; cad.setNum(numero);
                cad=" **"+cad+"**";
                basedatos::instancia()->insert11Diario (cadnumasiento, pase, cadfecha,
                                                        query.value(4).toString() , 0,
                                                        valoramortcontable,
                                                        concepto+ cad,"",usuario,
                                                        ui.ejerciciocomboBox->currentText(),borrador);

                pase++;
                basedatos::instancia()->actu_amoinv(numero,
                                                    cadimporte,
                                                    fechaasiento);
                actualizasaldo(query.value(4).toString(),cadimporte,false);
               }
            if (es_cuenta_para_analitica(query.value(5).toString()) &&
                !hay_analitica_tabla)
                    {
                      QSqlQuery query_c = basedatos::instancia()->selectCiasignacionci_amortcuenta(
                              query.value(0).toString() );
                      if (query_c.isActive())
                      while (query_c.next())
                            {
                             double asignacion=query_c.value(1).toDouble();
                             cadimporte.setNum(valoramortcontable*asignacion,'f',2);
                             sumagasto+=cadimporte.toDouble();

                             // insertamos cuenta de gasto con código de imputación (ci)
                             basedatos::instancia()->insert12Diario(cadnumasiento, pase, cadfecha,
                                                                    query.value(5).toString(),
                                                                    valoramortcontable*asignacion, 0,
                                                                    concepto,query_c.value(0).toString(),
                                                                    usuario,
                                                                    ui.ejerciciocomboBox->currentText(),borrador);

                             pase++;

                             actualizasaldo(query.value(5).toString(),cadimporte,true);
                             // cuenta de AA
                             basedatos::instancia()->insert11Diario(cadnumasiento, pase, cadfecha,
                                                                    query.value(4).toString(), 0,
                                                                    valoramortcontable*asignacion,
                                                                    concepto,"",usuario,
                                                                    ui.ejerciciocomboBox->currentText(),borrador);

                             pase++;

                             actualizasaldo(query.value(4).toString(),cadimporte,false);

                            }
                    }
          }  // fin del while
      // añadimos apunte de total cuenta de gasto
      if (sumagasto>0.001 &&
          (!es_cuenta_para_analitica(cuentagasto) ||
           (es_cuenta_para_analitica(cuentagasto) &&
            hay_analitica_tabla)))
      {
       if (es_cuenta_para_analitica(cuentagasto) && hay_analitica_tabla)
          {
           QString cadproxci; // averiguamos cadproxci
           QSqlQuery query = basedatos::instancia()->ci_input(cuentagasto);
           if (query.isActive())
             {
              qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
              cadproxci.setNum(proxnumci);
              while (query.next())
               {
                double imputacion=sumagasto*query.value(1).toDouble();
                QString cadimputacion;
                cadimputacion.setNum(imputacion,'f',4);
                basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                     query.value(0).toString(),
                                                     cadimputacion);
               }
              basedatos::instancia()->incrementa_prox_num_ci();
             }
           basedatos::instancia()->insertDiario(cadnumasiento, pase,
                cadfecha, cuentagasto,
                sumagasto, 0, concepto, "",
                "", "",usuario, cadproxci,ui.ejerciciocomboBox->currentText(),"",borrador);
           actualizasaldo(cuentagasto,cadimporte,true);
           pase++;
          }
          else
               {
                 // añadimos apunte de total cuenta de gasto
                 basedatos::instancia()->insert11Diario(cadnumasiento, pase, cadfecha, cuentagasto,
                                                        sumagasto, 0, concepto,"", usuario,
                                                        ui.ejerciciocomboBox->currentText(),borrador);

                 pase++;
                 actualizasaldo(cuentagasto,cadimporte,true);
                }
      }
    } // if query.isactive


// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------


   if (fila==0)
      {
          if (filafiscal==0)
            {
              QMessageBox::warning( this, tr("Asiento de amortización"),
		       tr("No hay elementos en el plan de amortización que generen asiento"));
              contabilidad.commit();
              basedatos::instancia()->desbloquea_y_commit();
              ui.progressBar->reset();
              return;
            }
      }
     else
            {
             if (ui.mescomboBox->currentText()==vartodos)
              basedatos::instancia()->insertAmortcontable( ui.ejerciciocomboBox->currentText() ,
                                                           cadnumasiento );
             else
                 {
                   if (!practamort(ui.ejerciciocomboBox->currentText()))
                       basedatos::instancia()->insertAmortcontable( ui.ejerciciocomboBox->currentText() ,
                                                                    "0" );
                   basedatos::instancia()->asig_amort_mes_asiento(ui.ejerciciocomboBox->currentText(),
                                                                  ui.mescomboBox->currentIndex(),
                                                                  cadnumasiento);
                 }
            }

    cadnumpase.setNum(pase);
    basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
    basedatos::instancia()->update_ejercicio_prox_asiento(ui.ejerciciocomboBox->currentText(), cadnum);

    // fin de la transacción
    basedatos::instancia()->desbloquea_y_commit();
    // contabilidad.commit();
   QMessageBox::information( this, tr("Asiento de amortización"),
               tr("Se han generado los registros correspondientes al proceso de amortización"));
   ui.progressBar->reset();
}


void genamort::eliminar()
{
  if (ejerciciocerrado(ui.ejerciciocomboBox->currentText()))
     {
         QMessageBox::warning( this, tr("Asiento de amortización"),
		       tr("ERROR, el ejercicio seleccionado está cerrado"));
         return;
     }
      
  if (!practamort(ui.ejerciciocomboBox->currentText()))
     {
          QMessageBox::warning( this, tr("Asiento de amortización"),
		       tr("ERROR, no se ha practicado asiento de amortización en este ejercicio"));
         return;
     }

  if (ui.mescomboBox->currentIndex()==0 &&
      basedatos::instancia()->amortmensual_practicada(ui.ejerciciocomboBox->currentText()))
  {
       QMessageBox::warning( this, tr("Asiento de amortización"),
                    tr("ERROR, hay asientos de amortización mensuales"));
      return;
  }

  if (ui.mescomboBox->currentIndex()>0)
    {
      // se ha practicado asiento de amortización en el mes ?
      if (!basedatos::instancia()->practamort_mes(ui.ejerciciocomboBox->currentText(),
                                             ui.mescomboBox->currentIndex()))
      {
          QMessageBox::warning( this, tr("Asiento de amortización"),
                       tr("ERROR, no se ha practicado asiento de amortización en el mes"));
         return;
      }

  }


  QString cadasiento;
  if (ui.mescomboBox->currentIndex()==0)
      cadasiento = basedatos::instancia()->selectAsientoamortcontableejercicio( ui.ejerciciocomboBox->currentText() );
  else
      cadasiento = basedatos::instancia()->selectAsientoamortcontableejercicio_mes (
          ui.ejerciciocomboBox->currentText(), ui.mescomboBox->currentIndex());

   // consultar si cadasiento + ejercicio está contabilizado
  if ( basedatos::instancia()->asiento_contabilizado(cadasiento, ui.ejerciciocomboBox->currentText())) {
      QMessageBox::warning( this, tr("Asiento de amortización"),
                           tr("ERROR, el asiento es definitivo"));
      return;
  }

  if (QMessageBox::question(this,tr("Asiento de amortización"),
    tr("Se borrarán todos los registros de amortizaciones para"
    " la selección, ¿ Desea continuar ?"))==QMessageBox::No)
      return;

    QProgressDialog progreso("Eliminando asiento ...", 0, 0, 100);
    progreso.setWindowModality(Qt::WindowModal);
    progreso.setMaximum(0);
    progreso.setValue(30);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(tr("PROCESANDO..."));
    QApplication::processEvents();

    // hay que ajustar la tabla de saldos de subcuentas antes de borrar



    QSqlQuery query = basedatos::instancia()->selectCuentadebehaberdiarioasiento(cadasiento, ui.ejerciciocomboBox->currentText());
     if ( query.isActive() )
       {
          while (query.next())
               {
                  actualizasaldo(query.value(0).toString(),query.value(1).toString(),false);
                  actualizasaldo(query.value(0).toString(),query.value(2).toString(),true);
                  if (query.value(3).toString().contains("**"))
                     {
                      QString c=query.value(3).toString();
                      QString cadnum=c.mid(c.indexOf('*'));
                      cadnum.remove("*");
                      double valor=query.value(2).toDouble()*-1;
                      QString cadvalor; cadvalor.setNum(valor,'f',2);
                      basedatos::instancia()->actu_amoinv(cadnum.toInt(),
                                                          cadvalor,
                                                          query.value(4).toDate());
                     }
               }
      }

   // eliminamos asiento de diario
    basedatos::instancia()->deleteDiarioasiento(cadasiento,
                                                ui.ejerciciocomboBox->currentText());

    // eliminamos registro de amortcontable
    if (ui.mescomboBox->currentIndex()==0)
      {
       basedatos::instancia()->deleteAmortcontableejercicio( ui.ejerciciocomboBox->currentText() );

       // eliminamos registros de amortfiscalycontable
       basedatos::instancia()->deleteAmortfiscalycontableejercicio( ui.ejerciciocomboBox->currentText() );
      }
      else
          {
           basedatos::instancia()->borra_amort_mes_asiento(ui.ejerciciocomboBox->currentText(),
                                                         ui.mescomboBox->currentIndex());
           basedatos::instancia()->deleteAmortfiscalycontableejercicio_mes(
                   ui.ejerciciocomboBox->currentText(), ui.mescomboBox->currentIndex() );
          }

  // progreso.close();

   QMessageBox::information( this, tr("Asiento de amortización"),
               tr("Los registros de amortizaciones de la selección se han eliminado"));
    
}


QString genamort::valamortpers( QString ejercicio, QString cuenta )
{
    return basedatos::instancia()->selectImporteamortpersejerciciocuenta( ejercicio, cuenta);
}
