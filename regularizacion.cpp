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

#include "regularizacion.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

regularizacion::regularizacion(QString qusuario) : QDialog() {
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

void regularizacion::proc_externo()
{
  proceso_externo=true;
}

void regularizacion::generar()
{
    generar_ejercicio(ui.ejerciciocomboBox->currentText());
}

void regularizacion::generar_ejercicio(QString qejercicio)
{
   if (ejerciciocerrado(qejercicio) ||
        ejerciciocerrando(qejercicio))
    {
         QMessageBox::warning( this, tr("Regularización"),
			              tr("Error, el ejercicio seleccionado está cerrado o\n"
				  "se está cerrando."));
         return;
    }
   
  if (sobreescribeproxasiento(qejercicio))
       {
              QMessageBox::warning( this, tr("Regularización"),
				   tr("Error, no se pueden sobreescribir asientos\n"
				      "revise número de próximo asiento en configuración."));
             return;
      }

  // funciones conanalitica o conanalitica_parc y  basedatos::analitica_tabla ()
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
   QString clave_gastos,clave_ingresos,filtro;
   QString pyg;

   QSqlQuery query = basedatos::instancia()->selectclavescuenta_pygconfiguracion();
    if ( query.isActive() ) {
          if ( query.next() )
                      {
	               clave_gastos=query.value(0).toString();
	               clave_ingresos=query.value(1).toString();
                       if (clave_gastos.length()==0 || clave_ingresos.length()==0)
                         {
                          QMessageBox::warning( this, tr("Regularización"),
			              tr("Error, no se han especificado códigos para ingresos\n"
				  "y gastos en la configuración de la empresa."));
                          return;
                         }

                       int partes=clave_gastos.count(',');
                       if (clave_gastos.length()>0)
                          {
                           for (int veces=0;veces<=partes;veces++)
                              {
                               if (veces==0) filtro=" and (";
                               if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
                                filtro += "codigo like '"+ clave_gastos.section(',',veces,veces).replace("'","''") +"%'";
                               }
                               else {
                                filtro+="position('";
                                filtro+=clave_gastos.section(',',veces,veces).replace("'","''");
                                filtro+="' in codigo)=1";
                               }
                               if (veces!=partes)          // filtrogastos+=")";
                                  filtro+=" or ";
                               }
                           }

                       partes=clave_ingresos.count(',');
                       if (clave_ingresos.length()>0)
                          {
                           for (int veces=0;veces<=partes;veces++)
                              {
                               if (veces==0 && filtro.length()==0) filtro=" and (";
                               if (veces==0 && filtro.length()>0) filtro+=" or ";
                               if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
                                filtro += "codigo like '"+ clave_ingresos.section(',',veces,veces).replace("'","''") +"%'";
                               }
                               else {
                                filtro+="position('";
                                filtro+=clave_ingresos.section(',',veces,veces).replace("'","''");
                                filtro+="' in codigo)=1";
                               }
                               if (veces==partes) filtro+=")";
                                  else filtro+=" or ";
                               }
                           }
                           else if (filtro.length()>0) filtro+=")";

                       pyg=expandepunto(query.value(2).toString(),anchocuentas());
	               if (!existesubcuenta(pyg) || pyg.length()==0)
		          {
                                    QMessageBox::warning( this, tr("Regularización"),
			              tr("Error, no se ha especificado subcuenta de pérdidas\n"
			 	        "y ganancias correcta en la configuración de la empresa."));
                                    return;
		          }
	              }
          }
   
   int anchoctas=anchocuentas();
   QString cadanchoctas;
   cadanchoctas.setNum(anchoctas);

    int numcuentas = basedatos::instancia()->cantidadplancontablefiltro(cadanchoctas, filtro);

    if (!proceso_externo) ui.progressBar->setMaximum(numcuentas);
   // empezamos una nueva transacción y averiguamos número de asiento
    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();

    basedatos::instancia()->bloquea_para_regularizacion();

    qlonglong vnum = basedatos::instancia()->proximoasiento(qejercicio);

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    basedatos::instancia()->update_ejercicio_prox_asiento(qejercicio,
                                                          cadnum);

   query = basedatos::instancia()->selectCodigoplancontablelengthauxiliarfiltro(cadanchoctas,
                                                                                filtro);
   QString codigo;
   int veces=0;
   double saldo=0;
   QString cadsaldo;
   QString ejercicio=qejercicio;
    if ( query.isActive() ) {
          while ( query.next() )
               {
	         veces++;
                  if (!proceso_externo) ui.progressBar->setValue(veces);
	         codigo=query.value(0).toString();
	         // vamos calculando el saldo de las cuentas para el intervalo de fechas

	         QSqlQuery query2 = basedatos::instancia()->selectSaldocuentaendiariofechas(codigo,
                           cadinicioej, cadfinej);
	         if (query2.isActive())
	         if (query2.next())
		     {
		        if (query2.value(0).toDouble()>0.001 || query2.value(0).toDouble()<-0.001)
		        {
			 // insertamos nuevo apunte
			 saldo+=query2.value(0).toDouble();
                         qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                         QString cadnumpase;
                         cadnumpase.setNum(pase);

                         QString qdebe="0";
                         QString qhaber="0";
			 if (query2.value(0).toDouble()<0) 
			        { 
			            qdebe = query2.value(0).toString();
			            qdebe.remove('-');
			        }

			 if (query2.value(0).toDouble()>0) qhaber = query2.value(0).toString();

                         basedatos::instancia()->insertDiarioRegularizacion(cadnumasiento,
                                                                            cadnumpase,
                                                     cadfinej, codigo, qdebe, qhaber,usuario,
                                                     ejercicio);
			        // queda actu saldo y proxpase
			        // queda actu saldo y proxpase

			 if (query2.value(0).toDouble()>0)
                            {
                             basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(
                                codigo, query2.value(0).toString(), false);
                            }
                           else 
                                {
				  QString lacad=query2.value(0).toString();
				  lacad.remove('-');
                                  basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo,
                                     lacad, true);
			        }

                        // actu proxpase
                        pase++;
                        cadnumpase.setNum(pase);
                        basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
			 // --------------------------------------------------
		        } // if value
		      } // if query2 next
	      } // while query next
          } // if query isactive
    if (saldo>0.001 || saldo<-0.001)
      {
        // contabilizamos contrapartida
         QString cadnumpase;
         qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
         cadnumpase.setNum(pase);

         QString qdebe="0";
         QString qhaber="0";
         cadsaldo.setNum(saldo,'f',2);
         if (saldo>0) qdebe = cadsaldo;
         else qdebe = "0";
         if (saldo<0) 
           { 
            cadsaldo.remove('-');
            qhaber = cadsaldo; 
           }
	   else qhaber = "0";
         basedatos::instancia()->insertDiarioRegularizacion(cadnumasiento, cadnumpase,
                     cadfinej, pyg, qdebe, qhaber, usuario, ejercicio);

         // queda actu saldo y proxpase
        if (saldo>0) 
	    {
            basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo, cadsaldo, true);
        }
        else 
	    {
            basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo, cadsaldo, false);
	    }

          // actu proxpase
          pase++;
          cadnumpase.setNum(pase);
          basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);

      //----------------------------------------------------------------------------------------------
      }
   basedatos::instancia()->desbloquea_y_commit();
    if (!proceso_externo) QMessageBox::information( this, tr("Regularización"),
		              tr("El proceso ha concluido."));
    if (!proceso_externo) ui.progressBar->reset();
   correcto=true;
   accept();

}


bool regularizacion::escorrecto()
{
  return correcto;
}

void regularizacion::generar_analitica(QString qejercicio)
{

  // funciones conanalitica o conanalitica_parc y  basedatos::analitica_tabla ()

   QDate inicioej; inicioej=inicioejercicio(qejercicio);
   QDate finej=finejercicio(qejercicio);
   QString cadinicioej=inicioej.toString("yyyy-MM-dd");
   QString cadfinej=finej.toString("yyyy-MM-dd");
   QString clave_gastos,clave_ingresos,filtro;
   QString pyg;

   QSqlQuery query = basedatos::instancia()->selectclavescuenta_pygconfiguracion();
    if ( query.isActive() ) {
          if ( query.next() )
                      {
                       clave_gastos=query.value(0).toString();
                       clave_ingresos=query.value(1).toString();
                       if (clave_gastos.length()==0 || clave_ingresos.length()==0)
                         {
                          QMessageBox::warning( this, tr("Regularización"),
                                      tr("Error, no se han especificado códigos para ingresos\n"
                                  "y gastos en la configuración de la empresa."));
                          return;
                         }

                       int partes=clave_gastos.count(',');
                       if (clave_gastos.length()>0)
                          {
                           for (int veces=0;veces<=partes;veces++)
                              {
                               if (veces==0) filtro=" (";
                               if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
                                filtro += "cuenta like '"+ clave_gastos.section(',',veces,veces).replace("'","''") +"%'";
                               }
                               else {
                                filtro+="position('";
                                filtro+=clave_gastos.section(',',veces,veces).replace("'","''");
                                filtro+="' in cuenta)=1";
                               }
                               if (veces!=partes)          // filtrogastos+=")";
                                  filtro+=" or ";
                               }
                           }

                       partes=clave_ingresos.count(',');
                       if (clave_ingresos.length()>0)
                          {
                           for (int veces=0;veces<=partes;veces++)
                              {
                               if (veces==0 && filtro.length()==0) filtro=" (";
                               if (veces==0 && filtro.length()>0) filtro+=" or ";
                               if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
                                filtro += "cuenta like '"+ clave_ingresos.section(',',veces,veces).replace("'","''") +"%'";
                               }
                               else {
                                filtro+="position('";
                                filtro+=clave_ingresos.section(',',veces,veces).replace("'","''");
                                filtro+="' in cuenta)=1";
                               }
                               if (veces==partes) filtro+=")";
                                  else filtro+=" or ";
                               }
                           }
                           else if (filtro.length()>0) filtro+=")";

                       pyg=expandepunto(query.value(2).toString(),anchocuentas());
                       if (!existesubcuenta(pyg) || pyg.length()==0)
                          {
                                    QMessageBox::warning( this, tr("Regularización"),
                                      tr("Error, no se ha especificado subcuenta de pérdidas\n"
                                        "y ganancias correcta en la configuración de la empresa."));
                                    return;
                          }
                      }
          }
    filtro += filtro.length()>0 ? " and ": "";
    filtro += "fecha>='" +cadinicioej+"'";
    filtro += " and fecha<='" +cadfinej +"'";

    query=basedatos::instancia()->selectDiario_campoci_filtro(filtro);
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

    basedatos::instancia()->bloquea_para_regularizacion();

    qlonglong vnum = basedatos::instancia()->proximoasiento(qejercicio);

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    basedatos::instancia()->update_ejercicio_prox_asiento(qejercicio,
                                                          cadnum);

   QString codigo;
   double saldo=0;
   QString cadsaldo;
   int veces=0;
   QString ejercicio=qejercicio;
    if ( query.isActive() ) {
          while ( query.next() )
               {
                 qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                 QString cadnumpase;
                 cadnumpase.setNum(pase);
                  if (!proceso_externo) ui.progressBar->setValue(veces);
                 codigo=query.value(0).toString();
                 QString ci=query.value(1).toString();
                 double valor=query.value(3).toDouble() - query.value(2).toDouble();
                 QString cadvalor; cadvalor.setNum(valor,'f',2);
                 QString qdebe,qhaber;
                 cadvalor.remove('-');
                 if (valor>0) qdebe=cadvalor; else qhaber=cadvalor;
                 saldo+=valor;
                 basedatos::instancia()->insertDiarioRegularizacion_ci(cadnumasiento,
                                                                            cadnumpase,
                                                     cadfinej, codigo, qdebe, qhaber,usuario,
                                                     ci,
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
    if (saldo>0.001 || saldo<-0.001)
      {
        // contabilizamos contrapartida
         saldo=-1*saldo;
         QString cadnumpase;
         qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
         cadnumpase.setNum(pase);

         QString qdebe="0";
         QString qhaber="0";
         cadsaldo.setNum(saldo,'f',2);
         if (saldo>0) qdebe = cadsaldo;
         else qdebe = "0";
         if (saldo<0)
           {
            cadsaldo.remove('-');
            qhaber = cadsaldo;
           }
           else qhaber = "0";
         basedatos::instancia()->insertDiarioRegularizacion(cadnumasiento, cadnumpase,
                     cadfinej, pyg, qdebe, qhaber, usuario, ejercicio);

         // queda actu saldo y proxpase
        if (saldo>0)
            {
            basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo, cadsaldo, true);
        }
        else
            {
            basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo, cadsaldo, false);
            }

          // actu proxpase
          pase++;
          cadnumpase.setNum(pase);
          basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);

      //----------------------------------------------------------------------------------------------
      }
   basedatos::instancia()->desbloquea_y_commit();
    if (!proceso_externo) QMessageBox::information( this, tr("Regularización"),
                              tr("El proceso ha concluido."));
    if (!proceso_externo) ui.progressBar->reset();
    correcto=true;
   accept();

}


// procesado de la contabilidad analítica en tabla de asientos
/* QString cadproxci="0";
if (basedatos::instancia()->analitica_tabla())
   {
    if (cuenta_para_analitica(ui.Tablaapuntes->item(fila,0)->text()))
       {
        QStringList lista_ci;
        QStringList lista_total;
        lista_ci=inputaciones[fila]->lista_ci();
        lista_total=inputaciones[fila]->lista_total();
        qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
        cadproxci.setNum(proxnumci);
        for (int veces=0; veces<lista_ci.count(); veces++)
           {
            basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                      lista_ci.at(veces),
                                                      convapunto(lista_total.at(veces)));
           }
        basedatos::instancia()->incrementa_prox_num_ci();
       }
   }

 // Registro del asiento correspondiente
  basedatos::instancia()->insertDiario(cadnumasiento, pase,
    ui.FechaApunte->date().toString("yyyy-MM-dd"), ui.Tablaapuntes->item(fila,0)->text(),
    ddebe, dhaber, ui.Tablaapuntes->item(fila,1)->text(), ui.Tablaapuntes->item(fila,4)->text(),
    diario, ui.Tablaapuntes->item(fila,16)->text(),elusuario, cadproxci,zejercicio);
select diario.cuenta, diario_ci.ci, sum(diario_ci.importe) from diario, diario_ci where diario.clave_ci=diario_ci.clave_ci group by diario.cuenta, diario_ci.ci;
*/



void regularizacion::generar_analitica_tabla(QString qejercicio)
{

  // funciones conanalitica o conanalitica_parc y  basedatos::analitica_tabla ()

   QDate inicioej; inicioej=inicioejercicio(qejercicio);
   QDate finej=finejercicio(qejercicio);
   QString cadinicioej=inicioej.toString("yyyy-MM-dd");
   QString cadfinej=finej.toString("yyyy-MM-dd");
   QString clave_gastos,clave_ingresos,filtro;
   QString pyg;

   QSqlQuery query = basedatos::instancia()->selectclavescuenta_pygconfiguracion();
    if ( query.isActive() ) {
          if ( query.next() )
                      {
                       clave_gastos=query.value(0).toString();
                       clave_ingresos=query.value(1).toString();
                       if (clave_gastos.length()==0 || clave_ingresos.length()==0)
                         {
                          QMessageBox::warning( this, tr("Regularización"),
                                      tr("Error, no se han especificado códigos para ingresos\n"
                                  "y gastos en la configuración de la empresa."));
                          return;
                         }

                       int partes=clave_gastos.count(',');
                       if (clave_gastos.length()>0)
                          {
                           for (int veces=0;veces<=partes;veces++)
                              {
                               if (veces==0) filtro=" (";
                               if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
                                filtro += "cuenta like '"+ clave_gastos.section(',',veces,veces).replace("'","''") +"%'";
                               }
                               else {
                                filtro+="position('";
                                filtro+=clave_gastos.section(',',veces,veces).replace("'","''");
                                filtro+="' in cuenta)=1";
                               }
                               if (veces!=partes)          // filtrogastos+=")";
                                  filtro+=" or ";
                               }
                           }

                       partes=clave_ingresos.count(',');
                       if (clave_ingresos.length()>0)
                          {
                           for (int veces=0;veces<=partes;veces++)
                              {
                               if (veces==0 && filtro.length()==0) filtro=" (";
                               if (veces==0 && filtro.length()>0) filtro+=" or ";
                               if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
                                filtro += "cuenta like '"+ clave_ingresos.section(',',veces,veces).replace("'","''") +"%'";
                               }
                               else {
                                filtro+="position('";
                                filtro+=clave_ingresos.section(',',veces,veces).replace("'","''");
                                filtro+="' in cuenta)=1";
                               }
                               if (veces==partes) filtro+=")";
                                  else filtro+=" or ";
                               }
                           }
                           else if (filtro.length()>0) filtro+=")";

                       pyg=expandepunto(query.value(2).toString(),anchocuentas());
                       if (!existesubcuenta(pyg) || pyg.length()==0)
                          {
                                    QMessageBox::warning( this, tr("Regularización"),
                                      tr("Error, no se ha especificado subcuenta de pérdidas\n"
                                        "y ganancias correcta en la configuración de la empresa."));
                                    return;
                          }
                      }
          }
    filtro += filtro.length()>0 ? " and ": "";
    filtro += "fecha>='" +cadinicioej+"'";
    filtro += " and fecha<='" +cadfinej +"'";

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

    basedatos::instancia()->bloquea_para_regularizacion();

    qlonglong vnum = basedatos::instancia()->proximoasiento(qejercicio);

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    basedatos::instancia()->update_ejercicio_prox_asiento(qejercicio,
                                                          cadnum);

   QString codigo;
   double saldo=0;
   QString cadsaldo;
   int veces=0;
   QString ejercicio=qejercicio;
   QString filtro2 = filtro + " and clave_ci>0";
   filtro2=filtro2+" and debe>=0 and haber>=0";
   query=basedatos::instancia()->select_cuenta_sumadebe_sumahaber_diario(filtro2);
    if ( query.isActive() ) {
          while ( query.next() )
               {
                 qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                 QString cadnumpase;
                 cadnumpase.setNum(pase);
                  if (!proceso_externo) ui.progressBar->setValue(veces);
                 codigo=query.value(0).toString();

                 double valor= - query.value(1).toDouble() + query.value(2).toDouble();
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
                   qdebe.toDouble(), qhaber.toDouble(), msjdiario_regularizacion(),
                   "",
                   diario_regularizacion(), "",usuario, cadproxci,
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
                  qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                  QString cadnumpase;
                  cadnumpase.setNum(pase);
                   if (!proceso_externo) ui.progressBar->setValue(veces);
                  codigo=query.value(0).toString();

                  double valor=-query.value(1).toDouble() + query.value(2).toDouble();
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
                    qdebe.toDouble(), qhaber.toDouble(), msjdiario_regularizacion(),
                    "",
                    diario_regularizacion(), "",usuario, cadproxci,
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
                   qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                   QString cadnumpase;
                   cadnumpase.setNum(pase);
                    if (!proceso_externo) ui.progressBar->setValue(veces);
                   codigo=query.value(0).toString();

                   double valor=- query.value(1).toDouble() + query.value(2).toDouble();
                   QString cadvalor; cadvalor.setNum(valor,'f',2);
                   QString qdebe,qhaber;
                   cadvalor.remove('-');
                   if (valor>0) qdebe=cadvalor; else qhaber=cadvalor;
                   saldo+=valor;

                   basedatos::instancia()->insertDiario(cadnumasiento, pase,
                     cadfinej, codigo,
                     qdebe.toDouble(), qhaber.toDouble(), msjdiario_regularizacion(),
                     "",
                     diario_regularizacion(), "",usuario, "",
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



    if (saldo>0.001 || saldo<-0.001)
      {
        // contabilizamos contrapartida
         QString cadnumpase;
         qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
         cadnumpase.setNum(pase);

         QString qdebe="0";
         QString qhaber="0";
         saldo=saldo*-1; // invertimos el signo del saldo
         cadsaldo.setNum(saldo,'f',2);
         if (saldo>0) qdebe = cadsaldo;
         else qdebe = "0";
         if (saldo<0)
           {
            cadsaldo.remove('-');
            qhaber = cadsaldo;
           }
           else qhaber = "0";
         basedatos::instancia()->insertDiarioRegularizacion(cadnumasiento, cadnumpase,
                     cadfinej, pyg, qdebe, qhaber, usuario, ejercicio);

         // queda actu saldo y proxpase
        if (saldo>0)
            {
            basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo, cadsaldo, true);
        }
        else
            {
            basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo, cadsaldo, false);
            }

          // actu proxpase
          pase++;
          cadnumpase.setNum(pase);
          basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);

      //----------------------------------------------------------------------------------------------
      }
   basedatos::instancia()->desbloquea_y_commit();
    if (!proceso_externo) QMessageBox::information( this, tr("Regularización"),
                              tr("El proceso ha concluido."));
    if (!proceso_externo) ui.progressBar->reset();
    correcto=true;
   accept();

}

