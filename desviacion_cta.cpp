/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#include "desviacion_cta.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>


desviacion_cta::desviacion_cta(QString qcuenta,QString qejercicio) : QDialog() {
    ui.setupUi(this);

    cuenta=qcuenta;
    ejercicio=qejercicio;
    comadecimal=haycomadecimal();
    decimales=haydecimales();
    ui.label->setText(ui.label->text()+" - "+cuenta+" - "+ejercicio);
    ui.label2->setText(descripcioncuenta(cuenta));

    // cargamos los datos
 QSqlQuery query = basedatos::instancia()->selectCodigoperiodoejercicioorderinicioasc(ejercicio);
   QStringList lfilas;
   int filas=0;
   if ( query.isActive() ) {
          while  ( query.next() ) 
                      {
                        filas++;
                        lfilas << query.value(0).toString();
                      }
          lfilas << tr("TOTAL");
          }
  ui.tableWidget->setRowCount(filas+1);
  ui.tableWidget->setVerticalHeaderLabels(lfilas);

  query = basedatos::instancia()->selectPresupuestospresupuestoejerciciocuenta(ejercicio,cuenta);
  if ( (query.isActive()) && (query.first()) )
  {
      for (int veces=0;veces<filas;veces++)
      {
           QString cadnum=formateanumero(query.value(veces).toDouble(),comadecimal, decimales);
           QTableWidgetItem *newItem = new QTableWidgetItem(cadnum);
           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.tableWidget->setItem(veces,0,newItem);
      }
  }

   int pos=0;
   double totalprevision=0;
   double totalrealizacion=0;

   query = basedatos::instancia()->selectIniciofinperiodosejercicioorderinicio(ejercicio);
   while (query.next() && pos<ui.tableWidget->rowCount())
       {

        QSqlQuery query2 = basedatos::instancia()->selectSumdebesumhaberdiariocuentafechasnocierrenoregularizacion( cuenta, query.value(0).toDate() , query.value(1).toDate() );;

        if (query2.first())
          {
           QString cadnum=formateanumero(query2.value(0).toDouble(),comadecimal, decimales);
           QTableWidgetItem *newItem = new QTableWidgetItem(cadnum);
           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.tableWidget->setItem(pos,1,newItem);
           if (ui.tableWidget->item(pos,0)!=0)
              {
                double prevision=convapunto(ui.tableWidget->item(pos,0)->text()).toDouble();
                double realizacion=query2.value(0).toDouble();
                totalprevision+=prevision;
                totalrealizacion+=realizacion;
                QTableWidgetItem *newItem2 = new QTableWidgetItem(
                                       formateanumero(prevision-realizacion,comadecimal, decimales));
                newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                ui.tableWidget->setItem(pos,2,newItem2);

                QString contenido;
                if (prevision>0.001 || prevision<-0.001) 
                    contenido=formateanumero((prevision-realizacion)/prevision*100,comadecimal, decimales);
                QTableWidgetItem *newItem3 = new QTableWidgetItem(contenido);
                newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                ui.tableWidget->setItem(pos,3,newItem3);
              }
          }
        pos++;
       }

   QTableWidgetItem *newItem = new QTableWidgetItem(
                       formateanumero(totalprevision,comadecimal, decimales));
   newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
   ui.tableWidget->setItem(filas,0,newItem);

   QTableWidgetItem *newItem2 = new QTableWidgetItem(
                       formateanumero(totalrealizacion,comadecimal, decimales));
   newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
   ui.tableWidget->setItem(filas,1,newItem2);

   QTableWidgetItem *newItem3 = new QTableWidgetItem(
           formateanumero(totalprevision-totalrealizacion,comadecimal, decimales));
   newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.tableWidget->setItem(filas,2,newItem3);

   QString contenido;
   if (totalprevision>0.001 || totalprevision<-0.001) 
        contenido=formateanumero((totalprevision-totalrealizacion)/totalprevision*100,comadecimal, decimales);
   QTableWidgetItem *newItem4 = new QTableWidgetItem(contenido);
   newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.tableWidget->setItem(filas,3,newItem4);

   connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprime()));
   connect(ui.copiarpushButton,SIGNAL(clicked()),SLOT(copiar()));

}


void desviacion_cta::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("desviacion_cta.tex");
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

    QString mensaje=ui.label->text();
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << filtracad(mensaje) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << filtracad(ui.label2->text()) << "}}";
    stream << "\\end{center}";


// ***************************************************************************************** 
    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|l|r|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";
    QString cadena;
    stream << "\\multicolumn{5}{|c|} {\\textbf{";
    cadena=tr("TABLA DESVIACIONES");
    // --------------------------------------------------------------------------------------
    stream << cadena;
    stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // --------------------------------------------------------------------------------------
    stream << "{\\small{" << tr("PERIODO") << "}} & ";
    stream << "{\\small{" << tr("Previsión") << "}} & ";
    stream << "{\\small{" << tr("Realización") << "}} & ";
    stream << "{\\small{" << tr("Desviación") << "}} &";
    stream << "{\\small{" << tr("Porcentaje") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";

    stream << "{\\small{" << tr("PERIODO") << "}} & ";
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
             stream << ui.tableWidget->verticalHeaderItem(fila)->text();
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,0)!=0) stream << filtracad(ui.tableWidget->item(fila,0)->text());
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,1)!=0) stream << ui.tableWidget->item(fila,1)->text();
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,2)!=0) stream << ui.tableWidget->item(fila,2)->text();
             stream << "} & {\\small ";
             if (ui.tableWidget->item(fila,3)!=0) stream << ui.tableWidget->item(fila,3)->text();
             stream << "} \\\\ \n  " << "\\hline\n";
             fila++;
          }

    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}

void desviacion_cta::imprime()
{

   generalatex();
   int valor=imprimelatex2(tr("desviacion_cta"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir desviaciones cuenta"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir desviaciones cuenta"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir desviaciones cuenta"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void desviacion_cta::copiar()
{
   QClipboard *cb = QApplication::clipboard();
   QString global;


    QString mensaje=ui.label->text();
    global+= mensaje;
    global+= "\n";
    global+= ui.label2->text();
    global+= "\n";

    global+=tr("PERIODO");
    global+="\t";
    global+=tr("Previsión");
    global+="\t";
    global+=tr("Realización");
    global+="\t";
    global+=tr("Desviación");
    global+="\t";
    global+=tr("Porcentaje");
    global+="\n";

    int fila=0;
    while (fila<ui.tableWidget->rowCount())
          {
             global+=ui.tableWidget->verticalHeaderItem(fila)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,0)!=0) global+= ui.tableWidget->item(fila,0)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,1)!=0) global+= ui.tableWidget->item(fila,1)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,2)!=0) global+= ui.tableWidget->item(fila,2)->text();
             global+="\t";
             if (ui.tableWidget->item(fila,3)!=0) global+= ui.tableWidget->item(fila,3)->text();
             global+="\n";
             fila++;
          }

   cb->setText(global);
   QMessageBox::information( this, tr("ANÁLISIS DE DESVIACIONES"),
			      tr("Se ha pasado el contenido al portapales"));

}
