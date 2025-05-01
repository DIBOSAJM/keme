/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

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

#include "cuadimprimayor.h"
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include <QMessageBox>
#include <QProgressDialog>

 
cuadimprimayor::cuadimprimayor(bool concomadecimal, bool condecimales) : QDialog() {
  ui.setupUi(this);

  comadecimal=concomadecimal;
  decimales=condecimales;
  QDate fechaactual;
  QString cadfechaactual;
  cadfechaactual=fechaactual.currentDate().toString("yyyy-MM-dd");

  ui.inicialdateEdit->setDate( basedatos::instancia()->selectAperturaejerciciosaperturacierre( cadfechaactual, cadfechaactual ) );
  ui.finaldateEdit->setDate( basedatos::instancia()->selectCierreejerciciosaperturacierre( cadfechaactual, cadfechaactual ) );

 connect(ui.subcuentalineEdit,SIGNAL(returnPressed()),SLOT(finedicsubcuenta()));
 connect(ui.subcuentapushButton,SIGNAL(clicked()),SLOT(botonsubcuentapulsado()));
 connect(ui.imprimepushButton,SIGNAL(clicked()),SLOT(botonimprimepulsado()));
 connect(ui.previsualizapushButton,SIGNAL(clicked()),SLOT(botonprevisualizapulsado()));
 connect(ui.subcuentalineEdit,SIGNAL(textChanged(QString)),SLOT(ctainicialcambiada()));
 connect(ui.finalpushButton,SIGNAL(clicked()),SLOT(botonctafinalpulsado()));
 connect(ui.finallineEdit,SIGNAL(returnPressed()),SLOT(finedicfinsubcuenta()));
 connect(ui.texpushButton,SIGNAL(clicked()),SLOT(latex()));
 connect(ui.copiapushButton,SIGNAL(clicked()),SLOT(copia()));

#ifdef NOEDITTEX
 ui.texpushButton->hide();
#endif

}


void cuadimprimayor::ctaexpandepunto()
{
  ui.subcuentalineEdit->setText(expandepunto(ui.subcuentalineEdit->text(),anchocuentas()));
}


void cuadimprimayor::finedicsubcuenta()
{
 ctaexpandepunto();
 if (ui.subcuentalineEdit->text().length()<anchocuentas() && !cod_longitud_variable()) botonsubcuentapulsado();
}


void cuadimprimayor::botonsubcuentapulsado()
{

    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.subcuentalineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (existesubcuenta(cadena2)) ui.subcuentalineEdit->setText(cadena2);
       else ui.subcuentalineEdit->setText("");
    delete labusqueda;

}


void cuadimprimayor::pasadatos( QString qsubcuenta, QDate qfecha )
{
   ui.subcuentalineEdit->setText(qsubcuenta);
   ui.finallineEdit->setText(qsubcuenta);
   QString ejercicio=ejerciciodelafecha(qfecha);
   QDate fechaini=inicioejercicio(ejercicio);
   QDate fechafin=finejercicio(ejercicio);
   ui.inicialdateEdit->setDate(fechaini);
   ui.finaldateEdit->setDate(fechafin);
}


void cuadimprimayor::fechasok()
{
   ctaexpandepunto();
    if (ejerciciodelafecha(ui.inicialdateEdit->date())!=ejerciciodelafecha(ui.finaldateEdit->date())
	&&(escuentadegasto(ui.subcuentalineEdit->text()) || 
	   escuentadeingreso(ui.subcuentalineEdit->text()) ))
	{
	       QMessageBox::warning( this, tr("Consultas de Mayor"),
				 tr("En consultas de cuentas de ingresos/gastos\n"
				      "las fechas han de pertenecer al mismo período"));
	   QString ejercicio=ejerciciodelafecha(ui.inicialdateEdit->date());
	   // QMessageBox::warning( this, tr("Consultas de Mayor"),ejercicio);
	   if (ejercicio.length()!=0) ui.finaldateEdit->setDate(finejercicio(ejercicio));
	      else ui.finaldateEdit->setDate(ui.inicialdateEdit->date());
        }
}



void cuadimprimayor::botonimprimepulsado()
{
  fechasok();
  bool resultado;
  if (ui.compactocheckBox->isChecked())
      resultado=generalatexmayor_compacto(ui.subcuentalineEdit->text(),
                      ui.finallineEdit->text(),
                      ui.inicialdateEdit->date(),ui.finaldateEdit->date());
      else
        resultado=generalatexmayor(ui.subcuentalineEdit->text(),
		   ui.finallineEdit->text(),
		   ui.inicialdateEdit->date(),ui.finaldateEdit->date());

  if (!resultado) {
     QMessageBox::warning( this, tr("IMPRIMIR MAYOR"),tr("ERROR: CONSULTA DE DATOS VACÍA"));
     return;
  }

   int valor=imprimelatex(tr("mayor"));
   if (valor==1)
       QMessageBox::warning( this, tr("IMPRIMIR MAYOR"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("IMPRIMIR MAYOR"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("IMPRIMIR MAYOR"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());
}


void cuadimprimayor::latex()
{
    fechasok();
    if (ui.compactocheckBox->isChecked())
        generalatexmayor_compacto(ui.subcuentalineEdit->text(),
                        ui.finallineEdit->text(),
                        ui.inicialdateEdit->date(),ui.finaldateEdit->date());
        else
          generalatexmayor(ui.subcuentalineEdit->text(),
             ui.finallineEdit->text(),
             ui.inicialdateEdit->date(),ui.finaldateEdit->date());
    int valor=editalatex("mayor");
    if (valor==1)
        QMessageBox::warning( this, tr("Mayor"),tr("PROBLEMAS al llamar al editor Latex"));

}


void cuadimprimayor::botonprevisualizapulsado()
{

  fechasok();
  bool resultado=false;

  if (ui.compactocheckBox->isChecked())
      resultado=generalatexmayor_compacto(ui.subcuentalineEdit->text(),
                      ui.finallineEdit->text(),
                      ui.inicialdateEdit->date(),ui.finaldateEdit->date());
      else
        resultado=generalatexmayor(ui.subcuentalineEdit->text(),
                   ui.finallineEdit->text(),
                   ui.inicialdateEdit->date(),ui.finaldateEdit->date());

   if (!resultado) {
       QMessageBox::warning( this, tr("VISTA PREVIA MAYOR"),tr("ERROR: Consulta de datos vacía"));
       return;
   }

   int valor=consultalatex(tr("mayor"));
   if (valor==1)
       QMessageBox::warning( this, tr("VISTA PREVIA MAYOR"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("VISTA PREVIA MAYOR"),
                             tr("PROBLEMAS al llamar a ")+visordvi());

}


void cuadimprimayor::ctainicialcambiada()
{
  ui.finallineEdit->setText(ui.subcuentalineEdit->text());
}


void cuadimprimayor::botonctafinalpulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.subcuentalineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (existesubcuenta(cadena2)) ui.finallineEdit->setText(cadena2);
       else ui.finallineEdit->setText("");
    delete labusqueda;
}


void cuadimprimayor::finedicfinsubcuenta()
{
 finalexpandepunto();
 if (ui.finallineEdit->text().length()<anchocuentas() && !cod_longitud_variable()) botonctafinalpulsado();
}



void cuadimprimayor::finalexpandepunto()
{
  ui.finallineEdit->setText(expandepunto(ui.finallineEdit->text(),anchocuentas()));
}


bool cuadimprimayor::generalatexmayor( QString qsubcuentaini, QString qsubcuentafinal, QDate fechainicial, QDate fechafinal )
{

    // -----------------------------------------------------------------------------------
   if (QString::compare(qsubcuentaini,qsubcuentafinal)>0) return false;

   QStringList l = basedatos::instancia()->auxiliares_entre_codigos(qsubcuentaini,qsubcuentafinal);
   if (l.count()==0) return false;


   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("mayor.tex");
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero(pasa);
    if (! fichero.open( QIODevice::WriteOnly ) ) return false;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    int indice=0;
    QString subctaactual=l.at(indice);
    int lineas=0;
    QSqlQuery query;
    //if (!esauxiliar(subctaactual))
    //   subctaactual=subcuentaposterior(subctaactual);
    while (indice<l.count())
      {
       while (1)
          {
           if (indice>=l.count()) break;
           /*cambiado a partir de aquí*/
           // Imprimimos cabecera del listado con nombre de empresa
           // imprimimos codigo de subcuenta, fecha inicial, fecha final
           // imprimimos fecha actual
           qlonglong num = basedatos::instancia()->selectCountasientodiariofechascuenta(fechainicial, fechafinal, subctaactual);
           if (num==0 && ui.csaldocheckBox->isChecked() && !(subctaactual.length()==0))
             {
               indice++;
               if (indice<l.count()) subctaactual=l.at(indice);
               continue;
             }
           if (ui.saltar_apertura_checkBox->isChecked() && basedatos::instancia()->vacios_y_solo_apertura_fechas_cuenta(fechainicial, fechafinal, subctaactual)) {
                 indice++;
                 if (indice<l.count()) subctaactual=l.at(indice);
                 continue;
             }
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
           stream << "\\end{center}" << "\n";
           stream << "\n";
           // buscar descripción de la cuenta
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << tr("MAYOR CUENTA: ") << subctaactual <<  "}}" << "\n";
           stream << "\\end{center}" << "\n";
           
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << filtracad(descripcioncuenta(subctaactual)) << "}}" << "\n";
           stream << "\\end{center}" << "\n";

           stream << "{\\textbf {" << tr("Extracto de ") << fechainicial.toString("dd.MM.yyyy") <<
	                         tr(" a ") << fechafinal.toString("dd.MM.yyyy") << "}}" << "\n";

           // cuerpo de listado (tablas)
           stream << "\\begin{center}\n";
           stream << "\\begin{tabular}{|l|r|p{8cm}|r|r|r|}\n";
           stream << "\\hline\n";
           stream << "\\rowcolor{gray!30}\n";
           stream << "{\\textbf {\\scriptsize " << tr("FECHA") << "}} & {\\textbf {\\scriptsize "
                   << tr("ASTO.") << "}} & {\\textbf {\\scriptsize " <<
             tr("CONCEPTO") << "}} & {\\textbf {\\scriptsize " << tr("DEBE")
             << "}} & {\\textbf {\\scriptsize " << tr("HABER") <<
             "}} & {\\textbf {\\scriptsize " << tr("SALDO") << "}} \\\\\n";
           stream << "\\hline\n";

            if (num==0)
            {
                indice++;
                if (indice<l.count()) subctaactual=l.at(indice);
                lineas=0;
                if (indice>=l.count() || subctaactual.length()==0) {
                    stream << "\\end{tabular}\n";
                    stream << "\\end{center}\n";
                    break;
                   }
                   else
                      {
                        stream << "\\end{tabular}\n";
                        stream << "\\end{center}\n";
                        stream << "\\newpage\n";
                      }
                continue;
            }
     
	   
           // calculamos saldo inicial
	   // calculamos acumulado de saldo inicial
           // primero observamos si el ejercicio anterior está cerrado
           QString ejercicio=ejerciciodelafecha(fechainicial);
           QDate fechainiejercicio=inicioejercicio(ejercicio);
           QDate fechaejercicioanterior=fechainiejercicio.addDays(-1);
           QString ejercicioanterior=ejerciciodelafecha(fechaejercicioanterior);
           if (ejerciciocerrado(ejercicioanterior) || escuentadegasto(subctaactual) || 
	     escuentadeingreso(subctaactual))		
            {
                query = basedatos::instancia()->selectSumdebesumhaberdiariofechascuenta(fechainiejercicio, fechainicial, subctaactual);
            }
           else query = basedatos::instancia()->selectSumdebesumhaberdiariofechacuenta(fechainicial, subctaactual);

           double saldoin=0;
           double sumadebe=0;
           double sumahaber=0;
           double sumadebeextracto=0;
           double sumahaberextracto=0;
	   double saldo=0;
	   QString cadsaldo="";
           if ( (query.isActive()) && (query.first()) )
              {
               saldoin=query.value(0).toDouble()-query.value(1).toDouble();
               //sumadebe=query.value(0).toDouble();
               //sumahaber=query.value(1).toDouble();
               if (saldoin>0) sumadebe=saldoin;
               if (saldoin<0) sumahaber=-1*saldoin;
              }
	 
         query = basedatos::instancia()->select5Diariofechascuentaorderfechapase(fechainicial, fechafinal, subctaactual);
	     saldo=saldoin;
         QString concepto;
         if (query.isActive())
         while (query.next())
	     {
	      lineas++;
              stream << "{\\scriptsize " << query.value(0).toDate().toString("dd.MM.yyyy") << "} & ";
	      // Asiento ------------------------------------------------------------------------------------------
              stream << "{\\scriptsize " << query.value(1).toString() << "} & ";
	      // ---------------------------------------------------------------------------------------------------
	      concepto=query.value(2).toString();
          //concepto=filtracad(concepto);
          QString cort=mcortaa(concepto,53);
          if (cort!=concepto) concepto=cort+"\\#"; else concepto=cort;
          stream << "{\\scriptsize " << filtracad(concepto) << "} & ";
	      double valdebe=query.value(3).toDouble();
	      if (valdebe>0.0001 || valdebe<-0.0001)
                  stream << "{ \\scriptsize "
                         << formateanumerosep(query.value(3).toDouble(),comadecimal,decimales)
                         << "} & ";
                else stream << " & ";
	      double valhaber=query.value(4).toDouble();
	      if (valhaber>0.0001 || valhaber<-0.0001)
                  stream << "{\\scriptsize "
                         << formateanumerosep(query.value(4).toDouble(),comadecimal,decimales)
                         << "} & ";
	        else stream << " & ";
	      saldo+=query.value(3).toDouble()-query.value(4).toDouble();
	      sumadebe+=query.value(3).toDouble();
	      sumahaber+=query.value(4).toDouble();
	      sumadebeextracto+=query.value(3).toDouble();
	      sumahaberextracto+=query.value(4).toDouble();
	      cadsaldo=formateanumerosep(saldo,comadecimal,decimales);
              stream << "{\\scriptsize " << cadsaldo << "} \\\\ \n ";
                      stream << "\\hline\n";
	      if (lineas==LINEASPAGINA)
                        {
	                  lineas=0;
                          stream  << "\\multicolumn{3}{|r|}{\\scriptsize " << tr("Sumas y siguen ...") <<
                                  "} & {\\scriptsize " <<
                               formateanumerosep(sumadebe,comadecimal,decimales);
                          stream << "} & {\\scriptsize " << formateanumerosep(sumahaber,comadecimal,decimales) << "} & \\\\";
                          stream << "\\hline\n";
       	                  stream << "\\end{tabular}\n";
                          stream << "\\end{center}\n";
                          stream << "\\newpage\n";
		  
                         // stream << "\\begin{flushleft}" << "\n";
                         // stream << "\\today" << "\n";
                         // stream << "\\newline" << "\n";
                         // stream << "\\end{flushleft}" << "\n";
     
                         stream << "\\begin{center}" << "\n";
                         stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
                         stream << "\\end{center}" << "\n";
                         stream << "\n";
                         // buscar descripción de la cuenta
                        stream << "\\begin{center}" << "\n";
                        stream << "{\\Large \\textbf {" << tr("MAYOR SUBCUENTA: ") << subctaactual <<  "}}" << "\n";
                        stream << "\\end{center}" << "\n";
           
                        stream << "\\begin{center}" << "\n";
                        stream << "{\\Large \\textbf {" << filtracad(descripcioncuenta(subctaactual)) << "}}" << "\n";
                        stream << "\\end{center}" << "\n";

                        stream << "{\\textbf {" << tr("Extracto de ") << fechainicial.toString("dd.MM.yyyy") <<
	                         tr(" a ") << fechafinal.toString("dd.MM.yyyy") << "}}" << "\n";

                       // cuerpo de listado (tablas)
                       stream << "\\begin{center}\n";
                       stream << "\\begin{tabular}{|l|r|p{8cm}|r|r|r|}\n";
                       stream << "\\hline\n";
                       stream << "\\rowcolor{gray!30}\n";
                       stream << "{\\textbf {\\scriptsize " << tr("FECHA")
                               << "}} & {\\textbf {\\scriptsize " << tr("ASTO.") <<
                        "}} & {\\textbf {\\scriptsize " << tr("CONCEPTO") <<
                        "}} & {\\textbf {\\scriptsize " << tr("DEBE") <<
                        "}} & {\\textbf {\\scriptsize " << tr("HABER") <<
                        "}} & {\\textbf {\\scriptsize " << tr("SALDO") << "}} \\\\\n";
                       stream << "\\hline\n";
  	       }
	     }
         stream  << "\\multicolumn{3}{|r|}{\\scriptsize " << tr("Sumas ...") << "} & {\\scriptsize " <<
              formateanumerosep(sumadebe,comadecimal,decimales);
         stream << "} & {\\scriptsize " << formateanumerosep(sumahaber,comadecimal,decimales) << "} & \\\\";
           stream << "\\hline\n";
           stream  << "\\multicolumn{3}{|r|}{\\scriptsize " << tr("Sumas extracto...") << "} & {\\scriptsize "
                   << formateanumerosep(sumadebeextracto,comadecimal,decimales);
           stream << "} & {\\scriptsize " << formateanumerosep(sumahaberextracto,comadecimal,decimales) << "} & \\\\";
           stream << "\\hline\n";
           // final de la tabla
           stream << "\\end{tabular}\n";
           stream << "\\end{center}\n";
           stream << "\\newpage\n";
           break;
          }
       indice++;
       // if (subctaactual.length()==0) break;
       if (indice>=l.count()) break;
       subctaactual=l.at(indice); //subcuentaposterior(subctaactual);
       lineas=0;
       // if (subctaactual.length()==0) break;
       }
    
    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();
    return true;

}



void cuadimprimayor::copia()
{
    QStringList l = basedatos::instancia()->auxiliares_entre_codigos(ui.subcuentalineEdit->text(),
                                                                     ui.finallineEdit->text());
    int indice=0;
    QString subctaactual=l.at(indice);
    // QString subctaactual=ui.subcuentalineEdit->text();
    // QString qsubcuentafinal=ui.finallineEdit->text();
    QSqlQuery query;
    // if (!esauxiliar(subctaactual))
    //   subctaactual=subcuentaposterior(subctaactual);
    int lineas=0;
    QString contenido;
    QDate fechainicial=ui.inicialdateEdit->date();
    QDate fechafinal=ui.finaldateEdit->date();
    while (indice<l.count())
      {
       while (1)
          {
           // Imprimimos cabecera del listado con nombre de empresa
           // imprimimos codigo de subcuenta, fecha inicial, fecha final
           // imprimimos fecha actual
           qlonglong num = basedatos::instancia()->selectCountasientodiariofechascuenta(fechainicial, fechafinal, subctaactual);
           if (num==0 && !(subctaactual.length()==0))
             {
               indice++;
               if (indice>=l.count()) break;
               subctaactual=l.at(indice);
               continue;
             }
           if (lineas==0)
             {
              contenido+=nombreempresa();
              contenido+= "\n";
             }
           contenido+= "\n";
           // buscar descripción de la cuenta
           contenido+= tr("MAYOR SUBCUENTA: ") +"\t"+ subctaactual + "\t";
           contenido += filtracad(descripcioncuenta(subctaactual)) + "\n";
           contenido += "\n";

           contenido+=tr("Extracto") + "\t" + fechainicial.toString("dd-MM-yyyy")+
                                 " \t " + fechafinal.toString("dd-MM-yyyy") + "\n";

           // cuerpo de listado (tablas)
           contenido += tr("FECHA") + "\t" + tr("ASIENTO") + "\t" +
             tr("CONCEPTO") + "\t" + tr("DEBE") + "\t" + tr("HABER") +
             "\t" + tr("SALDO")+ "\n";

            if (num==0)
               {
                indice++;
                if (indice<l.count()) subctaactual=l.at(indice);
                lineas=0;
                if (indice>=l.count()) {
                    contenido += "\n";
                    break;
                   }
                   else
                   {
                    contenido += "\n";
                    contenido += "\n";
                   }
                continue;
               }


           // calculamos saldo inicial
           // calculamos acumulado de saldo inicial
           // primero observamos si el ejercicio anterior está cerrado
           QString ejercicio=ejerciciodelafecha(fechainicial);
           QDate fechainiejercicio=inicioejercicio(ejercicio);
           QDate fechaejercicioanterior=fechainiejercicio.addDays(-1);
           QString ejercicioanterior=ejerciciodelafecha(fechaejercicioanterior);
           if (ejerciciocerrado(ejercicioanterior) || escuentadegasto(subctaactual) ||
             escuentadeingreso(subctaactual))
            {
                query = basedatos::instancia()->selectSumdebesumhaberdiariofechascuenta(fechainiejercicio, fechainicial, subctaactual);
            }
           else query = basedatos::instancia()->selectSumdebesumhaberdiariofechacuenta(fechainicial, subctaactual);

           double saldoin=0;
           double sumadebe=0;
           double sumahaber=0;
           double sumadebeextracto=0;
           double sumahaberextracto=0;
           double saldo=0;
           QString cadsaldo="";
           if ( (query.isActive()) && (query.first()) )
              {
               saldoin=query.value(0).toDouble()-query.value(1).toDouble();
               sumadebe=query.value(0).toDouble();
               sumahaber=query.value(1).toDouble();
              }

         query = basedatos::instancia()->select5Diariofechascuentaorderfechapase(fechainicial, fechafinal, subctaactual);
             saldo=saldoin;
         QString concepto;
         if (query.isActive())
         while (query.next())
             {
              lineas++;
              contenido += query.value(0).toDate().toString("dd-MM-yyyy") + "\t";
              // Asiento ------------------------------------------------------------------------------------------
              contenido += query.value(1).toString() += "\t";
              // ---------------------------------------------------------------------------------------------------
              concepto=query.value(2).toString();
              contenido += concepto +="\t";
              double valdebe=query.value(3).toDouble();
              if (valdebe>0.0001 || valdebe<-0.0001)
                 contenido+=formateanumerosep(query.value(3).toDouble(),comadecimal,decimales) + "\t";
                else contenido += "\t";
              double valhaber=query.value(4).toDouble();
              if (valhaber>0.0001 || valhaber<-0.0001)
                  contenido+=formateanumerosep(query.value(4).toDouble(),comadecimal,decimales) + "\t";
                else contenido += "\t";
              saldo+=query.value(3).toDouble()-query.value(4).toDouble();
              sumadebe+=query.value(3).toDouble();
              sumahaber+=query.value(4).toDouble();
              sumadebeextracto+=query.value(3).toDouble();
              sumahaberextracto+=query.value(4).toDouble();
              cadsaldo=formateanumerosep(saldo,comadecimal,decimales);
              contenido += cadsaldo + "\n";
             }
           contenido += "\t\t"+ tr("Sumas ...") + "\t" +
              formateanumerosep(sumadebe,comadecimal,decimales);
           contenido += "\t" + formateanumerosep(sumahaber,comadecimal,decimales) + "\n";
           contenido += "\t\t" + tr("Sumas extracto...") + "\t"
                   + formateanumerosep(sumadebeextracto,comadecimal,decimales);
           contenido += "\t" + formateanumerosep(sumahaberextracto,comadecimal,decimales) + "\n";
           contenido += "\n";
           // final de la tabla
           break;
          }
       indice++;
       if (indice>=l.count()) break;
       subctaactual=l.at(indice);
       lineas=0;
       // if (subctaactual.length()==0) break;

       }

    QClipboard *cb = QApplication::clipboard();
    cb->setText(contenido);
    QMessageBox::information( this, tr("Consulta de mayor"),
                              tr("Se ha pasado el contenido al portapapeles") );

}




bool cuadimprimayor::generalatexmayor_compacto( QString qsubcuentaini,
                                                QString qsubcuentafinal,
                                                QDate fechainicial,
                                                QDate fechafinal )
{

    // -----------------------------------------------------------------------------------
   if (QString::compare(qsubcuentaini,qsubcuentafinal)>0) return false;

   QStringList l = basedatos::instancia()->auxiliares_entre_codigos(qsubcuentaini, qsubcuentafinal);
   if (l.count()==0) return false;

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("mayor.tex");
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QString pasa;
   if (eswindows()) pasa=QFile::encodeName(qfichero);
       else pasa=qfichero;
   QFile fichero(pasa);
    if (! fichero.open( QIODevice::WriteOnly ) ) return false;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();



    int indice=0;

    QString subctaactual=l.at(indice);

    int lineas=0;
    QSqlQuery query;
    //if (!esauxiliar(subctaactual))
    //   subctaactual=subcuentaposterior(subctaactual);
    while (indice<l.count())
      {
       while (1)
          {
           /*cambiado a partir de aquí*/
           // Imprimimos cabecera del listado con nombre de empresa
           // imprimimos codigo de subcuenta, fecha inicial, fecha final
           // imprimimos fecha actual
           qlonglong num = basedatos::instancia()->selectCountasientodiariofechascuenta(fechainicial, fechafinal, subctaactual);
           if (num==0 && indice<l.count())
             {
               indice++;
               if (indice<l.count()) subctaactual=l.at(indice);
                  else break;
               continue;
             }
           if (ui.saltar_apertura_checkBox->isChecked() && basedatos::instancia()->vacios_y_solo_apertura_fechas_cuenta(fechainicial, fechafinal, subctaactual)) {
               indice++;
               if (indice<l.count()) subctaactual=l.at(indice);
               else break;
               continue;
           }
           if (lineas==0)
             {
              stream << "\\begin{center}" << "\n";
              stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
              stream << "\\end{center}" << "\n";
              stream << "\n";
             }
           // buscar descripción de la cuenta
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << tr("MAYOR CUENTA: ") << subctaactual <<  "}}" << "\n";
           stream << "\\end{center}" << "\n";

           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << filtracad(descripcioncuenta(subctaactual)) << "}}" << "\n";
           stream << "\\end{center}" << "\n";

           stream << "{\\textbf {" << tr("Extracto de ") << fechainicial.toString("dd.MM.yyyy") <<
                                 tr(" a ") << fechafinal.toString("dd.MM.yyyy") << "}}" << "\n";

           // cuerpo de listado (tablas)
           stream << "\\begin{center}\n";
           stream << "\\begin{tabular}{|l|r|p{7.5cm}|r|r|r|}\n";
           stream << "\\hline\n";
           stream << "\\rowcolor{gray!30}\n";
           stream << "{\\textbf {\\scriptsize " << tr("FECHA") << "}} & {\\textbf {\\scriptsize " << tr("ASTO.") << "}} & {\\textbf {\\scriptsize " <<
             tr("CONCEPTO") << "}} & {\\textbf {\\scriptsize " << tr("DEBE") << "}} & {\\textbf {\\scriptsize " << tr("HABER") <<
             "}} & {\\textbf {\\scriptsize " << tr("SALDO") << "}} \\\\\n";
           stream << "\\hline\n";

            if (num==0)
               {
                indice++;
                if (indice<l.count()) subctaactual=l.at(indice);
                lineas=0;
                if (subctaactual.length()==0 || indice>=l.count()) {
                    stream << "\\end{tabular}\n";
                    stream << "\\end{center}\n";
                    break;
                   }
                   else
                   {
                    stream << "\\end{tabular}\n";
                    stream << "\\end{center}\n";
                    stream << "\\newpage\n";
                   }
                continue;
               }


           // calculamos saldo inicial
           // calculamos acumulado de saldo inicial
           // primero observamos si el ejercicio anterior está cerrado
           QString ejercicio=ejerciciodelafecha(fechainicial);
           QDate fechainiejercicio=inicioejercicio(ejercicio);
           QDate fechaejercicioanterior=fechainiejercicio.addDays(-1);
           QString ejercicioanterior=ejerciciodelafecha(fechaejercicioanterior);
           if (ejerciciocerrado(ejercicioanterior) || escuentadegasto(subctaactual) ||
             escuentadeingreso(subctaactual))
            {
                query = basedatos::instancia()->selectSumdebesumhaberdiariofechascuenta(fechainiejercicio, fechainicial, subctaactual);
            }
           else query = basedatos::instancia()->selectSumdebesumhaberdiariofechacuenta(fechainicial, subctaactual);

           double saldoin=0;
           double sumadebe=0;
           double sumahaber=0;
           double sumadebeextracto=0;
           double sumahaberextracto=0;
           double saldo=0;
           QString cadsaldo="";
           if ( (query.isActive()) && (query.first()) )
              {
               saldoin=query.value(0).toDouble()-query.value(1).toDouble();
               sumadebe=query.value(0).toDouble();
               sumahaber=query.value(1).toDouble();
              }

         query = basedatos::instancia()->select5Diariofechascuentaorderfechapase(fechainicial, fechafinal, subctaactual);
             saldo=saldoin;
         QString concepto;
         if (query.isActive())
         while (query.next())
             {
              lineas++;
              stream << "\\scriptsize "<< query.value(0).toDate().toString("dd.MM.yyyy") << " & ";
              // Asiento ------------------------------------------------------------------------------------------
              stream << "\\scriptsize "<< query.value(1).toString() << " & ";
              // ---------------------------------------------------------------------------------------------------
              concepto=query.value(2).toString();

              QString cort=mcortaa(concepto,53);
              if (cort!=concepto) concepto=cort+"\\#"; else concepto=cort;
              stream << "{\\scriptsize " << filtracad(concepto) << "} & ";

              double valdebe=query.value(3).toDouble();
              if (valdebe>0.0001 || valdebe<-0.0001)
                 stream << "\\scriptsize "<< formateanumerosep(query.value(3).toDouble(),comadecimal,decimales) << " & ";
                else stream << " & ";
              double valhaber=query.value(4).toDouble();
              if (valhaber>0.0001 || valhaber<-0.0001)
                   stream << "\\scriptsize "<< formateanumerosep(query.value(4).toDouble(),comadecimal,decimales) << " & ";
                else stream << " & ";
              saldo+=query.value(3).toDouble()-query.value(4).toDouble();
              sumadebe+=query.value(3).toDouble();
              sumahaber+=query.value(4).toDouble();
              sumadebeextracto+=query.value(3).toDouble();
              sumahaberextracto+=query.value(4).toDouble();
              cadsaldo=formateanumerosep(saldo,comadecimal,decimales);
              stream << "\\scriptsize "<< cadsaldo << " \\\\ \n ";
                      stream << "\\hline\n";
              if (lineas==LINEASPAGINA)
                        {
                          lineas=0;
                          stream  << "\\multicolumn{3}{|r|}{" << tr("Sumas y siguen ...") << "} & " <<
                               formateanumerosep(sumadebe,comadecimal,decimales);
                          stream << " & " << formateanumerosep(sumahaber,comadecimal,decimales) << "& \\\\";
                          stream << "\\hline\n";
                          stream << "\\end{tabular}\n";
                          stream << "\\end{center}\n";
                          stream << "\\newpage\n";

                         // stream << "\\begin{flushleft}" << "\n";
                         // stream << "\\today" << "\n";
                         // stream << "\\newline" << "\n";
                         // stream << "\\end{flushleft}" << "\n";

                         stream << "\\begin{center}" << "\n";
                         stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
                         stream << "\\end{center}" << "\n";
                         stream << "\n";
                         // buscar descripción de la cuenta
                        stream << "\\begin{center}" << "\n";
                        stream << "{\\Large \\textbf {" << tr("MAYOR SUBCUENTA: ") << subctaactual <<  "}}" << "\n";
                        stream << "\\end{center}" << "\n";

                        stream << "\\begin{center}" << "\n";
                        stream << "{\\Large \\textbf {" << filtracad(descripcioncuenta(subctaactual)) << "}}" << "\n";
                        stream << "\\end{center}" << "\n";

                        stream << "{\\textbf {" << tr("Extracto de ") << fechainicial.toString("dd.MM.yyyy") <<
                                 tr(" a ") << fechafinal.toString("dd.MM.yyyy") << "}}" << "\n";

                       // cuerpo de listado (tablas)
                       stream << "\\begin{center}\n";
                       stream << "\\begin{tabular}{|l|r|p{7.5cm}|r|r|r|}\n";
                       stream << "\\hline\n";
                       stream << "\\rowcolor{gray!30}\n";
                       stream << "{\\textbf {\\scriptsize " << tr("FECHA") << "}} & {\\textbf {\\scriptsize " << tr("ASTO.") <<
                        "}} & {\\textbf {\\scriptsize " << tr("CONCEPTO") << "}} & {\\textbf {\\scriptsize " << tr("DEBE") <<
                        "}} & {\\textbf {\\scriptsize " << tr("HABER") << "}} & {\\textbf {\\scriptsize " << tr("SALDO") << "}} \\\\\n";
                       stream << "\\hline\n";
               }
             }
           stream  << "\\multicolumn{3}{|r|}{" << tr("\\scriptsize Sumas ...") << "} & \\scriptsize " <<
              formateanumerosep(sumadebe,comadecimal,decimales);
           stream << " & \\scriptsize " << formateanumerosep(sumahaber,comadecimal,decimales) << "& \\\\";
           stream << "\\hline\n";
           stream  << "\\multicolumn{3}{|r|}{" << tr("\\scriptsize Sumas extracto...") << "} & \\scriptsize "
                   << formateanumerosep(sumadebeextracto,comadecimal,decimales);
           stream << " & \\scriptsize " << formateanumerosep(sumahaberextracto,comadecimal,decimales) << "& \\\\";
           stream << "\\hline\n";
           // final de la tabla
           stream << "\\end{tabular}\n";
           stream << "\\end{center}\n";
           // stream << "\\newpage\n";
           break;
          }
       indice++;
       if (indice>=l.count()) break;
       subctaactual=l.at(indice);
       // comprobamos que tenga líneas
       if (lineas>LINEASPAGINA-9)
          { lineas=0; stream << "\\newpage\n";}
       else { stream << "\\vspace{0.15cm}\n"; lineas+=8;}
       // if (subctaactual.length()==0) break;

       }

    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

    return true;

}


void cuadimprimayor::generaxmlmayor( QString qsubcuentaini, QString qsubcuentafinal,
                                     QDate fechainicial, QDate fechafinal )
{
 // -----------------------------------------------------------------------------------
 if (QString::compare(qsubcuentaini,qsubcuentafinal)>0) return;

 QProgressDialog progreso(tr("Cargando información ..."), 0, 0, 3);
  progreso.setMinimumDuration(100);
  // progreso.setWindowModality(Qt::WindowModal);
  progreso.setValue(1);
  QCoreApplication::processEvents();


 QDomDocument doc("Mayor");
 QDomElement root = doc.createElement("Mayor");
 doc.appendChild(root);

 addElementoTextoDom(doc,root,"NombreEmpresa",filtracadxml(nombreempresa()));

 QStringList l = basedatos::instancia()->auxiliares_entre_codigos(qsubcuentaini,
                                                                  qsubcuentafinal);
 int indice=0;
 if (l.count()==0) return;
 QString subctaactual=l.at(indice);

 int lineas=0;
 QSqlQuery query;
 //if (!esauxiliar(subctaactual))
 //      subctaactual=subcuentaposterior(subctaactual);

 while (indice<l.count())
      {
       while (1)
          {
            QDomElement tag = doc.createElement("Cuenta");
            root.appendChild(tag);
            QDomElement tag2 = doc.createElement("CabeceraCuenta");
            tag.appendChild(tag2);
            addElementoTextoDom(doc,tag2,"CuentaAuxiliar",filtracadxml(subctaactual));
            addElementoTextoDom(doc,tag2,"DescripCuenta",filtracadxml(descripcioncuenta(subctaactual)));
            addElementoTextoDom(doc,tag2,"FechaInicial",filtracadxml(fechainicial.toString("yyyy/MM/dd")));
            addElementoTextoDom(doc,tag2,"FechaFinal",filtracadxml(fechafinal.toString("yyyy/MM/dd")));
           // calculamos saldo inicial
           // calculamos acumulado de saldo inicial
           // primero observamos si el ejercicio anterior está cerrado
           QString ejercicio=ejerciciodelafecha(fechainicial);
           QDate fechainiejercicio=inicioejercicio(ejercicio);
           QDate fechaejercicioanterior=fechainiejercicio.addDays(-1);
           QString ejercicioanterior=ejerciciodelafecha(fechaejercicioanterior);
           if (ejerciciocerrado(ejercicioanterior) || escuentadegasto(subctaactual) ||
             escuentadeingreso(subctaactual))
            {
                query = basedatos::instancia()->selectSumdebesumhaberdiariofechascuenta(fechainiejercicio, fechainicial, subctaactual);
            }
           else query = basedatos::instancia()->selectSumdebesumhaberdiariofechacuenta(fechainicial, subctaactual);

           double saldoin=0;
           double sumadebe=0;
           double sumahaber=0;
           double sumadebeextracto=0;
           double sumahaberextracto=0;
           double saldo=0;
           QString cadsaldo="";
           if ( (query.isActive()) && (query.first()) )
              {
               saldoin=query.value(0).toDouble()-query.value(1).toDouble();
               sumadebe=query.value(0).toDouble();
               sumahaber=query.value(1).toDouble();
              }
        QDomElement tag11 = doc.createElement("DetalleCuenta");
        tag.appendChild(tag11);

         query = basedatos::instancia()->select5Diariofechascuentaorderfechapase(fechainicial, fechafinal, subctaactual);
             saldo=saldoin;
         QString concepto;
         if (saldoin>0.0001 || saldoin<-0.0001)
            {
              QDomElement tag3 = doc.createElement("Apunte");
              tag11.appendChild(tag3);
              addElementoTextoDom(doc,tag3,"FechaAsiento",filtracadxml(fechainicial.toString("yyyy/MM/dd")));
              // Asiento ------------------------------------------------------------------------------------------
                addElementoTextoDom(doc,tag3,"Asiento","");
              // ---------------------------------------------------------------------------------------------------
              concepto=tr("Saldo inicial");
              addElementoTextoDom(doc,tag3,"Concepto",filtracadxml(concepto));
              if (saldoin>0)
                  addElementoTextoDom(doc,tag3,"Debe",formateanumero(saldoin,comadecimal,decimales));
                else addElementoTextoDom(doc,tag3,"Debe","");
              if (saldoin<0)
                 addElementoTextoDom(doc,tag3,"Haber",formateanumero(-saldoin,comadecimal,decimales));
                else addElementoTextoDom(doc,tag3,"Haber","");
              cadsaldo=formateanumero(saldo,comadecimal,decimales);
              addElementoTextoDom(doc,tag3,"Saldo",filtracadxml(cadsaldo));
              addElementoTextoDom(doc,tag3,"Documento","");
            }
         if (query.isActive())
         while (query.next())
             {
              lineas++;
              QDomElement tag3 = doc.createElement("Apunte");
              tag11.appendChild(tag3);
              addElementoTextoDom(doc,tag3,"FechaAsiento",filtracadxml(query.value(0).toDate().toString("yyyy/MM/dd")));
              // Asiento ------------------------------------------------------------------------------------------
              addElementoTextoDom(doc,tag3,"Asiento",filtracadxml(query.value(1).toString()));
              // ---------------------------------------------------------------------------------------------------
              concepto=query.value(2).toString();
              // concepto=filtracad(concepto);
              addElementoTextoDom(doc,tag3,"Concepto",filtracadxml(concepto));
              double valdebe=query.value(3).toDouble();
              if (valdebe>0.0001 || valdebe<-0.0001)
                 addElementoTextoDom(doc,tag3,"Debe",filtracadxml(
                         formateanumero(query.value(3).toDouble(),comadecimal,decimales)));
                else addElementoTextoDom(doc,tag3,"Debe","");

              double valhaber=query.value(4).toDouble();
              if (valhaber>0.0001 || valhaber<-0.0001)
                 addElementoTextoDom(doc,tag3,"Haber",filtracadxml(
                         formateanumero(query.value(4).toDouble(),comadecimal,decimales)));
                else addElementoTextoDom(doc,tag3,"Haber","");
              saldo+=query.value(3).toDouble()-query.value(4).toDouble();
              sumadebe+=query.value(3).toDouble();
              sumahaber+=query.value(4).toDouble();
              sumadebeextracto+=query.value(3).toDouble();
              sumahaberextracto+=query.value(4).toDouble();
              cadsaldo=formateanumero(saldo,comadecimal,decimales);
              addElementoTextoDom(doc,tag3,"Saldo",filtracadxml(cadsaldo));
              addElementoTextoDom(doc,tag3,"Documento",filtracadxml(query.value(5).toString()));
             }
           break;
         }
       indice++;
       if (indice>=l.count()) break;
       subctaactual=l.at(indice);
       lineas=0;
       //if (subctaactual.length()==0) break;
     }


    QString qficherologo=dirtrabajo();
    qficherologo.append(QDir::separator());
    QString rutagraf=qficherologo+"logo.png";
    //if (eswindows()) rutagraf=QFile::encodeName(rutagraf);
    // generamos imagen del logo
    QString imagen=basedatos::instancia()->logo_empresa();
    QPixmap foto;
    if (imagen.isEmpty()) imagen = logodefecto();
    if (imagen.length()>0)
       {
        QByteArray byteshexa;
        byteshexa.append ( imagen.toUtf8() );
        QByteArray bytes;
        bytes=bytes.fromHex ( byteshexa );
        foto.loadFromData ( bytes, "PNG");
        foto.save(rutagraf,"PNG");
       }


   QString xml = doc.toString();

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("mayor.xml");
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero(pasa);
   if (! fichero.open( QIODevice::WriteOnly ) ) return;
   QTextStream stream( &fichero );
   stream.setEncoding(QStringConverter::Utf8);

   stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  // bssstream << "<?xml-stylesheet type=\"text/xsl\" href=\"plan2html.xslt\"?>\n";

  // ------------------------------------------------------------------------------------
  stream << xml;

  fichero.close();


  progreso.setValue(2);
  QCoreApplication::processEvents();


  QString cadfichjasper=trayreport();
  cadfichjasper.append(QDir::separator());
  cadfichjasper+="mayor.jasper";

  QString cadpdf=dirtrabajo();
  cadpdf.append(QDir::separator());
  cadpdf+="mayor.pdf";



  informe_jasper_xml(cadfichjasper, pasa,
                     "/Mayor/Cuenta/DetalleCuenta/Apunte", cadpdf,
                     rutagraf);

  progreso.setValue(3);
  QCoreApplication::processEvents();

}
