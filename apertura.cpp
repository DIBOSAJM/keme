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

#include "apertura.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include "funciones.h"
#include "basedatos.h"


apertura::apertura(QString qusuario) : QDialog() {
  ui.setupUi(this);


usuario=qusuario;

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

   connect(ui.generarpushButton,SIGNAL(clicked()),SLOT(procesar()));

   correcto=false;
   proceso_externo=false;
}

void apertura::proc_externo()
{
  proceso_externo=true;
}

void apertura::procesar()
{
    procesar_ejercicio(ui.ejerciciocomboBox->currentText());
}

void apertura::procesar_ejercicio(QString qejercicio)
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
              QMessageBox::warning( this, tr("Asiento de apertura"),
				   tr("Error, no se pueden sobreescribir asientos\n"
				      "revise número de próximo asiento en configuración."));
             return;
      }

   QDate inicioej1; inicioej1=inicioejercicio(qejercicio);
   QString cadinicioej1=inicioej1.toString("yyyy-MM-dd");
   QDate finej1; finej1=finejercicio(qejercicio);
   QString cadfinej1=finej1.toString("yyyy-MM-dd");

   if (basedatos::instancia()->aperturaendiarioenfechas (cadinicioej1, cadfinej1))
     {
       QMessageBox::warning( this, tr("Regularización"),
           tr("Error, ya se ha practicado el asiento de apertura"));
       return;
     }

   // disminuir fecha en uno y buscar ejercicio correspondiente
   // si no existe salir
   QDate finejanterior;
   finejanterior=inicioej1.addDays(-1);
   QString ejanterior;
   ejanterior=ejerciciodelafecha(finejanterior);
   if (ejanterior=="") return;
   // buscamos intervalos de fecha del ejercicio anterior
   QDate inicioej; inicioej=inicioejercicio(ejanterior);
   QDate finej=finejercicio(ejanterior);
   QString cadinicioej=inicioej.toString("yyyy-MM-dd");
   QString cadfinej=finej.toString("yyyy-MM-dd");

   // ----------------------------------------------------------------------------------------------
   int registros = basedatos::instancia()->registroscierrediarioenfechas(cadinicioej,cadfinej);

    if (registros==0)
     {
         QMessageBox::warning( this, tr("APERTURA"),
			       tr("ERROR. No se ha encontrado asiento de cierre en el ejercicio anterior"));
         return;
     }
  //  ---------------------------------------------------------------------------------------------------------
    QSqlDatabase::database().transaction();
    basedatos::instancia()->bloquea_para_asientoapertura();

    qlonglong vnum = basedatos::instancia()->proximoasiento(qejercicio);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    if (basedatos::instancia()->ejercicio_primero_apertura (qejercicio)
        && !existeasiento("1",qejercicio) && vnum!=1)
        {
          cadnumasiento="1";
        }
        else
            {
             QString cadnum; cadnum.setNum(vnum+1);
             basedatos::instancia()->update_ejercicio_prox_asiento(
                     qejercicio, cadnum);
            }

   QString ejercicio=qejercicio;
  // ----------------------------------------------------------------------------------------------------------
   QSqlQuery query = basedatos::instancia()->selectCuentadebehaberdiariofechascierre(cadinicioej,cadfinej);
   if (!proceso_externo) ui.progressBar->setMaximum(registros);
   int veces=0;
   if ( query.isActive() ) {
          while ( query.next() )
          {
             veces++;
              if (!proceso_externo) ui.progressBar->setValue(veces);
             qlonglong vnum2 = basedatos::instancia()->selectProx_paseconfiguracion();
             QString cadnumpase;
             qlonglong pase = vnum2;
             cadnumpase.setNum(pase);
             if (!(conanalitica() || conanalitica_parc()))
               basedatos::instancia()->insertDiario_con_diario(cadnumasiento,cadnumpase,cadinicioej1,
                                                    query.value(0).toString().left(-1).replace("'","''") ,
                                                    query.value(2).toString().left(-1).replace("'","''") ,
                                                    query.value(1).toString().left(-1).replace("'","''") ,
                                                    tr("ASIENTO DE APERTURA"),"", diario_apertura(), usuario,
                                                    ejercicio, query.value(5).toString().left(-1).replace("'","''"));
               else
                   {
                    if (!basedatos::instancia()->analitica_tabla())
                      {
                        basedatos::instancia()->insertDiario_con_diario_ci(cadnumasiento,
                                   cadnumpase,cadinicioej1,query.value(0).toString(),
                                   query.value(2).toString(),
                                   query.value(1).toString(),
                                   tr("ASIENTO DE APERTURA"),"", diario_apertura(),usuario,
                                   query.value(3).toString(),
                                   ejercicio);

                      }
                      else
                          {
                            if (query.value(4).toInt()==0)
                               {
                                basedatos::instancia()->insertDiario_con_diario(cadnumasiento,cadnumpase,cadinicioej1,
                                                                     query.value(0).toString().left(-1).replace("'","''") ,
                                                                     query.value(2).toString().left(-1).replace("'","''") ,
                                                                     query.value(1).toString().left(-1).replace("'","''") ,
                                                                     tr("ASIENTO DE APERTURA"),"", diario_apertura(), usuario,
                                                                     ejercicio);
                               }
                               else
                                 {
                                   QString qdebe=query.value(1).toString();
                                   QString qhaber=query.value(2).toString();
                                   qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
                                   QString cadproxci; cadproxci.setNum(proxnumci);
                                   basedatos::instancia()->insertDiario(cadnumasiento,
                                                                        cadnumpase.toLongLong(),
                                     cadinicioej1, query.value(0).toString(),
                                     qhaber.toDouble(), qdebe.toDouble(),
                                     tr("ASIENTO DE APERTURA"),
                                     "",
                                     diario_apertura(), "",usuario, cadproxci,
                                     ejercicio,"");
                                   // ahora insertamos registros para la tabla ci
                                   qlonglong qclave_ci=query.value(4).toLongLong();

                                   QString clave_ci; clave_ci.setNum(qclave_ci);

                                   QSqlQuery qci = basedatos::instancia()->carga_de_tabla_ci(clave_ci);
                                     // "select ci, importe from diario_ci "
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
                                 }
                          }
                   }

   	        // queda actu saldo y proxpase
                 basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo( query.value(1).toString().left(-1).replace("'","''") , query.value(2).toString().left(-1).replace("'","''") , query.value(0).toString().left(-1).replace("'","''") );
                 // actu proxpase
                 pase++;
                 cadnumpase.setNum(pase);
                 basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
	      }
        }
 basedatos::instancia()->desbloquea_y_commit();
  if (!proceso_externo) QMessageBox::information( this, tr("Asiento de apertura"),
		              tr("El proceso ha concluido."));
 correcto=true;
 accept();

}
