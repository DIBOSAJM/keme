/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

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

#include "consolidacion_sinc.h"
#include "conex_consolidacion.h"
#include "funciones.h"
#include "basedatos.h"
#include <QProgressDialog>
#include <QMessageBox>

consolidacion_sinc::consolidacion_sinc() : QDialog() {
    ui.setupUi(this);

ui.conextableWidget->setColumnWidth(0,150);
ui.conextableWidget->setColumnWidth(1,230);
ui.conextableWidget->setColumnWidth(3,100);
for (int fila=0; fila<ui.conextableWidget->rowCount(); fila++)
    for (int col=0; col<ui.conextableWidget->columnCount(); col++)
        {
         QTableWidgetItem *newItem = new QTableWidgetItem("");
         if (col==2 || col==3)
           newItem->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.conextableWidget->setItem(fila,col,newItem);

        }

// cargamos conexiones en array [9,10]

QSqlQuery q=basedatos::instancia()->select_consolidables();

if (q.isActive())
  {
    while (q.next())
      {
        for (int veces=nombrebase; veces<import_tipos_diario; veces++)
           conex[q.value(0).toInt()-1][veces]=q.value(veces+1).toString();
        conex[q.value(0).toInt()-1][fecha_actu]=q.value(3).toDate().toString("dd-MM-yyyy");
        conex[q.value(0).toInt()-1][import_tipos_diario]=
                q.value(import_tipos_diario+1).toBool() ? "1" : "0";
        ui.conextableWidget->item(q.value(0).toInt()-1,0)->setText(q.value(1).toString());
        ui.conextableWidget->item(q.value(0).toInt()-1,1)->setText(q.value(2).toString());
        ui.conextableWidget->item(q.value(0).toInt()-1,2)->setText(q.value(3).toDate().toString("dd-MM-yyyy"));
        ui.conextableWidget->item(q.value(0).toInt()-1,3)->setText(q.value(4).toString());
      }
  }

connect(ui.conexionpushButton,SIGNAL(clicked()),SLOT(conexion()));
connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(boton_aceptar()));
connect(ui.actualizar1pushButton,SIGNAL(clicked()),SLOT(actualiza_conex()));
connect(ui.actualizarpushButton,SIGNAL(clicked()),SLOT(actualiza_todas_conex()));
connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borra_conex()));

}

void consolidacion_sinc::borra_conex()
{
    int fila=ui.conextableWidget->currentRow();
    if (fila<0 || fila >= ui.conextableWidget->rowCount()) return;
    QString cadcodigo; cadcodigo.setNum(fila+1);
    if (QMessageBox::question(this,
                    tr("Consolidación"),
                    tr("¿ Desea borrar la conexión '%1' con todos "
                       "los datos importados ?")
                      .arg( cadcodigo ),
                    tr("&Sí"), tr("&No"),
                              QString(), 0, 1 ) )
                  return ;
    basedatos::instancia()->borra_datos_consolidada(fila+1);
    for (int veces=nombrebase; veces<=import_tipos_diario; veces++)
        conex[fila][veces].clear();
    ui.conextableWidget->item(fila,0)->setText(QString());
    ui.conextableWidget->item(fila,1)->setText(QString());
    ui.conextableWidget->item(fila,2)->setText(QString());
    ui.conextableWidget->item(fila,3)->setText(QString());

    actu_tabla();
}


void consolidacion_sinc::actualiza_conex()
{
    QProgressDialog progreso(tr("Realizando consolidación... ")
                             , 0, 0, 0);
    progreso.setWindowModality(Qt::WindowModal);
    progreso.setMinimumDuration ( 0 );
    progreso.show();
    progreso.update();
    QApplication::processEvents();
    int fila=ui.conextableWidget->currentRow();
    if (fila<0 || fila >= ui.conextableWidget->rowCount()) return;
    consolida(fila+1);
    actu_tabla();
    QMessageBox::information( this, tr("CONSOLIDACIÓN"),
                          tr("El proceso ha terminado"));

}

void consolidacion_sinc::actualiza_todas_conex()
{
    QProgressDialog progreso(tr("Realizando consolidación... ")
                             , 0, 0, ui.conextableWidget->rowCount());
    progreso.setWindowModality(Qt::WindowModal);
    progreso.setMinimumDuration ( 0 );
    progreso.show();
    progreso.update();
    QApplication::processEvents();

    for (int fila=0; fila<ui.conextableWidget->rowCount(); fila++)
      {
       progreso.setValue(fila+1);
       if (!conex[fila][nombrebase].isEmpty())
          consolida(fila+1);
      }
    actu_tabla();
    QMessageBox::information( this, tr("CONSOLIDACIÓN"),
                          tr("El proceso ha terminado"));
}


void consolidacion_sinc::conexion()
{
    int fila=ui.conextableWidget->currentRow();
    if (fila<0 || fila >= ui.conextableWidget->rowCount()) return;


    conex_consolidacion *c = new conex_consolidacion();
    // pasamos info conexión
    // nombrebase, descrip, fecha_actu, hora_actu, usuario, clave, host,
    // puerto, controlador, ultimo_apunte, import_tipos_diario
    /* pasainfo(QString controlador, QString basedatos, QString usuario,
             QString clave, QString host, QString puerto,
             QString codigo, QString descripcion, bool infodiario) */
    QString codigo; codigo.setNum(fila+1);
    c->pasainfo(conex[fila][controlador],conex[fila][nombrebase], conex[fila][usuario],
                conex[fila][clave], conex[fila][host], conex[fila][puerto],
                codigo, conex[fila][descrip],
                conex[fila][import_tipos_diario]=="1");
    QString qcontrolador, basedatos, qusuario, qclave, qhost, qpuerto, descripcion;
    bool infodiario;

    int resultado=c->exec();
    if (resultado==QDialog::Rejected) return;

    c->recupera_info(&qcontrolador,&basedatos,&qusuario, &qclave, &qhost, &qpuerto,
                     &codigo, &descripcion, &infodiario);
    delete(c);
    conex[fila][controlador]=qcontrolador;
    conex[fila][nombrebase]=basedatos;
    conex[fila][usuario]=qusuario;
    conex[fila][clave]=qclave;
    conex[fila][host]=qhost;
    conex[fila][puerto]=qpuerto;
    conex[fila][descrip]=descripcion;
    conex[fila][import_tipos_diario]=infodiario ? "1" : "";
    ui.conextableWidget->item(fila,0)->setText(basedatos);
    ui.conextableWidget->item(fila,1)->setText(descripcion);
}


void consolidacion_sinc::actu_tabla()
{
    basedatos::instancia()->borra_consolidables();
    for (int veces=0; veces<ui.conextableWidget->rowCount(); veces++)
        {
         if (conex[veces][nombrebase].isEmpty()) continue;
         if (conex[veces][fecha_actu].isEmpty()) conex[veces][fecha_actu]="02-01-2000";
         QDate fecha; fecha=fecha.fromString(conex[veces][fecha_actu],"dd-MM-yyyy");
         basedatos::instancia()->inserta_reg_consolidable(conex[veces][controlador],
                          conex[veces][nombrebase], conex[veces][usuario],
                          conex[veces][clave], conex[veces][host], conex[veces][puerto],
                          veces+1,conex[veces][descrip],conex[veces][import_tipos_diario]=="1",
                          fecha,conex[veces][hora_actu],conex[veces][ultimo_apunte].toLong()
                          );
        }
}

void consolidacion_sinc::boton_aceptar()
{
  actu_tabla();
  accept();
}

void consolidacion_sinc::consolida(int codigo)
{
   // obtenemos una lista de los ejercicios de la consolidada
    // QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
    QStringList ejercicios=basedatos::instancia()->lista_cod_ejercicios();
    /* if ( query.isActive() ) {
            while ( query.next() )
                {
                  ejercicios << query.value(0).toString();
                }
        }*/

    // realizamos segunda conexión

    // conex[veces][controlador],
    // conex[veces][nombrebase], conex[veces][usuario],
    // conex[veces][clave], conex[veces][host], conex[veces][puerto],

    QString cadcod; cadcod.setNum(codigo);
    if (!basedatos::instancia()->abre2db(conex[codigo-1][nombrebase],
                                         conex[codigo-1][usuario],
                                         conex[codigo-1][clave],
                                         conex[codigo-1][host],
                                         conex[codigo-1][controlador],
                                         conex[codigo-1][puerto], "segunda"))
     {
        QMessageBox::warning( this, tr("CONEXIÓN BASE DATOS"),
                              tr("La conexión %1 no se ha podido realizar").arg(cadcod));
       return;
     }

    // actualizamos plan
    actualizaplan(codigo);

    for (int veces=0; veces<ejercicios.count(); veces++)
       {
        if (ejerciciocerrado(ejercicios.at(veces))) continue;
        consolida_ejercicio(codigo,ejercicios.at(veces));
       }

    // cerramos segunda conexión
    basedatos::instancia()->cierra_no_default("segunda");

}

void consolidacion_sinc::consolida_ejercicio(int codigo, QString ejercicio)
{
    // importar diario
    QDate fechainicial=inicioejercicio(ejercicio);
    QDate fechafinal=finejercicio(ejercicio);
    QString cadcodigo; cadcodigo.setNum(codigo);
    //QProgressDialog progreso(tr("Cargando información del diario de ")
    //                         +cadcodigo+"-"+ejercicio, 0, 0, 100);
    //progreso.setWindowModality(Qt::WindowModal);
    //progreso.setMinimumDuration ( 0 );
    if (conex[codigo-1][fecha_actu].isEmpty())
        conex[codigo-1][fecha_actu]="02-01-2000";
    //progreso.setValue(5);
    QApplication::processEvents();

    QDate fecha; fecha=fecha.fromString(conex[codigo-1][fecha_actu],"dd-MM-yyyy");
    QSqlQuery qborr=basedatos::instancia()->apuntes_borrados(fecha, "segunda");
    if (qborr.isActive())
       while (qborr.next())
           basedatos::instancia()->borra_apunte_consolidada(
                   qborr.value(0).toString(), codigo);

    // vemos antes número de registros para estrategia actualiza saldos
    QString cnum;
    cnum.setNum(codigo);
    cnum=cnum.trimmed();
    QString conv="C"+cnum;

    qlonglong num_ult_apunte=basedatos::instancia()->selectMaxpase_origen_entrefechas (
             conv, fechainicial, fechafinal);
    QString cadulapunte; cadulapunte.setNum(num_ult_apunte);// cadulapunte=conex[codigo-1][ultimo_apunte];
    // if (basedatos::instancia()->ejercicio_vacio(ejercicio,conv)) cadulapunte="0";


    int apuntes=basedatos::instancia()->consulta_diario_num_apuntes(fechainicial, fechafinal,
                                                 cadulapunte,"segunda");
    bool demora=apuntes>100;

    // ------------------------------------------------------------------

    // hay que averiguar último pase efectivo del ejercicio
    QSqlQuery query2;
    query2=basedatos::instancia()->consulta_diario_db_ap_apunte(fechainicial, fechafinal,
                                             cadulapunte,"segunda");
        long numasiento=0;
        long guardaasiento=-1;

        QString cadnumasiento;
        QSqlDatabase::database().transaction();
        basedatos::instancia()->bloquea_para_importar_asientos();

     // progreso.setValue(50);
     QApplication::processEvents();
     qlonglong ultimo_apunte_procesado=conex[codigo-1][ultimo_apunte].toLongLong();
     qlonglong vnum = basedatos::instancia()->proximoasiento(
             ejercicio);
     qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
     if (query2.isActive())
       while ( query2.next() ) {
           if (sobreescribeproxasiento(ejercicio) && numasiento==0)
             {
              QMessageBox::warning( this, tr("CONSOLIDACIÓN"),
                                   tr("Error, no se pueden sobreescribir asientos\n"
                                      "revise número de próximo asiento en configuración."));
              break;
             }
           QString asiento,cuenta,concepto,documento,diario,usuario;
           double debe,haber;
           asiento=query2.value(0).toString();
           numasiento=asiento.toLong();
           QDate fecha=query2.value(1).toDate();
           cuenta=query2.value(2).toString();
           debe=query2.value(3).toDouble();
           haber=query2.value(4).toDouble();
           concepto=query2.value(5).toString();
           documento=query2.value(6).toString();
           diario=query2.value(7).toString();

           if (!conex[codigo-1][import_tipos_diario].contains("1"))
              {
               if (diario!=diario_apertura() && diario!=diario_cierre() &&
                   diario!=diario_regularizacion())
                 diario.clear();
              }
           usuario=query2.value(8).toString();
           if (query2.value(9).toLongLong()>ultimo_apunte_procesado)
               ultimo_apunte_procesado=query2.value(9).toLongLong();
           // hay que convertir cuenta a equivalente en consolidación
           for (int veces=0; veces<origen_auxiliares.size();veces++)
               {
                if (origen_auxiliares[veces]==cuenta)
                   {
                    cuenta=conv_auxiliares[veces];
                    break;
                   }
               }
           if (numasiento!=guardaasiento)
           {
             guardaasiento=numasiento;
             cadnumasiento.setNum(vnum);
             vnum++;
             //  ----------------------------------------------------------
           }
           // procesamos aquí adición de pase
           QString cadnumpase;
           // qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
           cadnumpase.setNum(pase);

           basedatos::instancia()->insert_diario_consolidacion_conex (cadnumasiento, pase,
                                                                      fecha, cuenta, debe,
                                                                      haber, concepto,
                                                                      documento,diario,
                                                                      usuario,
                                     ejercicio,
                                     query2.value(9).toLongLong()
                                     );

           // queda actu saldo y proxpase
           QString qdebe, qhaber;
           qdebe.setNum(debe,'f',2);
           qhaber.setNum(haber,'f',2);

           if (!demora)
              basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo(qhaber,
                                                                          qdebe, cuenta);

           pase++;
           // cadnumpase.setNum(pase);

           // basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
           // -------------------------------------------------------------------
          } // final del while
        // progreso.setValue(90);
        QString cadnumpase;
        cadnumpase.setNum(pase);
        basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);

        QString cadnum; cadnum.setNum(vnum);

        basedatos::instancia()->update_ejercicio_prox_asiento(
                ejercicio, cadnum);

        basedatos::instancia()->desbloquea_y_commit();

        if (demora) actualizasaldos();

        QApplication::processEvents();

        QString cadena=fecha.currentDate().toString("dd-MM-yyyy");
        conex[codigo-1][fecha_actu]=cadena;
        ui.conextableWidget->item(codigo-1,fecha_actu)->setText(cadena);
        QTime hora;
        hora=hora.currentTime();
        cadena=hora.toString("hhmmss");
        conex[codigo-1][hora_actu]=cadena;
        ui.conextableWidget->item(codigo-1,hora_actu)->setText(cadena);

        // actualizamos campo último apunte procesado
        // calculaprimerosasientos();
        QString ult_apunte;
        ult_apunte.setNum(ultimo_apunte_procesado);
        conex[codigo-1][ultimo_apunte]=ult_apunte;
        // progreso.setValue(100);
        QApplication::processEvents();

}

void consolidacion_sinc::actualizaplan(int codigo)
{
    // se actualiza el plan a partir de la conexión en curso
    QSqlQuery query;
    query=basedatos::instancia()->select_codigo_descrip_no_aux_2db ("segunda");
    // Se trata de construir dos listas: la primera con los códigos originales, y la
    // segunda con las conversiones para las cuentas que no se subdividen en otras
    QStringList origen_cuentas, conv_cuentas, descrip_cuentas;
    QString cadcodigo; cadcodigo.setNum(codigo);
    //QProgressDialog progreso(tr("Actualizando plan contable conexión ")+cadcodigo, 0, 0, 100);
    //progreso.setWindowModality(Qt::WindowModal);
    QApplication::processEvents();
    if (query.isActive())
       while (query.next())
          {
           origen_cuentas << query.value(0).toString();
           QString conv=query.value(0).toString();
           if (!basedatos::instancia()->codsubdivis2db (query.value(0).toString(),"segunda"))
              {
               // habría que comprobar que posea cuenta auxiliar asociada
               if (basedatos::instancia()->posee_cuenta_aux_2db(query.value(0).toString(),"segunda"))
                  {
                    QString cadnum;
                    cadnum.setNum(codigo);
                    cadnum=cadnum.trimmed();
                    conv+="C"+cadnum;
                  }
              }
           conv_cuentas << conv;
           descrip_cuentas << query.value(1).toString();
          }
    //progreso.setValue(30);
    QApplication::processEvents();
    // QStringList origen_auxiliares,conv_auxiliares,descrip_auxiliares;
    origen_auxiliares.clear();
    conv_auxiliares.clear();
    descrip_auxiliares.clear();
    bool longvariable=cod_longitud_variable();
    int ancho=anchocuentas();
    query=basedatos::instancia()->select_codigo_descrip_aux_2db ("segunda");
    if (query.isActive())
       while (query.next())
          {
           if (!longvariable && (query.value(0).toString().length()+2)>ancho)
              {
                QMessageBox::critical( this, tr("Consolidación de contabilidades"),
                 tr("ERROR: LONGITUD DE CUENTAS A IMPORTAR EXCESIVA"));
                basedatos::instancia()->cierra_no_default("segunda");
                return;
              }
           origen_auxiliares << query.value(0).toString();
           descrip_auxiliares << query.value(1).toString();
           bool encontrado=false;
           for (int veces=0; veces<origen_cuentas.size(); veces++)
               {
                if (query.value(0).toString().startsWith(origen_cuentas[veces])
                    && origen_cuentas[veces]!=conv_cuentas[veces])
                   {
                    encontrado=true;
                    QString destino;
                    QString finalcod=query.value(0).toString().mid(origen_cuentas[veces].length(),-1);
                    destino=conv_cuentas[veces];
                    if (!longvariable)
                       {
                        // comprobamos la longitud de lo que sale juntándolo todo
                        int diferencia=ancho-conv_cuentas[veces].length()-finalcod.length();
                        // insertamos 'diferencia' ceros en la nueva subcuenta para equiparar longitud
                        // sólo si no es longitud variable
                        for (int veces2=0;veces2<diferencia;veces2++)
                            destino.append('0');
                       }
                    destino.append(finalcod);
                    conv_auxiliares << destino;
                    break;
                   }
               }
            // no se ha conseguido cuenta principal
            // lo solucionamos cogiendo los tres primeros caracteres
            // de la cuenta auxiliar
            if (not encontrado)
               {
                // construimos separador
                QString cadnum;
                cadnum.setNum(codigo);
                cadnum=cadnum.trimmed();
                QString conv="C"+cadnum;
                QString auxiliar=query.value(0).toString();
                QString destino=auxiliar.left(3); destino.append(conv);
                QString finalcod=auxiliar.right(auxiliar.length()-3);
                if (!longvariable)
                   {
                    // comprobamos la longitud de lo que sale juntándolo todo
                    int diferencia=ancho-destino.length()-finalcod.length();
                    // insertamos 'diferencia' ceros en la nueva subcuenta para equiparar longitud
                    // sólo si no es longitud variable
                    for (int veces2=0;veces2<diferencia;veces2++)
                        destino.append('0');
                   }
                destino.append(finalcod);
                conv_auxiliares << destino;
               }
          }

    // progreso.setValue(50);
    QApplication::processEvents();
    // Queda escribir el plan contable en la empresa consolidada

    // escribimos plan con cuentas principales
    for (int veces=0; veces<conv_cuentas.size();veces++)
        {
         QString descripcion;
         if (!existecodigoplan(conv_cuentas[veces],&descripcion))
            {
             descripcion=descrip_cuentas[veces];
             if (conv_cuentas[veces]!=origen_cuentas[veces] &&
                 conex[codigo-1][descrip].length()>0)
                {
                 descripcion.append(' ');
                 descripcion.append(conex[codigo-1][descrip]);
                }
             insertaenplan(conv_cuentas[veces],descripcion);
            }
        }

    //progreso.setValue(85);
    QApplication::processEvents();

    // escribimos cuentas auxiliares en plan y saldossubcuenta
    for (int veces=0; veces<conv_auxiliares.size();veces++)
        {
         if (!existesubcuenta(conv_auxiliares[veces]))
            {
             QString descripcion=descrip_auxiliares[veces];
             if (conex[codigo-1][descrip].length()>0)
                {
                 descripcion.append(' ');
                 descripcion.append(conex[codigo-1][descrip]);
                }
             inserta_auxiliar_enplan(conv_auxiliares[veces],descripcion);
             insertasaldosubcuenta(conv_auxiliares[veces]);
            }
        }
    //progreso.setValue(100);
    QApplication::processEvents();

}
