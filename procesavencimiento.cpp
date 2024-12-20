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

#include "procesavencimiento.h"
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include "basedatos.h"
#include <QMessageBox>

procesavencimiento::procesavencimiento(QString qusuario) : QDialog() {
    ui.setupUi(this);

    comadecimal=haycomadecimal();
    decimales=haydecimales();
    usuario=qusuario;

  connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(botonprocesarpulsado()));
  connect(ui.ctatesorerialineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuentacambiada()));
  connect(ui.ctatesorerialineEdit,SIGNAL(editingFinished ()),this,SLOT(finediccuenta()));
  connect(ui.buscapushButton,SIGNAL( clicked()),this,SLOT(buscacuenta()));
  connect(ui.busca2pushButton,SIGNAL( clicked()),this,SLOT(buscacuenta2()));
  connect(ui.gastos_lineEdit,SIGNAL(editingFinished()),this,SLOT(finedic_gastos()));
  connect(ui.cta_gastos_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuenta_gasto_cambiada()));
  connect(ui.cta_gastos_lineEdit,SIGNAL(editingFinished()),this,SLOT(finediccuenta_gastos()));
}


void procesavencimiento::pasa_fecha_realizacion(QDate fecha)
{
 ui.vencidateEdit->setDate(fecha);
}

void procesavencimiento::pasa_documento(QString documento) {
    ui.doclineEdit->setText(documento);
}

void procesavencimiento::cargadatos(QString numvenci )
{
   QSqlQuery query = basedatos::instancia()->select9Vencimientosnum(numvenci);

   ui.numvencilineEdit->setText(numvenci);
   QString cadpase;
   bool derecho=false;
   if ( query.isActive() ) {
       if ( query.next() )
	  {
            ui.fechadateEdit->setDate(query.value(1).toDate());
            ui.importelineEdit->setText(formateanumero(
                               query.value(2).toString().toDouble(),comadecimal,decimales));
	    ui.cuentalineEdit->setText(query.value(0).toString());
            if (usuario.isEmpty()) ui.usuariolineEdit->setText(query.value(7).toString());
               else ui.usuariolineEdit->setText(usuario);
	    cadpase=query.value(6).toString();
            concepto=query.value(8).toString();
            derecho=query.value(5).toBool();
            ui.externo_lineEdit->setText(query.value(9).toString());
            ui.descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(query.value(9).toString()));
	    if (query.value(5).toBool())
	       {
	          ui.derechoradioButton->setChecked(true);
	          ui.obligacionradioButton->setChecked(false);
	       }
	       else
	                 {
  	                   ui.derechoradioButton->setChecked(false);
	                   ui.obligacionradioButton->setChecked(true);
	                 }
	    ui.vencidateEdit->setDate(query.value(4).toDate());
            guarda_ejercicio=ejerciciodelafecha(query.value(4).toDate());
            ui.ctatesorerialineEdit->setText(query.value(3).toString());
            // ponemos info de la cuenta bancaria
            QString ccc, iban, cedente1, cedente2, nriesgo, sufijo;
            basedatos::instancia()->datos_cuenta_banco(ui.ctatesorerialineEdit->text(),
                                                       &ccc,
                                                       &iban,
                                                       &cedente1,
                                                       &cedente2,
                                                       &nriesgo,
                                                       &sufijo);
            if (iban.isEmpty())
                ui.cuenta_bancolineEdit->setText(ccc);
              else
                ui.cuenta_bancolineEdit->setText(iban);
            // ajustamos etiquetas de fecha realización y medio realización
            // según sea cobro o pago la operación
            if (derecho)
              {
                ui.fechatextLabel->setText(tr("Fecha de cobro:"));
                ui.mediolabel->setText(tr("Medio de cobro"));
              }
              else
                   {
                    ui.fechatextLabel->setText(tr("Fecha de pago:"));
                    ui.mediolabel->setText(tr("Medio de pago"));
                   }
	  }
     }

     ui.descripctalineEdit->setText( basedatos::instancia()->selectDescripcionplancontable(ui.cuentalineEdit->text()) );

     query = basedatos::instancia()->selectAsientodocumentodiariopase(cadpase);

     if ( query.isActive() ) 
     if ( query.next() )
	 {
            ui.asientolineEdit->setText(query.value(0).toString());
            ui.documentolineEdit->setText(query.value(1).toString());
	  }

     ui.descriptesorlineEdit->setText( basedatos::instancia()->selectDescripcionplancontable( ui.ctatesorerialineEdit->text() ) );

     if (concepto.length()>0) ui.conceptolineEdit->setText(concepto);
        else
        {
            QString qconcepto;
            if (derecho) qconcepto=basedatos::instancia()->msj_vto_dcho();
                 else qconcepto=basedatos::instancia()->msj_vto_obl();
            if (qconcepto.isEmpty())
              qconcepto=tr("Vencim. ");
             else qconcepto+=" ";
            if (ui.descrip_externo_lineEdit->text().isEmpty())
              qconcepto += ui.descripctalineEdit->text();
             else qconcepto += ui.descrip_externo_lineEdit->text();
            if (ui.documentolineEdit->text().length()>0)
            {
                qconcepto += " - ";
                qconcepto += ui.documentolineEdit->text();
            }
            qconcepto += " de ";
            qconcepto += ui.fechadateEdit->date().toString("dd.MM.yyyy");
            ui.conceptolineEdit->setText(qconcepto);
        }

     ui.cta_gastos_lineEdit->setText(basedatos::instancia()->selec_cuenta_gasto_vto_config());
}


void procesavencimiento::asignaasiento(QString elasiento )
{
    gasiento=elasiento;
}


void procesavencimiento::botonprocesarpulsado()
{

  if (basedatos::instancia()->hay_criterio_caja_aux(ui.cuentalineEdit->text()))
    {
     if (ui.cuenta_bancolineEdit->text().isEmpty() && ui.medio_realizacionlineEdit->text().isEmpty())
      {
         QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),
                               tr("ERROR: Falta contenido en cuenta bancaria y/o medio realización"));
       return;
      }
    }

  if (!existesubcuenta(ui.ctatesorerialineEdit->text()))
    {
      QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),
               tr("Cuenta auxiliar inexistente de tesorería; imposible procesar vencimiento"));
      return;
    }

  if (!ui.gastos_lineEdit->text().isEmpty() && !existesubcuenta(ui.cta_gastos_lineEdit->text()))
  {
    QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),
             tr("Cuenta auxiliar inexistente de gasto; imposible procesar vencimiento"));
    return;
  }

  QString ejercicio=ejerciciodelafecha(ui.vencidateEdit->date());
  if (ejerciciocerrado(ejercicio))
       {
          QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),
              tr("El ejercicio correspondiente al vencimiento de la cuenta está cerrado"));
          return;
       }

  if (basedatos::instancia()->ejercicio_bloqueado(ejercicio))
       {
          QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),
              tr("El ejercicio correspondiente al vencimiento de la cuenta está bloqueado"));
          return;
       }


  if (!fechacorrespondeaejercicio(ui.vencidateEdit->date()))
       {
          QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),
                tr("El ejercicio correspondiente al vencimiento de la cuenta no existe"));
          return;
       }

   if (basedatos::instancia()->fecha_periodo_bloqueado(ui.vencidateEdit->date()))
       {
        QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),tr("ERROR: La fecha del vencimiento corresponde "
                                                              "a un periodo bloqueado"));
        return;
       }

   if (basedatos::instancia()->existe_bloqueo_cta(ui.ctatesorerialineEdit->text(),
                                                  ui.vencidateEdit->date()))
       {
         QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),tr("ERROR: La cuenta %1 "
                                        "está bloqueada").arg(ui.ctatesorerialineEdit->text()));
         return;
       }

   if (basedatos::instancia()->existe_bloqueo_cta(ui.cuentalineEdit->text(),
                                                  ui.vencidateEdit->date()))
       {
          QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),tr("ERROR: La cuenta %1 "
                                        "está bloqueada").arg(ui.cuentalineEdit->text()));
          return;
       }

 // -----------------------------------------------------------------------------------------
   if (!ui.gastos_lineEdit->text().isEmpty() && conanalitica() && (escuentadegasto(ui.cta_gastos_lineEdit->text()) ||
         escuentadeingreso(ui.cta_gastos_lineEdit->text())))
     {
      QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),tr("ERROR: La cuenta %1 "
                                    "no se puede emplear porque es de analítica").arg(ui.cta_gastos_lineEdit->text()));
      return;
     }

   if (!ui.gastos_lineEdit->text().isEmpty() && basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
       codigo_en_analitica(ui.cta_gastos_lineEdit->text()))
    {
     QMessageBox::warning( this, tr("PROCESA VENCIMIENTO"),tr("ERROR: La cuenta %1 "
                                  "no se puede emplear porque es de analítica").arg(ui.cta_gastos_lineEdit->text()));
     return;
    }

 // -------------------------------------------------------------------------------------------

  // generamos asiento y actualizamos vencimientos en una sola transacción
     QSqlDatabase contabilidad=QSqlDatabase::database();

    contabilidad.transaction();

    basedatos::instancia()->bloquea_para_procesavencimiento();
    qlonglong vnum;
    if (gasiento.isEmpty() || guarda_ejercicio!=ejerciciodelafecha(ui.vencidateEdit->date()))
       {
         vnum = basedatos::instancia()->proximoasiento(ejercicio);
         gasiento.clear();
       }
       else vnum=gasiento.toLongLong();

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    if (gasiento.isEmpty())
       basedatos::instancia()->update_ejercicio_prox_asiento(ejercicio, cadnum);

    QString cadnumpase;
    qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
    // esto sería para cada pase del asiento; incrementar pase
    cadnumpase.setNum(pase);

    // primer pase del asiento, cuenta acreedor/deudor de cargo/abono
    QString qdebe, qhaber;
    if (ui.derechoradioButton->isChecked()) {
        qdebe = "0";
        qhaber = convapunto(ui.importelineEdit->text());
    }
    else {
        qdebe = convapunto(ui.importelineEdit->text());
        qhaber = "0";
	}
    QString qconcepto;
    qconcepto=ui.conceptolineEdit->text();

    /* if (concepto.length()>0) qconcepto = concepto;
    else
    {
        qconcepto ="Vto. ";
        qconcepto += ui.descripctalineEdit->text();
        if (ui.documentolineEdit->text().length()>0)
        {
            qconcepto += " Doc.";
            qconcepto += ui.documentolineEdit->text();
        }
        qconcepto += " de ";
        qconcepto += ui.fechadateEdit->date().toString("dd.MM.yyyy");
    }*/

    basedatos::instancia()->insertDiario10(cadnumasiento, cadnumpase, 
        ui.vencidateEdit->date().toString("yyyy-MM-dd"),
        ui.cuentalineEdit->text(), qdebe, qhaber, qconcepto, ui.doclineEdit->text(),
        usuario, ejercicio,ui.externo_lineEdit->text(),ui.contab_borr_checkBox->isChecked());

    QString pase_diario_vencimiento = cadnumpase;

   //------------------------------------------------------------------------------------------------------
    if (ui.derechoradioButton->isChecked())
        basedatos::instancia()->updateSaldossubcuentasaldocargocodigo( ui.cuentalineEdit->text() , convapunto(ui.importelineEdit->text()) , false );
    else
        basedatos::instancia()->updateSaldossubcuentasaldocargocodigo( ui.cuentalineEdit->text() , convapunto(ui.importelineEdit->text()) , true );

   //--------------------------------------------------------------------------------------------------------


    pase++;
    cadnumpase.setNum(pase);

    // procesamos pase de la cuenta de tesorería ó cobro/pago
    double gastos=0;
    if (!ui.gastos_lineEdit->text().isEmpty())
        gastos=convapunto(ui.gastos_lineEdit->text()).toDouble();

    double importe=convapunto(ui.importelineEdit->text()).toDouble();
    if (ui.derechoradioButton->isChecked()) importe-=gastos; else importe+=gastos;
    QString cadimporte; cadimporte.setNum(importe,'f',2);
    if (ui.derechoradioButton->isChecked()) {
        qdebe = convapunto(cadimporte);
        qhaber = "0";
    }
    else {
        qdebe = "0";
        qhaber = convapunto(cadimporte);
	}
    /*
    if (concepto.length()>0) qconcepto = concepto;
    else
    {
        qconcepto ="Vto. ";
        qconcepto += ui.descripctalineEdit->text();
        if (ui.documentolineEdit->text().length()>0)
        {
            qconcepto += " Doc.";
            qconcepto += ui.documentolineEdit->text();
        }
        qconcepto += " de ";
        qconcepto += ui.fechadateEdit->date().toString("dd.MM.yyyy");
    }
    */
    basedatos::instancia()->insertDiario10(cadnumasiento, cadnumpase,
                                           ui.vencidateEdit->date().toString("yyyy-MM-dd"),
                                           ui.ctatesorerialineEdit->text(), qdebe, qhaber,
                                           qconcepto, ui.doclineEdit->text(), usuario, ejercicio,
                                           ui.externo_lineEdit->text(),ui.contab_borr_checkBox->isChecked());


   //---------------------------------------------------------------------------------------------------------
    if (ui.derechoradioButton->isChecked())
        basedatos::instancia()->updateSaldossubcuentasaldocargocodigo( ui.ctatesorerialineEdit->text() , convapunto(ui.importelineEdit->text()) , true );
    else
        basedatos::instancia()->updateSaldossubcuentasaldocargocodigo( ui.ctatesorerialineEdit->text() , convapunto(ui.importelineEdit->text()) , false );

   //--------------------------------------------------------------------------------------------------------


    if (!ui.gastos_lineEdit->text().isEmpty())
       // apunte de cuenta de gasto
      {
        pase++;
        cadnumpase.setNum(pase);
        QString cadgasto; cadgasto.setNum(gastos,'f',2);
        // siempre el gasto estará en el debe
            qdebe = cadgasto;
            qhaber = "0";
        basedatos::instancia()->insertDiario10(cadnumasiento, cadnumpase,
                                               ui.vencidateEdit->date().toString("yyyy-MM-dd"),
                                               ui.cta_gastos_lineEdit->text(), qdebe, qhaber,
                                               qconcepto, ui.doclineEdit->text(), usuario, ejercicio,
                                               ui.externo_lineEdit->text(),ui.contab_borr_checkBox->isChecked());
        basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(
                    ui.cta_gastos_lineEdit->text() ,
                    cadgasto , true );
      }

    pase++;
    cadnumpase.setNum(pase);

    basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);


    basedatos::instancia()->updateVencimientos4num( ui.ctatesorerialineEdit->text() ,
                                                    ui.vencidateEdit->date() , false,
                                                    pase_diario_vencimiento,
                                                    ui.medio_realizacionlineEdit->text(),
                                                    ui.cuenta_bancolineEdit->text(),
                                                    ui.numvencilineEdit->text() );

    basedatos::instancia()->desbloquea_y_commit();

    basedatos::instancia()->update_cuenta_gasto_vto_config(ui.cta_gastos_lineEdit->text());
    accept() ;
}


void procesavencimiento::cuentacambiada()
{
   QString cadena;
   if (!existesubcuenta(ui.ctatesorerialineEdit->text()))
     {
       ui.aceptarpushButton->setEnabled(false);
       ui.descriptesorlineEdit->setText("");
       ui.cuenta_bancolineEdit->clear();
       return;
     }
     else 
            {
               ui.aceptarpushButton->setEnabled(true);
               if (existecodigoplan(ui.ctatesorerialineEdit->text(),&cadena))
                  ui.descriptesorlineEdit->setText(cadena);
               else ui.descriptesorlineEdit->setText("");
               QString ccc, iban, cedente1, cedente2, nriesgo, sufijo;
               basedatos::instancia()->datos_cuenta_banco(ui.ctatesorerialineEdit->text(),
                                                           &ccc,
                                                           &iban,
                                                           &cedente1,
                                                           &cedente2,
                                                           &nriesgo,
                                                           &sufijo);
               if (iban.isEmpty())
                    ui.cuenta_bancolineEdit->setText(ccc);
                 else
                    ui.cuenta_bancolineEdit->setText(iban);
            }
}


void procesavencimiento::finediccuenta()
{
 QString expandida=expandepunto(ui.ctatesorerialineEdit->text(),anchocuentas());
 ui.ctatesorerialineEdit->setText(expandida);
 /*
 QString ccc, iban, cedente1, cedente2, nriesgo, sufijo;
 basedatos::instancia()->datos_cuenta_banco(ui.ctatesorerialineEdit->text(),
                                             &ccc,
                                             &iban,
                                             &cedente1,
                                             &cedente2,
                                             &nriesgo,
                                             &sufijo);
 if (iban.isEmpty())
      ui.cuenta_bancolineEdit->setText(ccc);
   else
      ui.cuenta_bancolineEdit->setText(iban);
*/
}


void procesavencimiento::cuenta_gasto_cambiada()
{
   QString cadena;
   if (!existesubcuenta(ui.cta_gastos_lineEdit->text()))
     {
       ui.descrip_gastoslineEdit->clear();
       return;
     }
     else
            {
               if (existecodigoplan(ui.cta_gastos_lineEdit->text(),&cadena))
                  ui.descrip_gastoslineEdit->setText(cadena);
               else ui.descrip_gastoslineEdit->setText("");
            }
}


void procesavencimiento::finediccuenta_gastos()
{
 QString expandida=expandepunto(ui.cta_gastos_lineEdit->text(),anchocuentas());
 ui.cta_gastos_lineEdit->setText(expandida);
}



void procesavencimiento::buscacuenta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.ctatesorerialineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2)) ui.ctatesorerialineEdit->setText(cadena2);
       else ui.ctatesorerialineEdit->setText("");
    delete labusqueda;
}

void procesavencimiento::buscacuenta2()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cta_gastos_lineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2)) ui.cta_gastos_lineEdit->setText(cadena2);
       else ui.cta_gastos_lineEdit->setText("");
    delete labusqueda;
}

void procesavencimiento::finedic_gastos()
{
  if (comadecimal) ui.gastos_lineEdit->setText(convacoma(ui.gastos_lineEdit->text()));
     else
         ui.gastos_lineEdit->setText(convapunto(ui.gastos_lineEdit->text()));
}
