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

#include "consinput.h"
#include "funciones.h"
#include "basedatos.h"
#include "tabla_asientos.h"
#include "introci.h"
#include "editarasiento.h"
#include "buscasubcuenta.h"
#include <QMessageBox>

consinput::consinput(QString qusuario) : QDialog() {
  ui.setupUi(this);


  QDate fechaactual;
  QString cadfechaactual;
  comadecimal=haycomadecimal();
  decimales=haydecimales();
  cadfechaactual=fechaactual.currentDate().toString("yyyy-MM-dd");

    ui.inicialdateEdit->setDate( basedatos::instancia()->selectAperturaejerciciosaperturacierre(cadfechaactual, cadfechaactual) );
    ui.finaldateEdit->setDate( basedatos::instancia()->selectCierreejerciciosaperturacierre(cadfechaactual, cadfechaactual) );

 QStringList columnas;
 columnas << tr("FECHA") << tr("ASTO.") << tr("CONCEPTO");
 columnas << tr("DEBE") << tr("HABER") << tr("SALDO");
 columnas << tr("DOCUMENTO") << tr("SUBCUENTA") << tr("CI");

 ui.table->setColumnCount(9);

 ui.table->setEditTriggers ( QAbstractItemView::NoEditTriggers );

 ui.table->setHorizontalHeaderLabels(columnas);

 ui.table->setColumnWidth(0,80);
 ui.table->setColumnWidth(1,60);
 ui.table->setColumnWidth(2,300);
 ui.table->setColumnWidth(3,80);
 ui.table->setColumnWidth(4,80);
 ui.table->setColumnWidth(5,80);
 ui.table->setColumnWidth(6,125);
 ui.table->setColumnWidth(7,80);
 ui.table->setColumnWidth(8,125);

 connect(ui.buscapushButton,SIGNAL(clicked()),this,SLOT(buscapulsado()));
 connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(fijadescripciones()));
 connect(ui.refrescarpushButton,SIGNAL(clicked()),this,SLOT(refrescar()));
 connect(ui.editarpushButton,SIGNAL(clicked()),this,SLOT(editarpulsado()));
 connect(ui.imprimirpushButton,SIGNAL(clicked()),this,SLOT(imprimir()));

 connect(ui.visdocpushButton,SIGNAL(clicked()),SLOT(visdoc()));
 connect(ui.copiarpushButton,SIGNAL(clicked()),SLOT(copiar()));
 connect(ui.texpushButton,SIGNAL(clicked()),SLOT(latex()));
 connect(ui.buscactapushButton, SIGNAL(clicked()),SLOT(botoncuentapulsado()));
 connect(ui.cuentalineEdit,SIGNAL(editingFinished()),SLOT(ctaexpandepunto()));
 connect(ui.consultarpushButton,SIGNAL(clicked()),this,SLOT(consultarpulsado()));

 usuario=qusuario;

#ifdef NOEDITTEX
 ui.texpushButton->hide();
#endif

}


void consinput::pasadatos( QString qci, QDate qfecha )
{
 ui.cilineEdit->setText(qci);
   QString ejercicio=ejerciciodelafecha(qfecha);
   QDate fechaini=inicioejercicio(ejercicio);
   QDate fechafin=finejercicio(ejercicio);
   ui.inicialdateEdit->setDate(fechaini);
   ui.finaldateEdit->setDate(fechafin);
   if (ui.cilineEdit->text().length()>0) refrescar();
}

void consinput::buscapulsado()
{
 introci *c = new introci();
 c->hazparafiltro();
 c->pasaci(ui.cilineEdit->text());
 c->exec();
 ui.cilineEdit->setText(c->cadenaci());
 delete c;
 refrescar();
}


void consinput::fijadescripciones()
{
  ui.table->setRowCount(0);

  QString codigo=ui.cilineEdit->text();
  QString cadena,descripcion;
  QString qnivel=0;
  ui.ci1lineEdit->setText("");
  ui.ci2lineEdit->setText("");
  ui.ci3lineEdit->setText("");

 if (codigo.length()==0) return;

  if (codigo.startsWith("???"))
      {
       ui.ci1lineEdit->setText(tr("CUALQUIERA"));
      }
      else
           {
            bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
            if (encontrada && qnivel.toInt()==1)
                ui.ci1lineEdit->setText(descripcion);
           }

 if (codigo.length()<=3) return;

 if (codigo.mid(3,3)==QString("???")) ui.ci2lineEdit->setText(tr("CUALQUIERA"));
     else
          {
           bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
           int elnivel=qnivel.toInt();
           if (encontrada && elnivel==2)
              ui.ci2lineEdit->setText(descripcion);
          }

 if (codigo.length()<=6) return;

 if (codigo.length()==7 && codigo[6]=='*')  ui.ci3lineEdit->setText(tr("CUALQUIERA"));
    else
         {
          bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
          if (encontrada && qnivel.toInt()==3)
             ui.ci3lineEdit->setText(descripcion);
         }


}


void consinput::refrescar()
{
 terminaedicion();
 QString codigo,filtro;
 
 codigo=ui.cilineEdit->text();

 if (!cifiltrook(codigo))
     {
      QMessageBox::warning( this, tr("Consultas de imputaciones"),
			 tr("Código de imputación erróneo"));
      return;
     }
/* if (codigo.length()<3)
     {
      ui.table->setRowCount(0);
      return;
     }
*/

  filtro=filtroci(codigo,basedatos::instancia()->analitica_tabla());

  if (!ui.cuentalineEdit->text().isEmpty())
  {
   QString cuentatabladiario;
   cuentatabladiario =basedatos::instancia()->analitica_tabla()  ? "diario.cuenta" : "cuenta";
   if (!filtro.isEmpty()) filtro+= " and ";

   if (!esauxiliar(ui.cuentalineEdit->text()))
    {
     if (basedatos::instancia()->essqlite())
        filtro+=cuentatabladiario + " like '"+ ui.cuentalineEdit->text().replace("'","''") +"%'";
      else
        filtro+=" position('"+ ui.cuentalineEdit->text().replace("'","''") +
                "' in "+cuentatabladiario+")=1";
    }
    else
        filtro+=cuentatabladiario.replace("'","''")+"='"+
                ui.cuentalineEdit->text().replace("'","''")+"'";
   }

// QMessageBox::warning( this, tr("consci"),filtro);

 if (ejerciciodelafecha(ui.inicialdateEdit->date())!=ejerciciodelafecha(ui.finaldateEdit->date()))
	{
	   QMessageBox::warning( this, tr("Consultas de imputaciones"),
				 tr("Las fechas han de pertenecer al mismo período"));
	   QString ejercicio=ejerciciodelafecha(ui.inicialdateEdit->date());
	   // QMessageBox::warning( this, tr("Consultas de Mayor"),ejercicio);
	   if (ejercicio.length()!=0) ui.finaldateEdit->setDate(finejercicio(ejercicio));
	      else ui.finaldateEdit->setDate(ui.inicialdateEdit->date());
         }
 
 // ***************************************************************************************************
 qlonglong num=0;
 if (basedatos::instancia()->analitica_tabla())
     num = basedatos::instancia()->count_regs_diario_ci( ui.inicialdateEdit->date(),
                                                         ui.finaldateEdit->date(), filtro);
   else
     num = basedatos::instancia()->selectCountasientodiariofechasfiltro(ui.inicialdateEdit->date(),
                                                                        ui.finaldateEdit->date(),
                                                                        filtro);
    if (num>15000)
    {
        switch( QMessageBox::warning( this, tr("Consultas de CI"),
            tr("La consulta excede de 15000 registros,\n"
            "¿ desea cargarlos de todas formas ?"),
            tr("&Sí"), tr("&No"), 0, 0,1 ) ) 
	    {
            case 0: // proseguimos.
                break;
            case 1: // Cancelamos la carga
                return;
     	}
    }

   ui.table->setRowCount(num);

   QSqlQuery query;
   if (basedatos::instancia()->analitica_tabla())
     query=basedatos::instancia()->cons_diario_ci( ui.inicialdateEdit->date(),
                                                   ui.finaldateEdit->date(), filtro );
    else
       query= basedatos::instancia()->select8Diariofechasfiltroorderfechapase(
               ui.inicialdateEdit->date(), ui.finaldateEdit->date(), filtro );
   int fila=0;
    if (query.isActive())
    while ( query.next() ) {
        QTableWidgetItem *newItem = new QTableWidgetItem(query.value(0).toDate().toString("dd.MM.yyyy"));
        ui.table->setItem(fila,0,newItem);
        QTableWidgetItem *newItemx = new QTableWidgetItem(query.value(1).toString());
        newItemx->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.table->setItem(fila,1,newItemx);  // asiento
        QTableWidgetItem *newItemp = new QTableWidgetItem(query.value(2).toString());
        ui.table->setItem(fila,2,newItemp);  // concepto
        double valdebe=query.value(3).toDouble();
        // aquí nos hemos quedado
        // ***************************************************************************************
        // ***************************************************************************************
        if (valdebe>0.0001 || valdebe<-0.0001)
          {
           QTableWidgetItem *newItem = new QTableWidgetItem(formateanumero(valdebe,comadecimal,decimales));
           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.table->setItem(fila,3,newItem);  // debe
          }
          else 
               {
                QTableWidgetItem *newItempp = new QTableWidgetItem("");
                ui.table->setItem(fila,3,newItempp);
               }
        double valhaber=query.value(4).toDouble();
        if (valhaber>0.0001 || valhaber<-0.0001)
          {
           QTableWidgetItem *newItemjj = new QTableWidgetItem(formateanumero(valhaber,comadecimal,decimales));
           newItemjj->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.table->setItem(fila,4,newItemjj);  // haber
          }
          else 
               {
                QTableWidgetItem *newItemkk = new QTableWidgetItem("");
                ui.table->setItem(fila,4,newItemkk);
               }
        QTableWidgetItem *newItemj = new QTableWidgetItem(query.value(5).toString());
        ui.table->setItem(fila,6,newItemj); // documento
        QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(6).toString());
        ui.table->setItem(fila,7,newItem2); 
        QTableWidgetItem *newItem3 = new QTableWidgetItem(query.value(7).toString());
        ui.table->setItem(fila,8,newItem3); 
        fila++;
    }
   double saldoin=0;
   double sumadebe=0;
   double sumahaber=0;
   fila=0;
   while (fila<ui.table->rowCount())
       {
          if (ui.table->item(fila,3)->text().length()>0) 
	{
          sumadebe+=convapunto(ui.table->item(fila,3)->text()).toDouble();
          saldoin+=convapunto(ui.table->item(fila,3)->text()).toDouble();
	}
          if (ui.table->item(fila,4)->text().length()>0)
	  {
          sumahaber+=convapunto(ui.table->item(fila,4)->text()).toDouble();
          saldoin-=convapunto(ui.table->item(fila,4)->text()).toDouble();
	  }
        QTableWidgetItem *newItemx = new QTableWidgetItem(formateanumero(saldoin,comadecimal,decimales));
        newItemx->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.table->setItem(fila,5,newItemx); // saldo
        fila++;
       }
   ui.debelineEdit->setText(formateanumero(sumadebe,comadecimal,decimales));
   ui.haberlineEdit->setText(formateanumero(sumahaber,comadecimal,decimales));
   ui.saldolineEdit->setText(formateanumero(saldoin,comadecimal,decimales));

   ui.table->resizeColumnsToContents();

 // ***************************************************************************************************
}


void consinput::terminaedicion()
{
 if (!cifiltrook(ui.cilineEdit->text()))
      {
       QMessageBox::warning( this, tr("Consulta de imputaciones"),
                                    tr("El Código de imputación suministrado es incorrecto"));
       ui.cilineEdit->setText("");
      }
}


void consinput::editarpulsado()
{
    QString asiento;

    if (ui.table->currentItem()==0) return;


    int dia=ui.table->item(ui.table->currentRow(),0)->text().left(2).toInt();
    int mes=ui.table->item(ui.table->currentRow(),0)->text().mid(3,2).toInt();
    int anyo=ui.table->item(ui.table->currentRow(),0)->text().right(4).toInt();
    QDate fecha(anyo,mes,dia);
    asiento=ui.table->item(ui.table->currentRow(),1)->text();


    QString ejercicio=ejerciciodelafecha(fecha);
    fecha=fechadeasiento(asiento,ejercicio);

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


void consinput::generalatex()
{
   QDate fechainicial=ui.inicialdateEdit->date();
   QDate fechafinal=ui.finaldateEdit->date();
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+"consinput.tex";
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
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

    int lineas=0;
    // QString cadena;
    QSqlQuery query;
       while (1)
          {
           stream << "\\begin{flushleft}" << "\n";
           stream << "\\today" << "\n";
           stream << "\\newline" << "\n";
           stream << "\\end{flushleft}" << "\n";
     
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
           stream << "\\end{center}" << "\n";
           stream << "\n";
           //
           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << tr("CONSULTA IMPUTACIONES: ") << ui.cilineEdit->text() <<  "}}" << "\n";
           stream << "\\end{center}" << "\n";
           
/*           stream << "\\begin{center}" << "\n";
           stream << "{\\Large \\textbf {" << filtracad(descripcioncuenta(subctaactual)) << "}}" << "\n";
           stream << "\\end{center}" << "\n"; */

           stream << "{\\textbf {" << tr("Extracto de ") << fechainicial.toString("dd.MM.yyyy") <<
	                         tr(" a ") << fechafinal.toString("dd.MM.yyyy") << "}}" << "\n";

           // cuerpo de listado (tablas)
           stream << "\\begin{center}\n";
           stream << "\\begin{tabular}{|l|r|p{6cm}|l|l|r|r|r|}\n";
           stream << "\\hline\n";
           stream << "{\\scriptsize {" << tr("FECHA") << "}} & {\\scriptsize {" << tr("ASTO.") << 
                     "}} & {\\scriptsize {" << tr("CONCEPTO") << "}} &  {\\scriptsize {" << tr("CUENTA") << 
                     "}} &  {\\scriptsize {" << tr("CI") << "}} & {\\scriptsize {" << tr("DEBE") << 
                      "}} & {\\scriptsize {" << tr("HABER") << "}} & {\\scriptsize {" << tr("SALDO") << "}} \\\\\n";
           stream << "\\hline\n";
	   
           double saldoin=0;
           double sumadebe=0;
           double sumahaber=0;
	   double saldo=0;
	   QString cadsaldo="";

       QString filtro=filtroci(ui.cilineEdit->text(),basedatos::instancia()->analitica_tabla());
       if (basedatos::instancia()->essqlite()) {
           filtro += basedatos::instancia()->analitica_tabla() ? " and diario.cuenta like '" : " and cuenta like '";
           filtro += ui.cuentalineEdit->text().left(-1).replace("'","''") +"%' ";
          }
       else {
             filtro += " and position('";
             filtro += ui.cuentalineEdit->text().left(-1).replace("'","''");
             filtro += basedatos::instancia()->analitica_tabla() ? "' in diario.cuenta)=1" : "' in cuenta)=1";
           }

       // QString cadnum;
           if (basedatos::instancia()->analitica_tabla())
              {
               query=basedatos::instancia()->cons_diario_ci_tex(fechainicial,
                                                                fechafinal,
                                                                filtro);
              }
           else
             {
               query = basedatos::instancia()->select7Diariofechasfiltroorderfechapase(fechainicial,
                                                                               fechafinal,
                                                                               filtro);
             }

	   saldo=saldoin;
       QString concepto;

       if (query.isActive()) 
       while (query.next())
	     {
	      lineas++;
	      stream << "{\\scriptsize {" << query.value(0).toDate().toString("dd.MM.yyyy") << "}}" << " & ";
	      // Asiento ------------------------------------------------------------------------------------------
              stream << "{\\scriptsize {";
              stream << query.value(1).toString() << "}}" << " & ";
	      // ---------------------------------------------------------------------------------------------------
	      concepto=query.value(2).toString();
	      concepto=filtracad(concepto);
	      QString cort=mcortaa(concepto,43);
	      if (cort!=concepto) concepto=cort+"\\#"; else concepto=cort;	      
	      stream << "{\\scriptsize {" << concepto << "}}" << " & ";
              stream << "{\\scriptsize {" << query.value(3).toString() << "}}" << " & ";
              stream << "{\\scriptsize {" << query.value(4).toString() << "}}" << " & ";
              stream << "{\\scriptsize {";
	      double valdebe=query.value(5).toDouble();
	      if (valdebe>0.0001 || valdebe<-0.0001)
                 stream << formateanumerosep(query.value(5).toDouble(),comadecimal,decimales) << "}}" << " & ";
                else stream << "}}" << " & ";
	      double valhaber=query.value(6).toDouble();
              stream << "{\\scriptsize {";
	      if (valhaber>0.0001 || valhaber<-0.0001)
                   stream << formateanumerosep(query.value(6).toDouble(),comadecimal,decimales) << "}}" << " & ";
	        else stream << "}}" << " & ";
	      saldo+=query.value(5).toDouble()-query.value(6).toDouble();
	      sumadebe+=query.value(5).toDouble();
	      sumahaber+=query.value(6).toDouble();
	      cadsaldo=formateanumerosep(saldo,comadecimal,decimales);
	      stream << "{\\scriptsize {" << cadsaldo  << "}}" << " \\\\ \n ";
                      stream << "\\hline\n";

              // *************************
	      if (lineas==LINEASPAGINA)
                        {
	                  lineas=0;
                          stream  << "\\multicolumn{5}{|r|}{" << tr("Sumas y siguen ...") << "} & " <<
                                "{\\scriptsize {" << formateanumerosep(sumadebe,comadecimal,decimales) << "}}";
	                  stream << " & " << 
                                "{\\scriptsize {" << formateanumerosep(sumahaber,comadecimal,decimales) << 
                                "}}" << "& \\\\";
                          stream << "\\hline\n";
       	                  stream << "\\end{tabular}\n";
                          stream << "\\end{center}\n";
                          stream << "\\newpage";
		  
                         stream << "\\begin{flushleft}" << "\n";
                         stream << "\\today" << "\n";
                         stream << "\\newline" << "\n";
                         stream << "\\end{flushleft}" << "\n";
     
                         stream << "\\begin{center}" << "\n";
                         stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
                         stream << "\\end{center}" << "\n";
                         stream << "\n";


                         stream << "\\begin{center}" << "\n";
                         stream << "{\\Large \\textbf {" << tr("CONSULTA IMPUTACIONES: ") << 
                                        ui.cilineEdit->text() <<  "}}" << "\n";
                         stream << "\\end{center}" << "\n";


                        stream << "{\\textbf {" << tr("Extracto de ") << fechainicial.toString("dd.MM.yyyy") <<
	                         " a " << fechafinal.toString("dd.MM.yyyy") << "}}" << "\n";


                        // cuerpo de listado (tablas)
                        stream << "\\begin{center}\n";
                        stream << "\\begin{tabular}{|l|r|p{6cm}|l|l|r|r|r|}\n";
                        stream << "\\hline\n";
                        stream << "{\\scriptsize {" << tr("FECHA") << "}} & {\\scriptsize {" << tr("ASTO.") << 
                           "}} & {\\scriptsize {" << tr("CONCEPTO") << "}} &  {\\scriptsize {" << tr("CUENTA") << 
                           "}} &  {\\scriptsize {" << tr("CI") << "}} & {\\scriptsize {" << tr("DEBE") <<
                           "}} & {\\scriptsize {" << tr("HABER") << "}} & {\\scriptsize {" << tr("SALDO") << 
                           "}} \\\\\n";
                        stream << "\\hline\n";
  	       }
	     }
           stream  << "\\multicolumn{5}{|r|}{" << tr("Sumas ...") << "} & " << "{\\scriptsize {" <<
                      formateanumero(sumadebe,comadecimal,decimales) << "}}";
           stream << " & " << "{\\scriptsize {" << formateanumero(sumahaber,comadecimal,decimales) <<
                 "}}" << "& \\\\";
           stream << "\\hline\n";
           // final de la tabla
           stream << "\\end{tabular}\n";
           stream << "\\end{center}\n";
           stream << "\\newpage";
           break;
          }

    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();


}


void consinput::imprimir()
{
    generalatex();

   int valor=imprimelatex("consinput");
   if (valor==1)
       QMessageBox::warning( this, tr("IMPRIMIR IMPUTACIONES"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("IMPRIMIR IMPUTACIONES"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("IMPRIMIR IMPUTACIONES"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void consinput::visdoc()
{
  if (ui.table->currentItem()==0) return;

  QString asiento;

  int dia=ui.table->item(ui.table->currentRow(),0)->text().left(2).toInt();
  int mes=ui.table->item(ui.table->currentRow(),0)->text().mid(3,2).toInt();
  int anyo=ui.table->item(ui.table->currentRow(),0)->text().right(4).toInt();
  QDate fecha(anyo,mes,dia);
  QString ejercicio=ejerciciodelafecha(fecha);
  asiento=ui.table->item(ui.table->currentRow(),1)->text();

  QString fichdoc=basedatos::instancia()->copia_docdiario(asiento,ejercicio);

   if (fichdoc.isEmpty()) return;

  fichdoc=expanderutadocfich(fichdoc);

   if (!ejecuta(aplicacionabrirfich(extensionfich(fichdoc)),fichdoc))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+fichdoc+tr(" con ")+
                             aplicacionabrirfich(extensionfich(fichdoc)));

}



void consinput::copiar()
{
   QClipboard *cb = QApplication::clipboard();

   int lineas=0;
   QString cadena;
   QDate fechainicial=ui.inicialdateEdit->date();
   QDate fechafinal=ui.finaldateEdit->date();
   QSqlQuery query;
      while (1)
         {
          cadena+=filtracad(nombreempresa());
          cadena+= "\n";
          cadena+= tr("CONSULTA IMPUTACIONES: \t") + ui.cilineEdit->text() + "\n";


          cadena+= tr("Extracto de \t") + fechainicial.toString("dd.MM.yyyy") +
                   "\t" + fechafinal.toString("dd.MM.yyyy") + "\n";
          cadena+= tr("FECHA") + "\t" + tr("ASTO.") + "\t" + tr("CONCEPTO") + "\t" +
                   tr("CUENTA") + "\t" + tr("CI") + "\t" + tr("DEBE") + "\t" + tr("HABER") + "\t" +
                   tr("SALDO") + "\n";

          double saldoin=0;
          double sumadebe=0;
          double sumahaber=0;
          double saldo=0;
          QString cadsaldo="";

          QString filtro=filtroci(ui.cilineEdit->text(),basedatos::instancia()->analitica_tabla());
          if (basedatos::instancia()->essqlite()) {
              filtro += basedatos::instancia()->analitica_tabla() ? " and diario.cuenta like '" : " and cuenta like '";
              filtro += ui.cuentalineEdit->text().left(-1).replace("'","''") +"%' ";
             }
          else {
                filtro += " and position('";
                filtro += ui.cuentalineEdit->text().left(-1).replace("'","''");
                filtro += basedatos::instancia()->analitica_tabla() ? "' in diario.cuenta)=1" : "' in cuenta)=1";
              }


          if (basedatos::instancia()->analitica_tabla())
              query=basedatos::instancia()->cons_diario_ci_tex(fechainicial,
                                                               fechafinal,
                                                               filtro);
          else
             query = basedatos::instancia()->select7Diariofechasfiltroorderfechapase(fechainicial,
                                                                              fechafinal,
                                                                              filtro);
          saldo=saldoin;
      QString concepto;

      if (query.isActive())
      while (query.next())
            {
             lineas++;
             cadena+= query.value(0).toDate().toString("dd.MM.yyyy") + "\t";
             // Asiento ------------------------------------------------------------------------------------------
             cadena+= query.value(1).toString() + " \t ";
             // ---------------------------------------------------------------------------------------------------
             concepto=query.value(2).toString();
             concepto=filtracad(concepto);
             cadena+= concepto + "\t";
             cadena+= query.value(3).toString() + "\t";
             cadena+= query.value(4).toString() + "\t";
             double valdebe=query.value(5).toDouble();
             if (valdebe>0.0001 || valdebe<-0.0001)
                cadena+= formateanumerosep(query.value(5).toDouble(),comadecimal,decimales) + "\t";
               else cadena+= "\t";
             double valhaber=query.value(6).toDouble();
             if (valhaber>0.0001 || valhaber<-0.0001)
                  cadena+= formateanumerosep(query.value(6).toDouble(),comadecimal,decimales) + "\t";
               else cadena+= "\t";
             saldo+=query.value(5).toDouble()-query.value(6).toDouble();
             sumadebe+=query.value(5).toDouble();
             sumahaber+=query.value(6).toDouble();
             cadsaldo=formateanumerosep(saldo,comadecimal,decimales);
             cadena+= cadsaldo  + "\t\n";
            }
          cadena+= "\t\t\t\t" + tr("Sumas ...") + "\t" +
                     formateanumero(sumadebe,comadecimal,decimales);
          cadena+= "\t" + formateanumero(sumahaber,comadecimal,decimales) +
                "\n";
          break;
         }

   cb->setText(cadena);
   QMessageBox::information( this, tr("Consulta de mayor"),
                             tr("Se ha pasado el contenido al portapapeles") );

}


void consinput::latex()
{
    int valor=0;
    generalatex();
    valor=editalatex(tr("consinput"));
    if (valor==1)
        QMessageBox::warning( this, tr("Consulta imputaciones"),tr("PROBLEMAS al llamar al editor Latex"));

}



void consinput::botoncuentapulsado()
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

void consinput::ctaexpandepunto()
{
  ui.cuentalineEdit->setText(expandepunto(ui.cuentalineEdit->text(),anchocuentas()));
}



void consinput::consultarpulsado()
{
    QString asiento;

    if (ui.table->currentItem()==0) return;


    int dia=ui.table->item(ui.table->currentRow(),0)->text().left(2).toInt();
    int mes=ui.table->item(ui.table->currentRow(),0)->text().mid(3,2).toInt();
    int anyo=ui.table->item(ui.table->currentRow(),0)->text().right(4).toInt();
    QDate fecha(anyo,mes,dia);
    asiento=ui.table->item(ui.table->currentRow(),1)->text();


    QString ejercicio=ejerciciodelafecha(fecha);
    fecha=fechadeasiento(asiento,ejercicio);

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

    consultarasiento(asiento,usuario,ejercicio);

}
