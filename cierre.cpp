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

#include "cierre.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

cierre::cierre(QString qusuario) : QDialog() {
    ui.setupUi(this);

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

   connect(ui.generarpushButton,SIGNAL(clicked()),SLOT(generar()));
 usuario=qusuario;
 correcto=false;
 proceso_externo=false;
}


void cierre::proc_externo()
{
  proceso_externo=true;
}

void cierre::generar()
{
    generar_ejercicio(ui.ejerciciocomboBox->currentText());
}

void cierre::generar_ejercicio(QString qejercicio)
{
    if (basedatos::instancia()->asientos_borrador(qejercicio)) {
        QMessageBox::warning( this, tr("Cierre"),
                             tr("Error, el borrador tiene contenido o\n"
                                "para este ejercicio."));
        return;
    }


   if (ejerciciocerrado(qejercicio) ||
        ejerciciocerrando(qejercicio))
    {
         QMessageBox::warning( this, tr("Cierre"),
			              tr("Error, el ejercicio seleccionado está cerrado o\n"
				  "se está cerrando."));
         return;
    }

  if (sobreescribeproxasiento(qejercicio))
       {
              QMessageBox::warning( this, tr("Cierre"),
				   tr("Error, no se pueden sobreescribir asientos\n"
				      "revise número de próximo asiento en configuración."));
             return;
      }


   // Si analitica función aparte
   // Si analitica tabla, función aparte
  if ((conanalitica() || conanalitica_parc()) && !basedatos::instancia()->analitica_tabla())
     {
      generar_analitica(qejercicio);
      accept();
      return;
     }

  if ((conanalitica() || conanalitica_parc()) && basedatos::instancia()->analitica_tabla())
     {
      generar_analitica_tabla(qejercicio);
      accept();
      return;
     }


   QDate inicioej; inicioej=inicioejercicio(qejercicio);
   QDate finej=finejercicio(qejercicio);
   QString cadinicioej=inicioej.toString("yyyy-MM-dd");
   QString cadfinej=finej.toString("yyyy-MM-dd");

    basedatos::instancia()->updateEjercicioscerrandocodigo(true,
                                                           qejercicio);

   int anchoctas=anchocuentas();
   QString cadanchoctas;
   cadanchoctas.setNum(anchoctas);

   // int numcuentas = basedatos::instancia()->cantidadplancontable(cadanchoctas);

   if (!proceso_externo) ui.progressBar->setMaximum(0);
   // empezamos una nueva transacción y averiguamos número de asiento
    QSqlDatabase::database().transaction();

    basedatos::instancia()->bloquea_para_asientocierre();

    qlonglong vnum = basedatos::instancia()->proximoasiento(qejercicio);

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    basedatos::instancia()->update_ejercicio_prox_asiento(qejercicio,
                                                           cadnum);

   QSqlQuery query = basedatos::instancia()->selectCodigoplancontablelengthauxiliar(cadanchoctas);
   QString codigo;
   int veces=0;
   QString ejercicio=qejercicio;
    if ( query.isActive() ) {
          while ( query.next() )
               {
	         veces++;
                 if (!proceso_externo) ui.progressBar->setValue(veces);
	         codigo=query.value(0).toString();
	         // vamos calculando el saldo de las cuentas para el intervalo de fechas

             QSqlQuery query2 = basedatos::instancia()->selectSumdebesumhaberdiariocuentafecha(codigo,cadinicioej,cadfinej);

	         if (query2.isActive())
	             if (query2.next())
		         {
		        if (query2.value(0).toDouble()>0.001 || query2.value(0).toDouble()<-0.001)
		            {
			    // insertamos nuevo apunte
                             qlonglong vnum2 = basedatos::instancia()->selectProx_paseconfiguracion();
                              QString cadnumpase;
                              qlonglong pase=vnum2;
                              cadnumpase.setNum(pase);

                             QString lacad;
			      if (query2.value(0).toDouble()<0) 
			      { 
			          lacad = query2.value(0).toString();
			          lacad.remove('-');
			      }
                             QString lacadhaber;
			      if (query2.value(0).toDouble()>0)
                                {
                                 lacadhaber = query2.value(0).toString();
                                 lacad="0";
                                }
			      else lacadhaber = "0"; 

                             basedatos::instancia()->insertDiario_con_diario(cadnumasiento,
                                        cadnumpase,cadfinej,codigo,lacad,lacadhaber,
                                        tr("CIERRE DE EJERCICIO"),"", diario_cierre(),usuario,
                                        ejercicio);

			      // queda actu saldo y proxpase
                             QString importe = query2.value(0).toString();
                             importe.remove('-');
                             bool cargo;
			     if (query2.value(0).toDouble()>0) {
                                cargo = false;
                              }
                              else {
                                    cargo = true;
                                 }
                              basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo,importe,cargo);
                              // actu proxpase
                              pase++;
                              cadnumpase.setNum(pase);
                              basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
			    // --------------------------------------------------
		            }
		      }
	      }
          }

    /* QSqlQuery query = basedatos::instancia()->select_agrupacion_saldo_cuenta_externo (cadfinej);
    int veces=0;
    if ( query.isActive() ) {
          while ( query.next() ) {
              double valor=query.value(0).toDouble();
             if (valor<0.0001 && valor>-0.0001) continue;
             veces++;
             if (!proceso_externo) ui.progressBar->setValue(veces);
             QString cuenta=query.value(1).toString();
             QString externo=query.value(2).toString();
             bool deudor=query.value(0).toDouble()>0;

             if (valor<0) valor=valor*-1;
             QString cadnum; cadnum.setNum(valor,'f',2);
             // insertamos nuevo apunte
             qlonglong vnum2 = basedatos::instancia()->selectProx_paseconfiguracion();
             QString cadnumpase;
             qlonglong pase=vnum2;
             cadnumpase.setNum(pase);
             basedatos::instancia()->insertDiario_con_diario(cadnumasiento,
                                        cadnumpase,cadfinej,cuenta,deudor ? "0" : cadnum, deudor ? cadnum : "0",
                                        tr("CIERRE DE EJERCICIO"),"", diario_cierre(),usuario,
                                        qejercicio,externo);

             // queda actu saldo y proxpase
             basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(cuenta,cadnum,deudor);
             // actu proxpase
             pase++;
             cadnumpase.setNum(pase);
             basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
             // --------------------------------------------------
            }
    }*/

    basedatos::instancia()->updateEjercicioscerrandocodigo(false, qejercicio);
    basedatos::instancia()->updateEjercicioscerradocodigo(true, qejercicio);
    basedatos::instancia()->desbloquea_y_commit();

    if (!proceso_externo) QMessageBox::information( this, tr("CIERRE DE EJERCICIO"),
		              tr("El proceso ha concluido."));
   if (!proceso_externo) ui.progressBar->reset();
   correcto=true;
   accept();

 }


void cierre::generar_analitica(QString qejercicio)
{

  // funciones conanalitica o conanalitica_parc y  basedatos::analitica_tabla ()

   QDate inicioej; inicioej=inicioejercicio(qejercicio);
   QDate finej=finejercicio(qejercicio);
   QString cadinicioej=inicioej.toString("yyyy-MM-dd");
   QString cadfinej=finej.toString("yyyy-MM-dd");
   QString filtro;

    filtro = "fecha>='" +cadinicioej+"'";
    filtro += " and fecha<='" +cadfinej +"'";

    QSqlQuery query=basedatos::instancia()->selectDiario_campoci_filtro(filtro);
    // contamos registros
    int regs=0;
    if (query.isActive())
        while (query.next()) regs++;
    // volvemos a obtener la consulta
    query=basedatos::instancia()->selectDiario_campoci_filtro(filtro);

    if (!proceso_externo) ui.progressBar->setMaximum(regs);
   // empezamos una nueva transacción y averiguamos número de asiento
    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();

    basedatos::instancia()->bloquea_para_asientocierre();

    basedatos::instancia()->updateEjercicioscerrandocodigo(true,
                                                           qejercicio);
    qlonglong vnum = basedatos::instancia()->proximoasiento(qejercicio);

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    basedatos::instancia()->update_ejercicio_prox_asiento(qejercicio,
                                                          cadnum);

   QString codigo;
   double saldo=0;
   int veces=0;
   QString ejercicio=qejercicio;
    if ( query.isActive() ) {
          while ( query.next() )
               {
                 double valor=query.value(3).toDouble() - query.value(2).toDouble();
                 if (valor>-0.0001 and valor<0.0001) continue;
                 qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                 QString cadnumpase;
                 cadnumpase.setNum(pase);
                 if (!proceso_externo) ui.progressBar->setValue(veces);
                 codigo=query.value(0).toString();
                 QString ci=query.value(1).toString();
                 QString cadvalor; cadvalor.setNum(valor,'f',2);
                 QString qdebe,qhaber;
                 cadvalor.remove('-');
                 if (valor>0) qdebe=cadvalor; else qhaber=cadvalor;
                 saldo+=valor;
                 basedatos::instancia()->insertDiario_con_diario_ci(cadnumasiento,
                            cadnumpase,cadfinej,codigo,qdebe,qhaber,
                            tr("CIERRE DE EJERCICIO"),"", diario_cierre(),usuario, ci,
                            ejercicio);
                 // queda actu saldo y proxpase

                 if (valor>0)
                     {
                       basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(
                                codigo, cadvalor, false);
                     }
                       else
                            {
                              basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo,
                                     cadvalor, true);
                            }

                   // actu proxpase
                   pase++;
                   cadnumpase.setNum(pase);
                   basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
                   // --------------------------------------------------
              } // while query next
          } // if query isactive
    basedatos::instancia()->updateEjercicioscerrandocodigo(false, qejercicio);
    basedatos::instancia()->updateEjercicioscerradocodigo(true, qejercicio);
   basedatos::instancia()->desbloquea_y_commit();
    if (!proceso_externo) QMessageBox::information( this, tr("ASIENTO DE CIERRE"),
                              tr("El proceso ha concluido."));
   if (!proceso_externo) ui.progressBar->reset();
   correcto=true;
   accept();

}



void cierre::generar_analitica_tabla(QString qejercicio)
{

  // funciones conanalitica o conanalitica_parc y  basedatos::analitica_tabla ()

   QDate inicioej; inicioej=inicioejercicio(qejercicio);
   QDate finej=finejercicio(qejercicio);
   QString cadinicioej=inicioej.toString("yyyy-MM-dd");
   QString cadfinej=finej.toString("yyyy-MM-dd");
   QString filtro;

    filtro = "fecha>='" +cadinicioej+"'";
    filtro += " and fecha<='" +cadfinej +"'";

    QStringList cuentas;
    QList<bool> saldos_nulos;
    QSqlQuery query=basedatos::instancia()->select_cuenta_sumadebe_sumahaber_diario(filtro);
    if (query.isActive())
        while (query.next()) {
           cuentas << query.value(0).toString();
           double saldo = query.value(1).toDouble()-query.value(2).toDouble();
           saldos_nulos << (saldo>-0.001 && saldo<0.001);
        }


    query=basedatos::instancia()->select_cuenta_sumadebe_sumahaber_diario(filtro);

    // contamos registros
    int regs=0;
    if (query.isActive())
        while (query.next()) regs++;
    // volvemos a obtener la consulta - esta vez solo para debe o haber positivos

    if (!proceso_externo) ui.progressBar->setMaximum(regs);
   // empezamos una nueva transacción y averiguamos número de asiento
    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();

    basedatos::instancia()->bloquea_para_asientocierre();

    basedatos::instancia()->updateEjercicioscerrandocodigo(true,
                                          qejercicio);

    qlonglong vnum = basedatos::instancia()->proximoasiento(qejercicio);

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    basedatos::instancia()->update_ejercicio_prox_asiento(qejercicio,
                                                          cadnum);

   QString codigo;
   double saldo=0;
   int veces=0;
   QString filtro2 = filtro + " and clave_ci>0";
   filtro2=filtro2+" and debe>=0 and haber>=0";
   query=basedatos::instancia()->select_cuenta_sumadebe_sumahaber_diario(filtro2);
    if ( query.isActive() ) {
          while ( query.next() )
               {
                 codigo=query.value(0).toString();
                 int indice=cuentas.indexOf(codigo);
                 // nos saltamos las cuentas con saldos nulos
                 if (saldos_nulos.at(indice)) continue;
                 double valor= - query.value(1).toDouble() + query.value(2).toDouble();
                 if (valor>-0.0001 and valor<0.0001) continue;
                 qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                 QString cadnumpase;
                 cadnumpase.setNum(pase);
                 if (!proceso_externo) ui.progressBar->setValue(veces);

                 QString cadvalor; cadvalor.setNum(valor,'f',2);
                 QString qdebe,qhaber;
                 cadvalor.remove('-');
                 if (valor>0) qdebe=cadvalor; else qhaber=cadvalor;
                 saldo+=valor;
                 qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
                 QString cadproxci; cadproxci.setNum(proxnumci);

                 // PARA EVITAR PROBLEMAS hacemos dos pasadas
                 // PRIMERA: apuntes con signos positivos (ya sea debe o haber)
                 // SEGUNDA: apuntes con signos negativos (ya sea debe o haber)
                 // esta es la de los signos positivos
                 basedatos::instancia()->insertDiario(cadnumasiento, pase,
                   cadfinej, codigo,
                   qdebe.toDouble(), qhaber.toDouble(), tr("CIERRE DE EJERCICIO"),
                   "",
                   diario_cierre(), "",usuario, cadproxci,
                   qejercicio,"");
                 // ahora insertamos registros para la tabla ci

                 QSqlQuery qci=basedatos::instancia()->select_desglose_ci_tabla_positivos (
                         cadinicioej, cadfinej, codigo);
                 if (qci.isActive())
                     while (qci.next())
                     {
                      // respetamos el signo de la imputación
                     // OJO: tener en cuenta si es distinto al del diario
                      QString cadvalor; cadvalor.setNum(qci.value(2).toDouble(),'f',5);
                      basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                               qci.value(1).toString(),
                                                               cadvalor);

                     }
                 basedatos::instancia()->incrementa_prox_num_ci();

                 // queda actu saldo y proxpase

                 if (valor>0)
                     {
                       basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(
                                codigo, cadvalor, false);
                     }
                       else
                            {
                              basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo,
                                     cadvalor, true);
                            }

                   // actu proxpase
                   pase++;
                   cadnumpase.setNum(pase);
                   basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
                   veces++;
                   // --------------------------------------------------
              } // while query next
          } // if query isactive


    // repetimos para apuntes con cantidades negativas
    filtro2=filtro + " and clave_ci>0";
    filtro2+=" and debe<=0 and haber<=0";
    query=basedatos::instancia()->select_cuenta_sumadebe_sumahaber_diario(filtro2);
     if ( query.isActive() ) {
           while ( query.next() )
                {
                  codigo=query.value(0).toString();
                  int indice=cuentas.indexOf(codigo);
                  // nos saltamos las cuentas con saldos nulos
                  if (saldos_nulos.at(indice)) continue;

                  double valor= - query.value(1).toDouble() + query.value(2).toDouble();
                  if (valor>-0.0001 and valor<0.0001) continue;
                  qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                  QString cadnumpase;
                  cadnumpase.setNum(pase);
                  if (!proceso_externo) ui.progressBar->setValue(veces);

                  QString cadvalor; cadvalor.setNum(valor,'f',2);
                  QString qdebe,qhaber;
                  cadvalor.remove('-');
                  if (valor>0) qdebe=cadvalor; else qhaber=cadvalor;
                  saldo+=valor;
                  qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
                  QString cadproxci; cadproxci.setNum(proxnumci);

                  // PARA EVITAR PROBLEMAS tendríamos que hacer dos pasadas
                  // PRIMERA: apuntes con signos positivos (ya sea debe o haber)
                  // SEGUNDA: apuntes con signos negativos (ya sea debe o haber)
                  // esta es la de los signos negativos
                  basedatos::instancia()->insertDiario(cadnumasiento, pase,
                    cadfinej, codigo,
                    qdebe.toDouble(), qhaber.toDouble(), tr("CIERRE DE EJERCICIO"),
                    "",
                    diario_cierre(), "",usuario, cadproxci,
                    qejercicio,"");
                  // ahora insertamos registros para la tabla ci

                  QSqlQuery qci=basedatos::instancia()->select_desglose_ci_tabla_negativos (
                          cadinicioej, cadfinej, codigo);
                  if (qci.isActive())
                      while (qci.next())
                      {
                       // respetamos el signo de la imputación
                      // OJO: tener en cuenta si es distinto al del diario
                      QString cadvalor; cadvalor.setNum(-1*qci.value(2).toDouble(),'f',5);
                      basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                               qci.value(1).toString(),
                                                               cadvalor);

                      }
                  basedatos::instancia()->incrementa_prox_num_ci();

                  // queda actu saldo y proxpase

                  if (valor>0)
                      {
                        basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(
                                 codigo, cadvalor, false);
                      }
                        else
                             {
                               basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo,
                                      cadvalor, true);
                             }

                    // actu proxpase
                    pase++;
                    cadnumpase.setNum(pase);
                    basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
                    veces++;
                    // --------------------------------------------------
               } // while query next
           } // if query isactive



    // procedemos con los registros sin representación en la tabla analítica

     filtro2=filtro + " and clave_ci=0";
     query=basedatos::instancia()->select_cuenta_sumadebe_sumahaber_diario(filtro2);
      if ( query.isActive() ) {
            while ( query.next() )
                 {
                   double valor=- query.value(1).toDouble() + query.value(2).toDouble();
                   if (valor>-0.0001 and valor<0.0001) continue;
                   qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                   QString cadnumpase;
                   cadnumpase.setNum(pase);
                   if (!proceso_externo) ui.progressBar->setValue(veces);
                   codigo=query.value(0).toString();

                   QString cadvalor; cadvalor.setNum(valor,'f',2);
                   QString qdebe,qhaber;
                   cadvalor.remove('-');
                   if (valor>0) qdebe=cadvalor; else qhaber=cadvalor;
                   saldo+=valor;

                   basedatos::instancia()->insertDiario(cadnumasiento, pase,
                     cadfinej, codigo,
                     qdebe.toDouble(), qhaber.toDouble(), tr("CIERRE DE EJERCICIO"),
                     "",
                     diario_cierre(), "",usuario, "",
                     qejercicio,"");

                   // queda actu saldo y proxpase

                   if (valor>0)
                       {
                         basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(
                                  codigo, cadvalor, false);
                       }
                         else
                              {
                                basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo,
                                       cadvalor, true);
                              }

                     // actu proxpase
                     pase++;
                     cadnumpase.setNum(pase);
                     basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
                     veces++;
                     // --------------------------------------------------
                } // while query next
            } // if query isactive

      basedatos::instancia()->updateEjercicioscerrandocodigo(false, qejercicio);
      basedatos::instancia()->updateEjercicioscerradocodigo(true, qejercicio);
   basedatos::instancia()->desbloquea_y_commit();
    if (!proceso_externo) QMessageBox::information( this, tr("CIERRE DE EJERCICIO"),
                              tr("El proceso ha concluido."));
   if (!proceso_externo) ui.progressBar->reset();
   correcto=true;
   accept();

}

bool cierre::escorrecto()
{
    return correcto;
}
