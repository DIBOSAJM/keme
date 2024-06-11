/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2; aplicación para llevar contabilidades

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

#include "imprimediario.h"
#include "funciones.h"
#include "basedatos.h"
#include <QFileDialog>
#include <QMessageBox>
#include <qtrpt.h>
#include <QSqlDatabase>

imprimediario::imprimediario() : QDialog() {
    ui.setupUi(this);

    comadecimal=haycomadecimal();
    decimales=haydecimales();

    QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
    QStringList ej1;
    if ( query.isActive() ) {
            while ( query.next() )
                {
                  ej1 << query.value(0).toString();
            }
        }
    ui.ejercicioscomboBox->addItems(ej1);


    QDate fechaini=inicioejercicio(ui.ejercicioscomboBox->currentText());
    QDate fechafin=finejercicio(ui.ejercicioscomboBox->currentText());

    ui.inicialdateEdit->setDate(fechaini);
    ui.finaldateEdit->setDate(fechafin);


  connect(ui.visualizarpushButton,SIGNAL(clicked()),SLOT(visualizar()));
  connect(ui.imprimepushButton,SIGNAL(clicked()),SLOT(imprimir()));
  connect(ui.texpushButton,SIGNAL(clicked()),SLOT(latex()));
  connect(ui.xmlpushButton,SIGNAL(clicked()),SLOT(generaXML()));
  connect(ui.copiarpushButton,SIGNAL(clicked()),SLOT(copiar()));
  connect(ui.csvpushButton,SIGNAL(clicked()),SLOT(guardacsv()));

#ifdef NOEDITTEX
  ui.texpushButton->hide();
#endif


}

void imprimediario::guardacsv()
{
    QString nombre;

  #ifdef NOMACHINE
    directorio *dir = new directorio();
    QString ruta=rutacopiapdf();
    if (ruta.isEmpty()) ruta=dirtrabajo();
    dir->pasa_directorio(ruta);
    dir->filtrar("*.csv");
    dir->activa_pide_archivo("");
    if (dir->exec() == QDialog::Accepted)
      {
        nombre=dir->nuevo_conruta();
      }
     delete(dir);

  #else

    QFileDialog dialogofich(this);
    dialogofich.setFileMode(QFileDialog::AnyFile);
    dialogofich.setOption(QFileDialog::DontConfirmOverwrite,true);
    dialogofich.setAcceptMode (QFileDialog::AcceptSave );
    dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
    dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
    dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
    dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
    dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

    QStringList filtros;
    dialogofich.setDirectory(adapta(dirtrabajo()));
    filtros << tr("Archivos csv (*.csv)");
    dialogofich.setNameFilters(filtros);
    dialogofich.setWindowTitle(tr("ARCHIVO PARA ALMACENAR CSV"));
    QStringList fileNames;
    if (dialogofich.exec())
       {
        fileNames = dialogofich.selectedFiles();
        if (fileNames.at(0).length()>0)
            {
             // QString nombre=nombre.fromLocal8bit(fileNames.at(0));
             nombre=fileNames.at(0);
            }
       }

  #endif

    if (nombre.isEmpty()) return;

    if (nombre.right(4)!=".csv") nombre=nombre+".csv";

    QFile fichero( adapta(nombre)  );

    if (fichero.exists() && QMessageBox::question(this,
         tr("¿ Sobreescribir ?"),
         tr("'%1' ya existe."
            "¿ Desea sobreescribirlo ?")
           .arg( nombre )) == QMessageBox::No )
       return ;

    if ( !fichero.open( QIODevice::WriteOnly ) )
       {
        QMessageBox::warning(this,tr("Guardar diario"),tr("ERROR: No ha sido posible grabar el informe"));
        return;
       }

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cadcsv();
    fichero.close();

    QMessageBox::information(this,tr("Guardar diario"),tr("Se ha grabado el archivo CSV"));
}

QString imprimediario::qfiltro()
{
    QString pasafiltro=filtro;
    if (ui.ejerciciogroupBox->isChecked())
       {
        pasafiltro="fecha>='";
        pasafiltro+=inicioejercicio(ui.ejercicioscomboBox->currentText()).toString("yyyy-MM-dd");
        pasafiltro+="' and fecha<='";
        pasafiltro+=finejercicio(ui.ejercicioscomboBox->currentText()).toString("yyyy-MM-dd");
        pasafiltro+="'";
       }

    if (ui.fechasgroupBox->isChecked())
      {
        pasafiltro="fecha>='";
        pasafiltro+=ui.inicialdateEdit->date().toString("yyyy-MM-dd");
        pasafiltro+="' and fecha<='";
        pasafiltro+=ui.finaldateEdit->date().toString("yyyy-MM-dd");
        pasafiltro+="'";
      }
   return pasafiltro;
}

QString imprimediario::cadcsv()
{
    QString cabecerapagina;
    if (ui.fechaactualcheckBox->isChecked())
          {
           cabecerapagina+=QDate::currentDate().toString("dd/MM/yyyy");
           cabecerapagina+= "\n";
          }

    // cabecerapagina+= "\n"; cabecerapagina+= "\n";
    cabecerapagina+= tr("LIBRO DIARIO")+"\n";
    cabecerapagina+= "\n";

    cabecerapagina+=nombreempresa();
    cabecerapagina+="\n\n";

    cabecerapagina+=tr("ASIENTO");
    cabecerapagina+="\t";
    cabecerapagina+=tr("FECHA");
    cabecerapagina+="\t";
    cabecerapagina+=tr("CUENTA");
    cabecerapagina+="\t";
    cabecerapagina+= tr("TITULO CUENTA");
    cabecerapagina+="\t";
    cabecerapagina+= tr("DESCRIPCIÓN");
    cabecerapagina+="\t";
    cabecerapagina+= tr("DOCUMENTO");
    cabecerapagina+="\t";
    cabecerapagina+= tr("DEBE");
    cabecerapagina+="\t";
    cabecerapagina+= tr("HABER");
    cabecerapagina+= "\n";
    cabecerapagina+="\n";

    int totallineas = basedatos::instancia()->selectCountasientodiariofiltro(qfiltro());

    ui.progressBar->setMaximum(totallineas);
    QString detalle;

    QSqlQuery query = basedatos::instancia()->select7DiarioPlancontablecuentafiltroorderasientopase(qfiltro());
    if ( query.isActive() )
      {
          int glinea=0;
          double sumadebe=0;
          double sumahaber=0;
          while (query.next())
           {
              detalle+= query.value(0).toString();  // asiento
              detalle+="\t";
              detalle+=query.value(1).toDate().toString("dd/MM/yyyy");
              detalle+="\t";
              detalle+=query.value(2).toString();
              detalle+="\t";
              detalle+=query.value(3).toString();
              detalle+="\t";
              detalle+=query.value(4).toString();
              detalle+="\t";
              detalle+=query.value(7).toString();
              detalle+="\t";
              detalle+=formateanumero(query.value(5).toDouble(),comadecimal,decimales);
              detalle+="\t";
              detalle+=formateanumero(query.value(6).toDouble(),comadecimal,decimales);
              detalle+="\n";
              sumadebe+=query.value(5).toDouble();
              sumahaber+=query.value(6).toDouble();
              glinea++;
              ui.progressBar->setValue(glinea);
            }
           detalle+="\n\t\t\t\t";
           detalle+= tr("Sumas ...");
           detalle+="\t";
           detalle+=formateanumero(sumadebe,comadecimal,decimales);
           detalle+="\t";
           detalle+=formateanumero(sumahaber,comadecimal,decimales);
           detalle+="\n";
      }


      QString piepagina;
      if (ui.confiltrocheckBox->isChecked())
            {
             piepagina+="\n";
             piepagina+="\n";
             piepagina+= tr("FILTRO: ");
             QString filtrodef=filtro;
             piepagina+=filtrodef;
             piepagina+="\n";
            }

    QString global; global=cabecerapagina+detalle+piepagina;
    return global;
}

void imprimediario::copiar()
{
    QClipboard *cb = QApplication::clipboard();
    cb->setText(cadcsv());

    QMessageBox::information( this, tr("Imprimir diario"),
                              tr("Se ha pasado el contenido al portapapeles") );

    ui.progressBar->reset();
}


void imprimediario::informe()
{

    //int totallineas = basedatos::instancia()->selectCountasientodiariofiltro(qfiltro());
    QStringList asientos,fechas,cuentas,titulos,conceptos,documentos, ldebe, lhaber;
    QSqlQuery query = basedatos::instancia()->select7DiarioPlancontablecuentafiltroorderasientopase(qfiltro());
    if ( query.isActive() )
        while (query.next()) {
            //QString cadena = "select diario.asiento,diario.fecha,diario.cuenta,plancontable.descripcion,";
            //cadena += "diario.concepto,diario.debe,diario.haber, diario.documento from diario,plancontable where ";
            //cadena += "plancontable.codigo=diario.cuenta";
            asientos << query.value(0).toString();
            fechas << query.value(1).toDate().toString("dd/MM/yyyy");
            cuentas << query.value(2).toString();
            titulos << query.value(3).toString();
            conceptos << query.value(4).toString();
            ldebe << ((query.value(5).toDouble()>0.001 || query.value(5).toDouble() <-0.001) ? query.value(5).toString() : "");
            lhaber << ((query.value(6).toDouble()>0.001 || query.value(6).toDouble() <-0.001) ? query.value(6).toString() : "");
            documentos << query.value(7).toString();

        }



    QString fileName = ":/informes/diario.xml";
    if (ui.horizontal_checkBox->isChecked()) fileName = ":/informes/diario_horizontal.xml";
    QtRPT *report = new QtRPT(this);
    // report->recordCount << asientos.count();
    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("Imprimir diario"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }

    QObject::connect(report, &QtRPT::setDSInfo,
                     [&](DataSetInfo &dsInfo) {
        dsInfo.recordCount=asientos.count();;
    });

    QObject::connect(report, SIGNAL(setValueImage(const int, const QString, QImage&, const int)),
                         this, SLOT(setValueImage(const int, const QString, QImage&, const int)));

    connect(report, &QtRPT::setValue, [&](const int recNo, const QString paramName, QVariant &paramValue, const int reportPage) {
        Q_UNUSED (reportPage);

        if (paramName == "empresa") paramValue = basedatos::instancia()->selectEmpresaconfiguracion();
        if (paramName == "asiento") paramValue = asientos.at(recNo);
        if (paramName == "fecha") paramValue = fechas.at(recNo);
        if (paramName == "cuenta") paramValue = cuentas.at(recNo);
        if (paramName == "descripcion") paramValue = titulos.at(recNo);
        if (paramName == "concepto") paramValue = conceptos.at(recNo);
        if (paramName == "documento") paramValue = documentos.at(recNo);
        if (paramName == "debe") paramValue = ldebe.at(recNo);
        if (paramName == "haber") paramValue = lhaber.at(recNo);
        if (ui.confiltrocheckBox->isChecked())
            if (paramName == "filtro") paramValue = filtro;
        if (ui.fechaactualcheckBox->isChecked())
           if (paramName == "fecha_informe")
               paramValue = tr("Fecha: ") + QDate::currentDate().toString("dd/MM/yyyy");

    });






/*    QString cadena = "select diario.asiento, ";

    if (basedatos::instancia()->cualControlador() == basedatos::SQLITE)
       cadena +="strftime('%d-%m-%Y',diario.fecha) as fecha ,";

    if (basedatos::instancia()->cualControlador() == basedatos::POSTGRES)
       cadena +="to_char(diario.fecha,'DD/MM/YYYY') as fecha ,";

    if (basedatos::instancia()->cualControlador() == basedatos::MYSQL)
       cadena +="DATE_FORMAT(diario.fecha,'%d-%m-%Y') as fecha ,";

    cadena+="diario.cuenta,plancontable.descripcion,";
    cadena += "diario.concepto,diario.debe,diario.haber, diario.documento, configuracion.empresa from diario,plancontable, configuracion where ";
    cadena += "plancontable.codigo=diario.cuenta";
    QString filtro=qfiltro();
    if (filtro.length()>0) cadena += " and "+ filtro;
    cadena += " order by diario.asiento, diario.pase"; */


    /*setUserSqlConnection(int pageReport, QString dsName, QString dbType, QString dbName,
                                     QString dbHost, QString dbUser, QString dbPassword, int dbPort,
                                     QString dbConnectionName, QString sqlQuery, QString dbCoding,
                                     QString charsetCoding) */



   /* report->setUserSqlConnection(0, "", basedatos::instancia()->controlador(), basedatos::instancia()->nombre(),
                                 basedatos::instancia()->host(), basedatos::instancia()->usuario(), basedatos::instancia()->clave(),
                                 basedatos::instancia()->puerto(), "", cadena); */


    //QMessageBox::warning(this,tr("Conexión"), QLatin1String( RptSqlConnection ));

    //report->setSqlQuery(cadena);
    //report->activateUserSqlConnection(0,true);



    // report->printExec();
    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+(tr("diario.pdf"));
           report->printPDF(qfichero);
    }

}

void imprimediario::setValueImage(const int recNo, const QString paramName, QImage &paramValue, const int reportPage) {
    //RptFieldObject *fieldObject2 = fieldObject.parentBand->parentReportPage->findFieldObjectByName("field2");
    Q_UNUSED(recNo);
    Q_UNUSED(reportPage);

       if (paramName == "logo") {

           QString imagen=basedatos::instancia()->logo_empresa();
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
}


void imprimediario::generalatexdiario()
{
    int lineaspagina=42;
    if (ui.fechaactualcheckBox->isChecked()) lineaspagina-=3;
    if (ui.confiltrocheckBox->isChecked()) lineaspagina-=3;

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+="diario.tex";
   // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
    QFile fichero(adapta(qfichero));
    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    QString cabecerapagina="";
    if (ui.fechaactualcheckBox->isChecked())
          {
           cabecerapagina+= "\\begin{flushleft}"; cabecerapagina+="\n";
           cabecerapagina+= "\\today"; cabecerapagina+= "\n";
           cabecerapagina+= "\\newline"; cabecerapagina+="\n";
           cabecerapagina+= "\\end{flushleft}"; cabecerapagina+="\n";
          }
    
    cabecerapagina+= "\\begin{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "{\\huge \\textbf {";  cabecerapagina+="LIBRO DIARIO";
             cabecerapagina+="}}"; cabecerapagina+="\n";
    cabecerapagina+= "\\end{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "\n";
        
    cabecerapagina+= "\\begin{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "{\\Large \\textbf {";  cabecerapagina+=filtracad(nombreempresa());
             cabecerapagina+="}}"; cabecerapagina+="\n";
    cabecerapagina+= "\\end{center}"; cabecerapagina+= "\n";
    cabecerapagina+= "\n";
    cabecerapagina+="\\begin{center}\n";
    cabecerapagina+= "\\begin{tabular}{|r|l|l|l|l|r|r|}\n";
    cabecerapagina+="\\hline\n";
    cabecerapagina+="{\\scriptsize ";
    cabecerapagina+=tr("ASTO");
    cabecerapagina+="} & {\\scriptsize ";
    cabecerapagina+=tr("FECHA");
    cabecerapagina+= "} & {\\scriptsize ";
    cabecerapagina+=tr("CUENTA");
    cabecerapagina+= "} & {\\scriptsize ";
    cabecerapagina+= tr("TITULO CUENTA");
    cabecerapagina+= "} & {\\scriptsize ";
    cabecerapagina+= tr("DESCRIPCIÓN");
    cabecerapagina+="} & {\\scriptsize ";
    cabecerapagina+= tr("DEBE");
    cabecerapagina+= "} & {\\scriptsize ";
    cabecerapagina+= tr("HABER");
    cabecerapagina+= "} \\\\\n";
    cabecerapagina+="\\hline\n";
    QString piepagina="\\hline\n";
     piepagina+="\\end{tabular}\n";
     piepagina+="\\end{center}\n";
     if (ui.confiltrocheckBox->isChecked())
        {
	 piepagina+="{\\tiny ";
         piepagina+= tr("FILTRO: ");
     QString filtrodef=filtracad(qfiltro());
	 piepagina+=filtrodef;
	 piepagina+="}\n";
        }
     piepagina+="\\newpage \n";
	
    int totallineas = basedatos::instancia()->selectCountasientodiariofiltro(qfiltro());

    ui.progressBar->setMaximum(totallineas);

    QSqlQuery query = basedatos::instancia()->select7DiarioPlancontablecuentafiltroorderasientopase(qfiltro());
    if ( query.isActive() )
      {
          int linea=0;
          int glinea=0;
          double sumadebe=0;
          double sumahaber=0;
          QString cadcort;
          while (query.next())
	   {
	      if (linea==0) stream << cabecerapagina;
	      stream << "{\\tiny ";
              stream << query.value(0).toString();  // asiento
	      stream << "} & ";
	      stream << "{\\tiny ";
	      stream << query.value(1).toDate().toString("dd.MM.yyyy");
	      stream << "} &";
	      stream << "{\\tiny ";
	      stream << query.value(2).toString();
	      stream << "} &";
	      stream << "{\\tiny ";
          QString cadcons=query.value(3).toString();
	      cadcort=mcortaa(cadcons,40);
          cadcons=filtracad(cadcons);
          cadcort=filtracad(cadcort);
          if (cadcort!=cadcons)
              cadcort=cadcort+"\\#";
          if (cadcort.contains("\\\\#")) cadcort.replace("\\\\#","\\#");
          stream << cadcort;
	      stream << "} &";
	      stream << "{\\tiny ";
          cadcons=query.value(4).toString();
          cadcort=mcortaa(cadcons,40);
          cadcons=filtracad(cadcons);
          cadcort=filtracad(cadcort);
	      if (cadcort!=cadcons) cadcort=cadcort+"\\#";
          if (cadcort.contains("\\\\#")) cadcort.replace("\\\\#","\\#");
          stream << cadcort;
	      stream << "} &";
	      stream << "{\\tiny ";
              // aquí estamos **********************************************************
	      stream << formateanumerosep(query.value(5).toDouble(),comadecimal,decimales);
	      stream << "} &";
	      stream << "{\\tiny ";
	      stream << formateanumerosep(query.value(6).toDouble(),comadecimal,decimales);
	      stream << "}  \\\\ \n";
	      sumadebe+=query.value(5).toDouble();
	      sumahaber+=query.value(6).toDouble();
	      linea++;
	      glinea++;
	      if (linea>lineaspagina) 
	         {
	            if (glinea!=totallineas)
		  {
		     // sumas y siguen
             // QString cadnum;
		     stream << "\\hline\n";
		     stream  << "\\multicolumn{5}{|r|}{\\scriptsize " << tr("Sumas y siguen ...") << "} & {\\tiny " 
					    << formateanumerosep(sumadebe,comadecimal,decimales) ;
		     stream << "} & {\\tiny " << formateanumerosep(sumahaber,comadecimal,decimales)
			     << "}  \\\\";
	                     stream << piepagina;
		  }
	            linea=0;
	         }
	      ui.progressBar->setValue(glinea);
	    }
           QString cadnum;
           stream << "\\hline\n";
           stream  << "\\multicolumn{5}{|r|}{\\scriptsize " << tr("Sumas ...") << "} & {\\tiny "
	     << formateanumerosep(sumadebe,comadecimal,decimales) ;
           stream << "} & {\\tiny " << formateanumerosep(sumahaber,comadecimal,decimales)
	     << "}  \\\\";
           stream << piepagina;
      }
    
    
    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}


void imprimediario::pasafiltro( QString qfiltro )
{
  filtro=qfiltro;
  guardafiltro=qfiltro;
}

void imprimediario::pasafiltro_asiento(QString zfiltro) {
    filtro_asiento=zfiltro;
    if (filtro_asiento.isEmpty()) ui.imprime_asiento_checkBox->setEnabled(false);
}

void imprimediario::visualizar()
{
    generalatexdiario();

   int valor=consultalatex("diario");
   if (valor==1)
       QMessageBox::warning( this, tr("VISTA PREVIA DIARIO"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("VISTA PREVIA DIARIO"),
                             tr("PROBLEMAS al llamar a ")+visordvi());

    ui.progressBar->reset();
}


void imprimediario::imprimir()
{
    informe(); return;
/*
    // ----------------------------------------------------------------
    generalatexdiario();

   int valor=imprimelatex("diario");
   if (valor==1)
       QMessageBox::warning( this, tr("IMPRIMIR DIARIO"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("IMPRIMIR DIARIO"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("IMPRIMIR DIARIO"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

    ui.progressBar->reset();
*/
}


void imprimediario::latex()
{
    generalatexdiario();
    int valor=editalatex("diario");
    if (valor==1)
        QMessageBox::warning( this, tr("Imprime diario"),tr("PROBLEMAS al llamar al editor Latex"));

}


void imprimediario::generaXML()
{

    QDomDocument doc("Diario");
    QDomElement root = doc.createElement("Diario");
    doc.appendChild(root);

    addElementoTextoDom(doc,root,"NombreEmpresa",filtracadxml(nombreempresa()));
    addElementoTextoDom(doc,root,"Filtro",
                        ui.confiltrocheckBox->isChecked() ? filtracadxml(qfiltro()) : QString());

    int totallineas = basedatos::instancia()->selectCountasientodiariofiltro(qfiltro());

    ui.progressBar->setMaximum(totallineas+2);


    QSqlQuery query = basedatos::instancia()->select7DiarioPlancontablecuentafiltroorderasientopase(qfiltro());
    int glinea=0;
    if ( query.isActive() )
      {
          while (query.next())
           {
              QDomElement tag = doc.createElement("Line");
              root.appendChild(tag);
              // asiento
              addElementoTextoDom(doc,tag,"Asiento",filtracadxml(query.value(0).toString()));
              addElementoTextoDom(doc,tag,"Fecha",filtracadxml(query.value(1).toDate().toString("yyyy/MM/dd")));
              addElementoTextoDom(doc,tag,"Cuenta",filtracadxml(query.value(2).toString()));
              addElementoTextoDom(doc,tag,"Descrip",filtracadxml(query.value(3).toString()));
              addElementoTextoDom(doc,tag,"Concepto",filtracadxml(query.value(4).toString()));
              addElementoTextoDom(doc,tag,"Debe",
                              formateanumero(query.value(5).toDouble(),comadecimal,decimales));
              addElementoTextoDom(doc,tag,"Haber",
                              formateanumero(query.value(6).toDouble(),comadecimal,decimales));
              addElementoTextoDom(doc,tag,"Documento",filtracadxml(query.value(7).toString()));
              glinea++;
              ui.progressBar->setValue(glinea);
              QCoreApplication::processEvents();
          }
      }

      QString xml = doc.toString();

      QString qfichero=dirtrabajo();
      qfichero.append(QDir::separator());
      qfichero=qfichero+tr("diario.xml");
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

     QString cadfichjasper=trayreport();
     cadfichjasper.append(QDir::separator());
     cadfichjasper+="diario.jasper";

     QString cadpdf=dirtrabajo();
     cadpdf.append(QDir::separator());
     cadpdf+="diario.pdf";

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


     informe_jasper_xml(cadfichjasper, pasa,
                        "/Diario/Line", cadpdf,
                        rutagraf);

     ui.progressBar->setValue(totallineas+2);

     QMessageBox::information( this, tr("XML DIARIO"),tr("El archivo XML se ha generado"));
     ui.progressBar->reset();
}

void imprimediario::on_imprime_asiento_checkBox_toggled(bool checked)
{
    if (checked) filtro=filtro_asiento;
      else filtro=guardafiltro;

}
