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

#include "execasientomodelo.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include "tabla_asientos.h"
#include "introci.h"
#include "aritmetica.h"
#include "aux_express.h"
#include "directorio.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include "busca_externo.h"
#include "externos.h"

execasientomodelo::execasientomodelo(QString qusuario, bool borrador) : QDialog() {
    ui.setupUi(this);

    etiqueta = new CustomQLabel;
    etiqueta->setHidden(false);
    etiqueta->setMinimumWidth(100);
    etiqueta->setMinimumHeight(50);
    etiqueta->setText(tr("Soltar archivo aquí"));
    etiqueta->setAlignment(Qt::AlignHCenter);
    etiqueta->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    ui.verticalLayout->addWidget(etiqueta);

 QStringList columnas;
 columnas << tr("Descripción") << tr("Contenido") << tr("Variable") << tr("Tipo") << tr("Fórmula");
 ui.variablestable->setHorizontalHeaderLabels(columnas);
 // ui.variablestable->setEditTriggers(QAbstractItemView::AnyKeyPressed);
  // ui.variablestable->setSelectionBehavior ( QAbstractItemView::SelectItems);
 // ui.variablestable->setSelectionMode ( QAbstractItemView::NoSelection);
 ui.variablestable->setTabKeyNavigation(true); 
 numvariables=0;
 ui.variablestable->hideColumn(0);
 ui.variablestable->hideColumn(2);
 ui.variablestable->hideColumn(3);
 ui.variablestable->hideColumn(4);
 // ui.variablestable->verticalHeader()->hide();
 ui.variablestable->setColumnWidth(1,150);
 ui.variablestable->setCurrentCell(0,1);
 ui.variablestable->setFocus();
 comadecimal=haycomadecimal();
 decimales=haydecimales();
 con_aspiradora=false;
 chequeado=false;
 activa_msj_tabla=false;

  connect(ui.variablestable,SIGNAL( cellChanged ( int , int )),this,
           SLOT(valorcambiado(int,int )));

 connect(ui.generapushButton,SIGNAL(  clicked()),this,SLOT(botongenerar()));
 connect(ui.verificarpushButton,SIGNAL(  clicked()),this,SLOT(botonverificar()));

 connect(ui.variablestable,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(posicioncambiada ( int, int)));

 connect(ui.subcuentapushButton,SIGNAL(  clicked()),this,SLOT(botonsubcuenta()));

 connect(ui.cipushButton,SIGNAL(  clicked()),this,SLOT(botonci()));

 connect(ui.fichdocpushButton ,SIGNAL(clicked()),SLOT(buscacopiadoc()));
 connect(ui.visdocpushButton ,SIGNAL(clicked()),SLOT(visdoc()));
 connect(ui.borraasdocpushButton ,SIGNAL(clicked()),SLOT(borraasignaciondoc()));

 connect(ui.busca_externo_pushButton,SIGNAL(clicked(bool)),SLOT(buscar_externo()));
 connect(ui.quita_externo_pushButton,SIGNAL(clicked(bool)),SLOT(borra_externo()));
 connect(ui.datos_externo_pushButton,SIGNAL(clicked(bool)),SLOT(ver_externo()));
 connect(ui.aspirador_pushButton,SIGNAL(clicked(bool)),SLOT(exec_aspiradora()));

 connect(etiqueta, SIGNAL(filedroped()),SLOT(fichero_soltado()));

 usuario=qusuario;

 aib=false;
 autofactura_ue=false;
 autofactura_no_ue=false;
 eib=false;
 eib_servicios=false;
 isp_op_interiores=false;
 importacion=false;
 exportacion=false;
 diario_borrador=borrador;
 if (basedatos::instancia()->analitica_tabla()) ui.cipushButton->hide();
 if (!conanalitica()) ui.cipushButton->hide();
 if (!basedatos::instancia()->hay_externos())
    {
     ui.externo_label->hide();
     ui.externo_lineEdit->hide();
     ui.busca_externo_pushButton->hide();
     ui.datos_externo_pushButton->hide();
     ui.quita_externo_pushButton->hide();
    }
}

void execasientomodelo::activar_msj_tabla()
{
   activa_msj_tabla=true;
}

void execasientomodelo::cargadatos( QString asientomodelo )
{
    QSqlQuery query = basedatos::instancia()->selectFechacab_as_modeloasientomodelo(asientomodelo);
    if ( (query.isActive()) && (query.next()) )
    {
       /* "aib                   bool,"
       "autofactura_ue        bool,"
       "autofactura_no_ue     bool,"
       "eib                   bool,"
       "eib_servicios         bool," */
       aib=query.value(2).toBool();
       autofactura_ue=query.value(3).toBool();
       autofactura_no_ue=query.value(4).toBool();
       eib=query.value(5).toBool();
       eib_servicios=query.value(6).toBool();
       diario=query.value(7).toString();
       isp_op_interiores=query.value(8).toBool();
       importacion=query.value(9).toBool();
       exportacion=query.value(10).toBool();

   } else return;




    ui.modelolabel->setText(asientomodelo);
    int filas = basedatos::instancia()->selectCountvariablevar_as_modeloasientomodelo(asientomodelo);

   ui.variablestable->setRowCount(filas);
   numvariables=filas;
   

    query =
        basedatos::instancia()->selectTablaVariable(asientomodelo);

   filas=0;
   QStringList lfilas;

  ui.variablestable->disconnect(SIGNAL(cellChanged(int,int)));

   if ( query.isActive() ) {
          while  ( query.next() ) 
                      {
                        mensajes << query.value(2).toString();
                        QTableWidgetItem *newItem = new QTableWidgetItem(query.value(2).toString());
                        ui.variablestable->setItem(filas,0,newItem);
                        QString mensaje = query.value(1).toString()!=tipoCALC() ? query.value(2).toString() :
                                                                          query.value(2).toString() + "*";
                        lfilas << mensaje;
                        valdefecto << (query.value(1).toString()!=tipoCALC() ? query.value(3).toString() : "");
                        QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(0).toString());
                        ui.variablestable->setItem(filas,2,newItem2);

                        QTableWidgetItem *newItem3 = new QTableWidgetItem(query.value(1).toString());
                        ui.variablestable->setItem(filas,3,newItem3);

                        guardar[filas]=query.value(4).toBool();


                        if (query.value(1).toString()==tipoFecha())
                           {
                            QDateEdit  *fecha = new QDateEdit();
                            fecha->setDisplayFormat("dd.MM.yyyy");
                            if (query.value(3).toString().length()>0)
                               {
                                QDate fechavar;
                                fechavar=fechavar.fromString(query.value(3).toString(),"dd.MM.yyyy");
                                fecha->setDate(fechavar);
                               }
                               else
                                  fecha->setDate(QDate::currentDate());
                            ui.variablestable->setCellWidget(filas,1,fecha);
                           }
                           else
                               {
                                QString contenido;
                                if (query.value(1).toString()==tipoCALC())
                                  {
                                    QTableWidgetItem *newItemz = new QTableWidgetItem(query.value(3).toString());
                                    ui.variablestable->setItem(filas,4,newItemz);
                                  }
                                  else contenido=query.value(3).toString();
                                QTableWidgetItem *newItem1 = new QTableWidgetItem(contenido);
                                ui.variablestable->setItem(filas,1,newItem1);
                               }
 	                filas++;
	              }
      }

  connect(ui.variablestable,SIGNAL( cellChanged ( int , int )),this,
        SLOT(valorcambiado(int,int )));

  ui.variablestable->setVerticalHeaderLabels(lfilas);

  posicioncambiada(0,1);
  ui.variablestable->setCurrentCell(0,1);
  ui.variablestable->resizeColumnToContents ( 0 );
}


bool execasientomodelo::okvariables()
{
   for (int veces=0;veces<numvariables;veces++)
    {
       if (ui.variablestable->item(veces,3)->text()==tipoConceptoSII())
           concepto_sii=ui.variablestable->item(veces,1)->text();

       if (ui.variablestable->item(veces,1)!=0)
          {
           if (ui.variablestable->item(veces,3)->text()==tipoNumerico() && 
	    !cadnumvalido(convapunto(ui.variablestable->item(veces,1)->text())) &&
	    ui.variablestable->item(veces,1)->text().length()>0)
                  {
                    QMessageBox::warning( this, tr("Asiento automático"),
                 tr("ERROR en contenido numérico"));
	            return false;
                  }
           if (ui.variablestable->item(veces,3)->text()==tipoSubcuenta() &&
             (!existesubcuenta(ui.variablestable->item(veces,1)->text())))
                  {
                    QMessageBox::warning( this, tr("Asiento automático"),
			     tr("ERROR en código de cuenta"));
	            return false;
                  }
          if (ui.variablestable->item(veces,3)->text()==tipoCI() && 
               ui.variablestable->item(veces,1)->text().length()>0 &&
              !ciok(ui.variablestable->item(veces,1)->text()))
                  {
                    QMessageBox::warning( this, tr("Asiento automático"),
			     tr("ERROR en código de imputación: ")+ui.variablestable->item(veces,1)->text());
	            return false;
                  }
         }
    }
   return true;
}


void execasientomodelo::valorcambiado(int fila,int columna)
{
   if (columna==0)
      {
       ui.variablestable->disconnect(SIGNAL(cellChanged(int,int)));
       ui.variablestable->item(fila,columna)->setText(mensajes.at(fila));
       connect(ui.variablestable,SIGNAL( cellChanged ( int , int )),this,
           SLOT(valorcambiado(int,int )));
       return;
      }
   QString cadena2;
   if (ui.variablestable->item(fila,1)!=0)
    {
     if (ui.variablestable->item(fila,3)->text()==tipoNumerico())
      {
        double num=convapunto(ui.variablestable->item(fila,1)->text()).toDouble();
        ui.variablestable->item(fila,1)->setText(formateanumero(num,comadecimal,decimales));
      }

     // hacer bucle aquí buscando variables calculadas
     if (ui.variablestable->item(fila,3)->text()!=tipoCALC()
             && ui.variablestable->item(fila,3)->text()!=tipoFecha()) // evitamos cambios si editamos la propia variable calc
      {
        int veces=0;
        while (veces<ui.variablestable->rowCount())
          {
           if (ui.variablestable->item(veces,3)->text()!=tipoFecha() &&
                   ui.variablestable->item(veces,3)->text()==tipoCALC())
             {
              double num=convapunto(asignavalorexpresion(ui.variablestable->item(veces,4)->text())).toDouble();
              ui.variablestable->item(veces,1)->setText(formateanumero(num,comadecimal,decimales));
             }
           veces++;
          }
      }

     if (ui.variablestable->item(fila,3)->text()==tipoSubcuenta())
      {
       ui.variablestable->disconnect(SIGNAL(cellChanged(int,int)));
        ui.variablestable->item(fila,columna)->setText(
            expandepunto(ui.variablestable->item(fila,columna)->text(),anchocuentas()));
        if (!existesubcuenta(ui.variablestable->item(fila,columna)->text()) &&
            ( ui.variablestable->item(fila,columna)->text().length()==anchocuentas()
              || cod_longitud_variable()))
           {
            if (QMessageBox::question(
              this,
              tr("Asientos automáticos"),
              tr("Cuenta inexistente ¿ Desea añadirla ?")) ==QMessageBox::Yes )
                {
                 aux_express *lasubcuenta2=new aux_express();
                 lasubcuenta2->pasacodigo(ui.variablestable->item(fila,columna)->text());
                 lasubcuenta2->exec();
                 delete(lasubcuenta2);
                }
           }
        if (!existesubcuenta(ui.variablestable->item(fila,columna)->text()) || 
	      (ui.variablestable->item(fila,columna)->text().length()!=anchocuentas() 
                   && !cod_longitud_variable()))
          {
                  buscasubcuenta *labusqueda=new
                                buscasubcuenta(ui.variablestable->item(fila,columna)->text());
                  int resultado=labusqueda->exec();
                  if (resultado==QDialog::Accepted)
                    {
                     cadena2=labusqueda->seleccioncuenta();
                     if (existesubcuenta(cadena2))
                      ui.variablestable->item(fila,columna)->setText(cadena2);
                     else ui.variablestable->item(fila,columna)->setText("");
                    }
                  delete labusqueda;
          }
       connect(ui.variablestable,SIGNAL( cellChanged ( int , int )),this,
           SLOT(valorcambiado(int,int )));
      }
     }
   if (fila+1<ui.variablestable->rowCount()) ui.variablestable->setCurrentCell(fila+1,columna);
}


void execasientomodelo::botongenerar()
{
   if (!okvariables())
     {
        return;
     }
   generaasiento(false);
   ui.variablestable->setCurrentCell(0,1);
   // accept();
   etiqueta->borra_imagen();
}


void execasientomodelo::botonverificar()
{
   if (!okvariables())
     {
        return;
     }
   generaasiento(true);
}


void execasientomodelo::generaasiento(bool verificar)
{
   tabla_asientos *tablaasiento=new tabla_asientos(comadecimal,decimales,usuario);
   tablaasiento->pasafichprov(ui.fichdoclineEdit->text());
   tablaasiento->predefinido();
   if (diario_borrador) tablaasiento->set_borrador();
   if (activa_msj_tabla) tablaasiento->pasanocerrar(true);
   if (!ui.forzar_ed_venci_checkBox->isChecked()) tablaasiento->evita_pregunta_venci();
   QSqlQuery query = basedatos::instancia()->selectFechacab_as_modeloasientomodelo( ui.modelolabel->text() );
   QString cadfecha;
   QDate fecha;
   if ( query.isActive() ) {
          if  ( query.next() ) 
                      {
	               cadfecha=query.value(0).toString();
	               fecha=asignafechaasiento(cadfecha);
	               tablaasiento->pasafechaasiento(fecha);
	              }
      }
   if (aib || autofactura_ue || autofactura_no_ue || isp_op_interiores)
      {
       tablaasiento->activa_aibautonomo();
       if (verificar) tablaasiento->quitavar_aibautonomo();
      }
   if (eib || eib_servicios)
      {
       tablaasiento->activa_eibautonomo();
       if (verificar) tablaasiento->quitavar_eibautonomo();
      }
   diario=asignaconcepto(query.value(7).toString());
   tablaasiento->pasadiario(diario);
   QString autofactura;
   if (autofactura_ue) autofactura="1";
   query = basedatos::instancia()->selectDet_as_modeloasientomodeloorderorden( ui.modelolabel->text() );

   QString cuenta;
   QString concepto;
   QString valor;
   double numvalor=0;
   QString d_h;
   QString baseiva;
   double numbaseiva=0;
   QString cuentafra;
   QString cuentabaseiva;
   QString claveiva;
   QString documento;
   QString expresion,expresion2;
   double tipoiva;
   QString cadtipoiva;
   QString ctipoiva;
   QString ctipore;
   QString cuota;
   QString ci;
   bool hayivasoportado=false;
   int fila=0;
   int filaexento=-1;
   if ( query.isActive() ) {
          while  ( query.next() ) 
                      {
	               cuenta=asignacodigocuenta(query.value(0).toString());
	               concepto=asignaconcepto(query.value(1).toString());
                   expresion=query.value(2).toString();  // debe o haber
	               d_h=query.value(3).toString();
                   expresion2=query.value(5).toString(); // base iva
	               cuentabaseiva=asignacodigocuenta(query.value(6).toString());
	               cuentafra=asignacodigocuenta(query.value(7).toString());
	               claveiva=query.value(8).toString();
                       if ((aib || autofactura_ue || autofactura_no_ue) && verificar
                         && (escuentadeivasoportado(cuenta) || escuentadeivarepercutido(cuenta)))
                        continue;
	               if (escuentadeivasoportado(cuenta)) 
                           { tipoiva=tipoivanore(claveiva); hayivasoportado=true;}
                        else
                           { if (!hayivasoportado) tipoiva=tipoivaefectivo(claveiva);
                              else tipoiva=tipoivanore(claveiva); }
	               cadtipoiva.setNum(tipoiva,'f',4);
	               cadtipoiva=cadtipoiva.trimmed();
	               expresion.replace("{TIPO_IVA}",cadtipoiva);
	               expresion2.replace("{TIPO_IVA}",cadtipoiva);
                   valor=asignavalorexpresion(expresion); // es el valor del cargo/abono
	               baseiva=asignavalorexpresion(expresion2);
	               documento=asignaconcepto(query.value(9).toString());
                       ci=asignaci(query.value(4).toString());
                       QString afecto;
                       if (query.value(21).toString().trimmed().isEmpty()) afecto="0";
                          else
                            afecto=asignavalorexpresion( query.value(21).toString() );

                   if (!existesubcuenta(cuenta))
		           {
		            QMessageBox::warning( this, tr("Asiento automático"),
                            tr("Código de cuenta erróneo; apunte no procesado"));
		            continue;
		           }
                        if((escuentadeingreso(cuenta) || escuentadegasto(cuenta))
                            && ci.length()==0 && conanalitica() && !conanalitica_parc() &&
                                    !basedatos::instancia()->analitica_tabla())
                          {
		           QMessageBox::warning( this, tr("Asiento modelo"),
                                                 tr("Advertencia: Código de imputación no asignado;"));
                           verificar=true;
                           // continue; basedatos::instancia()->analitica_tabla()
                           // conanalitica_parc()
                           // codigo_en_analitica(cuenta)

                          }
                        if(conanalitica_parc() && codigo_en_analitica(cuenta) &&
                                    !basedatos::instancia()->analitica_tabla())
                          {
                           QMessageBox::warning( this, tr("Asiento modelo"),
                                                 tr("Advertencia: Código de imputación no asignado;"));
                           verificar=true;
                           // continue; basedatos::instancia()->analitica_tabla()
                           // conanalitica_parc()
                           // codigo_en_analitica(cuenta)

                          }
                   numvalor=valor.toDouble();
	               numbaseiva=baseiva.toDouble();
                   if ((numvalor<0.01 && numvalor>-0.01) && !escuentadeivasoportado(cuenta) && !escuentadeivarepercutido(cuenta) ) continue;
	               QString debe="",haber="";
	               if (d_h==tipoDEBE()) debe=valor; else haber=valor;
                       if (cuentafra.length()>0 &&
                           (claveiva==tipo_exento() || claveiva == tipo_nosujeto()
                             || claveiva == tipo_exento_nodeduc()) &&
                           !escuentadeivasoportado(cuenta)
                           && !escuentadeivarepercutido(cuenta) && filaexento<0)
                          {
                           // operación IVA exento en libros de IVA
		            if (!existesubcuenta(cuentafra))
 		               {
		                 QMessageBox::warning( this, tr("Asiento modelo"),
					tr("Cuenta factura errónea; apunte no procesado"));
		                 continue;
		               }
		            if (cuentabaseiva.length()>0 && !existesubcuenta(cuentabaseiva))
 		              {
		               QMessageBox::warning( this, tr("Asiento modelo"),
					tr("Cuenta base iva errónea; apunte no procesado"));
		               continue;
		              }
		            if (numbaseiva<0.01 && numbaseiva>-0.01)
 		               {
		                QMessageBox::warning( this, tr("Asiento modelo"),
					tr("Base imponible incorrecta; apunte no procesado"));
		                continue;
		               }

		            tablaasiento->pasadatos1(fila,cuenta,concepto,debe,
					     haber,documento,ci);

		            QString cadfecha2;
                            if (query.value(10).toString().length()>0)
                               {
                                QDate lafecha;
                                // habría que calcular el valor de la variable en posición 8
                                lafecha=asignafechaasiento(query.value(10).toString());
                                if (lafecha.isValid())
                                   cadfecha2=lafecha.toString("dd-MM-yyyy");
                                     else cadfecha2=fecha.toString("dd-MM-yyyy");
                               }
                             else
                               cadfecha2=fecha.toString("dd-MM-yyyy");
		            QString cadsoportado="0";
		            cuota="0";
		            baseiva=baseiva.trimmed();
                            if (d_h==tipoHABER())
                               {
                                // Suponemos libro de facturas recibidas
                                cadsoportado="1";
                               }
                            ctipoiva="";
                            ctipore="";
                            QString rectificativa= query.value(12).toBool() ? "1" : "0";
                            // QString autofactura= query.value(13).toBool() ? "1" : "0";
                            QString cadfechaop;
                            if (query.value(13).toString().length()>0)
                               {
                                QDate lafecha;
                                // habría que calcular el valor de la variable en posición 8
                                lafecha=asignafechaasiento(query.value(13).toString());
                                if (lafecha.isValid())
                                   cadfechaop=lafecha.toString("yyyy-MM-dd");
                                  else
                                      cadfechaop=cadfecha2.mid(6,4)+"-"+cadfecha2.mid(3,2)+"-"+
                                               cadfecha2.mid(0,2);
                               }
                               else cadfechaop=cadfecha2.mid(6,4)+"-"+cadfecha2.mid(3,2)+"-"+
                                               cadfecha2.mid(0,2);
                            QString claveop=asignaconcepto(query.value(14).toString());
                            QString bicoste=asignavalorexpresion( query.value(15).toString() );
                            QString rectificada=asignaconcepto( query.value(16).toString() );
                            double valor=asignavalorexpresion( query.value(17).toString() ).toDouble();
                            QString numfacts; numfacts.setNum(valor,'f',0);
                            QString facini=asignaconcepto( query.value(18).toString() );
                            QString facfinal=asignaconcepto( query.value(19).toString() );
                            QString binversion= query.value(20).toBool() ? "1" : "0";
                            QString afecto="1";
                            QString agrario=query.value(22).toBool() ? "1" : "0";
                            QString nombre=asignaconcepto( query.value(23).toString() );
                            QString cif=asignaconcepto( query.value(24).toString() );
                            QString exento_no_deduc=claveiva == tipo_exento_nodeduc() ? "1" : "";
                            QString qimportacion= importacion ? "1" : "";
                            QString qexportacion= exportacion ? "1" : "";
                            QString qisp_op_interiores= isp_op_interiores ? "1" : "";
                            // esto es para exento
                            tablaasiento->pasadatos2(fila,cuentabaseiva,baseiva,
					     claveiva,ctipoiva,ctipore,
                                             cuota,cuentafra,
                                             cadfecha2.mid(0,2),cadfecha2.mid(3,2),
                                             cadfecha2.mid(6,4),cadsoportado,"",
                                             rectificativa,autofactura,
                                             cadfechaop, claveop,bicoste, rectificada, numfacts,
                                             facini, facfinal, binversion,afecto,agrario,
                                             nombre,cif,qimportacion,qexportacion,exento_no_deduc,
                                             qisp_op_interiores,"");
                            if (eib_servicios) tablaasiento->make_linea_prest_servicio_ue(fila);
                            if (claveiva == tipo_nosujeto()) tablaasiento->activa_fila_nosujeta(fila);
                            tablaasiento->chequeatotales();
                            filaexento=fila;
                            fila++;
                            continue;
                          }

                       if (escuentadeivasoportado(cuenta) || escuentadeivarepercutido(cuenta))
	                   {
                            if (cuentafra.length()==0)
                             {
                              QMessageBox::warning( this, tr("Asiento modelo"),
                              tr("Cuenta factura errónea; apunte no procesado"));
                              continue;
                             }
                       if (!existesubcuenta(cuentafra))
                        {
		                 QMessageBox::warning( this, tr("Asiento modelo"),
                         tr("Cuenta factura errónea; apunte no procesado"));
		                 continue;
                        }
                       if (cuentabaseiva.length()>0 && !existesubcuenta(cuentabaseiva))
                       {
                        QMessageBox::warning( this, tr("Asiento modelo"),
                         tr("Cuenta base iva errónea; apunte no procesado"));
                        continue;
                       }
                      if (numbaseiva<0.01 && numbaseiva>-0.01)
 		               {
		                QMessageBox::warning( this, tr("Asiento modelo"),
                        tr("Base imponible incorrecta; apunte no procesado"));
		                continue;
		               }

                       double afeccion=100;
                       if (afecto.length()>0) afeccion=afecto.toDouble();
                       if (afeccion<0.00001 && afeccion >-0.00001) afeccion=100;
                       double afeccion_uno=afeccion/100;
                       int ndecimales=0;
                       if (decimales) ndecimales=2;
                       /*if (!(hay_prorratageneral() || escuentadeivarepercutido(cuenta)) &&
                                (numvalor-CutAndRoundNumberToNDecimals(numbaseiva*tipoiva*afeccion_uno,ndecimales)>0.011 ||
                                 numvalor-CutAndRoundNumberToNDecimals(numbaseiva*tipoiva*afeccion_uno,ndecimales)<-0.011))
                          {
                             QMessageBox::warning( this, tr("Asiento modelo"),
                                     tr("Base imponible ó cuota incorrectos; apunte no procesado"));
                             continue;
                          } */
                        QString prorrata;
                        double numprorrata=prorrata_iva();

                        if (prorrata_especial()) numprorrata=query.value(11).toString().toDouble();
                        prorrata.setNum(CutAndRoundNumberToNDecimals (numprorrata/100,2),'f',2); // ojo pasa en tanto por uno a la tabla asientos
                        /* if ((hay_prorratageneral() || prorrata_especial()) && escuentadeivasoportado(cuenta) &&
                           (numvalor-CutAndRoundNumberToNDecimals(numbaseiva*tipoiva*numprorrata*afeccion_uno,ndecimales)/100>0.01 ||
                           numvalor-CutAndRoundNumberToNDecimals(numbaseiva*tipoiva*numprorrata*afeccion_uno,ndecimales)/100<-0.01))
                          {
                           QMessageBox::warning( this, tr("Asiento modelo"),
                           tr("Base imponible ó cuota incorrectos; apunte no procesado"));
                                //QMessageBox::warning( this, tr("Asiento modelo"),
                                //        tr("Valor: %1 -- Resultado: %2").arg(numvalor,0,'F',2).
                                //        arg(numbaseiva*tipoiva*prorrata_iva()/100,0,'F',2));
                           continue;
                          } */
                         tablaasiento->pasadatos1(fila,cuenta,concepto,debe,
					     haber,documento,ci);
                            cadvalorestipoiva(claveiva,&ctipoiva,&ctipore);
                         QString cadfecha2;
                            if (query.value(10).toString().length()>0)
                               {
                                QDate lafecha;
                                // habría que calcular el valor de la variable en posición 8
                                lafecha=asignafechaasiento(query.value(10).toString());
                                if (lafecha.isValid()) cadfecha2=lafecha.toString("dd.MM.yyyy");
                                   else cadfecha2=fecha.toString("dd.MM.yyyy");
                               }
                             else
		               cadfecha2=fecha.toString("dd.MM.yyyy");
                       QString cadsoportado="0";
                            cuota=valor.trimmed(); // valor es el cargo/abono
                            if (hay_prorratageneral() || prorrata_especial() || afeccion<100)
                               {
                                double numbaseivaxtipoiva=CutAndRoundNumberToNDecimals (
                                        numbaseiva*tipoiva, 2);
                                cuota.setNum(numbaseivaxtipoiva,'f',2);
                               }
                      baseiva=baseiva.trimmed();
                      if (escuentadeivasoportado(cuenta))
		                {
		                 cadsoportado="1";
		                 ctipore="";
		                 if  (d_h==tipoHABER())
			             {
			              // hay que invertir signo de base y cuota
			              if (baseiva[0]!='-') baseiva="-"+baseiva;
			                else baseiva=baseiva.remove('-');
			              if (cuota[0]!='-') cuota="-"+cuota;
			                 else cuota=cuota.remove('-');
			             }
		                }
                   if (escuentadeivarepercutido(cuenta) && d_h==tipoDEBE())
                    {
                     // hay que invertir signo de base y cuota
                     if (baseiva[0]!='-') baseiva="-"+baseiva;
                       else baseiva=baseiva.remove('-');
                     if (cuota[0]!='-') cuota="-"+cuota;
                        else cuota=cuota.remove('-');
                    }

                    QString rectificativa= query.value(12).toBool() ? "1" : "0";
                      // QString autofactura= query.value(13).toBool() ? "1" : "0";
                    QString cadfechaop;
                    if (query.value(13).toString().length()>0)
                         {
                           QDate lafecha;
                           // habría que calcular el valor de la variable en posición 8
                           lafecha=asignafechaasiento(query.value(13).toString());
                           if (lafecha.isValid())
                              cadfechaop=lafecha.toString("yyyy-MM-dd");
                            else cadfechaop=cadfecha2.mid(6,4)+"-"+cadfecha2.mid(3,2)+"-"+
                                               cadfecha2.mid(0,2);

                         }
                        else cadfechaop=cadfecha2.mid(6,4)+"-"+cadfecha2.mid(3,2)+"-"+
                                           cadfecha2.mid(0,2);
                      QString claveop=asignaconcepto(query.value(14).toString());
                      QString bicoste=asignavalorexpresion( query.value(15).toString() );
                      QString rectificada=asignaconcepto( query.value(16).toString() );
                      double valor=asignavalorexpresion( query.value(17).toString() ).toDouble();
                      QString numfacts; numfacts.setNum(valor,'f',0);
                      QString facini=asignaconcepto( query.value(18).toString() );
                      QString facfinal=asignaconcepto( query.value(19).toString() );
                      QString binversion= query.value(20).toBool() ? "1" : "0";
                      if (afeccion_uno>0.0001)
                         afecto.setNum(afeccion_uno,'f',4);
                       else afecto="1";
                       QString agrario=query.value(22).toBool() ? "1" : "0";
                       QString nombre=asignaconcepto( query.value(23).toString() );
                       QString cif=asignaconcepto( query.value(24).toString() );
                       QString qimportacion= importacion ? "1" : "";
                       QString qexportacion= exportacion ? "1" : "";
                       QString qisp_op_interiores= isp_op_interiores ? "1" : "";
                       QString caja_iva;
                       if (basedatos::instancia()->hay_criterio_caja_config())
                           caja_iva="1";
                          else
                              {
                               if (basedatos::instancia()->hay_criterio_caja_aux(cuentafra))
                                  caja_iva="1";
                              }

                      tablaasiento->pasadatos2(fila,cuentabaseiva,baseiva,
                                             claveiva,ctipoiva,ctipore,
                                             cuota,cuentafra,
                                             cadfecha2.mid(0,2),cadfecha2.mid(3,2),
                                             cadfecha2.mid(6,4),cadsoportado,prorrata,
                                             rectificativa,autofactura,
                                             cadfechaop, claveop,bicoste, rectificada, numfacts,
                                             facini, facfinal, binversion, afecto, agrario,
                                             nombre,cif,qimportacion, qexportacion,"",
                                             qisp_op_interiores, caja_iva);
                      if (autofactura_no_ue) tablaasiento->make_linea_autofactura_no_ue(fila);
                      if (eib_servicios) tablaasiento->make_linea_prest_servicio_ue(fila);
                       tablaasiento->chequeatotales();
                     } else // no es cuenta de IVA
                      {
	                    tablaasiento->pasadatos1(fila,cuenta,concepto,debe,haber,documento,ci);
                            // aquí es donde procesaríamos las retenciones
                            QString cta_retenido=asignaconcepto( query.value(25).toString() );
                            QString arrendamiento=query.value(26).toBool() ? "1" : "0";
                            QString clave=asignaconcepto( query.value(27).toString() );
                            QString base_ret=asignavalorexpresion( query.value(28).toString() );
                            //QMessageBox::information( this, tr("base ret"),
                            //                base_ret);
                            QString tipo_ret=asignavalorexpresion(query.value(29).toString(),true);
                            //QMessageBox::information( this, tr("tipo ret"),
                            //                tipo_ret);
                            QString retencion=asignavalorexpresion(query.value(30).toString());
                            //QMessageBox::information( this, tr("retencion"),
                            //                retencion);
                            QString ing_a_cta=asignavalorexpresion(query.value(31).toString());
                            QString ing_a_cta_rep=asignavalorexpresion(query.value(32).toString());
                            QString nombre_ret=asignaconcepto( query.value(33).toString() );
                            QString cif_ret=asignaconcepto( query.value(34).toString() );
                            QString provincia=asignaconcepto( query.value(35).toString() );
                            bool procesar_ret=true;
                            if (!existesubcuenta(cta_retenido)) procesar_ret=false;
                            double cuota=retencion.toDouble()+ing_a_cta.toDouble();
                            double cuota_cal=base_ret.toDouble()*tipo_ret.toDouble();
                            //QString cadnum;
                            //cadnum.setNum(cuota,'f',4); QMessageBox::information( this, tr("retencion"),cadnum);
                            //cadnum.setNum(cuota_cal,'f',4); QMessageBox::information( this, tr("retencion"),cadnum);
                            if (cuota-cuota_cal>0.01 || cuota-cuota_cal<-0.01) procesar_ret=false;
                            // ponemos tipo_ret en tanto por cien, para pasarlo a la tabla de asientos
                            //if (procesar_ret)
                            //    QMessageBox::information( this, tr("retencion"), "procesar ret cantidades");
                            tipo_ret.setNum(tipo_ret.toDouble()*100,'f',2);
                            tipo_ret=convacoma(tipo_ret);
                            if (procesar_ret)
                               tablaasiento->pasadatos_ret(fila,cta_retenido, arrendamiento, clave,
                                                        base_ret, tipo_ret, retencion, ing_a_cta,
                                                        ing_a_cta_rep, nombre_ret, cif_ret, provincia);

                            // miramos si la cuenta es de analítica y analítica tabla
                            // -------------------------------------------------------
                            bool procesar_analitica=false;
                            if (basedatos::instancia()->analitica_tabla() && conanalitica() &&
                                                  (escuentadegasto(cuenta) ||
                                                   escuentadeingreso(cuenta)))
                                procesar_analitica=true;
                            if (basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
                                codigo_en_analitica(cuenta))
                               procesar_analitica=true;
                            if (procesar_analitica)
                               {
                                tablaasiento->realiza_asignainputdiario(fila,
                                  d_h ==tipoDEBE() ? debe : haber );
                               }
                            // -------------------------------------------------------------
                            tablaasiento->chequeatotales();
                        }
                 fila++;
          } // while
      }

   tablaasiento->pasa_externo(ui.externo_lineEdit->text());
   tablaasiento->pasa_concepto_sii(concepto_sii);

   if (!verificar)
     {
       if (tablaasiento->botonincorporaractivado())
          {
             if (!con_aspiradora) QMessageBox::information( this, tr("Asiento predefinido"),
				tr("ASIENTO GENERADO CORRECTAMENTE"));
             tablaasiento->incorporar();
             delete tablaasiento;
             borracontenidos();
             return;
          }
     }
   chequeado=true;
  if (filaexento>-1)
     {
      // borrar contenidos filas escondidas IVA en filaexento
      // y chequear registro de IVA EXENTO
      tablaasiento->borra_valores_iva(filaexento);
      tablaasiento->activa_opexenta();
     }
  tablaasiento->exec();
  delete tablaasiento;
  borracontenidos();
  ui.variablestable->setCurrentCell(0,1);
}

void execasientomodelo::borracontenidos()
{
  ui.variablestable->disconnect(SIGNAL(cellChanged(int,int)));
  for (int veces=0;veces<ui.variablestable->rowCount();veces++)
     {

        if (ui.variablestable->item(veces,3)->text()!=tipoFecha())
           {
             if (!guardar[veces]) ui.variablestable->item(veces,1)->setText(valdefecto[veces]);
           }

     }
  ui.fichdoclineEdit->clear();
  ui.externo_lineEdit->clear();
  etiqueta->setText(QString());
  connect(ui.variablestable,SIGNAL( cellChanged ( int , int )),this,
        SLOT(valorcambiado(int,int )));
}


QString execasientomodelo::asignacodigocuenta( QString expresion )
{
    QString cadena="";
    if (expresion[0]!='{') return expresion;
       else
          {
	   // extraemos la variable y la sustituimos
	   cadena=expresion;
	   cadena.remove('{');
	   cadena.remove('}');
	   return contenidovariable(cadena);
          }
}


QString execasientomodelo::asignaci( QString expresion )
{
    QString cadena="";
    if (expresion[0]!='{') return expresion;
       else
          {
	   // extraemos la variable y la sustituimos
	   cadena=expresion;
	   cadena.remove('{');
	   cadena.remove('}');
	   return contenidovariable(cadena);
          }
}


QString execasientomodelo::contenidovariable( QString qvariable )
{
   QString variable=qvariable;
   bool haydescripcioncuenta=false;

   if (qvariable=="_externo") return ui.externo_lineEdit->text();
   if (qvariable=="_nombre_externo") return basedatos::instancia()->razon_externo(ui.externo_lineEdit->text());

   if (qvariable.contains(':'))
      {
       QString comando=qvariable.section(':',0,0);
       variable=qvariable.section(':',1,1);
       if (comando==comandodescrip()) haydescripcioncuenta=true;
      }

   for (int veces=0;veces<numvariables;veces++)
       {
          if (ui.variablestable->item(veces,2)->text()==variable)
           {
            if (ui.variablestable->item(veces,3)->text()==tipoFecha())
                {
                 QDateEdit *fecha;
                 fecha=(QDateEdit*)ui.variablestable->cellWidget(veces,1);
                 return fecha->date().toString("dd.MM.yyyy");
                }
            if (ui.variablestable->item(veces,1)!=0)
             {
              if (ui.variablestable->item(veces,3)->text()==tipoNIF())
                 {
                  QString codigo_cuenta;
                  // EL CÓDIGO SE CORRESPONDE CON EXTERNO ?
                  if (basedatos::instancia()->existe_externo(ui.variablestable->item(veces,1)->text()))
                     {
                       ui.externo_lineEdit->setText(ui.variablestable->item(veces,1)->text());
                       codigo_cuenta=basedatos::instancia()->cuenta_externo(ui.variablestable->item(veces,1)->text());
                     }
                  if (codigo_cuenta.isEmpty())
                     codigo_cuenta=basedatos::instancia()->selectCuentaCifDatossubcuenta(
                              ui.variablestable->item(veces,1)->text());
                  if (haydescripcioncuenta) return descripcioncuenta(codigo_cuenta);
                  return codigo_cuenta;
                 }
              if (haydescripcioncuenta) return descripcioncuenta(ui.variablestable->item(veces,1)->text());
              return ui.variablestable->item(veces,1)->text();
             }
            else return "";
           }
       }
   return "";
}


QString execasientomodelo::asignaconcepto( QString expresion )
{
    QString cadena="";
    QString definitiva="";
    int pos=0;
    int pos2=0;
    if (!expresion.contains('"') && !expresion.contains('+') && !expresion.contains('}'))
	return expresion;
       else
          {
           cadena=expresion;
           cadena.remove('"');
           cadena.remove("+");
           while (pos<cadena.length())
            {
             /*if (cadena[pos]=='"')
	            {
                 pos2=pos+1;
                 while (cadena[pos2]!='"' && pos2<cadena.length()) pos2++;
                 if (pos2>=cadena.length()) pos2=cadena.length()-1;
                 definitiva+=cadena.mid(pos+1,pos2-pos-1);
                 pos=pos2;
                 if (cadena[pos]=='"' && pos==cadena.length()-1) pos++;
	            }
             if (cadena[pos]=='+') pos++; */

             if (cadena[pos]!='{') definitiva.append(cadena[pos]);
             if (cadena[pos]=='{')
              {
               pos2=pos+1;
               while (cadena[pos2]!='}' && pos2<cadena.length()) pos2++;
               if (pos2>=cadena.length()) pos2=cadena.length()-1;
               definitiva+=contenidovariable(cadena.mid(pos+1,pos2-pos-1));
               pos=pos2; //pos=pos2+1;
               if (cadena[pos]=='}' && pos==cadena.length()-1) pos++;
              }

             pos++;
            }
           }
       return definitiva;
}


QString execasientomodelo::asignavalorexpresion(QString qexpresion , bool dobleprecis)
{
   QString cadpru;
   QString extract;
   cadpru=qexpresion;
   cadpru.remove(' ');

   while(cadpru.contains("SI[") || cadpru.contains("si["))
    {
     int pos_si=0;
     int pos_fin_si=0;
     cadpru.replace("si[","SI[");
     if (cadpru.contains("SI[")) pos_si=cadpru.lastIndexOf("SI[");
     QString recorte=cadpru.mid(pos_si);
     pos_fin_si=recorte.indexOf("]");
     if (pos_fin_si==0) break;
     QString condicional=recorte.left(pos_fin_si+1);
     QString trabajo=condicional;
     // QMessageBox::warning( this, tr("Asiento modelo"),condicional);
     int pos_ini_expresion=0;
     pos_ini_expresion=trabajo.indexOf('[')+1;
     int pos_fin_expresion=0;
     pos_fin_expresion=trabajo.indexOf(">=")-1;
     if (pos_fin_expresion<=0)
       pos_fin_expresion=trabajo.indexOf("<=")-1;
     if (pos_fin_expresion<=0)
       pos_fin_expresion=trabajo.indexOf(">")-1;
     if (pos_fin_expresion<=0)
       pos_fin_expresion=trabajo.indexOf("<")-1;
     if (pos_fin_expresion<=0)
       pos_fin_expresion=trabajo.indexOf("!=")-1;
     if (pos_fin_expresion<=0)
       pos_fin_expresion=trabajo.indexOf("=")-1;
     if (pos_fin_expresion<=0) break;

     QString expresion=trabajo.mid(pos_ini_expresion, pos_fin_expresion-pos_ini_expresion+1);
     // QString cadnum; cadnum.setNum(trabajo.indexOf(">"));
     // QMessageBox::warning( this, tr("Asiento modelo"),condicion+" "+cadnum);
     QString cad_evaluar=asignavalorexpresion(expresion);

     enum tiposimbolo {cero, mayorigual, menorigual, mayor, menor, igual, distinto};
     int simbolo=0;


     simbolo= (simbolo==0 && trabajo.contains(">=")) ? mayorigual:simbolo;
     simbolo= (simbolo==0 && trabajo.indexOf("<=")>0) ? menorigual:simbolo;
     simbolo= (simbolo==0 && trabajo.indexOf(">")>0) ? mayor:simbolo;
     simbolo= (simbolo==0 && trabajo.indexOf("<")>0) ? menor:simbolo;
     simbolo= (simbolo==0 && trabajo.indexOf("!=")>0) ? distinto:simbolo;
     simbolo= (simbolo==0 && trabajo.indexOf("=")>0) ? igual:simbolo;
     //if (simbolo==mayorigual)
     //  QMessageBox::warning( this, tr("Asiento modelo"),"mayorigual");

     QString termino;
     int indice1=pos_fin_expresion+2;
     if (simbolo==mayorigual || simbolo==menorigual || simbolo==distinto) indice1=indice1+1;
     int indice2=trabajo.indexOf('?');
     termino=trabajo.mid(indice1,indice2-indice1);
     //QMessageBox::warning( this, tr("Asiento modelo"),termino);

     QString cad_comparar=asignavalorexpresion(termino);

     // QMessageBox::warning( this, tr("Asiento modelo"),cad_evaluar+ " " + cad_comparar);

     QString condicion_si, condicion_no;
     condicion_si=trabajo.mid(trabajo.indexOf('?')+1,trabajo.indexOf(':')-trabajo.indexOf('?')-1);
     condicion_no=trabajo.mid(trabajo.indexOf(':')+1,trabajo.indexOf(']')-trabajo.indexOf(':')-1);

     // QMessageBox::warning( this, tr("Asiento modelo"),condicion_si + " "+ condicion_no);

     bool verdadero=false;

     switch (simbolo) {
     case mayorigual:
         if (cad_evaluar.toDouble()-cad_comparar.toDouble()>-0.0001) verdadero=true;
         break;
     case menorigual:
         if (cad_evaluar.toDouble()-cad_comparar.toDouble()<0.0001) verdadero=true;
         break;
     case mayor:
         if (cad_evaluar.toDouble()-cad_comparar.toDouble()>0.0001) verdadero=true;
         break;
     case menor:
         if (cad_evaluar.toDouble()-cad_comparar.toDouble()<-0.0001) verdadero=true;
         break;
     case igual:
         if (cad_evaluar.toDouble()-cad_comparar.toDouble()>-0.0001
             && cad_evaluar.toDouble()-cad_comparar.toDouble()<0.0001) verdadero=true;
         break;
     case distinto:
         if (cad_evaluar.toDouble()-cad_comparar.toDouble()<-0.0001
             || cad_evaluar.toDouble()-cad_comparar.toDouble()>0.0001) verdadero=true;
         break;
     }

     if (verdadero)
         cadpru.replace(trabajo,condicion_si);
       else
         cadpru.replace(trabajo,condicion_no);

     //QMessageBox::warning( this, tr("Asiento modelo"),cadpru);

    }

   if (cadpru.contains('[') || cadpru.contains(']')) return "0";

   int final;
   int inicio;
   int indice=0;
  while (indice<cadpru.length())
      {
      if (cadpru[indice]=='{')
           {
              inicio=indice;
              final=inicio;
              while (final<cadpru.length() && cadpru[final]!='}') final++;
              // extraemos la cadena de la cuenta sin corchetes
              extract=cadpru.mid(inicio+1,final-inicio-1);
              cadpru.remove(inicio,final-inicio+1);
              if (contenidovariable(extract).length()!=0)
                 cadpru.insert(inicio,contenidovariable(extract)); // contemplar si cadena vacía
              else cadpru.insert(inicio,"0");
              continue;
           }
        indice++;
      }
 // QMessageBox::warning( this, tr("Asiento modelo"),cadpru);

  if (cadpru=="0") return cadpru;
  exparitmetica expresion;
  cadpru=cadpru.replace(',','.');
  cadpru.replace("--","+");

  QString cadcods="()+-/*0123456789.";

  indice=0;
  while (indice<cadpru.length())
      {
        if (!cadcods.contains(cadpru[indice])) return "0";
        indice++;
      }

  expresion.inicializa(cadpru);
  double valor=0;
  int okeval;
  valor=expresion.resultado(&okeval);
  valor=CutAndRoundNumberToNDecimals (valor, dobleprecis ? 4 : 2);

  if (!okeval) valor=0;
  QString cadval;
  cadval.setNum(valor,'f',dobleprecis ? 4 : 2);

  return cadval;
}


QDate execasientomodelo::asignafechaasiento( QString expresion )
{
    QString cadena="";
    QDate resultado;
    QString cadextract;
    int dia=0,mes=0,anyo=0;
    if (expresion[0]!='{')
         {
          if (cadfechavalida(expresion))
           {
              cadextract=expresion.mid(0,2);
              dia=cadextract.toInt();
              cadextract=expresion.mid(3,2);
              mes=cadextract.toInt();
              cadextract=expresion.mid(6,4);
              anyo=cadextract.toInt();

              if (QDate(anyo,mes,dia).isValid()) return QDate(anyo,mes,dia);
                 else return resultado.currentDate();
           }
           else return resultado.currentDate();
         }
         else
              {
	   // extraemos la variable y la sustituimos
	           cadena=expresion;
	           cadena.remove('{');
	           cadena.remove('}');
	           QString fechacad;
	           fechacad=contenidovariable(cadena);
               //QMessageBox::warning( this, tr("Asiento automático"),fechacad);
                   cadextract=fechacad.mid(0,2);
                   dia=cadextract.toInt();
                   cadextract=fechacad.mid(3,2);
                   mes=cadextract.toInt();
                   cadextract=fechacad.mid(6,4);
                   anyo=cadextract.toInt();
                   if (QDate(anyo,mes,dia).isValid()) return QDate(anyo,mes,dia);
                         else return resultado.currentDate();
              }

}



void execasientomodelo::posicioncambiada( int row, int col )
{
   if (col!=1) ui.variablestable->setCurrentCell(row,1);
   ui.variablestable->setFocus();
   if (ui.variablestable->item(row,3)!=0)
       {
        if (ui.variablestable->item(row,3)->text()==tipoSubcuenta()) ui.subcuentapushButton->setEnabled(true);
         else ui.subcuentapushButton->setEnabled(false);
        if (ui.variablestable->item(row,3)->text()==tipoCI()) ui.cipushButton->setEnabled(true);
         else ui.cipushButton->setEnabled(false);
       }
}


void execasientomodelo::botonsubcuenta()
{
     QString cadena2;
    if (ui.variablestable->item(ui.variablestable->currentRow(),ui.variablestable->currentColumn())!=0)
        cadena2=ui.variablestable->item(ui.variablestable->currentRow(),
                ui.variablestable->currentColumn())->text();
    buscasubcuenta *labusqueda=new buscasubcuenta(cadena2);
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    int fila=ui.variablestable->currentRow();
    int columna=ui.variablestable->currentColumn();
    ui.variablestable->disconnect(SIGNAL(cellChanged(int,int)));
    if (ui.variablestable->item(fila,columna)==0)
       {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(fila,columna,newItem);
       }
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.variablestable->item(fila,
                              columna)->setText(cadena2);
       else ui.variablestable->item(fila,
                                      columna)->setText("");
    delete labusqueda;
    connect(ui.variablestable,SIGNAL( cellChanged ( int , int )),this,
           SLOT(valorcambiado(int,int )));

}


void execasientomodelo::botonci()
{
    QString cadena2;
    if (ui.variablestable->item(ui.variablestable->currentRow(),ui.variablestable->currentColumn())!=0)
        cadena2=ui.variablestable->item(ui.variablestable->currentRow(),
                ui.variablestable->currentColumn())->text();

   introci *c = new introci();
   // c->hazparafiltro();
   if (ciok(cadena2)) c->pasaci(cadena2);
   int cod=c->exec();
   cadena2=c->cadenaci();
   int fila=ui.variablestable->currentRow();
   int columna=ui.variablestable->currentColumn();
    if (ui.variablestable->item(fila,columna)==0)
       {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(fila,columna,newItem);
       }
    if (cod==QDialog::Accepted)
      ui.variablestable->item(fila,
                              columna)->setText(cadena2);
   delete c;

}


QString execasientomodelo::fichdocumento()
{
  return ui.fichdoclineEdit->text();
}

void execasientomodelo::buscacopiadoc()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(rutacargadocs().isEmpty() ? adapta(dirtrabajo()):adapta(rutacargadocs()));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      ui.fichdoclineEdit->setText(dir->ruta_actual());
    }
  delete(dir);
#else

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Todos los archivos (*)");
  filtros << tr("Archivos jpg (*.jpg)");
  filtros << tr("Archivos jpg (*.jpeg)");
  filtros << tr("Archivos png (*.png)");
  filtros << tr("Archivos pdf (*.pdf)");
  dialogofich.setNameFilters(filtros);
  if (rutacargadocs().isEmpty())
     dialogofich.setDirectory(adapta(dirtrabajo()));
    else
      dialogofich.setDirectory(adapta(rutacargadocs()));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE COPIA DE DOCUMENTO"));
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
          ui.fichdoclineEdit->setText(fileNames.at(0));
          }
     }
#endif
}

void execasientomodelo::visdoc()
{
   if (ui.fichdoclineEdit->text().isEmpty()) return;
   if (!ejecuta(aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())),ui.fichdoclineEdit->text()))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+ui.fichdoclineEdit->text()+tr(" con ")+
                             aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())));

}

void execasientomodelo::borraasignaciondoc()
{
    ui.fichdoclineEdit->clear();
}


void execasientomodelo::aspiradora()
{
    QString cadfich;

    con_aspiradora=true;


#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      cadfich=dir->ruta_actual();
    }
  delete(dir);
#else

    QFileDialog dialogofich(this);
    dialogofich.setFileMode(QFileDialog::ExistingFile);
    dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
    dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
    dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
    dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
    dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

    QStringList filtros;
    filtros << tr("Archivos de texto tabulado (*.*)");
    dialogofich.setNameFilters(filtros);
    dialogofich.setDirectory(adapta(dirtrabajobd()));
    dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA LEER"));
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
             cadfich=fileNames.at(0);
            } else return;
       } else return;
#endif


   if (cadfich.length()==0) return;

   if (QMessageBox::question(this,
                   tr("PROCESAR ARCHIVO PARA ASIENTOS"),
                   tr("Se va a procesar el archivo '%1' \n"
                      "¿ es correcta la operación ?")
                     .arg( cadfich ))==QMessageBox::No) return;

   QFile fichero(adapta(cadfich));
   if (!fichero.exists()) return;

   int numlineas=0;
   // contamos número de líneas del fichero
    if ( fichero.open( QIODevice::ReadOnly ) ) {
         QTextStream stream( &fichero );
         stream.setEncoding(QStringConverter::Utf8);
         while ( !stream.atEnd() ) {
             QString linea;
             linea = stream.readLine(); // linea de texto excluyendo '\n'
             numlineas++;
             }
         fichero.close();
       } else return;

       QProgressDialog progress("Importando variables","Cancelar",0,numlineas,this);
       progress.setLabelText("Importando valores de archivo");
       progress.setWindowModality(Qt::WindowModal);
       progress.setMinimumDuration(0);
       // progress.setMinimum(0);
       progress.setMaximum(numlineas);
       progress.setFocus();
       progress.show();
       // progress.setValue(10);
       // processEvents(ExcludeUserInputEvents);
       // progress.setValue(20);

       // volvemos a abrir para leer variables
       int lineaactu=0;
       if ( fichero.open( QIODevice::ReadOnly ) ) {
            QTextStream stream( &fichero );
            stream.setEncoding(QStringConverter::Utf8);
            QString linea;
            QApplication::processEvents();
            while ( !stream.atEnd() ) {
               borracontenidos();
               lineaactu++;
               progress.setValue(lineaactu);
               QApplication::processEvents();
               linea = stream.readLine(); // linea de texto excluyendo '\n'
               if (linea.isEmpty()) break;
               QStringList lista_var = linea.split("\t");
               // ui.variablestable->item(fila,3)->text() == tipoFecha()
               // tipoNumerico()
               // la columna 2 tiene el contenido
               int tope=lista_var.count();
               if (tope>numvariables) tope=numvariables;
               for (int veces=0; veces<tope; veces++)
                  {
                    if (ui.variablestable->item(veces,3)->text() == tipoFecha())
                      {
                        QDateEdit *fecha;
                        fecha=(QDateEdit*)ui.variablestable->cellWidget(veces,1);
                        QDate fechavar;
                        bool corta=false;
                        if (lista_var.at(veces).length()==8) corta=true;
                        if (lista_var.at(veces).contains("-"))
                           fechavar=fechavar.fromString(lista_var.at(veces),corta ?  "dd-MM-yy" : "dd-MM-yyyy");
                          else
                              fechavar=fechavar.fromString(lista_var.at(veces), corta ? "dd/MM/yy" : "dd/MM/yyyy");
                        QDate anyo2000(2000,01,01);
                        if (fechavar<anyo2000) fechavar=fechavar.addYears(100);
                        fecha->setDate(fechavar);
                        continue;
                      }
                    if (ui.variablestable->item(veces,3)->text() == tipoNumerico())
                     {
                       if (comadecimal)
                           ui.variablestable->item(veces,1)->setText(convacoma(lista_var.at(veces)));
                       continue;
                     }
                    ui.variablestable->item(veces,1)->setText(lista_var.at(veces));
                  }
               // ejecutamos asiento automático
               // cuidamos de que no se interrumpa el proceso con mensaje
               botongenerar();
               if (chequeado)
                 {
                   if (QMessageBox::question(
                     this,
                     tr("Asientos predefinidos"),
                     tr("¿ Desea salir del proceso ?")) == QMessageBox::Yes ) return;
                    else chequeado=false;
                 }
             }
            progress.close();
            QMessageBox::information( this, tr("Asiento predefinido"),
                                            tr("SE HA TERMINADO DE PROCESAR EL ARCHIVO"));
        }
}


void execasientomodelo::buscar_externo()
{
   busca_externo *b = new busca_externo();
   int rdo=b->exec();
   if (rdo==QDialog::Accepted)
     {
       ui.externo_lineEdit->setText(b->codigo_elec());
     }
     else ui.externo_lineEdit->clear();
   delete(b);
}


void execasientomodelo::borra_externo()
{
  ui.externo_lineEdit->clear();
}

void execasientomodelo::ver_externo()
{
    externos *e = new externos();
    e->pasa_codigo(ui.externo_lineEdit->text());
    e->exec();
    delete(e);
}


void execasientomodelo::exec_aspiradora() {
    aspiradora();
}


void execasientomodelo::fichero_soltado() {
    ui.fichdoclineEdit->setText(etiqueta->nombre_fich());
}
