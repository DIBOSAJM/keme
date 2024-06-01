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

#include "inmovneto.h"
#include "funciones.h"
#include "basedatos.h"
#include <qtrpt.h>
#include <QMessageBox>


inmovneto::inmovneto() : QDialog() {
    ui.setupUi(this);


comadecimal=haycomadecimal(); 
 decimales=haydecimales();


 columnas << tr("CUENTA ACTIVO") <<  tr("DESCRIPCIÓN");
 columnas << tr("CUENTA AM.ACUM.") << tr("VAL.ADQUISICION") << tr("AMORT.ACUM.");
 columnas << tr("VALOR NETO") << tr("CUENTA/EXTERNO") << tr("PROVEEDOR");
 columnas << tr("AÑOS AM.") << tr("FECHA BAJA") << tr("MOTIVO BAJA");

 ui.tabla->setColumnCount(11);

 ui.tabla->setHorizontalHeaderLabels(columnas);

 cargar_datos();

     connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprimir()));
     connect(ui.informe_latex_pushButton,SIGNAL(clicked(bool)),SLOT(informe_latex()));
     connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));
     connect(ui.copiarpushButton,SIGNAL(clicked()),SLOT(copiar()));

#ifdef NOEDITTEX
  ui.latexpushButton->hide();
#endif

}

void inmovneto::cargar_datos()
{
  ui.tabla->clearContents();
  ui.tabla->setRowCount(0);
  // ui.tabla->setRowCount( basedatos::instancia()->selectCountcuenta_activoplanamortizaciones() );

  QSqlQuery query = basedatos::instancia()->selectCuenta_activocuenta_am_acumplanamortizacionesordercuenta_activo();
  /*
     "select cuenta_activo, cuenta_am_acum, "
     "cuenta_proveedor, coef_amort_contable, baja, fecha_baja, "
     "motivo_baja"
     " from planamortizaciones order by cuenta_activo"
   */

  int fila=0;
  if ( query.isActive() ) {
      while ( query.next() )
      {
          double saldo=saldocuentaendiario(query.value(0).toString());
          double aa=saldocuentaendiario(query.value(1).toString());
          double valneto=saldo+aa;
          if (ui.valor_neto_checkBox->isChecked())
             if (valneto<0.001 && valneto>-0.001) continue;

          aa=aa*-1;
          ui.tabla->insertRow(fila);
          QTableWidgetItem *newItem0 = new QTableWidgetItem(query.value(0).toString());
          QTableWidgetItem *newItem1 = new QTableWidgetItem(descripcioncuenta(query.value(0).toString()));
          QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(1).toString());

          ui.tabla->setItem(fila,0,newItem0);
          ui.tabla->setItem(fila,1,newItem1);
          ui.tabla->setItem(fila,2,newItem2);

          QTableWidgetItem *newItem3 = new QTableWidgetItem(formateanumerosep(saldo,comadecimal,decimales));
          newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui.tabla->setItem(fila,3,newItem3);

          QTableWidgetItem *newItem4 = new QTableWidgetItem(formateanumerosep(aa,comadecimal,decimales));
          newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui.tabla->setItem(fila,4,newItem4);

          QTableWidgetItem *newItem5 = new QTableWidgetItem(formateanumerosep(valneto,comadecimal,decimales));
          newItem5->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui.tabla->setItem(fila,5,newItem5);


          double years=100/(query.value(3).toDouble()*100);
          QString cadtiempo; cadtiempo.setNum(years,'f',2);
          if (comadecimal) cadtiempo=convacoma(cadtiempo);
          bool baja=query.value(4).toBool();
          QTableWidgetItem *newItem6 = new QTableWidgetItem(query.value(2).toString()); // proveedor
          QTableWidgetItem *newItem7 = new QTableWidgetItem(descripcioncuenta(query.value(2).toString()));
          if (!query.value(7).toString().isEmpty()) {
              newItem6->setText(query.value(7).toString());
              newItem7->setText(basedatos::instancia()->razon_externo(query.value(7).toString()));
          }
          QTableWidgetItem *newItem8 = new QTableWidgetItem(cadtiempo);
          newItem8->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          QTableWidgetItem *newItem9 = new QTableWidgetItem(baja ? query.value(5).toDate().toString("dd-MM-yyyy"):"");
          QTableWidgetItem *newItem10 = new QTableWidgetItem(baja ? query.value(6).toString():"");

          ui.tabla->setItem(fila,6,newItem6);
          ui.tabla->setItem(fila,7,newItem7);
          ui.tabla->setItem(fila,8,newItem8);
          ui.tabla->setItem(fila,9,newItem9);
          ui.tabla->setItem(fila,10,newItem10);

          fila++;
      }
  }

  /*fila=0;
  // QString cadsaldo,cadaa,cadneto;
  double valneto;
  while (fila<ui.tabla->rowCount())
  {
      double saldo=saldocuentaendiario(ui.tabla->item(fila,0)->text());
      QTableWidgetItem *newItem3 = new QTableWidgetItem(formateanumerosep(saldo,comadecimal,decimales));
      newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.tabla->setItem(fila,3,newItem3);

      double aa=saldocuentaendiario(ui.tabla->item(fila,2)->text());

      valneto=saldo+aa;
      aa=aa*-1;

      QTableWidgetItem *newItem4 = new QTableWidgetItem(formateanumerosep(aa,comadecimal,decimales));
      newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.tabla->setItem(fila,4,newItem4);

      QTableWidgetItem *newItem5 = new QTableWidgetItem(formateanumerosep(valneto,comadecimal,decimales));
      newItem5->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.tabla->setItem(fila,5,newItem5);
      fila++;
  }*/
  ui.tabla->resizeColumnsToContents();

}

void inmovneto::generalatex()
{
    QString qfichero=dirtrabajo();
    qfichero.append(QDir::separator());
    qfichero=qfichero+tr("inmovilizado_neto.tex");
    QFile ficheroa(adapta(qfichero));

     if (! ficheroa.open( QIODevice::WriteOnly ) ) return;
     QTextStream stream( &ficheroa );
     stream.setEncoding(QStringConverter::Utf8);
     stream << cabeceralatex();
     stream << margen_extra_latex();

     stream << "\\begin{landscape}" << "\n";

     stream << "\\begin{center}" << "\n";
     stream << "{\\Large \\textbf {";
     stream << filtracad(nombreempresa()) << "}}";
     stream << "\\end{center}";

     stream << "\\begin{center}" << "\n";
     stream << "\\begin{longtable}{|c|p{3.5cm}|c|r|r|r|l|p{2.5cm}|r|c|p{2.5cm}|}" << "\n";
     stream << "\\hline" << "\n";
     stream << "\\rowcolor{gray!30}\n";

    stream << "\\multicolumn{11}{|c|} {\\textbf{";
    QString cadena;
    cadena=tr("VALORES NETOS DE ELEMENTOS DE INMOVILIZADO");

    // --------------------------------------------------------------------------------------
    stream << cadena;
    stream <<  "}} \\\\";
     stream << "\\hline" << "\n";
     // -------------------------------------------------------------------------------------------------------
     stream << "\\rowcolor{gray!30}\n";
     stream << "{\\tiny{" << tr("Cuenta activo") << "}} & ";
     stream << "{\\tiny{" << tr("Descripción") << "}} & ";
     stream << "{\\tiny{" << tr("Cuenta a.acum.") << "}} & ";
     stream << "{\\tiny{" << tr("V.adquisición") << "}} & ";
     stream << "{\\tiny{" << tr("Am.acumul.") << "}} & ";
     stream << "{\\tiny{" << tr("Valor neto") << "}} & ";
     stream << "{\\tiny{" << tr("Cuenta prov.") << "}} & ";
     stream << "{\\tiny{" << tr("Proveedor") << "}} & ";
     stream << "{\\tiny{" << tr("Años amort") << "}} & ";
     stream << "{\\tiny{" << tr("Fecha Baja") << "}} & ";
     stream << "{\\tiny{" << tr("Motivo Baja") << "}}";
     stream << " \\\\" << "\n";
     stream << "\\hline" << "\n";
     stream << "\\endfirsthead";
     // --------------------------------------------------------------------------------------------------------
     stream << "\\hline" << "\n";
    // stream << tr(" \\\\") << "\n";
     stream << "\\rowcolor{gray!30}\n";
    stream << "\\multicolumn{11}{|c|} {\\textbf{";
    cadena=tr("VALORES NETOS DE ELEMENTOS DE INMOVILIZADO");

    // --------------------------------------------------------------------------------------
    stream << cadena;
    stream <<  "}} \\\\";
     stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
     stream << "{\\tiny{" << tr("Cuenta activo") << "}} & ";
     stream << "{\\tiny{" << tr("Descripción") << "}} & ";
     stream << "{\\tiny{" << tr("Cuenta a.acum.") << "}} & ";
     stream << "{\\tiny{" << tr("V.adquisición") << "}} & ";
     stream << "{\\tiny{" << tr("Am.acumul.") << "}} & ";
     stream << "{\\tiny{" << tr("Valor neto") << "}} & ";
     stream << "{\\tiny{" << tr("Cuenta prov.") << "}} & ";
     stream << "{\\tiny{" << tr("Proveedor") << "}} & ";
     stream << "{\\tiny{" << tr("Años amort") << "}} & ";
     stream << "{\\tiny{" << tr("Fecha Baja") << "}} & ";
     stream << "{\\tiny{" << tr("Motivo Baja") << "}}";
     stream << " \\\\" << "\n";
     stream << "\\hline" << "\n";
     stream << "\\endhead" << "\n";
     // ---------------------------------------------------------------------------------------------------------
     int veces=0;
           while ( veces<ui.tabla->rowCount() )
                {
                 stream << "{\\tiny " << ui.tabla->item(veces,0)->text() << "} & {\\tiny ";
                 stream << filtracad(ui.tabla->item(veces,1)->text()) << "} & {\\tiny ";
                 stream << ui.tabla->item(veces,2)->text() << "} & {\\tiny ";
                 stream << ui.tabla->item(veces,3)->text() << "} & {\\tiny ";
                 stream << ui.tabla->item(veces,4)->text() << "} & {\\tiny ";
                 stream << ui.tabla->item(veces,5)->text() << "} & {\\tiny ";
                 stream << ui.tabla->item(veces,6)->text() << "} & {\\tiny ";
                 stream << filtracad(ui.tabla->item(veces,7)->text()) << "} & {\\tiny ";
                 stream << ui.tabla->item(veces,8)->text() << "} & {\\tiny ";
                 stream << ui.tabla->item(veces,9)->text() << "} & {\\tiny ";
                 stream << ui.tabla->item(veces,10)->text();
                 stream << "} \\\\ \n  " << "\\hline\n";
                 veces++;
               }

     stream << "\\end{longtable}" << "\n";
     stream << "\\end{center}" << "\n";
     stream << "\\end{landscape}\n";

     stream << "% FIN_CUERPO\n";
     stream << "\\end{document}" << "\n";

     ficheroa.close();

}



void inmovneto::imprimir()
{

// ------------------------------------------------------------------------------------------------------
    QString fileName;
    fileName =  ":/informes/inmov_neto.xml" ;
    QtRPT *report = new QtRPT(this);
    // report->recordCount << cuentas.count();
    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("Balance de sumas y saldos"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }

    QObject::connect(report, &QtRPT::setDSInfo,
                     [&](DataSetInfo &dsInfo) {
        dsInfo.recordCount=ui.tabla->rowCount();
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
        // campos: descrip, nota, cifra1, cifra2
        // recordCount es una lista, añadir también segunda página

        // QMessageBox::warning( this, tr("Estados Contables"),tr("Num %1").arg(recNo));
        Q_UNUSED(reportPage);
        if (paramName == "cuenta_activo") paramValue = ui.tabla->item(recNo,0)->text();

        if (paramName == "descripcion") paramValue = ui.tabla->item(recNo,1)->text();

        if (paramName == "empresa") paramValue = basedatos::instancia()->selectEmpresaconfiguracion();

        if (paramName == "valor_adq") paramValue = convapunto(ui.tabla->item(recNo,3)->text().remove("."));

        if (paramName == "amort_acum") paramValue = convapunto(ui.tabla->item(recNo,4)->text().remove("."));

        if (paramName == "valor_neto") paramValue = convapunto(ui.tabla->item(recNo,5)->text().remove("."));

        if (paramName == "proveedor") paramValue = ui.tabla->item(recNo,7)->text();

        if (paramName == "anyos") paramValue = ui.tabla->item(recNo,8)->text();

        if (paramName == "fecha_baja") paramValue = ui.tabla->item(recNo,9)->text();

        if (paramName == "motivo_baja") paramValue = ui.tabla->item(recNo,10)->text();

    });


    //  report->printExec(true);

    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+tr("inmov_neto.pdf");
           report->printPDF(qfichero);
    }

}

void inmovneto::informe_latex() {
    generalatex();
    QString fichero=tr("inmovilizado_neto");

    int valor=imprimelatex2(fichero);
    if (valor==1)
        QMessageBox::warning( this, tr("Imprimir inmovilizado neto"),tr("PROBLEMAS al llamar a Latex"));
    if (valor==2)
        QMessageBox::warning( this, tr("Imprimir inmovilizado neto"),
                                 tr("PROBLEMAS al llamar a 'dvips'"));
    if (valor==3)
        QMessageBox::warning( this, tr("Imprimir inmovilizado neto"),
                              tr("PROBLEMAS al llamar a ")+programa_imprimir());
}

void inmovneto::latex()
{

     generalatex();
     QString fichero=tr("inmovilizado_neto");

    int valor=editalatex(fichero);
    if (valor==1)
        QMessageBox::warning( this, tr("ESTADOS CONTABLES"),tr("PROBLEMAS al llamar al editor Latex"));

}


void inmovneto::copiar()
{
    QString cadena;
    for (int veces=0; veces<columnas.count(); veces ++)
       {
        cadena=cadena+columnas.at(veces);
        if (veces!=columnas.count()-1) cadena+="\t";
       }
    cadena+="\n";

    for (int veces=0; veces<ui.tabla->rowCount(); veces++)
      {
        for (int veces2=0; veces2<ui.tabla->columnCount(); veces2++)
            {
             cadena+=ui.tabla->item(veces,veces2)->text();
             if (veces2!=ui.tabla->columnCount()-1) cadena+="\t";
            }
        cadena+="\n";
      }

    QClipboard *cb = QApplication::clipboard();
    cb->setText(cadena);

    QMessageBox::information( this, tr("Inmovilizado neto"),
                              tr("Se ha pasado el contenido al portapapeles") );

}

void inmovneto::on_valor_neto_checkBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    cargar_datos();
}

