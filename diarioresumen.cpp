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

#include "diarioresumen.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

diarioresumen::diarioresumen() : QDialog() {
    ui.setupUi(this);

    comadecimal=haycomadecimal();
    decimales=haydecimales();
  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

  connect(ui.consultarpushButton,SIGNAL(clicked()),SLOT(consultar()));
  connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprimir()));
  connect(ui.XMLpushButton,SIGNAL(clicked()),SLOT(generaxml()));
  connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));

}


void diarioresumen::generalatexdiario()
{
    if (ui.principalescheckBox->isChecked())
       {
        generalatexdiarioprincipales();
        return;
       }
    int lineaspagina=40;
    if (ui.fechacheckBox->isChecked()) lineaspagina-=3;


   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("diario_resumen.tex");
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

    QString cabecerapagina="";
    if (ui.fechacheckBox->isChecked())
          {
           cabecerapagina+= "\\begin{flushleft}"; cabecerapagina+="\n";
           cabecerapagina+= "\\today"; cabecerapagina+= "\n";
           cabecerapagina+= "\\newline"; cabecerapagina+="\n";
           cabecerapagina+= "\\end{flushleft}"; cabecerapagina+="\n";
          }

    cabecerapagina+= "\\begin{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "{\\huge \\textbf {";  cabecerapagina+="DIARIO RESUMEN";
             cabecerapagina+="}}"; cabecerapagina+="\n";
    cabecerapagina+= "\\end{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "\n";

    cabecerapagina+= "\\begin{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "{\\Large \\textbf {";  cabecerapagina+=filtracad(nombreempresa());
             cabecerapagina+="}}"; cabecerapagina+="\n";
    cabecerapagina+= "\\end{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "\n";

    // inicio de la tabla
    QString cabecerapagina2="\\begin{center}\n";
    cabecerapagina2+= "\\begin{tabular}{|r|l|r|r|}\n";
    cabecerapagina2+="\\hline\n";
    cabecerapagina2+="{\\scriptsize ";
    cabecerapagina2+= tr("CUENTA");
    cabecerapagina2+= "} & {\\scriptsize ";
    cabecerapagina2+= tr("TITULO CUENTA");
    cabecerapagina2+= "} &  {\\scriptsize ";
    cabecerapagina2+= tr("DEBE");
    cabecerapagina2+= "} & {\\scriptsize ";
    cabecerapagina2+= tr("HABER");
    cabecerapagina2+= "} \\\\\n";
    cabecerapagina2+="\\hline\n";

    QString piepagina="\\hline\n";
     piepagina+="\\end{tabular}\n";
     piepagina+="\\end{center}\n";
     piepagina+="\\newpage \n";

     double totsumadebe=0;
     double totsumahaber=0;
     ui.progressBar->setMaximum(15);
     for (int veces=0;veces<15;veces++)
     {
         ui.progressBar->setValue(veces+1);
         QString cabeceraperiodo;
         cabeceraperiodo= "\\begin{center}"; cabeceraperiodo+= "\n";
         cabeceraperiodo+= "{\\Large \\textbf {"; 
         int mescorriente=0;
         int mesini=0;
         switch  (veces)
	 {
	   case 0 : cabeceraperiodo+=diario_apertura();
	       break;
	   case 1:
	   case 2:
	   case 3:
	   case 4:
	   case 5:
	   case 6:
	   case 7:
	   case 8:
	   case 9:
	   case 10:
	   case 11:
	   case 12:
	       mesini=inicioejercicio(ui.ejerciciocomboBox->currentText()).month();
	       mescorriente=mesini+veces-1;
	       if (mescorriente>12) mescorriente-=12;
	       cabeceraperiodo+=cadmes(mescorriente);
	       break;
	   case 13 : cabeceraperiodo+=diario_regularizacion();
	       break;
	   case 14 : cabeceraperiodo+=diario_cierre();
	       break;
	 }
        cabeceraperiodo+=" ";
        cabeceraperiodo+=ui.ejerciciocomboBox->currentText();
        cabeceraperiodo+="}}"; cabeceraperiodo+="\n";
        cabeceraperiodo+= "\\end{center}"; cabeceraperiodo+= "\n";
        cabeceraperiodo+= "\n";

        int totallineas;
        QString periodo;
        QSqlQuery query = basedatos::instancia()->selectCuentasumdebesumhaberdiariofechas(&totallineas, &periodo, ui.ejerciciocomboBox->currentText() , veces, mescorriente);
        // QString cadena2;
        // QMessageBox::warning( this, tr("dr"),cadena2.setNum(totallineas));
        if ( query.isActive() )
          {
           int linea=0;
           int glinea=0;
           double sumadebe=0;
           double sumahaber=0;
           while (query.next())
	  {
	    if (linea==0)
	       {
	           stream << cabecerapagina;
	           stream << cabeceraperiodo;
	           stream << cabecerapagina2;
	       }
            stream << "{\\tiny ";
	    stream << query.value(0).toString();
	    stream << "} & ";
	    stream << "{\\tiny ";
	    // stream << descripcioncuenta(query.value(0).asString()).replace(34,39);
	    stream << filtracad(descripcioncuenta(query.value(0).toString()));
	    stream << "} & ";
            stream << "{\\tiny ";
	    stream << formateanumerosep(query.value(1).toDouble(),comadecimal,decimales);
	    stream << "} &";
	    stream << "{\\tiny ";
	    stream << formateanumerosep(query.value(2).toDouble(),comadecimal,decimales);
	    stream << "}  \\\\ \n";
            sumadebe+=query.value(1).toDouble();
	    sumahaber+=query.value(2).toDouble();
            totsumadebe+=query.value(1).toDouble();
	    totsumahaber+=query.value(2).toDouble();
	    linea++;
	    glinea++;
	    if (linea>lineaspagina) 
	      {
	            if (glinea!=totallineas)
		  {
		     // sumas y siguen
		     
		     stream << "\\hline\n";
		     stream  << "\\multicolumn{2}{|r|}{\\scriptsize " << tr("Sumas y siguen ...") << "} & {\\tiny " 
				    << formateanumerosep(sumadebe,comadecimal,decimales) ;
		     stream << "} & {\\tiny " << formateanumerosep(sumahaber,comadecimal,decimales)
			     << "}  \\\\\n";
	             stream << piepagina;
		  }
	            linea=0;
	      }
	  }
            
             if (totallineas>0)
	       {
                stream << "\\hline\n";
                stream  << "\\multicolumn{2}{|r|}{\\scriptsize " << tr("Sumas ") << periodo <<   " ...} & {\\tiny "
	        << formateanumerosep(sumadebe,comadecimal,decimales) ;
                      stream << "} & {\\tiny " << formateanumerosep(sumahaber,comadecimal,decimales)
	        << "}  \\\\\n";
		
                stream << "\\hline\n";
                stream  << "\\multicolumn{2}{|r|}{\\scriptsize " << tr("TOTAL ACUMULADO DIARIO ") 
		   << ui.ejerciciocomboBox->currentText() <<   " ...} & {\\tiny "
	           << formateanumerosep(totsumadebe,comadecimal,decimales) ;
                stream << "} & {\\tiny " << formateanumerosep(totsumahaber,comadecimal,decimales)
	           << "}  \\\\\n";
		
                 stream << piepagina;
                }
          } // query active
     }
    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}

void diarioresumen::generaxml()
{
 if (ui.principalescheckBox->isChecked())
    {
     generaxmlprincipales();
     return;
    }
 QDomDocument doc("DiarioResumen");
 QDomElement root = doc.createElement("DiarioResumen");
 doc.appendChild(root);

 QDomElement tag = doc.createElement("Cabecera");
 root.appendChild(tag);
 addElementoTextoDom(doc,tag,"NombreEmpresa",filtracadxml(nombreempresa()));

 QDomElement tag2 = doc.createElement("Detalle");
 root.appendChild(tag2);

 ui.progressBar->setMaximum(15);
 for (int veces=0;veces<15;veces++)
     {
         ui.progressBar->setValue(veces+1);
         QString cabeceraperiodo;
         int mescorriente=0;
         int mesini=0;
         switch  (veces)
         {
           case 0 : cabeceraperiodo=diario_apertura();
               break;
           case 1:
           case 2:
           case 3:
           case 4:
           case 5:
           case 6:
           case 7:
           case 8:
           case 9:
           case 10:
           case 11:
           case 12:
               mesini=inicioejercicio(ui.ejerciciocomboBox->currentText()).month();
               mescorriente=mesini+veces-1;
               if (mescorriente>12) mescorriente-=12;
               cabeceraperiodo=cadmes(mescorriente);
               break;
           case 13 : cabeceraperiodo=diario_regularizacion();
               break;
           case 14 : cabeceraperiodo=diario_cierre();
               break;
         }
        cabeceraperiodo+=" ";
        cabeceraperiodo+=ui.ejerciciocomboBox->currentText();

        QDomElement tag3 = doc.createElement("Periodo");
        tag2.appendChild(tag3);

        addElementoTextoDom(doc,tag3,"NombrePeriodo",filtracadxml(cabeceraperiodo));

        QDomElement tag4 = doc.createElement("DetallePeriodo");
        tag3.appendChild(tag4);

        QString periodo;
        int totallineas;
        QSqlQuery query = basedatos::instancia()->selectCuentasumdebesumhaberdiariofechas(&totallineas,
                                                                              &periodo,
                                                                              ui.ejerciciocomboBox->currentText(),
                                                                              veces, mescorriente);
        // QString cadena2;
        // QMessageBox::warning( this, tr("dr"),cadena2.setNum(totallineas));
        if ( query.isActive() )
          {
           while (query.next())
              {
               QDomElement tag5 = doc.createElement("Linea");
               tag4.appendChild(tag5);
               addElementoTextoDom(doc,tag5,"Cuenta",filtracadxml(query.value(0).toString()));
               addElementoTextoDom(doc,tag5,"Descrip",filtracadxml(descripcioncuenta(query.value(0).toString())));
               addElementoTextoDom(doc,tag5,"Debe",formateanumero(query.value(1).toDouble(),comadecimal,decimales));
               addElementoTextoDom(doc,tag5,"Haber",formateanumero(query.value(2).toDouble(),comadecimal,decimales));
              }
          }
     }

  QString xml = doc.toString();

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("diario_resumen");
   qfichero+=".xml";


  QFile dr( adapta(qfichero)  );

  if ( !dr.open( QIODevice::WriteOnly ) )
       {
        QMessageBox::warning( this, tr("Diario resumen (XML)"),tr("Error: Imposible grabar fichero"));
        return;
       }
  QTextStream drstream( &dr );
  drstream.setEncoding(QStringConverter::Utf8);

  drstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  // bssstream << "<?xml-stylesheet type=\"text/xsl\" href=\"plan2html.xslt\"?>\n";

  // ------------------------------------------------------------------------------------
  drstream << xml;

  dr.close();

  QMessageBox::information( this, tr("Diario resumen (XML)"),tr("El archivo XML se ha grabado "
                                                                "en el directorio de trabajo"));
  ui.progressBar->reset();

}


void diarioresumen::consultar()
{
  generalatexdiario();

   int valor=consultalatex(tr("diario_resumen"));
   if (valor==1)
       QMessageBox::warning( this, tr("VISTA PREVIA DIARIO RESUMEN"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("VISTA PREVIA DIARIO RESUMEN"),
                             tr("PROBLEMAS al llamar a ")+visordvi());

    ui.progressBar->reset();

}


void diarioresumen::imprimir()
{
    generalatexdiario();

   int valor=imprimelatex(tr("diario_resumen"));
   if (valor==1)
       QMessageBox::warning( this, tr("IMPRIMIR DIARIO RESUMEN"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("IMPRIMIR DIARIO RESUMEN"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("IMPRIMIR DIARIO RESUMEN"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

    ui.progressBar->reset();

}




void diarioresumen::generalatexdiarioprincipales()
{
    int lineaspagina=40;
    if (ui.fechacheckBox->isChecked()) lineaspagina-=3;


   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("diario_resumen.tex");
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

    QString cabecerapagina="";
    if (ui.fechacheckBox->isChecked())
          {
           cabecerapagina+= "\\begin{flushleft}"; cabecerapagina+="\n";
           cabecerapagina+= "\\today"; cabecerapagina+= "\n";
           cabecerapagina+= "\\newline"; cabecerapagina+="\n";
           cabecerapagina+= "\\end{flushleft}"; cabecerapagina+="\n";
          }

    cabecerapagina+= "\\begin{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "{\\huge \\textbf {";  cabecerapagina+="DIARIO RESUMEN";
             cabecerapagina+="}}"; cabecerapagina+="\n";
    cabecerapagina+= "\\end{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "\n";

    cabecerapagina+= "\\begin{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "{\\Large \\textbf {";  cabecerapagina+=filtracad(nombreempresa());
             cabecerapagina+="}}"; cabecerapagina+="\n";
    cabecerapagina+= "\\end{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "\n";

    // inicio de la tabla
    QString cabecerapagina2="\\begin{center}\n";
    cabecerapagina2+= "\\begin{tabular}{|r|l|r|r|}\n";
    cabecerapagina2+="\\hline\n";
    cabecerapagina2+="{\\scriptsize ";
    cabecerapagina2+= tr("CUENTA");
    cabecerapagina2+= "} & {\\scriptsize ";
    cabecerapagina2+= tr("TITULO CUENTA");
    cabecerapagina2+= "} &  {\\scriptsize ";
    cabecerapagina2+= tr("DEBE");
    cabecerapagina2+= "} & {\\scriptsize ";
    cabecerapagina2+= tr("HABER");
    cabecerapagina2+= "} \\\\\n";
    cabecerapagina2+="\\hline\n";

    QString piepagina="\\hline\n";
     piepagina+="\\end{tabular}\n";
     piepagina+="\\end{center}\n";
     piepagina+="\\newpage \n";

     double totsumadebe=0;
     double totsumahaber=0;
     ui.progressBar->setMaximum(15);
     // construimos una lista con las cuentas principales
     // y una paralela para pasar los saldos.
     QStringList cuentas, nombres;
     QList<double> sumasdebe,sumashaber;
     QSqlQuery consulta =
         basedatos::instancia()->selectCodigodescripcionplancontablelengthauxiliarordercodigo(false);
     if (consulta.isActive())
       while (consulta.next())
          {
           if (codsubdivis(consulta.value(0).toString())) continue; // buscamos últimas ramas del plan
           cuentas << consulta.value(0).toString();
           nombres << consulta.value(1).toString();
           sumasdebe << 0.00;
           sumashaber << 0.00;
          }
     for (int veces=0;veces<15;veces++)
     {
         ui.progressBar->setValue(veces+1);
         QString cabeceraperiodo;
         cabeceraperiodo= "\\begin{center}"; cabeceraperiodo+= "\n";
         cabeceraperiodo+= "{\\Large \\textbf {";
         int mescorriente=0;
         int mesini=0;
         switch  (veces)
         {
           case 0 : cabeceraperiodo+=diario_apertura();
               break;
           case 1:
           case 2:
           case 3:
           case 4:
           case 5:
           case 6:
           case 7:
           case 8:
           case 9:
           case 10:
           case 11:
           case 12:
               mesini=inicioejercicio(ui.ejerciciocomboBox->currentText()).month();
               mescorriente=mesini+veces-1;
               if (mescorriente>12) mescorriente-=12;
               cabeceraperiodo+=cadmes(mescorriente);
               break;
           case 13 : cabeceraperiodo+=diario_regularizacion();
               break;
           case 14 : cabeceraperiodo+=diario_cierre();
               break;
         }
        cabeceraperiodo+=" ";
        cabeceraperiodo+=ui.ejerciciocomboBox->currentText();
        cabeceraperiodo+="}}"; cabeceraperiodo+="\n";
        cabeceraperiodo+= "\\end{center}"; cabeceraperiodo+= "\n";
        cabeceraperiodo+= "\n";

        for (int i = 0; i < cuentas.size(); ++i)
          {
            sumasdebe.replace(i,0);
            sumashaber.replace(i,0);
          }

        //

        int totallineas;
        QString periodo;
        QSqlQuery query = basedatos::instancia()->selectCuentasumdebesumhaberdiariofechas(&totallineas, &periodo,
                                                    ui.ejerciciocomboBox->currentText() , veces, mescorriente);
        // QString cadena2;
        // QMessageBox::warning( this, tr("dr"),cadena2.setNum(totallineas));
        bool contenido=false;
        if ( query.isActive() )
          {
            while (query.next())
              {
                if (!(query.value(1).toDouble()>-0.00001 && query.value(1).toDouble()< 0.00001 &&
                query.value(2).toDouble()>-0.00001 && query.value(2).toDouble()<0.00001))
                  {
                   contenido=true;
                   for (int i = 0; i < cuentas.size(); ++i) {
                    if (query.value(0).toString().startsWith(cuentas.at(i)))
                     {                       
                       sumasdebe.replace(i,sumasdebe.at(i)+query.value(1).toDouble());
                       sumashaber.replace(i,sumashaber.at(i)+query.value(2).toDouble());
                       break;
                     }
                  }
                 }
              }
          }
         if (!contenido) continue;
         // ahora nos tocaría recorrer de arriba a abajo la lista
         // y reproducir solo las cuentas con algo de saldo

        int linea=0;
        int glinea=0;
        double sumadebe=0;
        double sumahaber=0;
        totallineas=0;
        for (int i =0; i < cuentas.size(); ++i)
          {
            if (sumasdebe.at(i)>-0.00001 && sumasdebe.at(i)<0.00001 &&
                sumashaber.at(i)>-0.00001 && sumashaber.at(i)<0.00001) continue;
            if (linea==0)
               {
                   stream << cabecerapagina;
                   stream << cabeceraperiodo;
                   stream << cabecerapagina2;
               }
            totallineas++;
            stream << "{\\tiny ";
            stream << cuentas.at(i);
            stream << "} & ";
            stream << "{\\tiny ";
            // stream << descripcioncuenta(query.value(0).asString()).replace(34,39);
            stream << filtracad(nombres.at(i));
            stream << "} & ";
            stream << "{\\tiny ";
            stream << formateanumerosep(sumasdebe.at(i),comadecimal,decimales);
            stream << "} &";
            stream << "{\\tiny ";
            stream << formateanumerosep(sumashaber.at(i),comadecimal,decimales);
            stream << "}  \\\\ \n";
            sumadebe+=sumasdebe.at(i);
            sumahaber+=sumashaber.at(i);
            totsumadebe+=sumasdebe.at(i);
            totsumahaber+=sumashaber.at(i);
            linea++;
            glinea++;
            if (linea>lineaspagina)
              {
                    if (glinea!=totallineas)
                  {
                     // sumas y siguen

                     stream << "\\hline\n";
                     stream  << "\\multicolumn{2}{|r|}{\\scriptsize " << tr("Sumas y siguen ...") << "} & {\\tiny "
                                    << formateanumerosep(sumadebe,comadecimal,decimales) ;
                     stream << "} & {\\tiny " << formateanumerosep(sumahaber,comadecimal,decimales)
                             << "}  \\\\\n";
                     stream << piepagina;
                  }
                    linea=0;
              }
          }

             if (totallineas>0)
               {
                stream << "\\hline\n";
                stream  << "\\multicolumn{2}{|r|}{\\scriptsize " << tr("Sumas ") << periodo <<   " ...} & {\\tiny "
                << formateanumerosep(sumadebe,comadecimal,decimales) ;
                      stream << "} & {\\tiny " << formateanumerosep(sumahaber,comadecimal,decimales)
                << "}  \\\\\n";

                stream << "\\hline\n";
                stream  << "\\multicolumn{2}{|r|}{\\scriptsize " << tr("TOTAL ACUMULADO DIARIO ")
                   << ui.ejerciciocomboBox->currentText() <<   " ...} & {\\tiny "
                   << formateanumerosep(totsumadebe,comadecimal,decimales) ;
                stream << "} & {\\tiny " << formateanumerosep(totsumahaber,comadecimal,decimales)
                   << "}  \\\\\n";

                 stream << piepagina;
                }
          } // query active

    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}



void diarioresumen::generaxmlprincipales()
{

 QDomDocument doc("DiarioResumen");
 QDomElement root = doc.createElement("DiarioResumen");
 doc.appendChild(root);

 QDomElement tag = doc.createElement("Cabecera");
 root.appendChild(tag);
 addElementoTextoDom(doc,tag,"NombreEmpresa",filtracadxml(nombreempresa()));

 QDomElement tag2 = doc.createElement("Detalle");
 root.appendChild(tag2);

 ui.progressBar->setMaximum(15);

     QStringList cuentas, nombres;
     QList<double> sumasdebe,sumashaber;
     QSqlQuery consulta =
         basedatos::instancia()->selectCodigodescripcionplancontablelengthauxiliarordercodigo(false);
     if (consulta.isActive())
       while (consulta.next())
          {
           if (codsubdivis(consulta.value(0).toString())) continue; // buscamos últimas ramas del plan
           cuentas << consulta.value(0).toString();
           nombres << consulta.value(1).toString();
           sumasdebe << 0.00;
           sumashaber << 0.00;
          }

 for (int veces=0;veces<15;veces++)
     {
         ui.progressBar->setValue(veces+1);
         QString cabeceraperiodo;
         int mescorriente=0;
         int mesini=0;
         switch  (veces)
         {
           case 0 : cabeceraperiodo=diario_apertura();
               break;
           case 1:
           case 2:
           case 3:
           case 4:
           case 5:
           case 6:
           case 7:
           case 8:
           case 9:
           case 10:
           case 11:
           case 12:
               mesini=inicioejercicio(ui.ejerciciocomboBox->currentText()).month();
               mescorriente=mesini+veces-1;
               if (mescorriente>12) mescorriente-=12;
               cabeceraperiodo=cadmes(mescorriente);
               break;
           case 13 : cabeceraperiodo=diario_regularizacion();
               break;
           case 14 : cabeceraperiodo=diario_cierre();
               break;
         }
        cabeceraperiodo+=" ";
        cabeceraperiodo+=ui.ejerciciocomboBox->currentText();

        QDomElement tag3 = doc.createElement("Periodo");
        tag2.appendChild(tag3);

        addElementoTextoDom(doc,tag3,"NombrePeriodo",filtracadxml(cabeceraperiodo));

        QDomElement tag4 = doc.createElement("DetallePeriodo");
        tag3.appendChild(tag4);

        QString periodo;
        int totallineas;
        QSqlQuery query = basedatos::instancia()->selectCuentasumdebesumhaberdiariofechas(&totallineas,
                                                                              &periodo,
                                                                              ui.ejerciciocomboBox->currentText(),
                                                                              veces, mescorriente);


        bool contenido=false;
        if ( query.isActive() )
          {
            while (query.next())
              {
                if (!(query.value(1).toDouble()>-0.00001 && query.value(1).toDouble()< 0.00001 &&
                query.value(2).toDouble()>-0.00001 && query.value(2).toDouble()<0.00001))
                  {
                   contenido=true;
                   for (int i = 0; i < cuentas.size(); ++i) {
                    if (query.value(0).toString().contains(cuentas.at(i)))
                     {
                       sumasdebe.replace(i,sumasdebe.at(i)+query.value(1).toDouble());
                       sumashaber.replace(i,sumashaber.at(i)+query.value(2).toDouble());
                       break;
                     }
                  }
                 }
              }
          }
         if (!contenido) continue;
        // QString cadena2;
        // QMessageBox::warning( this, tr("dr"),cadena2.setNum(totallineas));
        totallineas=0;
        for (int i =0; i < cuentas.size(); ++i)
          {
            if (sumasdebe.at(i)>-0.00001 && sumasdebe.at(i)<0.00001 &&
                sumashaber.at(i)>-0.00001 && sumashaber.at(i)<0.00001) continue;
            QDomElement tag5 = doc.createElement("Linea");
            tag4.appendChild(tag5);
            addElementoTextoDom(doc,tag5,"Cuenta",filtracadxml(cuentas.at(i)));
            addElementoTextoDom(doc,tag5,"Descrip",filtracadxml(nombres.at(i)));
            addElementoTextoDom(doc,tag5,"Debe",formateanumero(sumasdebe.at(i),comadecimal,decimales));
            addElementoTextoDom(doc,tag5,"Haber",formateanumero(sumashaber.at(i),comadecimal,decimales));
          }
       }


  QString xml = doc.toString();

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("diario_resumen");
   qfichero+=".xml";


  QFile dr( adapta(qfichero)  );

  if ( !dr.open( QIODevice::WriteOnly ) )
       {
        QMessageBox::warning( this, tr("Diario resumen (XML)"),tr("Error: Imposible grabar fichero"));
        return;
       }
  QTextStream drstream( &dr );
  drstream.setEncoding(QStringConverter::Utf8);

  drstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  // bssstream << "<?xml-stylesheet type=\"text/xsl\" href=\"plan2html.xslt\"?>\n";

  // ------------------------------------------------------------------------------------
  drstream << xml;

  dr.close();

  QMessageBox::information( this, tr("Diario resumen (XML)"),tr("El archivo XML se ha grabado "
                                                                "en el directorio de trabajo"));
  ui.progressBar->reset();

}

void diarioresumen::latex()
{
    generalatexdiario();
    int valor=editalatex("diario_resumen");
    if (valor==1)
        QMessageBox::warning( this, tr("Diario resumen"),tr("PROBLEMAS al llamar al editor Latex"));

}
