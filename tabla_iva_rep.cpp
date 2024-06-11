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

#include "tabla_iva_rep.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "editarasiento.h"
#include "privilegios.h"
#include "buscar_num.h"
#include "buscar_cad.h"
#include "desglose_iva_caja.h"
#include <QProgressDialog>
#include <QMessageBox>

tabla_iva_rep::tabla_iva_rep(QString qusuario, bool qcomadecimal, bool qdecimales) : QDialog() {
    ui.setupUi(this);

comadecimal=qcomadecimal;
decimales=qdecimales;
usuario=qusuario;

paraliquidacion=false;

QString privileg=basedatos::instancia()->privilegios(qusuario);

connect(ui.edasientopushButton,SIGNAL(clicked()),SLOT(edit_asiento()));
connect(ui.consasientopushButton,SIGNAL(clicked()),SLOT(cons_asiento()));
connect(ui.borra_asientoButton,SIGNAL(clicked()),SLOT(borra_asiento()));
connect(ui.refreshpushButton,SIGNAL(clicked()),SLOT(refrescar()));
connect(ui.ordenarpushButton,SIGNAL(clicked()),SLOT(ordena_col()));
connect(ui.buscarpushButton,SIGNAL(clicked()),SLOT(busca_en_col()));
connect(ui.desglosepushButton,SIGNAL(clicked()),SLOT(desglose()));
connect(ui.ptes_cajacheckBox,SIGNAL(stateChanged(int)),SLOT(solo_ptes_criterio_caja()));

ui.tableWidget->setColumnWidth(0,60);
ui.tableWidget->setColumnWidth(5,200);
ui.tableWidget->setColumnWidth(7,60);

if (privileg[edi_asiento]=='0') ui.edasientopushButton->setEnabled(false);
if (privileg[borrar_asiento]=='0') ui.borra_asientoButton->setEnabled(false);

col_busqueda=-1;

}


void tabla_iva_rep::pasa_info_liquidacion(QDate qfechaini, QDate qfechafin)
{
    fechaini=qfechaini;
    fechafin=qfechafin;
    paraliquidacion=true;
    refrescar();
}


void tabla_iva_rep::pasa_info(QString qejercicio,bool qfechacontable, QDate qfechaini,
                                       QDate qfechafin, bool qinteriores, bool qaib, bool qeib,
                                       bool qautofactura, bool qrectificativa,
                                       bool qautofacturanoue,
                                       bool qprestserviciosue,
                                       bool qporseries,
                                       QString qserie_inicial, QString qserie_final,
                                       bool qfiltrarnosujetas, bool qsolonosujetas,
                                       bool qsinnosujetas, bool qexportacion,
                                       bool qisp_interior, bool qexenta_no_deduc, bool qventas_fuera_tac,
                                       QString qcuenta, QString qexterno)
{
    ejercicio=qejercicio;
    fechacontable=qfechacontable;
    fechaini=qfechaini;
    fechafin=qfechafin;
    interiores=qinteriores;
    aib=qaib;
    eib=qeib;
    autofactura=qautofactura;
    rectificativa=qrectificativa;
    autofacturanoue=qautofacturanoue;
    prestserviciosue=qprestserviciosue;
    porseries=qporseries;
    serie_inicial=qserie_inicial;
    serie_final=qserie_final;
    filtrarnosujetas=qfiltrarnosujetas;
    solonosujetas=qsolonosujetas;
    sinnosujetas=qsinnosujetas;
    exportacion=qexportacion;
    isp_interior=qisp_interior;
    exenta_no_deduc=qexenta_no_deduc;
    cuentap=qcuenta;
    ventas_fuera_tac=qventas_fuera_tac;
    externo=qexterno;
    refrescar();

}


void tabla_iva_rep::refrescar()
{
 ui.tableWidget->clearContents();
 ui.tableWidget->setRowCount(0);
 int registros=0;

 if (!paraliquidacion)
   registros= basedatos::instancia()->num_registros_emitidas(ejercicio,
                                     fechacontable,fechaini,
                                     fechafin, interiores, aib, eib, autofactura,
                                     rectificativa, autofacturanoue, prestserviciosue,
                                     porseries, serie_inicial, serie_final, filtrarnosujetas,
                                     solonosujetas, sinnosujetas, exportacion, isp_interior,
                                     exenta_no_deduc, ventas_fuera_tac,cuentap,externo);

 if (registros>1000)
     if (QMessageBox::question(
              this,
              tr("Libro de facturas"),
           tr("Se van a cargar más de 1000 registros ¿ Correcto ?")) == QMessageBox::No )
                            return;


 QProgressDialog progreso("Cargando registros ...",  0, 0, 100);
 progreso.setWindowModality(Qt::WindowModal);
 progreso.setMaximum(registros);
 progreso.setMinimumDuration ( 0 );
 progreso.setWindowTitle(tr("PROCESANDO..."));
 // progreso.setValue(1);
 QApplication::processEvents();

 lfilas.clear();
 apunte.clear();
 fecha_cont.clear();
 fecha_fra.clear();
 documento.clear();
 cuenta.clear();
 descripcion.clear();
 nif.clear();
 asiento.clear();
 bi.clear();
 tipo.clear();
 cuota.clear();
 tipore.clear();
 re.clear();
 total.clear();
 cuota_liquidada.clear();
 cuenta_iva.clear();

 hash.clear();

 if (registros==0 && !paraliquidacion) return;

 QSqlQuery query;
 if (!paraliquidacion)
   query = basedatos::instancia()->registros_emitidas(ejercicio,
      fechacontable,fechaini,
      fechafin, interiores, aib, eib, autofactura,
      rectificativa, autofacturanoue, prestserviciosue,
      porseries, serie_inicial, serie_final, filtrarnosujetas,
      solonosujetas, sinnosujetas, exportacion, isp_interior,
      exenta_no_deduc, ventas_fuera_tac, cuentap, externo);
 else query=basedatos::instancia()->registros_emitidas_liq_caja(fechaini, fechafin);
 //ui.progressBar->setMaximum(registros);
 // QStringList lfilas;
 int numorden=0;
 if ( query.isActive() ) {
    while ( query.next() )
      {
       //ui.progressBar->setValue(numprog);
        progreso.setValue(numorden);
        QApplication::processEvents();

        QString externo=(!paraliquidacion ? query.value(17).toString() : query.value(18).toString());
        bool hayexterno=!externo.isEmpty();


       if (ui.tableWidget->rowCount()-1<numorden) ui.tableWidget->insertRow(numorden);

       cuenta_iva << query.value(17).toString();

       QString cadnum; cadnum.setNum(numorden+1);
       if (query.value(16).toBool())
         {
          // criterio de caja
          // ui.variablestable->setVerticalHeaderLabels(lfilas);
          cadnum+="*";
         }
       lfilas << cadnum;

       QTableWidgetItem *newItem1 = new QTableWidgetItem(query.value(15).toString());
       newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tableWidget->setItem(numorden,0,newItem1);
       apunte << query.value(15).toString();
       hash[query.value(15).toString()] = numorden;

       QDate fechacontab=query.value(1).toDate();
       QString cadfecha=fechacontab.toString("dd-MM-yyyy");
       ui.tableWidget->setItem(numorden,1,new QTableWidgetItem(cadfecha));
       fecha_cont << fechacontab.toString("yyyy-MM-dd");

       QDate fechafra=query.value(11).toDate();
       QString cadfechafra=fechafra.toString("dd-MM-yyyy");
       ui.tableWidget->setItem(numorden,2,new QTableWidgetItem(cadfechafra));
       fecha_fra << fechafra.toString("yyyy-MM-dd");

       ui.tableWidget->setItem(numorden,3,new QTableWidgetItem(query.value(0).toString()));
       documento << query.value(0).toString();

       ui.tableWidget->setItem(numorden,4,new QTableWidgetItem(query.value(2).toString()));
       cuenta << query.value(2).toString();

       QString nombre;

       if (!hayexterno)
         nombre = (query.value(13).toString().isEmpty() ?
                 filtracad(query.value(3).toString()) :
                 filtracad(query.value(13).toString()))
               + " "; // nombre
         else
           nombre=basedatos::instancia()->razon_externo(externo);

       ui.tableWidget->setItem(numorden,5,new QTableWidgetItem(nombre));
       descripcion << nombre;

       QString cif;
       if (!hayexterno)
          cif = (query.value(14).toString().isEmpty() ?
                 cifcta(query.value(2).toString()) :
                 filtracad(query.value(14).toString())); // cif
         else
          cif=basedatos::instancia()->cif_externo(externo);

       ui.tableWidget->setItem(numorden,6,new QTableWidgetItem(cif));
       nif << cif;

       QTableWidgetItem *newItem11 = new QTableWidgetItem(query.value(4).toString());
       newItem11->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tableWidget->setItem(numorden,7,newItem11); // asiento
       asiento << query.value(4).toString();

       // base imponible
        QTableWidgetItem *newItem21 = new QTableWidgetItem(
                formateanumerosep(query.value(5).toDouble(),comadecimal,decimales));
        newItem21->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.tableWidget->setItem(numorden,8,newItem21);
        bi << formateanumerosep(query.value(5).toDouble(),comadecimal,decimales);

        // tipo
        QTableWidgetItem *newItem31 = new QTableWidgetItem(
                formateanumerosep(query.value(6).toDouble(),comadecimal,decimales));
        newItem31->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.tableWidget->setItem(numorden,9,newItem31);
        tipo << formateanumerosep(query.value(6).toDouble(),comadecimal,decimales);

       // cuota
       double valcuota=CutAndRoundNumberToNDecimals (query.value(7).toDouble(),2);
       QTableWidgetItem *newItem41 = new QTableWidgetItem(
            formateanumerosep(valcuota,comadecimal,decimales));
       newItem41->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tableWidget->setItem(numorden,10,newItem41);
       cuota << formateanumerosep(valcuota,comadecimal,decimales);

       // cuota liq
       QTableWidgetItem *newItem42 = new QTableWidgetItem("");
       newItem42->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tableWidget->setItem(numorden,11,newItem42);

       // tipo re
       QTableWidgetItem *newItem51 = new QTableWidgetItem(
               formateanumerosep(query.value(8).toDouble(),comadecimal,decimales));
       newItem51->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tableWidget->setItem(numorden,12,newItem51);
       tipore << formateanumerosep(query.value(8).toDouble(),comadecimal,decimales);

       // re
       double valre=CutAndRoundNumberToNDecimals (query.value(5).toDouble()*
                                                  query.value(8).toDouble()/100,2);
       QTableWidgetItem *newItem61 = new QTableWidgetItem(
            formateanumerosep(valre,comadecimal,decimales));
       newItem61->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tableWidget->setItem(numorden,13,newItem61);
       re << formateanumerosep(valre,comadecimal,decimales);

       // total
       QTableWidgetItem *newItem71 = new QTableWidgetItem(
            formateanumerosep(query.value(10).toDouble(),comadecimal,decimales));
       newItem71->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tableWidget->setItem(numorden,14,newItem71);
       total << formateanumerosep(query.value(10).toDouble(),comadecimal,decimales);

       numorden++;
      }
     }

 // cargamos ahora info para de cuota liquidada para cada línea
 ui.tableWidget->setVerticalHeaderLabels(lfilas);
 for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
    {
     // la columna 0 tiene el apunte en diario
     // double iva_caja_liquidado(QString apuntediario,fechaini,fechafin)
     if (!lfilas.at(veces).contains("*"))
        {
         ui.tableWidget->item(veces,11)->setText(ui.tableWidget->item(veces,10)->text());
         cuota_liquidada << ui.tableWidget->item(veces,14)->text();
        }
       else
           {
             double tanto=basedatos::instancia()->ratio_iva_caja_liquidado(
                     ui.tableWidget->item(veces,0)->text(),
                     ejercicio.isEmpty() ? fechaini : inicioejercicio(ejercicio),
                     ejercicio.isEmpty() ? fechafin : finejercicio(ejercicio));
             QString cadval=ui.tableWidget->item(veces,10)->text();
             if (comadecimal) cadval=cadval.remove(".");
                else cadval=cadval.remove(",");
             double valor=tanto*convapunto(cadval).toDouble();
             valor=CutAndRoundNumberToNDecimals (valor, 2);
             QString cadnum=formateanumerosep(valor,comadecimal,decimales);
             ui.tableWidget->item(veces,11)->setText(cadnum);
             cuota_liquidada << cadnum;
           }
    }

}



void tabla_iva_rep::edit_asiento()
{
 // necesitamos conocer número de asiento y ejercicio


    if (ui.tableWidget->currentItem()==0) return;
    QString pase=ui.tableWidget->item(ui.tableWidget->currentRow(),0)->text();
    QString asiento=basedatos::instancia()->asientodepase(pase);
    QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
    QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

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


void tabla_iva_rep::cons_asiento()
{
    // necesitamos conocer número de asiento y ejercicio


       if (ui.tableWidget->currentItem()==0) return;
       QString pase=ui.tableWidget->item(ui.tableWidget->currentRow(),0)->text();
       QString asiento=basedatos::instancia()->asientodepase(pase);
       QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
       QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

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

}


void tabla_iva_rep::borra_asiento()
{
    if (ui.tableWidget->currentItem()==0) return;
    QString pase=ui.tableWidget->item(ui.tableWidget->currentRow(),0)->text();
    QString asiento=basedatos::instancia()->asientodepase(pase);
    QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
    QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

    borrarasientofunc(asiento,ejercicio,true);
    refrescar();

}



void tabla_iva_rep::ordena_col()
{
    if (ui.tableWidget->currentItem()==0) return;
    bool porvalor=false;
    int columna=0; columna=ui.tableWidget->currentColumn();

    if (columna==0 || columna>6) porvalor=true;

    QStringList col_destino_apunte;
    QStringList col_destino;
    QStringList col_orden;
    QStringList col_orden_apunte=apunte;

    switch (columna)
      {
       case 0:
               col_orden=apunte;
               break;
       case 1:
               col_orden=fecha_cont;
               break;
       case 2:
               col_orden=fecha_fra;
               break;
       case 3:
               col_orden=documento;
               break;
       case 4:
               col_orden=cuenta;
               break;
       case 5:
               col_orden=descripcion;
               break;
       case 6:
               col_orden=nif;
               break;
       case 7:
               col_orden=asiento;
               break;
       case 8:
               col_orden=bi;
               break;
       case 9:
               col_orden=tipo;
               break;
       case 10:
               col_orden=cuota;
               break;
       case 11:
               col_orden=cuota_liquidada;
               break;
       case 12:
               col_orden=tipore;
               break;
       case 13:
               col_orden=re;
               break;
       case 14:
              col_orden=total;
              break;
      }


    // sacamos siempre el mínimo

    for (int veces=0;veces<ui.tableWidget->rowCount();veces++)
       {
         // vamos poblando col_destino y col_destino_apunte
         // con los valores ordenados
         // buscamos valor en col_orden
        int colorigen=0;
        if (porvalor) colorigen=cursor_valor_min_lista(col_orden);
           else colorigen=cursor_cad_min_lista(col_orden);
        // pasamos par de valores de col_orden y col_orden_apunte
        // a col_destino y col_destino_apunte
        col_destino << col_orden.at(colorigen);
        col_destino_apunte << col_orden_apunte.at(colorigen);
        // eliminamos de col_orden y col_orden_apunte la posición colorigen
        col_orden.removeAt(colorigen);
        col_orden_apunte.removeAt(colorigen);
       }
    // tenemos en col_destino y col_destino_apunte
    // la columna ordenada
    // ahora recorremos col_destino_apunte y llenamos la tabla
    // apuntando a las filas de las listas originales
    QStringList cab_filas_orden=lfilas;
   for (int veces=0;veces<col_destino_apunte.count();veces++)
      {
        // primero averiguamos fila en tabla hash
        // QStringList apunte, fecha_cont, fecha_fra, documento, cuenta, descripcion;
        // QStringList nif, asiento, bi, tipo, cuota, tipore, re, total;
        // llenamos la tabla
        int pos=hash[col_destino_apunte.at(veces)];
        cab_filas_orden[veces]=lfilas.at(pos);
        ui.tableWidget->item(veces,0)->setText(apunte.at(pos));
        // OJO !! hay que reformatear las fechas
        QDate fecha1,fecha2;
        fecha1= QDate::fromString (fecha_cont.at(pos), "yyyy-MM-dd" );
        fecha2= QDate::fromString (fecha_fra.at(pos), "yyyy-MM-dd" );
        ui.tableWidget->item(veces,1)->setText(fecha1.toString("dd-MM-yyyy"));
        ui.tableWidget->item(veces,2)->setText(fecha2.toString("dd-MM-yyyy"));

        ui.tableWidget->item(veces,3)->setText(documento.at(pos));
        ui.tableWidget->item(veces,4)->setText(cuenta.at(pos));
        ui.tableWidget->item(veces,5)->setText(descripcion.at(pos));
        ui.tableWidget->item(veces,6)->setText(nif.at(pos));
        ui.tableWidget->item(veces,7)->setText(asiento.at(pos));
        ui.tableWidget->item(veces,8)->setText(bi.at(pos));
        ui.tableWidget->item(veces,9)->setText(tipo.at(pos));
        ui.tableWidget->item(veces,10)->setText(cuota.at(pos));
        ui.tableWidget->item(veces,11)->setText(cuota_liquidada.at(pos));
        ui.tableWidget->item(veces,12)->setText(tipore.at(pos));
        ui.tableWidget->item(veces,13)->setText(re.at(pos));
        ui.tableWidget->item(veces,14)->setText(total.at(pos));

      }
   ui.tableWidget->setVerticalHeaderLabels(cab_filas_orden);
}


int tabla_iva_rep::cursor_valor_min_lista(QStringList lista)
{
  QString cadvalor=lista.at(0);
  double min=convapunto(cadvalor.remove(comadecimal ? '.' : ',')).toDouble();
  int cursor=0;
  for (int veces=0;veces<lista.count();veces++)
     {
      QString cad=lista.at(veces);
      double valor=convapunto(cad.remove(comadecimal ? '.' : ',')).toDouble();
      if (valor<min)
         {
          min=valor ;
          cursor=veces;
         }
     }
  return cursor;
}


int tabla_iva_rep::cursor_cad_min_lista(QStringList lista)
{
  QString min=lista.at(0);
  int cursor=0;
  for (int veces=0;veces<lista.count();veces++)
     {
      if (lista.at(veces)<min)
         {
          min=lista.at(veces);
          cursor=veces;
         }
     }
  return cursor;
}


void tabla_iva_rep::busca_en_col()
{
    if (ui.tableWidget->currentItem()==0) return;
    bool porvalor=false;
    int columna=0; columna=ui.tableWidget->currentColumn();

    if (columna==0 || columna>6) porvalor=true;

    bool apartircursor=false;

    if (porvalor)
      {
       buscar_num *b = new buscar_num();
       if (columna==col_busqueda)
          {
           b->pasadatos(buscanumero,buscamargen);
           b->activarprincipio(false);
          }
          else
              {
                col_busqueda=columna;
              }          
       int resultado=b->exec();
       if (!resultado == QDialog::Accepted) return;
       buscanumero=convapunto(b->cadnum());
       buscamargen=convapunto(b->margen());
       apartircursor=b->a_partir_cursor();
       delete(b);
      }
      else
          {
           buscar_cad *c = new buscar_cad();
           if (columna==col_busqueda)
              {
               c->pasacadena(buscacadena);
               c->activarprincipio(false);
              }
              else
                  {
                    col_busqueda=columna;
                  }
           int resultado=c->exec();
           if (!resultado == QDialog::Accepted) return;
           buscacadena=c->contenido();
           apartircursor=c->a_partir_cursor();
           delete(c);
          }
      int inicio=0;
      if (apartircursor) { inicio=ui.tableWidget->currentRow(); inicio++; }
      bool encontrado=false;
      bool buscado2=false;
      bool salir=false;
      while (!salir)
      {
      for (int veces=inicio; veces<ui.tableWidget->rowCount(); veces++)
         {
           if (porvalor)
             {
               double valor=ui.tableWidget->item(veces,columna)->text().remove(comadecimal ? '.' : ',').toDouble();
               double numero=buscanumero.toDouble();
               double margen=buscamargen.toDouble();
               if (valor>=numero-margen-0.0001 && valor<=numero+margen+0.0001)
                 {
                   encontrado=true;
                   ui.tableWidget->setCurrentCell(veces,columna);
                   break;
                 }
             }
             else
                  {
                    if (ui.tableWidget->item(veces,columna)->text().contains(buscacadena))
                       {
                        encontrado=true;
                        ui.tableWidget->setCurrentCell(veces,columna);
                        break;
                       }
                  }
         }
       if (encontrado || buscado2) salir=true;
       if (!encontrado && !buscado2)
          {
            buscado2=true;
            if (QMessageBox::question(this,
                            tr("Libro de facturas"),
                            tr("La búsqueda no ha tenido éxito.\n"
                               "¿ Desea comenzar desde la primera fila ?"))==QMessageBox::No)
                          salir=true; else inicio=0;
          }
      }
}

void tabla_iva_rep::desglose()
{
   if (!ui.tableWidget->currentIndex().isValid()) return;
   int fila=ui.tableWidget->currentRow();
   double cuotaiva=0;
   double totalfactura=basedatos::instancia()->total_factura_pase_iva(
           ui.tableWidget->item(fila,0)->text(), &cuotaiva);
   desglose_iva_caja *d = new desglose_iva_caja;
   QDate fecha; fecha=fecha.fromString(ui.tableWidget->item(fila,2)->text(),"dd-MM-yyyy");
   d->pasa_info(ui.tableWidget->item(fila,0)->text(),
                ui.tableWidget->item(fila,3)->text(), fecha, totalfactura, cuotaiva);
   d->exec();
   if (d->hay_cambios()) refrescar();
   delete(d);
}


void tabla_iva_rep::solo_ptes_criterio_caja()
{
   for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
      {
       double ivasop_efectivo=convapunto(ui.tableWidget->item(veces,10)->text().remove(comadecimal ? '.' : ',')).toDouble();
       double ivasop_liquidado=convapunto(ui.tableWidget->item(veces,11)->text().remove(comadecimal ? '.' : ',')).toDouble();
       if (ui.ptes_cajacheckBox->isChecked())
          {
           if (ivasop_efectivo-ivasop_liquidado<0.001) ui.tableWidget->hideRow(veces);
             else ui.tableWidget->showRow(veces);
          }
           else
               {
                 ui.tableWidget->showRow(veces);
               }
      }
}


void tabla_iva_rep::calcula_listas_sel()
{
    for (int veces=0; veces<selcuenta_iva.count(); veces++)
       {
        selbi[veces] =0;
        selcuota[veces] =0;
        seltotal[veces] =0;
        selcuota_liquidada[veces]=0;
       }


    for (int veces=0; veces<cuenta_iva.count(); veces++)
        {
          QDate fecha;
          fecha=fecha.fromString(fecha_cont.at(veces),"yyyy-MM-dd");
          // investigamos si la posición actual tiene representación en selcuenta_iva y seltipo
          bool encontrado=false;
          for (int a=0; a<selcuenta_iva.count(); a++)
            {
              if (selcuenta_iva.at(a)==cuenta_iva.at(veces) &&
                  seltipo.at(a)==tipo.at(veces))
                 {
                  encontrado=true;
                  // asignamos valores a la posición
                  QString cadval;
                  cadval=bi.at(veces);
                  cadval=comadecimal ? cadval.remove(".") : cadval.remove(",");
                  if (fecha>=fechaini && fecha<=fechafin)
                     selbi[a]+=convapunto(cadval).toDouble();

                  cadval=cuota.at(veces);
                  cadval=comadecimal ? cadval.remove(".") : cadval.remove(",");
                  if (fecha>=fechaini && fecha<=fechafin)
                     selcuota[a]+=convapunto(cadval).toDouble();

                  cadval=total.at(veces);
                  cadval=comadecimal ? cadval.remove(".") : cadval.remove(",");
                  if (fecha>=fechaini && fecha<=fechafin)
                    seltotal[a]+=convapunto(cadval).toDouble();

                  cadval=cuota_liquidada.at(veces);
                  cadval=comadecimal ? cadval.remove(".") : cadval.remove(",");
                  selcuota_liquidada[a]+=convapunto(cadval).toDouble();
                  break;
                 }
            }
          if (!encontrado)
            {
              // añadimos elemento a listas sel
              selcuenta_iva << cuenta_iva.at(veces);
              seltipo << tipo.at(veces);

              QString cadval;
              cadval=bi.at(veces);
              cadval=comadecimal ? cadval.remove(".") : cadval.remove(",");
              if (fecha>=fechaini && fecha<=fechafin)
                selbi << convapunto(cadval).toDouble();
               else selbi << 0;

              cadval=cuota.at(veces);
              cadval=comadecimal ? cadval.remove(".") : cadval.remove(",");
              if (fecha>=fechaini && fecha<=fechafin)
                selcuota << convapunto(cadval).toDouble();
               else
                   selcuota << 0;

              cadval=total.at(veces);
              cadval=comadecimal ? cadval.remove(".") : cadval.remove(",");
              if (fecha>=fechaini && fecha<=fechafin)
                seltotal << convapunto(cadval).toDouble();
               else
                   seltotal << 0;

              cadval=cuota_liquidada.at(veces);
              cadval=comadecimal ? cadval.remove(".") : cadval.remove(",");
              if (fecha>=fechaini && fecha<=fechafin)
                selcuota_liquidada << convapunto(cadval).toDouble();
               else
                  selcuota_liquidada << 0;
            }
        }
    // QString cadnum; cadnum.setNum(selcuenta);
    // QMessageBox::warning( this, tr("Tabla IVA"),cadnum);

}

QStringList tabla_iva_rep::qselcuenta_iva()
{
  return selcuenta_iva;
}

QStringList tabla_iva_rep::qseltipo()
{
  return seltipo;
}

QList<double> tabla_iva_rep::qselbi()
{
  return selbi;
}

QList<double> tabla_iva_rep::qselcuota()
{
  return selcuota;
}

QList<double> tabla_iva_rep::qseltotal()
{
  return seltotal;
}

QList<double> tabla_iva_rep::qselcuota_liquidada()
{
  return selcuota_liquidada;
}
