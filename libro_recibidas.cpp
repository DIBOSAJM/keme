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

#include "libro_recibidas.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "tabla_iva_sop.h"
#include <QMessageBox>
#include "buscasubcuenta.h"
#include "busca_externo.h"
#include <qtrpt.h>
#include <QFileDialog>

libro_recibidas::libro_recibidas(QString qusuario) : QDialog() {
    ui.setupUi(this);


usuario=qusuario;
  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

  comadecimal=haycomadecimal(); decimales=haydecimales();

  connect(ui.consultarpushButton,SIGNAL( clicked()),this,SLOT(consultatex()));
  connect(ui.imprimirpushButton,SIGNAL( clicked()),this,SLOT(imprime()));
  connect(ui.copiarpushButton,SIGNAL( clicked()),this,SLOT(copiar()));

  connect(ui.prorratacheckBox,SIGNAL(stateChanged(int)),SLOT(prorratacambiado()));

  connect(ui.bicheckBox ,SIGNAL(stateChanged(int)),SLOT(bicheckcambiado()));
  connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));

  connect(ui.tablapushButton,SIGNAL(clicked()),SLOT(tabla()));

  connect(ui.codigopushButton,SIGNAL(clicked(bool)),SLOT(boton_cuenta_pulsado()));
  connect(ui.cuentalineEdit,SIGNAL(editingFinished()),this,SLOT(cuenta_finedicion()));
  connect(ui.cuentalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuenta_cambiada()));

  connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(externo_cambiado()));
  connect(ui.externo_pushButton,SIGNAL(clicked(bool)),this,SLOT(buscaexterno()));
  connect(ui.externo_groupBox,SIGNAL(toggled(bool)),this,SLOT(check_grupo_externo_cambiado()));

  ui.inicialdateEdit->setDate(QDate::currentDate());
  ui.finaldateEdit->setDate(QDate::currentDate());

  ordenrecepcion=basedatos::instancia()->hay_secuencia_recibidas();

#ifdef NOEDITTEX
 ui.latexpushButton->hide();
#endif


}

void libro_recibidas::prorratacambiado()
{
 if (ui.prorratacheckBox->isChecked())
    {
     ui.horizontalradioButton->setChecked(true);
     ui.verticalradioButton->setChecked(false);
     ui.groupBox->setEnabled(false);
    }
    else
        {
         ui.horizontalradioButton->setChecked(false);
         ui.verticalradioButton->setChecked(true);
         ui.groupBox->setEnabled(true);
        }
}


void libro_recibidas::bicheckcambiado()
{
    ui.prorratacheckBox->disconnect(SIGNAL(stateChanged(int)));

    if (ui.bicheckBox->isChecked())
       {
        ui.horizontalradioButton->setChecked(true);
        ui.verticalradioButton->setChecked(false);
        ui.groupBox->setEnabled(false);
        ui.prorratacheckBox->setChecked(false);
        ui.prorratacheckBox->setEnabled(false);
       }
       else
           {
            ui.horizontalradioButton->setChecked(false);
            ui.verticalradioButton->setChecked(true);
            ui.groupBox->setEnabled(true);
            ui.prorratacheckBox->setChecked(false);
            ui.prorratacheckBox->setEnabled(true);
           }
     connect(ui.prorratacheckBox,SIGNAL(stateChanged(int)),SLOT(prorratacambiado()));
}

bool libro_recibidas::generalatexsoportado()
{
   if (!ui.interiorescheckBox->isChecked() && !ui.aibcheckBox->isChecked() && 
       !ui.autofacturascheckBox->isChecked() && !ui.rectificativascheckBox->isChecked()
       && !ui.autofacturasnouecheckBox->isChecked() && !ui.agrariocheckBox->isChecked()
       && !ui.isp_op_interiorescheckBox->isChecked() && !ui.importacionescheckBox->isChecked())
       return false;

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=nombrefichero();
   qfichero+=".tex";
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return false;

   QString ejercicio=ui.ejerciciocomboBox->currentText();

   QList<int> asientos_ret;
   QStringList imp_retenciones;

   QSqlQuery qr = basedatos::instancia()->pases_retenciones_ejercicio(ejercicio);
   if ( qr.isActive() ) {
       while ( qr.next() ) {
           asientos_ret << qr.value(1).toInt();
           imp_retenciones << formatea_redondeado(qr.value(2).toDouble(),false,decimales);
       }
   }


    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    QString externo;
    if (ui.externo_groupBox->isChecked()) externo=ui.externo_lineEdit->text();

    if (!cuenta.isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\Large \\textbf {";
      stream << filtracad(cuenta) + " " + descripcioncuenta(cuenta) << "}}";
      stream << "\\end{center}";
     }

    if (!externo.isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\Large \\textbf {";
      stream << filtracad(externo) + " " + basedatos::instancia()->razon_externo(externo) << "}}";
      stream << "\\end{center}";
     }

    stream << "\\begin{center}" << "\n";
    if (ui.info_ret_checkBox->isChecked())
      stream << "\\begin{longtable}{|r|p{1.3cm}|c|c|c|p{2.5cm}|r|r|r|r|r|r|}" << "\n";
     else
        stream << "\\begin{longtable}{|r|p{1.5cm}|c|c|c|p{3cm}|r|r|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";

   if (ui.info_ret_checkBox->isChecked())
       stream << "\\multicolumn{12}{|c|} {\\textbf{";
    else
       stream << "\\multicolumn{11}{|c|} {\\textbf{";
   QString cadena=filtracad(ui.cabeceralineEdit->text());
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // -------------------------------------------------------------------------------------
    if (ordenrecepcion) stream << "{\\tiny{" << tr("Recep.") << "}} & ";
        else
            stream << "{\\tiny{" << tr("Orden") << "}} & ";
    stream << "{\\tiny{" << tr("Factura") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha Fra.") << "}} & ";
    stream << "{\\tiny{" << tr("Contab.") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Proveedor/acreedor") << "}} & ";
    // stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Asto.") << "}} & ";
    stream << "{\\tiny{" << tr("Base Imp.") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}} & ";
    stream << "{\\tiny{" << tr("Cuota") << "}} & ";
    if (ui.info_ret_checkBox->isChecked()) {
        stream << "{\\tiny{" << tr("Retención") << "}} & ";
    }
    stream << "{\\tiny{" << tr("Total") << "}}";

    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    // stream << tr(" \\\\") << "\n";
    if (ordenrecepcion) stream << "{\\tiny{" << tr("Recep.") << "}} & ";
        else
          stream << "{\\tiny{" << tr("Orden") << "}} & ";
    stream << "{\\tiny{" << tr("Factura") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha Fra.") << "}} & ";
    stream << "{\\tiny{" << tr("Contab.") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Proveedor/acreedor") << "}} & ";
    // stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Asto.") << "}} & ";
    stream << "{\\tiny{" << tr("Base Imp.") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}} & ";
    stream << "{\\tiny{" << tr("Cuota") << "}} & ";
    if (ui.info_ret_checkBox->isChecked()) {
        stream << "{\\tiny{" << tr("Retención") << "}} & ";
    }
    stream << "{\\tiny{" << tr("Total") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ---------------------------------------------------------------------------------------

    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false,cuenta,externo);

   // ÚLTIMOS PARÁMETROS
   // bool filtrarbinversion, bool solobi, bool sinbi,
   // bool filtrarnosujetas, bool solonosujetas, bool sinnosujetas


    QSqlQuery query =
      basedatos::instancia()->registros_recibidas(ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(), cuenta,
         ui.orden_fecha_faccheckBox->isChecked(), externo);

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    double base=0;
    double cuota=0;
    double ret=0;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                // posición 20 sería el externo***********
                QString externo=query.value(20).toString();
                bool hayexterno=!externo.isEmpty();
                bool esautofactura=query.value(11).toBool();
                ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0 
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++; 
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                stream << "{\\tiny " <<
                        (ordenrecepcion ? query.value(15).toString(): cadnum.setNum(numorden))
                        << "} & {\\tiny ";
                stream << filtracad(query.value(0).toString()) << "} & {\\tiny "; // fra.
                stream << query.value(9).toDate().toString("dd.MM.yyyy") << "} & {\\tiny "; // fecha fra
                stream << query.value(1).toDate().toString("dd.MM.yyyy") << "} & {\\tiny "; // fecha contable
                if (!esautofactura)
                  {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();
                    stream << ( query.value(14).toString().isEmpty() ?
                                nif :
                                filtracad(query.value(14).toString()))
                                << "} & {\\tiny "; // cif
                   // stream <<  query.value(2).toString() << "} & {\\tiny "; // cuenta
                   stream <<
                           (query.value(13).toString().isEmpty() ?
                           filtracad(razon) :
                           filtracad(query.value(13).toString())) << "} & {\\tiny "; // proveedor
	           //if (aib || eib) stream <<  nifcomunitariocta(query.value(2).toString()) << "} & {\\tiny "; // cif
		   //else stream <<  cifcta(query.value(2).toString()) << "} & {\\tiny "; // cif
                  }
                  else
                      {
                       stream <<  basedatos::instancia()->cif() << "} & {\\tiny "; // cif
                       // stream << "} & {\\tiny "; // nada de cuenta
                       stream <<  filtracad(nombreempresa()) << "} & {\\tiny "; // proveedor
                      }
                stream <<  query.value(4).toString() << "} & {\\tiny "; // asiento
                stream <<  formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // base iva
                stream <<  formatea_redondeado_sep(query.value(6).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // tipo
                stream <<  formatea_redondeado_sep(query.value(7).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // cuota
                if (ui.info_ret_checkBox->isChecked()) {
                    int pos=-1;
                    pos=asientos_ret.indexOf(query.value(4).toInt());
                    //qDebug() << QString().setNum(pos);
                    //qDebug() << QString().setNum(query.value(17).toInt());
                    if (pos>=0) {
                          stream << formatea_redondeado(convapunto(imp_retenciones.at(pos)).toDouble(),comadecimal,decimales) << "} & ";
                          ret+=convapunto(imp_retenciones.at(pos)).toDouble();
                         }
                      else stream << "0" << "} & ";
                    if (pos>=0)
                       stream << "{\\tiny " <<formatea_redondeado(query.value(8).toDouble()
                                                      -convapunto(imp_retenciones.at(pos)).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                                                      << "\\hline\n"; // total
                      else
                        stream << "{\\tiny " <<  formatea_redondeado(query.value(8).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                                                     << "\\hline\n"; // total

                }
                else {
                      if (query.value(10).toBool() || query.value(11).toBool() || query.value(16).toBool() || query.value(22).toBool())
                          stream <<  formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                                  << "\\hline\n"; // total
                      else
                        stream <<  formatea_redondeado_sep(query.value(8).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                         << "\\hline\n"; // total
                 }
                if (query.value(18).toBool())
                   {
                    // criterio de caja
                    QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(
                            query.value(19).toString());
                    // v.fecha_vencimiento, v.liq_iva_fecha, v.forzar_liq_iva, v.importe,
                    // v.num, v.medio_realizacion, v.cuenta_bancaria
                    bool vacio=true;
                    if (q.isActive())
                        while (q.next())
                         {
                          vacio=false;
                          stream << "\\multicolumn{2}{|r|} {\\tiny ";
                          stream << tr("Importe pago");
                          stream << "} & {\\tiny ";
                          stream << formatea_redondeado_sep(q.value(3).toDouble(),comadecimal,decimales);
                          stream << "} & {\\tiny ";
                          stream << tr("FECHA:");
                          stream << "} & {\\tiny ";
                          stream << q.value(1).toDate().toString("dd-MM-yyyy");
                          stream << "} & \\multicolumn{2}{|r|} {\\tiny ";
                          stream << q.value(5).toString();
                          stream << "} & {\\tiny ";
                          stream << tr("Cuenta:");
                          stream << "} & \\multicolumn{3}{|l|} {\\tiny ";
                          stream << q.value(6).toString();
                          stream << "} \\\\ \n  "
                                  << "\\hline\n";
                         }
                    if (vacio)
                       {
                        stream << "\\multicolumn{2}{|r|} {\\tiny ";
                        stream << tr("Importe pago");
                        stream << "} & {\\tiny ";
                        stream << "} & {\\tiny ";
                        stream << tr("FECHA:");
                        stream << "} & {\\tiny ";
                        stream << "} & \\multicolumn{2}{|r|} {\\tiny ";
                        stream << "} & {\\tiny ";
                        stream << tr("Cuenta:");
                        stream << "} & \\multicolumn{3}{|l|} {\\tiny ";
                        stream << "} \\\\ \n  "
                             << "\\hline\n";
                       }
                   }
                numprog++;
	        base+=query.value(5).toDouble();
	        cuota+=query.value(7).toDouble();
	      }
      }

    stream << "\\multicolumn{7}{|r|} {\\textbf{";
   // --------------------------------------------------------------------------------------
    stream << tr("SUMAS TOTALES:") << "}} ";
    stream << " & {\\tiny " << formatea_redondeado_sep(base,comadecimal,decimales) << "} & & {\\tiny ";
    stream << formatea_redondeado_sep(cuota,comadecimal,decimales) << "} &  {\\tiny ";
    if (ui.info_ret_checkBox->isChecked())
        stream << formatea_redondeado_sep(ret,comadecimal,decimales) << "} &  {\\tiny ";
    stream << formatea_redondeado_sep(base+cuota-ret,comadecimal,decimales);
    stream <<  "} \\\\ \n";
    stream << "\\hline" << "\n";


    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";
    
    fichero.close();

    ui.progressBar->reset();

    return true;

}





void libro_recibidas::consultatex()
{

   if (ui.prorratacheckBox->isChecked())
      {
       if (!generalatexprorrata())
          return;
      }
     else
       {
        if (!ui.horizontalradioButton->isChecked())
          {
           if  (!generalatexsoportado())
              return;
          }

        if (ui.horizontalradioButton->isChecked() && !ui.bicheckBox->isChecked())
          {
            if  (!generalatexsoportado_horizontal())
            return;
          }

        if (ui.horizontalradioButton->isChecked() && ui.bicheckBox->isChecked())
          {
            if  (!generalatexsoportado_horizontal_bi())
            return;
          }
       }

   int valor=consultalatex2(nombrefichero());
   if (valor==1)
       QMessageBox::warning( this, tr("Libros de FACTURAS"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Libros de FACTURAS"),
                             tr("PROBLEMAS al llamar a ")+visordvi());


}


void libro_recibidas::imprime()
{
    informe();
    /*
   if (ui.prorratacheckBox->isChecked())
      {
       if (!generalatexprorrata())
          return;
      }
     else
       {
        if (!ui.horizontalradioButton->isChecked())
          {
           if  (!generalatexsoportado())
            return;
          }
        if (ui.horizontalradioButton->isChecked())
         {
          if  (!generalatexsoportado_horizontal())
          return;
         }
       }
   int valor=imprimelatex2(nombrefichero());
   if (valor==1)
       QMessageBox::warning( this, tr("Libros de IVA"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Libros de IVA"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Libros de IVA"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());
*/
}


QString libro_recibidas::nombrefichero()
{
  QString cadena;
  if (!ui.prorratacheckBox->isChecked())
     cadena=tr("facturas_recibidas");
   else cadena=tr("facturas_recibidas_prorrata");
  return cadena;
}


void libro_recibidas::copiar()
{
   if (ui.prorratacheckBox->isChecked())
      {
       copiaprorrata();
       ui.progressBar->reset();
       return;
      }
   else
    {
     if (!ui.horizontalradioButton->isChecked())
      {
       copiavertical();
       ui.progressBar->reset();
       return;
      }

     if (ui.horizontalradioButton->isChecked())
      {
       if (ui.bicheckBox->isChecked()) copiahorizontal_bi();
         else copiahorizontal();
       ui.progressBar->reset();
       return;
      }
    }
}



bool libro_recibidas::generalatexsoportado_horizontal()
{
   if (!ui.interiorescheckBox->isChecked() && !ui.aibcheckBox->isChecked() &&
       !ui.autofacturascheckBox->isChecked()  && !ui.rectificativascheckBox->isChecked() &&
       !ui.autofacturasnouecheckBox->isChecked() && !ui.agrariocheckBox->isChecked() &&
       !ui.isp_op_interiorescheckBox->isChecked() && !ui.importacionescheckBox->isChecked())
       return false;

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=nombrefichero();
   qfichero+=".tex";
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return false;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{landscape}" << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    QString externo;
    if (ui.externo_groupBox->isChecked()) externo=ui.externo_lineEdit->text();

    if (!cuenta.isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\Large \\textbf {";
      stream << filtracad(cuenta) + " " + descripcioncuenta(cuenta) << "}}";
      stream << "\\end{center}";
     }

    if (!externo.isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\Large \\textbf {";
      stream << filtracad(externo) + " " + basedatos::instancia()->razon_externo(externo) << "}}";
      stream << "\\end{center}";
     }

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|r|p{2cm}|c|c|c|p{5cm}|l|r|r|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{12}{|c|} {\\textbf{";
   QString cadena=filtracad(ui.cabeceralineEdit->text());
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // -------------------------------------------------------------------------------------
   if (ordenrecepcion) stream << "{\\tiny{" << tr("Recep.") << "}} & ";
       else
          stream << "{\\tiny{" << tr("Orden") << "}} & ";
    stream << "{\\tiny{" << tr("Factura") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha Fra.") << "}} & ";
    stream << "{\\tiny{" << tr("Contab.") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
    stream << "{\\tiny{" << tr("Proveedor/acreedor") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Asto.") << "}} & ";
    stream << "{\\tiny{" << tr("Base Imp.") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}} & ";
    stream << "{\\tiny{" << tr("Cuota") << "}} & ";
    stream << "{\\tiny{" << tr("Total") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    // stream << tr(" \\\\") << "\n";
    if (ordenrecepcion) stream << "{\\tiny{" << tr("Recep.") << "}} & ";
        else
           stream << "{\\tiny{" << tr("Orden") << "}} & ";
    stream << "{\\tiny{" << tr("Factura") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha Fra.") << "}} & ";
    stream << "{\\tiny{" << tr("Contab.") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
    stream << "{\\tiny{" << tr("Proveedor/acreedor") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Asto.") << "}} & ";
    stream << "{\\tiny{" << tr("Base Imp.") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}} & ";
    stream << "{\\tiny{" << tr("Cuota") << "}} & ";
    stream << "{\\tiny{" << tr("Total") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ---------------------------------------------------------------------------------------

    QString ejercicio=ui.ejerciciocomboBox->currentText();
    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false, cuenta, externo);

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas(ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),cuenta,false,externo);

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    double base=0;
    double cuota=0;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                QString externo=query.value(20).toString();
                bool hayexterno=!externo.isEmpty();
                bool esautofactura=query.value(11).toBool();
	        ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0 
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++; 
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();               
                stream << "{\\tiny " <<
                        (ordenrecepcion ? query.value(15).toString() : cadnum.setNum(numorden))
                        << "} & {\\tiny ";
	        stream << filtracad(query.value(0).toString()) << "} & {\\tiny "; // fra.
	        stream << query.value(9).toDate().toString("dd.MM.yyyy") << "} & {\\tiny "; // fecha fra
	        stream << query.value(1).toDate().toString("dd.MM.yyyy") << "} & {\\tiny "; // fecha contable
                if (!esautofactura)
                   {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();

	            stream <<  query.value(2).toString() << "} & {\\tiny "; // cuenta
                    stream << (query.value(13).toString().isEmpty() ?
                               filtracad(razon) :
                               filtracad(query.value(13).toString()))
                            << "} & {\\tiny "; // proveedor
                    stream << ( query.value(14).toString().isEmpty() ?
                                nif :
                                filtracad(query.value(14).toString()))
                                << "} & {\\tiny "; // cif
                   }
                   else
                       {
	                stream <<  "} & {\\tiny "; // nada de cuenta
	                stream <<  filtracad(nombreempresa()) << "} & {\\tiny "; // proveedor
                        stream <<  basedatos::instancia()->cif() << "} & {\\tiny "; // cif
                       }

                stream <<  query.value(4).toString() << "} & {\\tiny "; // asiento
                stream <<  formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // base iva
                stream <<  formatea_redondeado_sep(query.value(6).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // tipo
                stream <<  formatea_redondeado_sep(query.value(7).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // cuota
                if (query.value(10).toBool() || query.value(11).toBool() || query.value(16).toBool() || query.value(22).toBool())
                    stream <<  formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                            << "\\hline\n"; // total
                  else
                    stream <<  formatea_redondeado_sep(query.value(8).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                       << "\\hline\n"; // total
                if (query.value(18).toBool())
                   {
                    // criterio de caja
                    QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(
                            query.value(19).toString());
                    // v.fecha_vencimiento, v.liq_iva_fecha, v.forzar_liq_iva, v.importe,
                    // v.num, v.medio_realizacion, v.cuenta_bancaria
                    bool vacio=true;
                    if (q.isActive())
                        while (q.next())
                         {
                          vacio=false;
                          stream << "\\multicolumn{3}{|r|} {\\tiny ";
                          stream << tr("Importe pago");
                          stream << "} & {\\tiny ";
                          stream << formatea_redondeado_sep(q.value(3).toDouble(),comadecimal,decimales);
                          stream << "} & \\multicolumn{2}{|l|} {\\tiny ";
                          stream << q.value(5).toString();
                          stream << "} & {\\tiny ";
                          stream << tr("FECHA:");
                          stream << "} & {\\tiny ";
                          stream << q.value(1).toDate().toString("dd-MM-yyyy");
                          stream << "} & {\\tiny ";
                          stream << tr("Cuenta:");
                          stream << "} & \\multicolumn{3}{|l|} {\\tiny ";
                          stream << q.value(6).toString();
                          stream << "} \\\\ \n  "
                                  << "\\hline\n";
                         }
                    if (vacio)
                       {
                        stream << "\\multicolumn{3}{|r|} {\\tiny ";
                        stream << tr("Importe pago");
                        stream << "} & {\\tiny ";
                        stream << "} & \\multicolumn{2}{|l|} {\\tiny ";
                        stream << "} & {\\tiny ";
                        stream << tr("FECHA:");
                        stream << "} & {\\tiny ";
                        stream << "} & {\\tiny ";
                        stream << tr("Cuenta:");
                        stream << "} & \\multicolumn{3}{|l|} {\\tiny ";
                        stream << "} \\\\ \n  "
                                << "\\hline\n";
                       }
                   }
                numprog++;
	        base+=query.value(5).toDouble();
	        cuota+=query.value(7).toDouble();
	      }
      }

    stream << "\\multicolumn{8}{|r|} {\\textbf{";
   // --------------------------------------------------------------------------------------
    stream << tr("SUMAS TOTALES:") << "}} ";
    stream << " & {\\tiny " << formatea_redondeado_sep(base,comadecimal,decimales) << "} & & {\\tiny ";
    stream << formatea_redondeado_sep(cuota,comadecimal,decimales) << "} &  {\\tiny ";
    stream << formatea_redondeado_sep(base+cuota,comadecimal,decimales);
    stream <<  "} \\\\ \n";
    stream << "\\hline" << "\n";


    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "\\end{landscape}\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

    ui.progressBar->reset();

    return true;

}


void libro_recibidas::copiavertical()
{

    QList<int> asientos_ret;
    QStringList imp_retenciones;
    QString ejercicio=ui.ejerciciocomboBox->currentText();
    QSqlQuery qr = basedatos::instancia()->pases_retenciones_ejercicio(ejercicio);
    if ( qr.isActive() ) {
        while ( qr.next() ) {
            asientos_ret << qr.value(1).toInt();
            imp_retenciones << formatea_redondeado(qr.value(2).toDouble(),false,decimales);
        }
    }

   QClipboard *cb = QApplication::clipboard();
   QString global;


   global=filtracad(nombreempresa()) + "\n" ;
   global+=ui.cabeceralineEdit->text();
   global+="\n";
   global+="\n";
   global+=(ordenrecepcion ? tr("Recep.") : tr("Orden"))+"\t";
   global+=tr("Factura")+"\t";
   global+=tr("Fecha Fra.")+"\t";
   global+=tr("Contab.")+"\t";
   global+=tr("CIF/NIF")+"\t";
   global+=tr("Proveedor/acreedor")+"\t";
    // tr("CIF/NIF") << "}} & ";
   global+=tr("Asto.")+"\t";
   global+=tr("Base Imp.")+"\t";
   global+=tr("Tipo")+"\t";
   global+=tr("Cuota")+"\t";
   if (ui.info_ret_checkBox->isChecked()) {
       global+=tr("Retención")+"\t";
   }
   global+=tr("Total")+"\t";
   global+="\n";

    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false,cuenta,ui.externo_lineEdit->text());

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(), cuenta, false, ui.externo_lineEdit->text());

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    double base=0;
    double cuota=0;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                QString externo=query.value(20).toString();
                bool hayexterno=!externo.isEmpty();
                bool esautofactura=query.value(11).toBool();
	        ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0 
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++; 
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                if (ordenrecepcion)
                    global+=query.value(15).toString()+"\t";
                  else
                    global+=cadnum.setNum(numorden)+"\t";
	        global+=query.value(0).toString()+"\t"; // fra.
            global+=query.value(9).toDate().toString("dd/MM/yyyy")+"\t"; // fecha fra
            global+=query.value(1).toDate().toString("dd/MM/yyyy")+"\t"; // fecha contable
                if (!esautofactura)
                  {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();
                   global +=  ( query.value(14).toString().isEmpty() ?
                                nif :
                                filtracad(query.value(14).toString()));
                   global +="\t";
                   // global+=query.value(2).toString()+"\t"; // cuenta
                   global+= (query.value(13).toString().isEmpty() ?
                             filtracad(razon) :
                             filtracad(query.value(13).toString()))
                             +"\t"; // proveedor
	           //if (aib || eib) stream <<  nifcomunitariocta(query.value(2).toString());
		   //else stream <<  cifcta(query.value(2).toString()) // cif
                  }
                  else
                      {
                       global+=  basedatos::instancia()->cif(); // cif
                       global+="\t"; // cuenta
	               global+=filtracad(nombreempresa())+"\t"; // proveedor
                      }
                global+=query.value(4).toString()+"\t"; // asiento
                global+=formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)+"\t"; // base iva
                global+=formatea_redondeado_sep(query.value(6).toDouble(),comadecimal,decimales)+"\t"; // tipo
                global+=formatea_redondeado_sep(query.value(7).toDouble(),comadecimal,decimales)+"\t"; // cuota
                if (ui.info_ret_checkBox->isChecked()) {
                    int pos=-1;
                    pos=asientos_ret.indexOf(query.value(4).toInt());
                    //qDebug() << QString().setNum(pos);
                    //qDebug() << QString().setNum(query.value(17).toInt());
                    if (pos>=0) global+= formatea_redondeado(convapunto(imp_retenciones.at(pos)).toDouble(),comadecimal,decimales) + "\t";
                      else global += "0\t";
                    if (pos>=0)
                       global +=  formatea_redondeado(query.value(8).toDouble()
                                                      -convapunto(imp_retenciones.at(pos)).toDouble(),comadecimal,decimales) + "\t"; // total
                      else
                        global +=  formatea_redondeado(query.value(8).toDouble(),comadecimal,decimales) + "\t"; // total

                }
                else {
                       if (query.value(10).toBool() || query.value(11).toBool() || query.value(16).toBool() || query.value(22).toBool())
                         global+=formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)+"\t"; // total
                       else
                           global+=formatea_redondeado_sep(query.value(8).toDouble(),comadecimal,decimales)+"\t"; // total
                }
                global+="\n";
                if (query.value(18).toBool())
                   {
                    // criterio de caja
                    QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(
                            query.value(19).toString());
                    // v.fecha_vencimiento, v.liq_iva_fecha, v.forzar_liq_iva, v.importe,
                    // v.num, v.medio_realizacion, v.cuenta_bancaria
                    bool vacio=true;
                    if (q.isActive())
                        while (q.next())
                         {
                          vacio=false;
                          global+= "\t\t";
                          global+= tr("Importe pago");
                          global+= "\t";
                          global+= formatea_redondeado_sep(q.value(3).toDouble(),comadecimal,decimales);
                          global+= "\t";
                          global+= tr("FECHA:");
                          global+= "\t";
                          global+= q.value(1).toDate().toString("dd-MM-yyyy");
                          global+= "\t";
                          global+= q.value(5).toString();
                          global+= "\t\t";
                          global+= tr("Cuenta:");
                          global+= "\t";
                          global+= q.value(6).toString();
                          global+= "\n  ";
                         }
                    if (vacio)
                       {
                        global+= "\t\t";
                        global+= tr("Importe pago");
                        global+= "\t";
                        global+= "\t";
                        global+= tr("FECHA:");
                        global+= "\t";
                        global+= "\t";
                        global+= "\t\t";
                        global+= tr("Cuenta:");
                        global+= "\t";
                        global+= "\n  ";
                       }
                   }
                numprog++;
	        base+=query.value(5).toDouble();
	        cuota+=query.value(7).toDouble();
	      }
      }

   // --------------------------------------------------------------------------------------
    global+="\t\t\t\t\t\t"+tr("SUMAS TOTALES:")+"\t";
    global+=formatea_redondeado_sep(base,comadecimal,decimales) + "\t\t";
    global+=formatea_redondeado_sep(cuota,comadecimal,decimales) + "\t";
    // global+=formatea_redondeado_sep(base+cuota,comadecimal,decimales);
    global+="\n";

   cb->setText(global);
   QMessageBox::information( this, tr("Libro de Facturas"),
                            tr("Se ha pasado el contenido al portapapeles") );
}


void libro_recibidas::copiahorizontal()
{
   QClipboard *cb = QApplication::clipboard();
   QString global;


   global=filtracad(nombreempresa()) + "\n" ;
   global+=ui.cabeceralineEdit->text();
   global+="\n";
   global+="\n";
   global+=(ordenrecepcion ? tr("Recep.") : tr("Orden"))+"\t";
   global+=tr("Factura")+"\t";
   global+=tr("Fecha Fra.")+"\t";
   global+=tr("Contab.")+"\t";
   global+=tr("Cuenta")+"\t";
   global+=tr("Proveedor/acreedor")+"\t";
   global+=tr("CIF/NIF")+"\t";
   global+=tr("Asto.")+"\t";
   global+=tr("Base Imp.")+"\t";
   global+=tr("Tipo")+"\t";
   global+=tr("Cuota")+"\t";
   global+=tr("Total")+"\t";
   global+="\n";

    QString ejercicio=ui.ejerciciocomboBox->currentText();
    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false, cuenta, ui.externo_lineEdit->text());

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas(ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(), cuenta,false, ui.externo_lineEdit->text());

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    double base=0;
    double cuota=0;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                QString externo=query.value(20).toString();
                bool hayexterno=!externo.isEmpty();
                bool esautofactura= query.value(11).toBool();
	        ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0 
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++; 
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                if (ordenrecepcion) global+=query.value(15).toString()+"\t";
                    else
                      global+=cadnum.setNum(numorden)+"\t";
	        global+=query.value(0).toString()+"\t"; // fra.
            global+=query.value(9).toDate().toString("dd/MM/yyyy")+"\t"; // fecha fra
            global+=query.value(1).toDate().toString("dd/MM/yyyy")+"\t"; // fecha contable
                if (!esautofactura)
                   {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();
                    global+=query.value(2).toString()+"\t"; // cuenta
                    global+= (query.value(13).toString().isEmpty() ?
                              filtracad(razon) :
                              filtracad(query.value(13).toString()))
                              +"\t"; // proveedor
                    global+=(query.value(14).toString().isEmpty() ?
                             nif :
                             query.value(14).toString()); // cif
                   }
                   else
                      {
	               global+="\t"; // cuenta
	               global+=filtracad(nombreempresa())+"\t"; // proveedor
                       global+=basedatos::instancia()->cif();
                      }

                global+="\t";
                global+=query.value(4).toString()+"\t"; // asiento
                global+=formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)+"\t"; // base iva
                global+=formatea_redondeado_sep(query.value(6).toDouble(),comadecimal,decimales)+"\t"; // tipo
                global+=formatea_redondeado_sep(query.value(7).toDouble(),comadecimal,decimales)+"\t"; // cuota
                if (query.value(10).toBool() || query.value(11).toBool() || query.value(16).toBool() || query.value(22).toBool())
                   global+=formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)+"\t"; // total
                 else
                    global+=formatea_redondeado_sep(query.value(8).toDouble(),comadecimal,decimales)+"\t"; // total
                global+="\n";
                if (query.value(18).toBool())
                   {
                    // criterio de caja
                    QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(
                            query.value(19).toString());
                    // v.fecha_vencimiento, v.liq_iva_fecha, v.forzar_liq_iva, v.importe,
                    // v.num, v.medio_realizacion, v.cuenta_bancaria
                    bool vacio=true;
                    if (q.isActive())
                        while (q.next())
                         {
                          vacio=false;
                          global+= "\t\t";
                          global+= tr("Importe pago");
                          global+= "\t";
                          global+= formatea_redondeado_sep(q.value(3).toDouble(),comadecimal,decimales);
                          global+= "\t";
                          global+= tr("FECHA:");
                          global+= "\t";
                          global+= q.value(1).toDate().toString("dd-MM-yyyy");
                          global+= "\t";
                          global+= q.value(5).toString();
                          global+= "\t\t";
                          global+= tr("Cuenta:");
                          global+= "\t";
                          global+= q.value(6).toString();
                          global+= "\n  ";
                         }
                    if (vacio)
                       {
                        global+= "\t\t";
                        global+= tr("Importe pago");
                        global+= "\t";
                        global+= "\t";
                        global+= tr("FECHA:");
                        global+= "\t";
                        global+= "\t";
                        global+= "\t\t";
                        global+= tr("Cuenta:");
                        global+= "\t";
                        global+= "\n  ";
                       }
                   }
                numprog++;
	        base+=query.value(5).toDouble();
	        cuota+=query.value(7).toDouble();
	      }
      }

   // --------------------------------------------------------------------------------------
    global+="\t\t\t\t\t\t\t"+tr("SUMAS TOTALES:")+"\t";
    global+=formatea_redondeado_sep(base,comadecimal,decimales) + "\t\t";
    global+=formatea_redondeado_sep(cuota,comadecimal,decimales) + "\t";
    global+=formatea_redondeado_sep(base+cuota,comadecimal,decimales);
    global+="\n";

   cb->setText(global);
   QMessageBox::information( this, tr("Libro de Facturas"),
                            tr("Se ha pasado el contenido al portapapeles") );
}


void libro_recibidas::informe() {
    QString ejercicio=ui.ejerciciocomboBox->currentText();
    QList<int> asientos_ret;
    QStringList imp_retenciones;
    QSqlQuery qr = basedatos::instancia()->pases_retenciones_ejercicio(ejercicio);
    if ( qr.isActive() ) {
        while ( qr.next() ) {
            asientos_ret << qr.value(1).toInt();
            imp_retenciones << formatea_redondeado(qr.value(2).toDouble(),false,decimales);
        }
    }


    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    QString externo;
    if (ui.externo_groupBox->isChecked()) externo=ui.externo_lineEdit->text();

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas_prorrata(ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false, cuenta, externo);

    QStringList lorden, lapunte;
    QStringList lfactura,lfecha_fra, lfecha_contable, lcuenta, lproveedor, lnif, lasiento;
    QStringList lbase, ltipo, lcuota, ltotal, lprorrata, lafecto, lcuota_corregida,lret;
    QList<bool> lcaja;
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    bool criterio_caja=false;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                QString externo=query.value(23).toString();
                bool hayexterno=!externo.isEmpty();
                bool esautofactura=query.value(13).toBool();
                //ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++;
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                lorden <<  (ordenrecepcion ? query.value(18).toString() : cadnum.setNum(numorden));
                lfactura << query.value(0).toString(); // fra.
                lfecha_fra << query.value(11).toDate().toString("dd-MM-yyyy"); // fecha fra
                lfecha_contable << query.value(1).toDate().toString("dd-MM-yyyy"); // fecha contable
                if (!esautofactura)
                   {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();
                    lcuenta <<  query.value(2).toString(); // cuenta
                    lproveedor << (query.value(15).toString().isEmpty() ?
                               filtracad(razon) :
                               filtracad(query.value(15).toString())); // proveedor
                    lnif << (query.value(16).toString().isEmpty() ?
                               nif :
                               filtracad(query.value(16).toString())); // cif
                   }
                   else
                    {
                     lcuenta << "";
                     lproveedor <<  filtracad(nombreempresa()); // proveedor
                     lnif <<  basedatos::instancia()->cif(); // cif
                    }
                lasiento <<  query.value(4).toString(); // asiento
                lapunte << query.value(17).toString(); // apunte
                lbase << formatea_redondeado(query.value(5).toDouble(),false,decimales); // base iva
                ltipo <<  formatea_redondeado(query.value(6).toDouble(),false,decimales); // tipo
                lcuota <<  formatea_redondeado(query.value(7).toDouble(),false,decimales); // cuota
                lprorrata <<  formatea_redondeado(query.value(9).toDouble()*100,false,decimales); // prorrata
                lafecto <<  formatea_redondeado(query.value(14).toDouble()*100,false,decimales); // afecto
                lcuota_corregida <<  formatea_redondeado(query.value(10).toDouble(),false,decimales); // cuota corregida prorrata
                lcaja << (query.value(21).toBool() ? true : false); // criterio de caja
                int pos=-1;
                pos=asientos_ret.indexOf(query.value(4).toInt());
                //qDebug() << QString().setNum(pos);
                //qDebug() << QString().setNum(query.value(17).toInt());
                if (pos>=0) lret << imp_retenciones.at(pos);
                  else lret <<"0";
                if (pos>=0 && ui.info_ret_checkBox->isChecked())
                   ltotal <<  formatea_redondeado(query.value(8).toDouble()
                                                  -convapunto(imp_retenciones.at(pos)).toDouble(),false,decimales); // total
                  else {
                         if (query.value(12).toBool() || query.value(13).toBool() || query.value(19).toBool() || query.value(26).toBool())
                           ltotal <<  formatea_redondeado(query.value(5).toDouble(),false,decimales); // total
                         else
                           ltotal <<  formatea_redondeado(query.value(8).toDouble(),false,decimales); // total
                  }
                if (query.value(21).toBool()) criterio_caja=true;
                numprog++;
             }
      } // query active

    QString fileName = ":/informes/facturas_recibidas_afecto.xml";
    if (criterio_caja) fileName = ":/informes/facturas_recibidas_afecto_caja.xml";
    if (!ui.prorratacheckBox->isChecked()) {
        fileName = ":/informes/facturas_recibidas.xml";
        if (criterio_caja) fileName=":/informes/facturas_recibidas_caja.xml";
    }
    if (ui.info_ret_checkBox->isChecked()) fileName=":/informes/facturas_recibidas_ret.xml";

    QtRPT *report = new QtRPT(this);
    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("Imprimir diario"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }

    QObject::connect(report, &QtRPT::setDSInfo,
                     [&](DataSetInfo &dsInfo) {
        dsInfo.recordCount=lorden.count();;
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
        Q_UNUSED (reportPage);

        if (paramName == "empresa") paramValue = basedatos::instancia()->selectEmpresaconfiguracion();
        if (paramName == "cabecera") paramValue = ui.cabeceralineEdit->text();

        if (paramName == "orden") paramValue = lorden.at(recNo);
        if (paramName == "factura") paramValue = lfactura.at(recNo);
        if (paramName == "fecha_fac") paramValue = lfecha_fra.at(recNo);
        if (paramName == "fecha_cont") paramValue = lfecha_contable.at(recNo);
        if (paramName == "nombre") paramValue = lproveedor.at(recNo);
        if (paramName == "nif") paramValue = lnif.at(recNo);
        if (paramName == "asiento") paramValue = lasiento.at(recNo);
        if (paramName == "base") paramValue = lbase.at(recNo);
        if (paramName == "tipo") paramValue = ltipo.at(recNo);
        if (paramName == "cuota") paramValue = lcuota.at(recNo);
        if (paramName == "total") paramValue = ltotal.at(recNo);
        if (paramName == "prorrata") paramValue = lprorrata.at(recNo);
        if (paramName == "afecto") paramValue = lafecto.at(recNo);
        if (paramName == "cuota_efectiva") paramValue = lcuota_corregida.at(recNo);
        if (paramName == "ret") paramValue = lret.at(recNo);
        if (lcaja.at(recNo)) {
            QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(
                    lapunte.at(recNo));
            // v.fecha_vencimiento, v.liq_iva_fecha, v.forzar_liq_iva, v.importe,
            // v.num, v.medio_realizacion, v.cuenta_bancaria
            bool vacio=true;
            QString mensaje;
            //bool primero=true;
            //bool segundo=false;
            if (q.isActive())
                while (q.next())
                 {
                  //if (!primero && ! segundo) mensaje+=" *** ";
                  //primero=false;
                  vacio=false;
                  mensaje+= tr("FECHA:") + q.value(1).toDate().toString("dd-MM-yyyy");
                  mensaje+= "\t" + q.value(5).toString();
                  mensaje+= "\t" + tr("Cuenta:") + q.value(6).toString();
                  mensaje+= "\t" + tr("Importe pago:");
                  mensaje+= formatea_redondeado_sep(q.value(3).toDouble(),comadecimal,decimales);
                  mensaje+="\n";
                  //if (segundo)
                  //  { mensaje+="\n"; segundo=false;}
                  //if(!segundo) segundo=true;
                 }
                  if (vacio)
                   {
                          mensaje+= tr("FECHA:  ") ;
                          mensaje+= "  " ;
                          mensaje+= "  " + tr("Cuenta: ");
                          mensaje+= "     " + tr("Importe pago:  ");
                          //mensaje+="\n";
                   }
             if (paramName == "caja_descrip") paramValue=mensaje;
            }
    });
    // report->printExec();
    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+(tr("facturas_recibidas.pdf"));
           report->printPDF(qfichero);
    }

}


bool libro_recibidas::generalatexprorrata()
{
   if (!ui.interiorescheckBox->isChecked() && !ui.aibcheckBox->isChecked() &&
       !ui.autofacturascheckBox->isChecked() && !ui.rectificativascheckBox->isChecked()
       && !ui.autofacturasnouecheckBox->isChecked()
       && !ui.agrariocheckBox->isChecked()
       && !ui.isp_op_interiorescheckBox->isChecked()
       && !ui.importacionescheckBox->isChecked()) return false;

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=nombrefichero();
   qfichero+=".tex";
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return false;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{landscape}" << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();
    QString externo;
    if (ui.externo_groupBox->isChecked()) externo=ui.externo_lineEdit->text();

    if (!cuenta.isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\Large \\textbf {";
      stream << filtracad(cuenta) + " " + descripcioncuenta(cuenta) << "}}";
      stream << "\\end{center}";
     }

    if (!externo.isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\Large \\textbf {";
      stream << filtracad(externo) + " " + basedatos::instancia()->razon_externo(externo) << "}}";
      stream << "\\end{center}";
     }

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|r|p{2cm}|c|c|c|p{3.5cm}|l|r|r|r|r|r|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{15}{|c|} {\\textbf{";
   QString cadena=filtracad(ui.cabeceralineEdit->text());
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // -------------------------------------------------------------------------------------
    if (ordenrecepcion) stream << "{\\tiny{" << tr("Recep.") << "}} & ";
     else
         stream << "{\\tiny{" << tr("Orden") << "}} & ";
    stream << "{\\tiny{" << tr("Factura") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha Fra.") << "}} & ";
    stream << "{\\tiny{" << tr("Contab.") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
    stream << "{\\tiny{" << tr("Proveedor/acreedor") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Asto.") << "}} & ";
    stream << "{\\tiny{" << tr("Base Imp.") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}} & ";
    stream << "{\\tiny{" << tr("Cuota") << "}} & ";
    stream << "{\\tiny{" << tr("Total") << "}} &";
    stream << "{\\tiny{" << tr("Prorrata") << "}} &";
    stream << "{\\tiny{" << tr("Afectación") << "}} &";
    stream << "{\\tiny{" << tr("Cuota ef.") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    // stream << tr(" \\\\") << "\n";
    if (ordenrecepcion) stream << "{\\tiny{" << tr("Recep.") << "}} & ";
       else
          stream << "{\\tiny{" << tr("Orden") << "}} & ";
    stream << "{\\tiny{" << tr("Factura") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha Fra.") << "}} & ";
    stream << "{\\tiny{" << tr("Contab.") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
    stream << "{\\tiny{" << tr("Proveedor/acreedor") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Asto.") << "}} & ";
    stream << "{\\tiny{" << tr("Base Imp.") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}} & ";
    stream << "{\\tiny{" << tr("Cuota") << "}} & ";
    stream << "{\\tiny{" << tr("Total") << "}} &";
    stream << "{\\tiny{" << tr("Prorrata") << "}} &";
    stream << "{\\tiny{" << tr("Afectación") << "}} &";
    stream << "{\\tiny{" << tr("Cuota ef.") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ---------------------------------------------------------------------------------------


    QString ejercicio=ui.ejerciciocomboBox->currentText();
    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false, cuenta, externo);

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas_prorrata(ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false, cuenta,externo);

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    double base=0;
    double cuota=0;
    double cuotaefectiva=0;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                QString externo=query.value(23).toString();
                bool hayexterno=!externo.isEmpty();
                bool esautofactura=query.value(13).toBool();
	        ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0 
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++; 
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                stream << "{\\tiny " <<
                        (ordenrecepcion ? query.value(18).toString() : cadnum.setNum(numorden))
                        << "} & {\\tiny ";
	        stream << filtracad(query.value(0).toString()) << "} & {\\tiny "; // fra.
	        stream << query.value(11).toDate().toString("dd.MM.yyyy") << "} & {\\tiny "; // fecha fra
	        stream << query.value(1).toDate().toString("dd.MM.yyyy") << "} & {\\tiny "; // fecha contable
                if (!esautofactura)
                   {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();
                    stream <<  query.value(2).toString() << "} & {\\tiny "; // cuenta
                    stream << (query.value(15).toString().isEmpty() ?
                               filtracad(razon) :
                               filtracad(query.value(15).toString()))
                               << "} & {\\tiny "; // proveedor
                    stream << (query.value(16).toString().isEmpty() ?
                               nif :
                               filtracad(query.value(16).toString()))
                           << "} & {\\tiny "; // cif
                   }
                   else
                    {
	             stream << "} & {\\tiny "; // cuenta
	             stream <<  filtracad(nombreempresa()) << "} & {\\tiny "; // proveedor
                     stream <<  basedatos::instancia()->cif() << "} & {\\tiny "; // cif
                    }
                stream <<  query.value(4).toString() << "} & {\\tiny "; // asiento
                stream <<  formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // base iva
                stream <<  formatea_redondeado_sep(query.value(6).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // tipo
                stream <<  formatea_redondeado_sep(query.value(7).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // cuota
                if (query.value(12).toBool() || query.value(13).toBool() || query.value(19).toBool() || query.value(26).toBool())
                      stream <<  formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)
                          << "} & {\\tiny "; // total
                   else
                        stream <<  formatea_redondeado_sep(query.value(8).toDouble(),comadecimal,decimales)
                          << "} & {\\tiny "; // total
                stream <<  formatea_redondeado_sep(query.value(9).toDouble()*100,comadecimal,decimales)
                       << "} & {\\tiny "; // prorrata
                stream <<  formatea_redondeado_sep(query.value(14).toDouble()*100,comadecimal,decimales)
                       << "} & {\\tiny "; // afecto
                stream <<  formatea_redondeado_sep(query.value(10).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                       << "\\hline\n"; // cuota corregida prorrata
                if (query.value(21).toBool())
                   {
                    // criterio de caja
                    QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(
                            query.value(17).toString());
                    // v.fecha_vencimiento, v.liq_iva_fecha, v.forzar_liq_iva, v.importe,
                    // v.num, v.medio_realizacion, v.cuenta_bancaria
                    bool vacio=true;
                    if (q.isActive())
                        while (q.next())
                         {
                          vacio=false;
                          stream << "\\multicolumn{3}{|r|} {\\tiny ";
                          stream << tr("Importe pago");
                          stream << "} & {\\tiny ";
                          stream << formatea_redondeado_sep(q.value(3).toDouble(),comadecimal,decimales);
                          stream << "} & \\multicolumn{2}{|l|} {\\tiny ";
                          stream << q.value(5).toString();
                          stream << "} & {\\tiny ";
                          stream << tr("FECHA:");
                          stream << "} & {\\tiny ";
                          stream << q.value(1).toDate().toString("dd-MM-yyyy");
                          stream << "} & {\\tiny ";
                          stream << tr("Cuenta:");
                          stream << "} & \\multicolumn{3}{|l|} {\\tiny ";
                          stream << q.value(6).toString();
                          stream << "} & \\multicolumn{3}{|l|} {\\tiny ";
                          stream << "} \\\\ \n  "
                                  << "\\hline\n";
                         }
                    if (vacio)
                       {
                        stream << "\\multicolumn{3}{|r|} {\\tiny ";
                        stream << tr("Importe pago");
                        stream << "} & {\\tiny ";
                        stream << "} & \\multicolumn{2}{|l|} {\\tiny ";
                        stream << "} & {\\tiny ";
                        stream << tr("FECHA:");
                        stream << "} & {\\tiny ";
                        stream << "} & {\\tiny ";
                        stream << tr("Cuenta:");
                        stream << "} & \\multicolumn{3}{|l|} {\\tiny ";
                        stream << "} & \\multicolumn{3}{|l|} {\\tiny ";
                        stream << "} \\\\ \n  "
                                << "\\hline\n";
                       }
                   }
                numprog++;
	        base+=query.value(5).toDouble();
	        cuota+=query.value(7).toDouble();
                cuotaefectiva+=query.value(10).toDouble();
	      }
      }

    stream << "\\multicolumn{8}{|r|} {\\textbf{";
   // --------------------------------------------------------------------------------------
    stream << tr("SUMAS TOTALES:") << "}} ";
    stream << " & {\\tiny " << formatea_redondeado_sep(base,comadecimal,decimales) << "} & & {\\tiny ";
    stream << formatea_redondeado_sep(cuota,comadecimal,decimales) << "} &  {\\tiny ";
    stream << formatea_redondeado_sep(base+cuota,comadecimal,decimales) << "} & & & {\\tiny ";
    stream << formatea_redondeado_sep(cuotaefectiva,comadecimal,decimales);
    stream <<  "} \\\\ \n";
    stream << "\\hline" << "\n";


    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "\\end{landscape}\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

    ui.progressBar->reset();

    return true;


}


void libro_recibidas::copiaprorrata()
{
   QClipboard *cb = QApplication::clipboard();
   QString global;

   global=filtracad(nombreempresa());
   global+="\n\n";
   QString cadena=ui.cabeceralineEdit->text();
   // --------------------------------------------------------------------------------------
   global+=cadena;
   global+="\n\n";
    // -------------------------------------------------------------------------------------
    if (ordenrecepcion) global+=tr("Orden")+"\t";
       else global+=tr("Recep.")+"\t";
    global+=tr("Factura")+"\t";
    global+=tr("Fecha Fra.")+"\t";
    global+=tr("Contab.")+"\t";
    global+=tr("Cuenta")+"\t";
    global+=tr("Proveedor/acreedor")+"\t";
    global+=tr("CIF/NIF")+"\t";
    global+=tr("Asto.")+"\t";
    global+=tr("Base Imp.")+"\t";
    global+=tr("Tipo")+"\t";
    global+=tr("Cuota")+"\t";
    global+=tr("Total")+"\t";
    global+=tr("Prorrata")+"\t";
    global+=tr("Afectación")+"\t";
    global+=tr("Cuota ef.")+"\n";

    QString ejercicio=ui.ejerciciocomboBox->currentText();
    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false, cuenta,ui.externo_lineEdit->text());

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas_prorrata(ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false, cuenta, ui.externo_lineEdit->text());

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    double base=0;
    double cuota=0;
    double cuotaefectiva=0;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                bool esautofactura= query.value(13).toBool();
                QString externo=query.value(23).toString();
                bool hayexterno=!externo.isEmpty();
                ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0 
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++; 
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                global+=(ordenrecepcion ? query.value(18).toString() : cadnum.setNum(numorden))+ "\t";
	        global+=filtracad(query.value(0).toString()) + "\t"; // fra.
	        global+=query.value(11).toDate().toString("dd.MM.yyyy") + "\t"; // fecha fra
	        global+=query.value(1).toDate().toString("dd.MM.yyyy") + "\t"; // fecha contable
                if (!esautofactura)
                  {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();
                   global+=query.value(2).toString() + "\t"; // cuenta
                   global+= ( query.value(15).toString().isEmpty() ?
                              filtracad(razon) :
                              filtracad(query.value(15).toString()) ) + "\t"; // proveedor
                   global+=( query.value(16).toString().isEmpty() ?
                             nif :
                             filtracad(query.value(16).toString()) )+ "\t"; // cif
                  }
                  else
                     {
	              global+="\t"; // cuenta
	              global+=filtracad(nombreempresa()) + "\t"; // proveedor
                      global+= basedatos::instancia()->cif() + "\t"; // cif
                      }
                global+=query.value(4).toString() + "\t"; // asiento
                global+=formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)+"\t"; // base iva
                global+=formatea_redondeado_sep(query.value(6).toDouble(),comadecimal,decimales)+"\t"; // tipo
                global+=formatea_redondeado_sep(query.value(7).toDouble(),comadecimal,decimales)+"\t"; // cuota
                if (query.value(12).toBool() || query.value(13).toBool() || query.value(19).toBool() || query.value(26).toBool())
                   global+=formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)+"\t"; // total
                 else
                    global+=formatea_redondeado_sep(query.value(8).toDouble(),comadecimal,decimales)+"\t"; // total
                global+=formatea_redondeado_sep(query.value(9).toDouble(),comadecimal,decimales)+"\t"; // prorrata
                global+=formatea_redondeado_sep(query.value(14).toDouble(),comadecimal,decimales)+"\t"; // prorrata
                global+=formatea_redondeado_sep(query.value(10).toDouble(),comadecimal,decimales)+"\n"; // cuota corregida prorrata
                if (query.value(21).toBool())
                   {
                    // criterio de caja
                    QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(
                            query.value(17).toString());
                    // v.fecha_vencimiento, v.liq_iva_fecha, v.forzar_liq_iva, v.importe,
                    // v.num, v.medio_realizacion, v.cuenta_bancaria
                    bool vacio=true;
                    if (q.isActive())
                        while (q.next())
                         {
                          vacio=false;
                          global+= "\t\t";
                          global+= tr("Importe pago");
                          global+= "\t";
                          global+= formatea_redondeado_sep(q.value(3).toDouble(),comadecimal,decimales);
                          global+= "\t";
                          global+= tr("FECHA:");
                          global+= "\t";
                          global+= q.value(1).toDate().toString("dd-MM-yyyy");
                          global+= "\t";
                          global+= q.value(5).toString();
                          global+= "\t\t";
                          global+= tr("Cuenta:");
                          global+= "\t";
                          global+= q.value(6).toString();
                          global+= "\n  ";
                         }
                    if (vacio)
                       {
                        global+= "\t\t";
                        global+= tr("Importe pago");
                        global+= "\t";
                        global+= "\t";
                        global+= tr("FECHA:");
                        global+= "\t";
                        global+= "\t";
                        global+= "\t\t";
                        global+= tr("Cuenta:");
                        global+= "\t";
                        global+= "\n  ";
                       }
                   }
                numprog++;
	        base+=query.value(5).toDouble();
	        cuota+=query.value(7).toDouble();
                cuotaefectiva+=query.value(10).toDouble();
	      }
      }

    global+= "\t\t\t\t\t\t\t";
   // --------------------------------------------------------------------------------------
    global+=tr("SUMAS TOTALES:") + "\t";
    global+=formatea_redondeado_sep(base,comadecimal,decimales) + "\t\t";
    global+=formatea_redondeado_sep(cuota,comadecimal,decimales) + "\t";
    global+=formatea_redondeado_sep(base+cuota,comadecimal,decimales) + "\t\t\t";
    global+=formatea_redondeado_sep(cuotaefectiva,comadecimal,decimales);
    global+="\n";


   cb->setText(global);
   QMessageBox::information( this, tr("Libro de Facturas"),
                            tr("Se ha pasado el contenido al portapapeles") );

}



bool libro_recibidas::generalatexsoportado_horizontal_bi()
{
   if (!ui.interiorescheckBox->isChecked() && !ui.aibcheckBox->isChecked() &&
       !ui.autofacturascheckBox->isChecked()  && !ui.rectificativascheckBox->isChecked() &&
       !ui.autofacturasnouecheckBox->isChecked() && !ui.agrariocheckBox->isChecked() &&
       !ui.isp_op_interiorescheckBox->isChecked() && !ui.importacionescheckBox->isChecked())
       return false;

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=nombrefichero();
   qfichero+=".tex";
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return false;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{landscape}" << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();
    QString externo;
    if (ui.externo_groupBox->isChecked()) externo=ui.externo_lineEdit->text();

    if (!cuenta.isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\Large \\textbf {";
      stream << filtracad(cuenta) + " " + descripcioncuenta(cuenta) << "}}";
      stream << "\\end{center}";
     }


    if (!externo.isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\Large \\textbf {";
      stream << filtracad(externo) + " " + basedatos::instancia()->razon_externo(externo) << "}}";
      stream << "\\end{center}";
     }

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|r|p{2.5cm}|c|c|c|p{3.5cm}|l|p{3.5cm}|r|r|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{13}{|c|} {\\textbf{";
   QString cadena=filtracad(ui.cabeceralineEdit->text());
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // -------------------------------------------------------------------------------------
   if (ordenrecepcion)
    stream << "{\\tiny{" << tr("Recep.") << "}} & ";
    else
        stream << "{\\tiny{" << tr("Orden") << "}} & ";
    stream << "{\\tiny{" << tr("Factura") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha Fra.") << "}} & ";
    stream << "{\\tiny{" << tr("Contab.") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
    stream << "{\\tiny{" << tr("Proveedor/acreedor") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("Asto.") << "}} & ";
    stream << "{\\tiny{" << tr("Base Imp.") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}} & ";
    stream << "{\\tiny{" << tr("Cuota") << "}} & ";
    stream << "{\\tiny{" << tr("Total") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    // stream << tr(" \\\\") << "\n";
    if (ordenrecepcion)
      stream << "{\\tiny{" << tr("Recep.") << "}} & ";
      else
         stream << "{\\tiny{" << tr("Orden") << "}} & ";
    stream << "{\\tiny{" << tr("Factura") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha Fra.") << "}} & ";
    stream << "{\\tiny{" << tr("Contab.") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
    stream << "{\\tiny{" << tr("Proveedor/acreedor") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("Asto.") << "}} & ";
    stream << "{\\tiny{" << tr("Base Imp.") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}} & ";
    stream << "{\\tiny{" << tr("Cuota") << "}} & ";
    stream << "{\\tiny{" << tr("Total") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ---------------------------------------------------------------------------------------

    QString ejercicio=ui.ejerciciocomboBox->currentText();
    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false,cuenta,externo);

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas(ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(), cuenta, false, externo);

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    double base=0;
    double cuota=0;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                QString externo=query.value(20).toString();
                bool hayexterno=!externo.isEmpty();
                bool esautofactura= query.value(11).toBool();
                ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++;
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                stream << "{\\tiny " <<
                        (ordenrecepcion ? query.value(15).toString():cadnum.setNum(numorden))
                        << "} & {\\tiny ";
                stream << filtracad(query.value(0).toString()) << "} & {\\tiny "; // fra.
                stream << query.value(9).toDate().toString("dd.MM.yyyy") << "} & {\\tiny "; // fecha fra
                stream << query.value(1).toDate().toString("dd.MM.yyyy") << "} & {\\tiny "; // fecha contable
                if (!esautofactura)
                   {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();
                    stream <<  query.value(2).toString() << "} & {\\tiny "; // cuenta
                    stream << (query.value(13).toString().isEmpty() ?
                               filtracad(razon) :
                               filtracad(query.value(13).toString()))
                            << "} & {\\tiny "; // proveedor
                    stream << (query.value(14).toString().isEmpty() ?
                               nif :
                               filtracad(query.value(14).toString()))
                            << "} & {\\tiny "; // cif
                   }
                   else
                       {
                        stream <<  "} & {\\tiny "; // nada de cuenta
                        stream <<  filtracad(nombreempresa()) << "} & {\\tiny "; // proveedor
                        stream <<  basedatos::instancia()->cif() << "} & {\\tiny "; // cif
                       }

                stream << filtracad(descripcioncuenta(query.value(12).toString()))
                        << "} & {\\tiny "; // descripción cuenta gasto

                stream <<  query.value(4).toString() << "} & {\\tiny "; // asiento
                stream <<  formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // base iva
                stream <<  formatea_redondeado_sep(query.value(6).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // tipo
                stream <<  formatea_redondeado_sep(query.value(7).toDouble(),comadecimal,decimales)
                       << "} & {\\tiny "; // cuota
                if (query.value(10).toBool() || query.value(11).toBool() || query.value(16).toBool() || query.value(22).toBool())
                    stream <<  formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                            << "\\hline\n"; // total
                   else
                     stream <<  formatea_redondeado_sep(query.value(8).toDouble(),comadecimal,decimales) << "} \\\\ \n  "
                       << "\\hline\n"; // total
                numprog++;
                base+=query.value(5).toDouble();
                cuota+=query.value(7).toDouble();
                if (query.value(18).toBool())
                   {
                    // criterio de caja
                    QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(
                            query.value(19).toString());
                    // v.fecha_vencimiento, v.liq_iva_fecha, v.forzar_liq_iva, v.importe,
                    // v.num, v.medio_realizacion, v.cuenta_bancaria

                   }
              }
      }

    stream << "\\multicolumn{9}{|r|} {\\textbf{";
   // --------------------------------------------------------------------------------------
    stream << tr("SUMAS TOTALES:") << "}} ";
    stream << " & {\\tiny " << formatea_redondeado_sep(base,comadecimal,decimales) << "} & & {\\tiny ";
    stream << formatea_redondeado_sep(cuota,comadecimal,decimales) << "} &  {\\tiny ";
    stream << formatea_redondeado_sep(base+cuota,comadecimal,decimales);
    stream <<  "} \\\\ \n";
    stream << "\\hline" << "\n";


    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "\\end{landscape}\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

    ui.progressBar->reset();

    return true;

}


void libro_recibidas::copiahorizontal_bi()
{
   QClipboard *cb = QApplication::clipboard();
   QString global;


   global=filtracad(nombreempresa()) + "\n" ;
   global+=ui.cabeceralineEdit->text();
   global+="\n";
   global+="\n";
   global+=(ordenrecepcion ? tr("Recep.") : tr("Orden"))+"\t";
   global+=tr("Factura")+"\t";
   global+=tr("Fecha Fra.")+"\t";
   global+=tr("Contab.")+"\t";
   global+=tr("Cuenta")+"\t";
   global+=tr("Proveedor/acreedor")+"\t";
   global+=tr("CIF/NIF")+"\t";
   global+=tr("Concepto")+"\t";
   global+=tr("Asto.")+"\t";
   global+=tr("Base Imp.")+"\t";
   global+=tr("Tipo")+"\t";
   global+=tr("Cuota")+"\t";
   global+=tr("Total")+"\t";
   global+="\n";

    QString ejercicio=ui.ejerciciocomboBox->currentText();
    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false, cuenta, ui.externo_lineEdit->text());

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas(ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(), cuenta, false, ui.externo_lineEdit->text());

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    double base=0;
    double cuota=0;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                bool esautofactura=query.value(11).toBool();
                QString externo=query.value(20).toString();
                bool hayexterno=!externo.isEmpty();
                ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0
                    || numprog==1 || guardaprov!=query.value(2).toString()) numorden++;
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                global+=(ordenrecepcion ? query.value(15).toString() : cadnum.setNum(numorden))+"\t";
                global+=query.value(0).toString()+"\t"; // fra.
                global+=query.value(9).toDate().toString("dd.MM.yyyy")+"\t"; // fecha fra
                global+=query.value(1).toDate().toString("dd.MM.yyyy")+"\t"; // fecha contable
                if (!esautofactura)
                   {
                    QString razon,nif;
                    if (!hayexterno)
                      basedatos::instancia()->razon_nif_datos(query.value(2).toString(), &razon, &nif);
                     else
                         {
                          razon=basedatos::instancia()->razon_externo(externo);
                          nif=basedatos::instancia()->cif_externo(externo);
                         }
                    if (razon.trimmed().isEmpty()) razon=query.value(3).toString();
                    global+=query.value(2).toString()+"\t"; // cuenta
                    global+= (query.value(13).toString().isEmpty() ?
                              filtracad(razon) :
                              filtracad(query.value(13).toString()))
                              +"\t"; // proveedor
                    global+= (query.value(14).toString().isEmpty() ?
                              nif :
                              filtracad(query.value(14).toString())); // cif
                   }
                   else
                      {
                       global+="\t"; // cuenta
                       global+=filtracad(nombreempresa())+"\t"; // proveedor
                       global+=basedatos::instancia()->cif();
                      }

                global+="\t";
                global+=filtracad(descripcioncuenta(query.value(12).toString()));
                global+="\t";
                global+=query.value(4).toString()+"\t"; // asiento
                global+=formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)+"\t"; // base iva
                global+=formatea_redondeado_sep(query.value(6).toDouble(),comadecimal,decimales)+"\t"; // tipo
                global+=formatea_redondeado_sep(query.value(7).toDouble(),comadecimal,decimales)+"\t"; // cuota
                if (query.value(10).toBool() || query.value(11).toBool() || query.value(16).toBool() || query.value(22).toBool())
                   global+=formatea_redondeado_sep(query.value(5).toDouble(),comadecimal,decimales)+"\t"; // total
                 else
                    global+=formatea_redondeado_sep(query.value(8).toDouble(),comadecimal,decimales)+"\t"; // total

                global+="\n";
                numprog++;
                base+=query.value(5).toDouble();
                cuota+=query.value(7).toDouble();
              }
      }

   // --------------------------------------------------------------------------------------
    global+="\t\t\t\t\t\t\t\t"+tr("SUMAS TOTALES:")+"\t";
    global+=formatea_redondeado_sep(base,comadecimal,decimales) + "\t\t";
    global+=formatea_redondeado_sep(cuota,comadecimal,decimales) + "\t";
    global+=formatea_redondeado_sep(base+cuota,comadecimal,decimales);
    global+="\n";

   cb->setText(global);
   QMessageBox::information( this, tr("Libro de Facturas"),
                            tr("Se ha pasado el contenido al portapapeles") );
}



void libro_recibidas::latex()
{
    if (ui.prorratacheckBox->isChecked())
       {
        if (!generalatexprorrata())
           return;
       }
      else
        {
         if (!ui.horizontalradioButton->isChecked())
           {
            if  (!generalatexsoportado())
             return;
           }
         if (ui.horizontalradioButton->isChecked())
          {
           if  (!generalatexsoportado_horizontal())
           return;
          }
        }

    int valor=editalatex(nombrefichero());
    if (valor==1)
        QMessageBox::warning( this, tr("LIBRO FACTURAS"),tr("PROBLEMAS al llamar al editor Latex"));

}


void libro_recibidas::tabla()
{
    QString ejercicio=ui.ejerciciocomboBox->currentText();

    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    tabla_iva_sop *t = new tabla_iva_sop(usuario, comadecimal, decimales);
    t->pasa_info(ejercicio,
                 fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
                 ui.interiorescheckBox->isChecked(),
                 ui.aibcheckBox->isChecked(),
                 ui.autofacturascheckBox->isChecked(),
                 ui.autofacturasnouecheckBox->isChecked(),
                 ui.rectificativascheckBox->isChecked(),
                 ui.seriesgroupBox->isChecked(),
                 ui.serieiniciallineEdit->text(),
                 ui.seriefinallineEdit->text(),
                 ui.binversiongroupBox->isChecked(),
                 ui.solobiradioButton->isChecked(),
                 ui.sinbiradioButton->isChecked(),
                 ui.sujeciongroupBox->isChecked(),
                 ui.solonosujetasradioButton->isChecked(),
                 ui.excluirnosujetasradioButton->isChecked(),
                 ui.agrariocheckBox->isChecked(),
                 ui.isp_op_interiorescheckBox->isChecked(),
                 ui.importacionescheckBox->isChecked(),cuenta, ui.externo_lineEdit->text());
    t->exec();
    delete(t);

}


void libro_recibidas::cuenta_finedicion()
{
    ui.cuentalineEdit->setText(expandepunto(ui.cuentalineEdit->text(),anchocuentas()));
}


void libro_recibidas::boton_cuenta_pulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentalineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.cuentalineEdit->setText(cadena2);
       else ui.cuentalineEdit->setText("");
    delete labusqueda;
}


void libro_recibidas::cuenta_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.cuentalineEdit->text(),&cadena))
    ui.descripcuentalabel->setText(cadena);
  else
    ui.descripcuentalabel->setText("");
}


void libro_recibidas::externo_cambiado() {
    ui.descrip_externo_label->setText(basedatos::instancia()->razon_externo(ui.externo_lineEdit->text()));
}


void libro_recibidas::buscaexterno() {
    busca_externo *b = new busca_externo();
    int resultado=b->exec();
    if (resultado==QDialog::Accepted)
       {
        ui.externo_lineEdit->setText(b->codigo_elec());
        //ui.descrip_externo_lineEdit->setText(b->nombre_elec());

       }
    delete(b);

}

void libro_recibidas::check_grupo_externo_cambiado(){
    if (!ui.externo_groupBox->isChecked()) ui.externo_lineEdit->clear();
}

void libro_recibidas::on_documentos_pushButton_clicked()
{
    QString ejercicio=ui.ejerciciocomboBox->currentText();
    bool fechacontable=false;
    if (ui.fechasgroupBox->isChecked())
       {
        ejercicio.clear();
        if (ui.fcontableradioButton->isChecked()) fechacontable=true;
       }

    QString cuenta;
    if (ui.cuentagroupBox->isChecked()) cuenta=ui.cuentalineEdit->text();

    int registros = basedatos::instancia()->num_registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(),false,cuenta,ui.externo_lineEdit->text());

    QSqlQuery query =
      basedatos::instancia()->registros_recibidas( ejercicio,
         fechacontable,ui.inicialdateEdit->date(),ui.finaldateEdit->date(),
         ui.interiorescheckBox->isChecked(),
         ui.aibcheckBox->isChecked(),
         ui.autofacturascheckBox->isChecked(),
         ui.autofacturasnouecheckBox->isChecked(),
         ui.rectificativascheckBox->isChecked(),
         ui.seriesgroupBox->isChecked(),
         ui.serieiniciallineEdit->text(),
         ui.seriefinallineEdit->text(),
         ui.binversiongroupBox->isChecked(),
         ui.solobiradioButton->isChecked(),
         ui.sinbiradioButton->isChecked(),
         ui.sujeciongroupBox->isChecked(),
         ui.solonosujetasradioButton->isChecked(),
         ui.excluirnosujetasradioButton->isChecked(),
         ui.agrariocheckBox->isChecked(),
         ui.isp_op_interiorescheckBox->isChecked(),
         ui.importacionescheckBox->isChecked(), cuenta, false, ui.externo_lineEdit->text());

    ui.progressBar->setMaximum(registros);
    int numorden=0;
    int numprog=1;
    QString cadnum,guardadoc,guardaprov;
    QStringList fich_docs, regs_orden;
    QStringList vacios;
    if ( query.isActive() ) {
          while ( query.next() )
              {
                ui.progressBar->setValue(numprog);
                if (guardadoc!=query.value(0).toString() || query.value(0).toString().length()==0
                    || numprog==1 || guardaprov!=query.value(2).toString()) {
                    numorden++;
                }
                guardadoc=query.value(0).toString();
                guardaprov=query.value(2).toString();
                fich_docs << query.value(21).toString();
                if (ordenrecepcion) {
                    QString ceros; ceros.fill('0',4-query.value(15).toString().trimmed().length());
                    regs_orden << ceros+query.value(15).toString();
                }
                  else {
                    cadnum.setNum(numorden);
                    QString ceros; ceros.fill('0',4-cadnum.trimmed().length());
                    regs_orden << ceros+cadnum.setNum(numorden);
                  }
                if (query.value(21).toString().isEmpty()) {
                    if (ordenrecepcion)
                        vacios << query.value(15).toString();
                      else
                        vacios << cadnum.setNum(numorden);

                }
                numprog++;
              }
      }

    if (!vacios.isEmpty()) {
        if (QMessageBox::question(
               this,
               tr("LIBRO DE FACTURAS"),
               tr("Hay registros sin documento:\n")+vacios.join("-")+
                  tr("\n ¿ Desea continuar?"),
               tr("&Sí"), tr("&No"),
                      QString(), 0, 1 ) ==1 )
                             return;

    }


    QFileDialog dialogofich(this);
    dialogofich.setFileMode(QFileDialog::Directory);
    dialogofich.setViewMode(QFileDialog::List);
    dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
    dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
    dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
    dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
    dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );
    dialogofich.setDirectory(dirtrabajo());

    dialogofich.setWindowTitle(tr("SELECCIÓN DE DIRECTORIO"));
    QDir directorio;
    if (dialogofich.exec())
       {directorio=dialogofich.directory();}
      else return;


    for (int i=0;i<fich_docs.count();i++) {
        QString fichdoc=fich_docs.at(i);
        if (fichdoc.isEmpty()) continue;
        QString dest_fich;
        int p=fichdoc.indexOf("-")+1;
        if (!fichdoc.contains('-')) dest_fich=fichdoc;
          else dest_fich=fichdoc.mid(p);
        dest_fich.prepend(regs_orden.at(i)+"-");
        if (basedatos::instancia()->gestor_doc_bd()) {
            QString cadnum;
            if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
                else cadnum=fichdoc.section('-',0,0);
            qlonglong num=cadnum.toLongLong();
            QByteArray outByteArray;
            outByteArray=basedatos::instancia()->documento_bd(num);
            QString url=directorio.absolutePath().append(QDir::separator()).append(dest_fich);
            QFile file(url);
            file.open(QIODevice::WriteOnly);

            //EL BLOB OBTENIDO DE LA CONSULTA LO PASAMOS AL ARCHIVO
            file.write(outByteArray);
            file.close();
            continue;
        }
        // No docs en base de datos
        QString fichdoc_ruta=expanderutadocfich(fichdoc);
        // copiamos el fichero a la ruta elegida
        QFile fichero(fichdoc_ruta);
        fichero.copy(directorio.absolutePath().append(QDir::separator()).append(dest_fich));
    }

}
