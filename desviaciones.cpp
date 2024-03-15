/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

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

#include "desviaciones.h"
#include "funciones.h"
#include "basedatos.h"
#include "desviacion_cta.h"
#include <QProgressDialog>
#include <QMessageBox>


desviaciones::desviaciones(bool qsubcuentas,QString qejercicio) : QDialog() {
    ui.setupUi(this);

    subcuentas=qsubcuentas;
    ejercicio=qejercicio;
    comadecimal=haycomadecimal();
    decimales=haydecimales();
    QString cadena=ui.titulolabel->text();
    cadena+=" ";
    ui.titulolabel->setText(cadena+ejercicio);
    ui.tableWidget->setColumnWidth(1,220);
    cargainfo();
   connect(ui.refrescarpushButton,SIGNAL(clicked()),SLOT(cargainfo()));
   connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprime()));
   connect(ui.detallepushButton,SIGNAL(clicked()),SLOT(detalle()));
   connect(ui.copiarpushButton,SIGNAL(clicked()),SLOT(copiar()));
}

void desviaciones::cargainfo()
{
   if ( basedatos::instancia()->selectCountcuentapresupuestoauxiliar( subcuentas, ui.seleclineEdit->text() , ejercicio ) >= 0 )
      ui.tableWidget->setRowCount( basedatos::instancia()->selectCountcuentapresupuestoauxiliar( subcuentas, ui.seleclineEdit->text() , ejercicio ) );;

 QProgressDialog progreso(tr("Cargando información ..."), 0, 0, ui.tableWidget->rowCount()-1);
 progreso.setWindowModality(Qt::WindowModal);

 QSqlQuery query = basedatos::instancia()->selectCuentapresupuestopresupuestocuentaauxiliar( subcuentas, ui.seleclineEdit->text() , ejercicio );
 int fila=0;
 while (query.next() && fila<ui.tableWidget->rowCount())
   {
    progreso.setValue(fila);
    QApplication::processEvents();
    QTableWidgetItem *newItem = new QTableWidgetItem(query.value(0).toString());
    ui.tableWidget->setItem(fila,0,newItem);

    QTableWidgetItem *newItem2 = new QTableWidgetItem(descripcioncuenta(query.value(0).toString()));
    ui.tableWidget->setItem(fila,1,newItem2);

    QString cadnum=formateanumero(query.value(1).toDouble(),comadecimal, decimales);
    double prevision=query.value(1).toDouble();
    QTableWidgetItem *newItem3 = new QTableWidgetItem(cadnum);
    newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.tableWidget->setItem(fila,2,newItem3);

    // calculamos realización

    double realizacion=0;
   QSqlQuery queryr = basedatos::instancia()->selectSumdebesumhaberdiariocuentafechasnocierrenoregularizacion( query.value(0).toString() , inicioejercicio(ejercicio) , finejercicio(ejercicio) );
   if ( (queryr.isActive()) && (queryr.first()) )
        realizacion=queryr.value(0).toDouble();

   cadnum=formateanumero(realizacion,comadecimal,decimales);
   QTableWidgetItem *newItem4 = new QTableWidgetItem(cadnum);
   newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
   ui.tableWidget->setItem(fila,3,newItem4);

   cadnum=formateanumero(prevision-realizacion,comadecimal,decimales);
   QTableWidgetItem *newItem5 = new QTableWidgetItem(cadnum);
   newItem5->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
   ui.tableWidget->setItem(fila,4,newItem5);

   if (prevision>0.001 || prevision <-0.001)
      {
       cadnum=formateanumero((prevision-realizacion)/prevision*100,comadecimal,decimales);
       QTableWidgetItem *newItem6 = new QTableWidgetItem(cadnum);
       newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tableWidget->setItem(fila,5,newItem6);
      }
   progreso.setValue(fila);

   fila++;
  }
 if (fila<ui.tableWidget->rowCount()-1)
     {
      fila=ui.tableWidget->rowCount()-1;
      progreso.setValue(fila);
     }
}


void desviaciones::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("desviaciones.tex");
   QFile fichero(adapta(qfichero));
    if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    QString mensaje=tr("ANÁLISIS DESVIACIONES EJERCICIO: ");
    mensaje+=ejercicio;
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << mensaje << "}}";
    stream << "\\end{center}";

    if (ui.seleclineEdit->text().length()>0)
       {
        mensaje=tr("SELECCIÓN: ");
        mensaje+=ui.seleclineEdit->text();
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << mensaje << "}}";
        stream << "\\end{center}";
       }

// ***************************************************************************************** 
    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|p{6cm}|r|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";
    QString cadena;
    stream << "\\multicolumn{6}{|c|} {\\textbf{";
    cadena=tr("TABLA DESVIACIONES");
    // --------------------------------------------------------------------------------------
    stream << cadena;
    stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // --------------------------------------------------------------------------------------
    stream << "{\\small{" << tr("CUENTA") << "}} & ";
    stream << "{\\small{" << tr("DESCRIPCIÓN") << "}} & ";
    stream << "{\\small{" << tr("Previsión") << "}} & ";
    stream << "{\\small{" << tr("Realización") << "}} & ";
    stream << "{\\small{" << tr("Desviación") << "}} &";
    stream << "{\\small{" << tr("Porcentaje") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";

    stream << "{\\small{" << tr("CUENTA") << "}} & ";
    stream << "{\\small{" << tr("DESCRIPCIÓN") << "}} & ";
    stream << "{\\small{" << tr("Previsión") << "}} & ";
    stream << "{\\small{" << tr("Realización") << "}} & ";
    stream << "{\\small{" << tr("Desviación") << "}} &";
    stream << "{\\small{" << tr("Porcentaje") << "}}";

    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // --------------------------------------------------------------------------------------------
    int fila=0;
    while (fila<ui.tableWidget->rowCount())
          {
             stream << "{\\small "; 
             if (ui.tableWidget->item(fila,0)!=0) stream << ui.tableWidget->item(fila,0)->text();
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,1)!=0) stream << filtracad(ui.tableWidget->item(fila,1)->text());
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,2)!=0) stream << ui.tableWidget->item(fila,2)->text();
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,3)!=0) stream << ui.tableWidget->item(fila,3)->text();
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,4)!=0) stream << ui.tableWidget->item(fila,4)->text();
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,5)!=0) stream << ui.tableWidget->item(fila,5)->text();
             stream << "} \\\\ \n  " << "\\hline\n";
             fila++;
          }

    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}

void desviaciones::imprime()
{

   generalatex();
   int valor=imprimelatex2(tr("desviaciones"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir desviaciones"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir desviaciones"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir desviaciones"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}

void desviaciones::detalle()
{
    int fila=ui.tableWidget->currentRow();
    if (fila>=ui.tableWidget->rowCount() || ui.tableWidget->rowCount()==0 || fila<0) return;
    QString cuenta=ui.tableWidget->item(fila,0)->text();
    desviacion_cta *d = new desviacion_cta(cuenta,ejercicio);
    d->exec();
    delete(d);
}

void desviaciones::copiar()
{
   QClipboard *cb = QApplication::clipboard();
   QString global;


    QString mensaje=tr("ANÁLISIS DESVIACIONES EJERCICIO: ");
    mensaje+=ejercicio;
    global+= mensaje ;
    global+="\n";
    if (ui.seleclineEdit->text().length()>0)
       {
        mensaje=tr("SELECCIÓN: ");
        mensaje+=ui.seleclineEdit->text();
        global += mensaje;
        global+="\n";
       }

// ***************************************************************************************** 
    // --------------------------------------------------------------------------------------
    global+=tr("CUENTA");
    global+="\t";
    global+=tr("DESCRIPCIÓN");
    global+="\t";
    global+=tr("Previsión");
    global+="\t";
    global+=tr("Realización");
    global+="\t";
    global+=tr("Desviación");
    global+="\t";
    global+=tr("Porcentaje");
    global+="\n";
    // --------------------------------------------------------------------------------------------
    int fila=0;
    while (fila<ui.tableWidget->rowCount())
          {
             if (ui.tableWidget->item(fila,0)!=0) global+= ui.tableWidget->item(fila,0)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,1)!=0) global+= ui.tableWidget->item(fila,1)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,2)!=0) global+= ui.tableWidget->item(fila,2)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,3)!=0) global+= ui.tableWidget->item(fila,3)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,4)!=0) global+= ui.tableWidget->item(fila,4)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,5)!=0) global+= ui.tableWidget->item(fila,5)->text();
             global+="\n";
             fila++;
          }

   cb->setText(global);
   QMessageBox::information( this, tr("ANÁLISIS DE DESVIACIONES"),
			      tr("Se ha pasado el contenido al portapales"));

}
