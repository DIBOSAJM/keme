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

#include "funciones.h"
#include "tabla_asientos.h"
#include "basedatos.h"
#include "procesavencimiento.h"
#include <QMessageBox>

void cargatabla_ci(tabla_asientos *t,int fila, QString clave_ci, double total, bool modoconsulta)
{
  // consultamos todos los registros de la tabla
QSqlQuery query = basedatos::instancia()->carga_de_tabla_ci(clave_ci);
t->crea_tabla_ci(fila);
if (total<0) total=total*-1;
if ( query.isActive() ) {
    while ( query.next() ) {
     t->pasainputacion(fila, query.value(0).toString(), total,
                       query.value(1).toDouble()/total);
    }
  t->actualiza_inputacion(fila, total, modoconsulta);
  t->marca_ci(fila);
 }
}


bool edasiento(QString elasiento, QString usuario, bool modoconsulta, QString ejercicio)
{
      // QDate fechapase=fechadeasiento(elasiento);
      // fechapase=punterodiario->fechapaseactual();
      // QString ejercicio=ejerciciodelafecha(fechapase);
      QString eldiario=diariodeasiento(elasiento,ejercicio);
      if (eldiario==diario_apertura())
        {
          QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                                QObject::tr("ERROR: El asiento de apertura no es editable"));
          return false;

        }
      if ((ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio)) && !modoconsulta)
       {
         QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                               QObject::tr("ERROR: El pase seleccionado corresponde a un ejercicio cerrado"));
         return false;
       }

      if (basedatos::instancia()->asiento_en_periodo_bloqueado(elasiento,ejercicio) && !modoconsulta)
       {
         QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                               QObject::tr("ERROR: El asiento seleccionado corresponde a un periodo bloqueado"));
         return false;
       }

      if (basedatos::instancia()->ejercicio_bloqueado(ejercicio) && !modoconsulta)
       {
         QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                               QObject::tr("ERROR: El asiento seleccionado corresponde a un ejercicio bloqueado"));
         return false;
       }


      if (basedatos::instancia()->asiento_con_cuentas_bloqueadas(elasiento,ejercicio) && !modoconsulta)
       {
         QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                               QObject::tr("ERROR: El asiento seleccionado posee cuentas bloqueadas"));
         return false;
       }


  //comprobamos si el asiento es vencimiento procesado
  QString pasevto;
  if (basedatos::instancia()->asiento_es_vencimiento_procesado(elasiento, ejercicio, &pasevto)
      && !modoconsulta)
     {
      if (QMessageBox::question(
            0, QObject::tr("Edición de asiento"),
            QObject::tr("El asiento a editar se corresponde con un vencimiento,\n"
                        "procesado; se va a proceder a su borrado y a la asignación\n"
                        "de un nuevo pago/cobro\n"
                        "¿ Desea continuar ?")) == QMessageBox::No )
             return false;
      // hay que averiguar el número de registro del vencimiento
      // antes de borrar el asiento
      QString numvto=basedatos::instancia()->numvto_paseprocesado(pasevto);
      borrarasientofunc(elasiento,ejercicio,false);
      if (!existeasiento(elasiento,ejercicio) && !numvto.isEmpty())
         {
           procesavencimiento *procvenci=new procesavencimiento(usuario);
           procvenci->cargadatos(numvto);
           procvenci->asignaasiento(elasiento);
           int resultado=procvenci->exec();
           delete procvenci;
           if (resultado!=QDialog::Accepted)
              {
                QMessageBox::information( 0, QObject::tr("Vencimientos"),
                                          QObject::tr("El procesado del vencimiento se ha cancelado"));
                return false;
              }

         }

      return true;
     }
  //comprobamos si los pases del asiento están asignados a vencimientos
  if (basedatos::instancia()->asiento_con_pases_vencimientos_realizados(elasiento,ejercicio) && !modoconsulta)
     {
      if (QMessageBox::question(
            0, QObject::tr("Edición de asiento"),
            QObject::tr("El asiento a editar posee apuntes en el registro de vencimientos,\n"
                        "si lo edita se crearán nuevos apuntes y quedarán vencimientos\n"
                        "procesados no asignados a este asiento,\n"
                        "¿ Desea continuar ?")) == QMessageBox::No )
             return false;
     }

  //comprobamos si los pases del asiento están asignados a facturas
  if (basedatos::instancia()->asiento_con_pases_factura(elasiento,ejercicio) && !modoconsulta)
     {
      QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                            QObject::tr("El asiento a editar corresponde a una factura emitida,\n"
                                        "para cambiarlo se debe de borrar y volver a contabilizar\n"
                                        "la factura\n"
                                        ));
      return false;
     }
  // ------------------------------------------------------------------------------------

  //comprobamos enlace externo del asiento
  if (basedatos::instancia()->asiento_con_enlace(elasiento,ejercicio) && !modoconsulta)
     {
      QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                            QObject::tr("El asiento a editar está enlazado con una aplicación externa,\n"
                                        "la edición y borrado no se puede realizar desde KEME "
                                        ));
      return false;
     }
  // ------------------------------------------------------------------------------------
       tabla_asientos *t = new tabla_asientos(haycomadecimal(),haydecimales(),usuario);

       t->pasafichdoc(basedatos::instancia()->copia_docdiario (elasiento,ejercicio));

       t->activaparaedicion(); // Hacer posible verificar aquí el cambio de ejercicio


       bool esautofactura=false;
       bool autofactura_no_ue=false;
       bool isp_op_interiores=false;
       bool pr_servicios_ue=false;
       bool esaib=false;
       bool eseib=false;
       // bool opexenta=false;
       // bool opnosujeta=false;
       if (basedatos::instancia()->asientoenautofactura(elasiento,ejercicio))
          { t->preparaautofactura();
            esautofactura=true; // solo operaciones intracomunitarias
          }
       if (basedatos::instancia()->asiento_isp_op_interiores(elasiento,ejercicio))
          {
            t->prepara_isp_op_interiores();
            isp_op_interiores=true;
          }

       if (basedatos::instancia()->asientoenautofactura_no_ue (elasiento,ejercicio))
         {
           // -----------------------------
           t->preparaautofactura_no_ue();
           autofactura_no_ue=true;
         }
       if (basedatos::instancia()->asientoenpr_servicios_ue(elasiento,ejercicio))
         {
           // -----------------------------
           t->preparapr_servicios_ue();
           pr_servicios_ue=true;
         }
       if (asientoenaib(elasiento,ejercicio)) { t->preparaaib(); esaib=true; }
       if (asientoeneib(elasiento,ejercicio)) { t->preparaeib(); eseib=true; }
       if (basedatos::instancia()->asiento_exento_noaib_noeib(elasiento,ejercicio)
           && !esautofactura && !autofactura_no_ue && !pr_servicios_ue) // ----------------
         {
          t->activa_opexenta();
          //opexenta=true;
         }
       if (basedatos::instancia()->asientoenopnosujeta (elasiento,ejercicio))
         {
          //opnosujeta=true;
          t->activa_opexenta();
          t->selec_nosujeta();
         }

       int fila=0;
       QString cadasiento;
       QString cadpase;
       QDate fechaasiento;
       QString caddebe;
       QString cadhaber;
       QString qdiario;
       bool pasado=false;
       QSqlQuery query2;

       QSqlQuery query = basedatos::instancia()->select10Diariodiarioasientoorderpase (
                        elasiento, ejercicio );
            if ( query.isActive() ) {
                while ( query.next() ) {
                    if (!pasado)
                       {
                        qdiario=query.value(9).toString();
                        if (qdiario.length()==0) qdiario=diario_no_asignado();
                        t->pasadiario(qdiario);
                        pasado=true;
                       }
                    t->pasa_externo(query.value(12).toString());
                    t->pasa_concepto_sii(query.value(13).toString());
                    if ((esaib || esautofactura || autofactura_no_ue || isp_op_interiores)
                        && escuentadeivasoportado(query.value(0).toString()))
                       {
                         QSqlQuery p = basedatos::instancia()->selectProrratalibroivapase(
                                  query.value(7).toString() );
                         if ( (p.isActive()) && (p.next()) )
                            {
                             // pasamos la prorrata
                             double valor=p.value(0).toDouble();
                             QString cadnum; cadnum.setNum(valor*100,'f',2);
                             if (valor>0.99 and valor<1.01)
                                t->pasacadenaprorrata("");
                             else
                                t->pasacadenaprorrata(cadnum);
                            }

                       }
                    cadpase=query.value(7).toString();
                    if (esaib || esautofactura || autofactura_no_ue || isp_op_interiores)
                      {
                       // intentamos cargar los datos del libro del IVA
                       // de forma especial para el cuadro de diálogo AIB/Autofactura
                       query2 = basedatos::instancia()->select14Libroivapase(cadpase);
                       if (query2.isActive())
                          if (query2.next())
                           {
                            QString cuenta;
                            if (escuentadeivasoportado(query.value(0).toString()) ||
                                escuentadeivarepercutido(query.value(0).toString()))
                                cuenta=query.value(0).toString();
                            if (query2.value(10).toBool()) // es de IVA soportado
                               {
                                t->pasainfoaib1(query2.value(0).toString(),
                                           query2.value(1).toString(),
                                           query2.value(2).toString(),
                                           cuenta, query2.value(6).toString(),
                                           query2.value(14).toDate(),
                                           query2.value(15).toDate(),
                                           query2.value(17).toString(),
                                           query2.value(22).toBool(),
                                                query2.value(34).toBool());
                              }
                             else t->pasainfoaib2(cuenta); // ya se pasó la información principal con
                                                           // el anterior pase
                            }
                      }
                    if ((esaib || esautofactura || autofactura_no_ue || isp_op_interiores) &&
                        (escuentadeivasoportado(query.value(0).toString())
			|| escuentadeivarepercutido(query.value(0).toString()))) continue;
		    if (query.value(2).toDouble()>-0.0001 && query.value(2).toDouble()<0.0001)
		        caddebe=""; else caddebe=query.value(2).toString();
		    if (query.value(3).toDouble()>-0.0001 && query.value(3).toDouble()<0.0001)
		        cadhaber=""; else cadhaber=query.value(3).toString();
		    t->pasadatos1(fila,query.value(0).toString() ,
                                                          query.value(1).toString(),
                                                          caddebe,
                                                          cadhaber,
                                                          query.value(4).toString(),
                                                          query.value(8).toString());
                    // aquí podríamos procesar la incorporación
                    // de la tabla diario_ci
                    if (basedatos::instancia()->analitica_tabla() &&
                        t->cuenta_para_analitica(query.value(0).toString()))
                          cargatabla_ci(t,fila, query.value(10).toString(),
                          caddebe.isEmpty() ? cadhaber.toDouble(): caddebe.toDouble(),
                          modoconsulta);
		    cadasiento=query.value(5).toString();
		    fechaasiento=query.value(6).toDate();
		    t->pasanumasiento(cadasiento);
		    t->pasafechaasiento(fechaasiento);
                    t->pasa_num_recepcion(query.value(11).toString());
                    if (eseib || pr_servicios_ue)
                       {
                        // intentamos cargar los datos del libro del IVA
                        // de forma especial para el cuadro de diálogo EIB
                        query2 = basedatos::instancia()->select14Libroivapase(cadpase);
                        if (query2.isActive())
                           if (query2.next())
                            {
                             // controlar aquí
                             t->pasainfoeib(query2.value(0).toString(),
                                            query2.value(1).toString(),
                                            query2.value(6).toString(),
                                            query2.value(14).toDate(),
                                            query2.value(15).toDate(),
                                            query2.value(16).toString());
                            }
                         fila++;
                         continue;
                       } // cambiamos condición anterior
                         // (eseib || opexenta)
                    // Investigamos si hay retención
                    query2 = basedatos::instancia()->datos_ret_pase (cadpase);
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
                            QString retencion=query2.value(6).toString();
                            QString ing_cta=query2.value(7).toString();
                            QString ing_cta_repercutido=query2.value(8).toString();
                            QString nombre_ret=query2.value(9).toString();
                            QString cif_ret= query2.value(10).toString();
                            QString provincia=query2.value(11).toString();
                            t->pasa_datos_ref(fila,
                                              cta_retenido, ret_arrendamiento,
                                              clave, base_percepciones,
                                              tipo_ret, retencion,
                                              ing_cta, ing_cta_repercutido,
                                              nombre_ret, cif_ret,
                                              provincia);

                         }
                    // ---------------------------------------------------------
		    query2 = basedatos::instancia()->select14Libroivapase(cadpase);
            if (query2.isActive() && !esaib && !esautofactura)
              if (query2.next()) {
			    QString cadsoportado="0";
			    if (query2.value(10).toBool()==true )
				cadsoportado="1";
                            QString cadrectif,cadautofact,cadagrario;
			    if (query2.value(12).toBool()==true )
				cadrectif="1";
			    if (query2.value(13).toBool()==true )
				cadautofact="1";
                if (query2.value(24).toBool()==true )
                    cadagrario="1";
                if (escuentadeivasoportado(query.value(0).toString()) ||
                   escuentadeivarepercutido(query.value(0).toString()))
                   t->pasadatos2(fila,query2.value(0).toString(),
                             query2.value(1).toString(),// base iva
                             query2.value(2).toString(),
						     query2.value(3).toString(),
						     query2.value(4).toString(),
						     query2.value(5).toString(),
						     query2.value(6).toString(),
						     query2.value(7).toString(),
						     query2.value(8).toString(),
						     query2.value(9).toString(),
						     cadsoportado,
                             query2.value(11).toString(),
                             cadrectif,
                             cadautofact,
                             query2.value(15).toDate().toString("yyyy-MM-dd"),
                             query2.value(16).toString(),
                             query2.value(17).toString(),
                             query2.value(18).toString(),
                             query2.value(19).toString(),
                             query2.value(20).toString(),
                             query2.value(21).toString(),
                             query2.value(22).toBool() ? "1" : "",
                             query2.value(23).toString(),
                             cadagrario,
                             query2.value(25).toString(),
                             query2.value(26).toString(),
                             query2.value(29).toBool() ? "1" : "",
                             query2.value(30).toBool() ? "1" : "",
                             query2.value(27).toBool() ? "" : "1",
                             query2.value(28).toBool() ? "1" : "",
                             query2.value(31).toBool() ? "1" : "",
                             query2.value(32).toBool(),
                             query2.value(35).toBool(),
                             query2.value(36).toBool(),
                             query2.value(37).toString()
                                                        );
                            else
                               {
                                // pasamos parámetros de operación exenta
                                // QString qfrectificativaexento,
                                //    QString qfrectificadaexento, QString qnfacturasexento,
                                //     QString qinicialexento,
                                //    QString qfinalexento
                                t->pasainfoexento(query2.value(10).toBool(),query2.value(0).toString(),
                                                  query2.value(1).toString(),query2.value(6).toString(),
                                                  query2.value(14).toDate(),
                                                  query2.value(15).toDate(),
                                                  query2.value(16).toString(),
                                                  query2.value(12).toBool() ? "1" : "",
                                                  query2.value(18).toString(),
                                                  query2.value(19).toString(),
                                                  query2.value(20).toString(),
                                                  query2.value(21).toString(),
                                                  query2.value(22).toBool(),
                                                  query2.value(25).toString(),
                                                  query2.value(26).toString(), // cif
                                                  query2.value(29).toBool(),   // importación
                                                  query2.value(30).toBool(), // exportación
                                                  ! query2.value(27).toBool(), // ex.no dcho.deduc
                                                  query2.value(33).toBool(),
                                                  query2.value(28).toBool(), // isp emitidas interiores
                                                  query2.value(41).toBool(), // donación
                                                  query2.value(42).toString(), // clave_donación
                                                  query2.value(43).toBool(), //donación especie
                                                  query2.value(44).toBool(), // donación recurrente
                                                  query2.value(45).toString(), // comunidad autónomoa
                                                  query2.value(46).toDouble(), // porcentaje deducción autonomía
                                                  query2.value(37).toString() // DUA
                                                  );
                               }
			}
			
		    fila++;
		}
	    }
        t->chequeatotales();
        if (modoconsulta) t->activa_modoconsulta();
        int resultado=t->exec();
        delete(t);
        bool aceptado=resultado==QDialog::Accepted;

        return aceptado;
}


bool editarasiento(QString elasiento, QString usuario, QString ejercicio)
{
    return edasiento(elasiento, usuario, false, ejercicio);
}

void consultarasiento(QString elasiento, QString usuario, QString ejercicio)
{
    edasiento(elasiento, usuario, true, ejercicio);
}


void contabilizar_factura(QString serie, QString numero, QString usuario, bool nomsj)
{
    // Tipo de operación:
    /*
    01 - Operación convencional
    02 - Entregas interiores de bienes o servicios exentos
    03 - Entregas interiores de bienes o servicios no sujetos
    04 - Entrega intracomunitaria de bienes
    05 - Entrega intracomunitaria de servicios
    06 - Exportación de bienes
    07 - Operación exenta que no da derecho a deducción
    08 - Donación
    09 - PRESTACIÓN SERVICIOS EXTRANJERO ISP
    */
    // generamos tantos apuntes como cuentas de gasto + tipoiva

    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie, numero);
    QString tipo_doc, cuenta, tipo_ret, retencion;
    QString cta_anticipos, externo;
    QString cuenta_ret; double tipobd;
    bool contabilizable=false, contabilizado=false;
    basedatos::instancia()->carga_irpf(&cuenta_ret,&tipobd);
    bool con_ret, con_re;
    QDate fecha, fecha_fac, fecha_op;
    QString concepto_sii;
    int clave=0;
    if (q.isActive())
    {
       if (q.next())
        {
         // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
         // "contabilizado,contabilizable, con_ret, con_re, tipo_ret, retencion, "
         // "tipo_doc, notas, pie1, pie2, pase_diario_cta, clave "
        cuenta=q.value(2).toString();
        fecha=q.value(3).toDate();
        fecha_fac=q.value(4).toDate();
        fecha_op=q.value(5).toDate();
        contabilizado=q.value(6).toBool();
        contabilizable=q.value(7).toBool();
        con_ret=q.value(8).toBool();
        con_re=q.value(9).toBool();
        tipo_ret=q.value(10).toString();
        retencion=q.value(11).toString();
        tipo_doc=q.value(12).toString();
        clave=q.value(17).toInt();
        cta_anticipos=q.value(19).toString();
        externo=q.value(20).toString();
        concepto_sii=q.value(21).toString();
        } else return;
    }

    if (contabilizado)
    {
      QMessageBox::warning( 0, QObject::tr("CONTABILIZAR FACTURAS"),
                            QObject::tr("ERROR: El documento seleccionado ya ha sido contabilizado"));
      return;
    }

    contabilizable=basedatos::instancia()->tipo_doc_contabilizable(tipo_doc);


    if (!contabilizable)
    {
      QMessageBox::warning( 0, QObject::tr("CONTABILIZAR FACTURAS"),
                            QObject::tr("ERROR: El documento seleccionado no es contabilizable"));
      return;
    }

    int tipo_operacion=basedatos::instancia()->tipo_operacion_doc(tipo_doc);
    bool rectificativa=basedatos::instancia()->tipo_doc_rectificativo(tipo_doc);

    QString ejercicio=ejerciciodelafecha(fecha);
    if ((ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio)))
     {
       QMessageBox::warning( 0, QObject::tr("CONTABILIZAR FACTURAS"),
                             QObject::tr("ERROR: La fecha de operación corresponde a ejercicio cerrado"));
       return;
     }

    if (basedatos::instancia()->fecha_periodo_bloqueado(fecha))
     {
       QMessageBox::warning( 0, QObject::tr("CONTABILIZAR FACTURAS"),
                             QObject::tr("ERROR: La fecha de operación corresponde a período bloqueado"));
       return;
     }


    //QString cuenta_tesoreria;
    q = basedatos::instancia()->select_lin_doc(clave);
    if (q.isActive())
       while (q.next())
        {
         QString ref=q.value(1).toString();
         // buscamos cuenta de gasto
         QString cuenta_ingreso=basedatos::instancia()->cuenta_ref(ref);
         //cuenta_tesoreria=basedatos::instancia()->cuenta_ref(ref);
         // comprobamos que cuenta_ingreso no esté bloqueada
         if (basedatos::instancia()->existe_bloqueo_cta(cuenta_ingreso,fecha_op))
           {
             QMessageBox::warning( 0, QObject::tr("CONTABILIZAR FACTURAS"),
                                 QObject::tr("ERROR: La cuenta de ingreso está bloqueada"));
             return;
          }
        }


    // construimos listas a partir de clave_iva con bases imponibles, tipo, tipore y cuotas,
    // cuotasre

    // AQUÍ CONTROL DE NÚMERO DE FACTURAS PROVEEDOR SI TIPO PROVEEDOR ACTIVADO

    if (tipo_operacion==1) // Operación convencional con IVA
    {
    q = basedatos::instancia()->select_lin_doc(clave);
    tabla_asientos *tablaasiento=new tabla_asientos(haycomadecimal(),haydecimales(),usuario);
    tablaasiento->pasafechaasiento(fecha);
    if (nomsj) tablaasiento->evita_pregunta_venci();
    double totalfactura=0;
    int fila=0;
    QString descripcion="Nuestra factura: ";
    descripcion+=serie + numero;
    descripcion+=" - ";
    if (!externo.isEmpty())
       descripcion+=basedatos::instancia()->razon_externo(externo);
     else
        descripcion+= descripcioncuenta(cuenta);

    QStringList cuentas_ingreso;
    bool con_anticipo=false;
    bool con_anticipo_pos=false;
    QList<double> importes;
    QList<bool> anticipos;
    QString cuenta_ingreso_exenta;

    if (q.isActive())
       while (q.next())
        {
         QString ref=q.value(1).toString();
         // buscamos cuenta de ingreso
         QString cuenta_ingreso=basedatos::instancia()->cuenta_ref(ref);
         if (q.value(5).toString()=="EXENTO") cuenta_ingreso_exenta=cuenta_ingreso;
         //descripcion=q.value(2).toString();
         double importe=q.value(3).toDouble()*q.value(4).toDouble()*(1-q.value(8).toDouble()/100);
         if (importe<0.001 && importe>-0.001) continue;
         int pos=cuentas_ingreso.indexOf(cuenta_ingreso);
         if (pos>-1)
            {
             importes[pos] = importes.at(pos) + importe;
            }
           else
               {
                 cuentas_ingreso << cuenta_ingreso;
                 importes << importe;
                con_anticipo=basedatos::instancia()->ref_con_anticipo(ref);
                con_anticipo_pos=con_anticipo && importe>0;
                 anticipos << con_anticipo;
               }
         totalfactura+=importe;
        }

    //if (con_anticipo_pos) tablaasiento->desactiva_avisar_cta_libro_fact_no_diario();


    if ((conanalitica() || conanalitica_parc()) && !basedatos::instancia()->analitica_tabla())
      {
        q.first();
        do
          {
            // int fila, QString col0, QString col1, QString col2,
            // QString col3, QString col4, QString col16
            //
            // clave, codigo, descripcion, cantidad, precio, "
            // "clave_iva, tipo_iva, tipo_re, dto, notas, ci
            QString cadvalor;
            cadvalor.setNum(redond(q.value(3).toDouble()*q.value(4).toDouble()*(1-q.value(8).toDouble()/100),2),'f',2);
            tablaasiento->pasadatos1(fila,basedatos::instancia()->ref_con_anticipo(q.value(1).toString()) ?
                                         cta_anticipos : basedatos::instancia()->cuenta_ref(q.value(1).toString()),
              q.value(2).toString(),"",cadvalor,serie+numero,q.value(10).toString());
            //if (con_anticipo_pos) tablaasiento->pasalineafactura(fila);
            fila++;
          } while (q.next());

      }
     else
       // ESTE BUCLE EN CASO DE NO ANALÍTICA NORMAL
       for (int veces=0;veces<cuentas_ingreso.count();veces++)
         {
         // -------------------
         QString cuenta_ingreso=cuentas_ingreso.at(veces);
         double importe=redond(importes.at(veces),2);
         bool es_anticipo=anticipos.at(veces);
         QString cadvalor; cadvalor.setNum(importe,'f',2);
         tablaasiento->pasadatos1(fila,es_anticipo ? cta_anticipos : cuenta_ingreso,descripcion,"",cadvalor,serie+numero,"");
         // miramos si la cuenta es de analítica tabla
         // -------------------------------------------------------
         bool procesar_analitica=false;
         if (basedatos::instancia()->analitica_tabla() && conanalitica() &&
                               (escuentadegasto(cuenta_ingreso) ||
                                escuentadeingreso(cuenta_ingreso)))
             procesar_analitica=true;
         if (basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
             codigo_en_analitica(cuenta_ingreso))
            procesar_analitica=true;
         if (procesar_analitica)
            {
             tablaasiento->realiza_asignainputdiario(fila,
               cadvalor );
            }
         //if (con_anticipo_pos) tablaasiento->pasalineafactura(fila);
         fila++;
        }

      // procesamos líneas IVA
    q = basedatos::instancia()->select_iva_lin_doc (clave);
    double base_exenta=0;
    if (q.isActive())
        while (q.next())
         {
         // "sum(cantidad*precio*(1-dto/100)), "
         // "clave_iva, max(tipo_iva), max(tipo_re) "
         double importe=q.value(0).toDouble();
         QString cadbase; cadbase.setNum(importe,'f',2);
         QString claveiva=q.value(1).toString();
         QString cuentaiva=basedatos::instancia()->cuenta_tipo_iva(claveiva);
         QString tipoiva=q.value(2).toString();
         QString tipore=q.value(3).toString();
         if (claveiva=="EXENTO") {base_exenta+=importe; claveiva="";}
         if (cuentaiva.isEmpty()) continue;
         double cuota=redond(importe*(tipoiva.toDouble()+tipore.toDouble())/100,2);
         totalfactura+=cuota;
         QString cadcuota; cadcuota.setNum(cuota,'f',2);
         tablaasiento->pasadatos1(fila,cuentaiva,descripcion,"",cadcuota,serie+numero,"");

         // pasamos ahora resto de detalle del IVA
         // fecha_op - desglosar
         QString dia, mes, anyo;
         dia.setNum(fecha_fac.day());
         mes.setNum(fecha_fac.month());
         anyo.setNum(fecha_fac.year());

         tablaasiento->pasadatos2(fila,/*cuenta_ingreso*/ "",cadbase,claveiva,tipoiva,tipore,
                                 cadcuota, cuenta, dia, mes, anyo, "0", "1",
                                 rectificativa ? "1" : "0",
                                 "0",
                                 fecha_op.toString("yyyy-MM-dd"), "", "", "", "","", "",
                                 "0","1","","","","","","","","");
         fila++;
         }

      // nos queda el último apunte: total factura
      if (con_ret) totalfactura=totalfactura-retencion.toDouble();
      QString cadvalor;

      cadvalor.setNum(redond(totalfactura,2),'f',2);

      QString cad_base_exenta; cad_base_exenta.setNum(base_exenta,'f',2);
      // qDebug() << cad_base_exenta;
      // qDebug() << cuenta_ingreso_exenta;

      tablaasiento->pasadatos1(fila,cuenta,descripcion,cadvalor,"",serie+numero,"");
      if (!(base_exenta>-0.001 && base_exenta<0.001))  {
          QString dia, mes, anyo;
          dia.setNum(fecha_fac.day());
          mes.setNum(fecha_fac.month());
          anyo.setNum(fecha_fac.year());

          tablaasiento->pasadatos2(fila,cuenta_ingreso_exenta,cad_base_exenta,/*tipo_exento()*/ "",
                                   "", //tipo iva
                                   "", // tipo re
                                   "0", // cuota iva
                                   cuenta, // cuenta fra
                                   dia, mes, anyo,
                                   "0", // soportado
                                   "1", // prorrata
                                   rectificativa ? "1" : "0", // rectificativa
                                   "0",  // autofactura (nunca para emitidas)
                                   fecha_op.toString("yyyy-MM-dd"), // fecha operación
                                   "", // clave operación
                                   "", // bi a costo
                                   "", // rectificada
                                   "", // nfacturas
                                   "", // finicial
                                   "", // ffinal
                                   "0", // bien inversion
                                   "1",
                                   "","","","",
                                   tipo_operacion==6 ? "1" : "",
                                   tipo_operacion==7 ? "1" : "",
                                   "","");
      }

      // if (tipo_operacion==3) tablaasiento->activa_fila_nosujeta(fila);// no sujetas



      // marcamos esta línea en tabla de asientos para recuperarla y asignarla a cabecera facturas
      tablaasiento->pasalineafactura(fila);
      if (con_ret)
         {
          fila++;
          tablaasiento->pasadatos1(fila,cuenta_ret,descripcion,retencion,"",serie+numero,"");
         }
      tablaasiento->pasa_externo(externo);
      tablaasiento->pasa_concepto_sii(concepto_sii);
      tablaasiento->chequeatotales();
      if (tablaasiento->botonincorporaractivado())
         {
            tablaasiento->incorporar();
            qlonglong pase_fra=tablaasiento->qpasefactura();
            if (!nomsj && pase_fra>0)
              QMessageBox::information( NULL, QObject::tr("CONTABILIZAR DOCUMENTO"),
                               QObject::tr("ASIENTO GENERADO CORRECTAMENTE"));
            QString cadpase; cadpase.setNum(pase_fra);
            if (pase_fra>0)
             basedatos::instancia()->update_contabilizado_doc(serie, numero, cadpase);
         }
         else
         {
             int resultado = tablaasiento->exec();
             qlonglong pase_fra=tablaasiento->qpasefactura();
             QString cadpase; cadpase.setNum(pase_fra);
             if (resultado == QDialog::Accepted && pase_fra!=0)
                basedatos::instancia()->update_contabilizado_doc(serie, numero, cadpase);
         }

      delete tablaasiento;
      return;
      // -------------------------------------------------------------------------------------
      // Final tipo operacion 1
      // -------------------------------------------------------------------------------------
     }
     else
         {
          // operaciones exentas y no sujetas
          // tipo_operacion
          // 02 - Entregas interiores de bienes o servicios exentos
          // 03 - Entregas interiores de bienes o servicios no sujetos
          // 04 - Entrega intracomunitaria de bienes
          // 05 - Entrega intracomunitaria de servicios
          // 06 - Exportación de bienes o servicios
          // 07 - Operación exenta que no da derecho a deducción
          // 08 - Donación
          // 09 - EXP SERVICIOS ISP
         q = basedatos::instancia()->select_lin_doc(clave);
         tabla_asientos *tablaasiento=new tabla_asientos(haycomadecimal(),haydecimales(),usuario);
         tablaasiento->pasafechaasiento(fecha);
         tablaasiento->pasa_externo(externo);
         tablaasiento->pasa_concepto_sii(concepto_sii);
         if (nomsj) tablaasiento->evita_pregunta_venci();
         if (tipo_operacion==4) tablaasiento->activa_eib_exterior();
         if (tipo_operacion==6 || tipo_operacion==9) tablaasiento->activa_exportacionexento();
         if (tipo_operacion==7) tablaasiento->activa_nodeduccionexento();

         double totalfactura=0;
         int fila=0;
         QString descripcion="Nuestra factura: "+serie+numero+" - ";
         if (!externo.isEmpty())
            descripcion+=basedatos::instancia()->razon_externo(externo);
          else
             descripcion+= descripcioncuenta(cuenta);
         //descripcion+=" - ";
         //descripcion+=serie + numero;
         QString cuenta_ingreso,cadvalor;
         QStringList cuentas_ingreso;
         QList<double> importes;
         QList<bool> anticipos;
         bool con_anticipo=false;
         bool con_anticipo_pos=false;
         if (q.isActive())
            while (q.next())
             {
              QString ref=q.value(1).toString();
              // buscamos cuenta de ingreso
              cuenta_ingreso=basedatos::instancia()->cuenta_ref(ref);
              //descripcion=q.value(2).toString();
              double importe=q.value(3).toDouble()*q.value(4).toDouble()*(1-q.value(8).toDouble()/100);
              if (importe<0.001 && importe>-0.001) continue;
              totalfactura+=importe;
              int pos=cuentas_ingreso.indexOf(cuenta_ingreso);
              if (pos>-1)
                 {
                  importes[pos] = importes.at(pos) + importe;
                 }
                else
                    {
                      cuentas_ingreso << cuenta_ingreso;
                      importes << importe;
                      con_anticipo=basedatos::instancia()->ref_con_anticipo(ref);
                      con_anticipo_pos=con_anticipo && importe>0;
                      anticipos<<con_anticipo;
                    }
              }

             //if (con_anticipo_pos) tablaasiento->desactiva_avisar_cta_libro_fact_no_diario();

             if ((conanalitica() || conanalitica_parc()) && !basedatos::instancia()->analitica_tabla())
              {
               q.first();
               do
                {
                 // int fila, QString col0, QString col1, QString col2,
                 // QString col3, QString col4, QString col16
                 //
                 // clave, codigo, descripcion, cantidad, precio, "
                 // "clave_iva, tipo_iva, tipo_re, dto, notas, ci
                 QString cadvalor;
                 cadvalor.setNum(redond(q.value(3).toDouble()*q.value(4).toDouble()*(1-q.value(8).toDouble()/100),2),'f',2);
                 tablaasiento->pasadatos1(fila,basedatos::instancia()->ref_con_anticipo(q.value(1).toString()) ? cta_anticipos : basedatos::instancia()->cuenta_ref(q.value(1).toString()),
                   q.value(2).toString(),"",cadvalor,serie+numero,q.value(10).toString());
                 // if (con_anticipo_pos) tablaasiento->pasalineafactura(fila);
                 fila++;
                } while (q.next());
           }
          else
            // ESTE BUCLE EN CASO DE NO ANALÍTICA NORMAL
           for (int veces=0;veces<cuentas_ingreso.count();veces++)
             {
                   // -------------------
              QString cuenta_ingreso=cuentas_ingreso.at(veces);
              double importe=redond(importes.at(veces),2);
              bool es_con_anticipo=anticipos.at(veces);
              cadvalor.setNum(importe,'f',2);

              tablaasiento->pasadatos1(fila,es_con_anticipo ? cta_anticipos : cuenta_ingreso,descripcion,"",cadvalor,serie+numero,"");
              // miramos si la cuenta es de analítica tabla
              // -------------------------------------------------------
              bool procesar_analitica=false;
              if (basedatos::instancia()->analitica_tabla() && conanalitica() &&
                                    (escuentadegasto(cuenta_ingreso) ||
                                     escuentadeingreso(cuenta_ingreso)))
                  procesar_analitica=true;
              if (basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
                  codigo_en_analitica(cuenta))
                 procesar_analitica=true;
              if (procesar_analitica)
                 {
                  tablaasiento->realiza_asignainputdiario(fila,
                    cadvalor );
                 }
              //if (con_anticipo_pos) tablaasiento->pasalineafactura(fila);
              fila++;

             }

         if (con_ret)
            {
             tablaasiento->pasadatos1(fila,cuenta_ret,descripcion,retencion,"",serie+numero,"");
             fila++;
             //tablaasiento->pasadatos1(fila,cuenta,tipo_doc+": "+serie+numero,"",retencion,serie+numero,"");
             //fila++;
            }
           // nos queda el último apunte: total factura
           QString totaloper; totaloper.setNum(totalfactura,'f',2);
           if (con_ret) totalfactura=totalfactura-retencion.toDouble();
           cadvalor.setNum(redond(totalfactura,2),'f',2);
           tablaasiento->pasadatos1(fila,cuenta,descripcion,cadvalor,"",serie+numero,"");
           // queda pasar detalles para el libro de facturas emitidas
           // fecha_op - desglosar
           QString dia, mes, anyo;
           dia.setNum(fecha_fac.day());
           mes.setNum(fecha_fac.month());
           anyo.setNum(fecha_fac.year());

           QString info_donacion;
           if (tipo_operacion==8) {
              info_donacion=basedatos::instancia()->info_donacion_doc(tipo_doc);
           }

           tablaasiento->pasadatos2(fila,cuenta_ingreso,totaloper,/*tipo_exento()*/ "",
                                    "", //tipo iva
                                    "", // tipo re
                                    "0", // cuota iva
                                    cuenta, // cuenta fra
                                    dia, mes, anyo,
                                    "0", // soportado
                                    "1", // prorrata
                                    rectificativa ? "1" : "0", // rectificativa
                                    "0",  // autofactura (nunca para emitidas)
                                    fecha_op.toString("yyyy-MM-dd"), // fecha operación
                                    "", // clave operación
                                    "", // bi a costo
                                    "", // rectificada
                                    "", // nfacturas
                                    "", // finicial
                                    "", // ffinal
                                    "0", // bien inversion
                                    "1", // afecto
                                    "","","","", // agrario, nombre, cif, importacion
                                    (tipo_operacion==6 || tipo_operacion==9) ? "1" : "", // exportación
                                    tipo_operacion==7 ? "1" : "", // exenta no deduc
                                    "","", // isp_op_interiores, caja_iva
                                    false, false, false, // no 347 - arrto_ret - arrto_sin_ret
                                    "",info_donacion);
           // qDebug() << info_donacion;
           if (tipo_operacion==3 || tipo_operacion==9) tablaasiento->activa_fila_nosujeta(fila);// no sujetas
           if (tipo_operacion==5) tablaasiento->activa_fila_prservicios_ue(fila);// pr servicios ue
           int filaexento=0; // guarda la línea donde se guarda info del libro facturas
           filaexento=fila;

           // marcamos esta línea en tabla de asientos para recuperarla y asignarla a cabecera facturas
           tablaasiento->pasalineafactura(fila);
           tablaasiento->chequeatotales();
           if (tablaasiento->botonincorporaractivado())
              {
                 tablaasiento->incorporar();
                 qlonglong pase_fra=tablaasiento->qpasefactura();
                 if (!nomsj && pase_fra>0)
                   QMessageBox::information( NULL, QObject::tr("CONTABILIZAR DOCUMENTO"),
                                    QObject::tr("ASIENTO GENERADO CORRECTAMENTE"));
                 QString cadpase; cadpase.setNum(pase_fra);
                 if (pase_fra>0)
                   basedatos::instancia()->update_contabilizado_doc(serie, numero, cadpase);
              }
              else
              {
                  tablaasiento->borra_valores_iva(filaexento);
                  if (tipo_operacion==4) tablaasiento->desactiva_eib_exterior();
                  if (tipo_operacion==2 || tipo_operacion==3) tablaasiento->activa_opexenta();
                  int resultado = tablaasiento->exec();
                  qlonglong pase_fra=tablaasiento->qpasefactura();
                  QString cadpase; cadpase.setNum(pase_fra);
                  if (resultado == QDialog::Accepted && pase_fra!=0)
                     basedatos::instancia()->update_contabilizado_doc(serie, numero, cadpase);
              }
           // queda actualizar pasefactura, cerrado y contabilizado
           delete tablaasiento;

         }

}
