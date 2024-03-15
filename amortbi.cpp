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

#include "amortbi.h"
#include "basedatos.h"
#include "funciones.h"
#include <QMessageBox>


amortbi::amortbi() : QDialog() {
    ui.setupUi(this);

  // cargar combo de ejercicios
 comadecimal=haycomadecimal();
 decimales=haydecimales();

 QStringList ej1;
 QSqlQuery query = basedatos::instancia()->selectEjercicioamortcontableorderejercicio();
 if ( query.isActive() ) {
          while ( query.next() )
              {
                ej1 << query.value(0).toString();
	      }

          }

  ui.ejerciciocomboBox->addItems(ej1);


 QStringList columnas;
 columnas << tr("CUENTA ACTIVO") << tr("CÓDIGO ACTIVO") << tr("DESCRIPCIÓN");
 columnas << tr("GASTO CONTABLE") << tr("GASTO FISCAL") << tr("DIFERENCIA");

 ui.table->setColumnCount(6);

 ui.table->setHorizontalHeaderLabels(columnas);

 connect(ui.refrescapushButton,SIGNAL(clicked()),SLOT(refrescar()));
 connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprimir()));


 cargaelementos();


}


void amortbi::cargaelementos()
{
  ui.ejerciciolabel->setText(ui.ejerciciocomboBox->currentText());

  /*QSqlQuery query = basedatos::instancia()->selectCountcuenta_activoamortfiscalycontableejercicio(ui.ejerciciocomboBox->currentText());

  if ( (query.isActive()) && ( query.next() ) )
    {
          ui.table->setRowCount(query.value(0).toInt());
    }*/

  QSqlQuery query=basedatos::instancia()->select5amortfiscalycontableejercicio(ui.ejerciciocomboBox->currentText());

  int fila=0;
  if ( query.isActive() ) {
          while ( query.next() )
              {
                ui.table->insertRow(fila);
                QTableWidgetItem *newItem = new QTableWidgetItem(query.value(0).toString());
	        ui.table->setItem(fila,0,newItem);
                QTableWidgetItem *newItem1 = new QTableWidgetItem(query.value(1).toString());
	        ui.table->setItem(fila,1,newItem1);
                QTableWidgetItem *newItem2 = new QTableWidgetItem(descripcioncuenta(query.value(0).toString()));
	        ui.table->setItem(fila,2,newItem2);
                QTableWidgetItem *newItem3 = new QTableWidgetItem(
                       formateanumerosep(query.value(2).toDouble(),comadecimal,decimales));
                newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	        ui.table->setItem(fila,3,newItem3);
                QTableWidgetItem *newItem4 = new QTableWidgetItem(
                       formateanumerosep(query.value(3).toDouble(),comadecimal,decimales));
                newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	        ui.table->setItem(fila,4,newItem4);
                QTableWidgetItem *newItem5 = new QTableWidgetItem(
                       formateanumerosep(query.value(4).toDouble(),comadecimal,decimales));
                newItem5->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	        ui.table->setItem(fila,5,newItem5);
	        fila++;
	      }
          }
  ui.table->resizeColumnsToContents();

  query = basedatos::instancia()->selectSumasamortfiscalycontableejercicio(ui.ejerciciocomboBox->currentText());
  if ( query.isActive() ) 
          if ( query.next() )
              {
                ui.contablelineEdit->setText(formateanumerosep(query.value(0).toDouble(),comadecimal,decimales));
	        ui.fiscallineEdit->setText(formateanumerosep(query.value(1).toDouble(),comadecimal,decimales));
	        ui.diferencialineEdit->setText(formateanumerosep(query.value(2).toDouble(),
                                        comadecimal,decimales));
              }
}


void amortbi::refrescar()
{
  ui.table->clearContents();
  cargaelementos();
}


void amortbi::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("ajustes_amortizaciones.tex");
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero(pasa);

    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << nombreempresa() << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|c|p{3cm}|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{6}{|c|} {\\textbf{";
   QString cadena;
   cadena=tr("AJUSTES EN BASE IMPONIBLE POR AMORTIZACIONES ");
   cadena+=ui.ejerciciolabel->text();

   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // ----------------------------------------------------------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("Cuenta activo") << "}} & ";
    stream << "{\\tiny{" << tr("Cód. activo") << "}} & ";
    stream << "{\\tiny{" << tr("Descripción") << "}} & ";
    stream << "{\\tiny{" << tr("Gasto contable") << "}} & ";
    stream << "{\\tiny{" << tr("Gasto fiscal") << "}} & ";
    stream << "{\\tiny{" << tr("Diferencia") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << " \\\\" << "\n";
    stream << "{\\tiny{" << tr("Cuenta activo") << "}} & ";
    stream << "{\\tiny{" << tr("Cód. activo") << "}} & ";
    stream << "{\\tiny{" << tr("Descripción") << "}} & ";
    stream << "{\\tiny{" << tr("Gasto contable") << "}} & ";
    stream << "{\\tiny{" << tr("Gasto fiscal") << "}} & ";
    stream << "{\\tiny{" << tr("Diferencia") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // -------------------------------------------------------------------------------------------------------
    int veces=0;
          while ( veces<ui.table->rowCount() )
                      {
	        stream << "{\\tiny " << ui.table->item(veces,0)->text() << "} & {\\tiny "; 
	        stream << ui.table->item(veces,1)->text() << "} & {\\tiny "; 
	        stream << filtracad(ui.table->item(veces,2)->text()) << "} & {\\tiny "; 
	        stream << formateanumerosep(convapunto(ui.table->item(veces,3)->text()).toDouble(),
                                            comadecimal,decimales)
                          << "} & {\\tiny "; 
	        stream << formateanumerosep(convapunto(ui.table->item(veces,4)->text()).toDouble(),
                                            comadecimal,decimales)
                          << "} & {\\tiny "; 
	        stream << formateanumerosep(convapunto(ui.table->item(veces,5)->text()).toDouble(),
                                            comadecimal,decimales);
	        stream << "} \\\\ \n  " << "\\hline\n";
	        veces++;
	      }

    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();


}


void amortbi::imprimir()
{
   QString fichero=tr("ajustes_amortizaciones");

    generalatex();

   int valor=imprimelatex2(fichero);
   if (valor==1)
       QMessageBox::warning( this, tr("Ajustes de amortizaciones"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Ajustes de amortizaciones"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Ajustes de amortizaciones"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}
