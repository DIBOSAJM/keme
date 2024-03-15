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

#include "notas.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>
#include <qtrpt.h>

notas::notas(bool cargar) : QDialog() {
    ui.setupUi(this);


connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(botonaceptar()));

connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprimir()));

connect(ui.texpushButton,SIGNAL(clicked()),SLOT(editlatex()));

connect(ui.copiarpushButton, SIGNAL(clicked()), SLOT(copiar()));

modoconsulta=false;

#ifdef NOEDITTEX
 ui.texpushButton->hide();
#endif

// cargamos contenido
 if (cargar)
 {
  QString contenido=basedatos::instancia()->notas();
  ui.textEdit->setText(contenido);
 }
}

void notas::pasa_contenido(QString contenido)
{
  ui.textEdit->setText(contenido);
}

void notas::activa_modoconsulta()
{
    modoconsulta=true;
    ui.textEdit->setReadOnly(true);

}

void notas::esconde_tex_imprimir()
{
  ui.texpushButton->hide();
  ui.imprimirpushButton->hide();
  ui.cancelarpushButton->hide();
}

void notas::botonaceptar()
{
    if (!modoconsulta)
    // grabamos contenido
     {
      QString contenido=ui.textEdit->toPlainText();
      basedatos::instancia()->grabarnotas(contenido);
     }
    accept();
}

void notas::copiar()
{
    QClipboard *cb = QApplication::clipboard();
    cb->setText(ui.textEdit->toPlainText());
    QMessageBox::information( this, tr("Notas de empresa"),
                              tr("Se ha pasado el contenido al portapapeles") );

}

void notas::generalatex()
{
    QString qfichero=dirtrabajo();
    qfichero.append(QDir::separator());
    qfichero+="notas.tex";
    // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
     QFile fichero(adapta(qfichero));
     if (! fichero.open( QIODevice::WriteOnly ) ) return;
     QTextStream stream( &fichero );
     stream.setEncoding(QStringConverter::Utf8);

     stream << cabeceralatex();
     // stream << margen_extra_latex();

     QString cabecerapagina="";
     cabecerapagina+= "\\begin{flushleft}"; cabecerapagina+="\n";
     cabecerapagina+= "\\today"; cabecerapagina+= "\n";
     cabecerapagina+= "\\newline"; cabecerapagina+="\n";
     cabecerapagina+= "\\end{flushleft}"; cabecerapagina+="\n";

     cabecerapagina+= "\\begin{center}"; cabecerapagina+= "\n";
     cabecerapagina+= "{\\huge \\textbf {";  cabecerapagina+="NOTAS DE LA EMPRESA";
              cabecerapagina+="}}"; cabecerapagina+="\n";
     cabecerapagina+= "\\end{center}"; cabecerapagina+= "\n";
     cabecerapagina+= "\n";

     cabecerapagina+= "\\begin{center}"; cabecerapagina+= "\n";
     cabecerapagina+= "{\\Large \\textbf {";  cabecerapagina+=filtracad(nombreempresa());
              cabecerapagina+="}}"; cabecerapagina+="\n";
     cabecerapagina+= "\\end{center}"; cabecerapagina+= "\n";
     cabecerapagina+= "\n";

     stream << cabecerapagina;

     stream << ui.textEdit->toPlainText();

     // imprimimos final del documento latex
     stream << "% FIN_CUERPO" << "\n";
     stream << "\\end{document}" << "\n";

     fichero.close();

}


void notas::imprimir()
{
    informe();
    /*
    generalatex();
    int valor=imprimelatex("notas");
    if (valor==1)
        QMessageBox::warning( this, tr("IMPRIMIR NOTAS"),tr("PROBLEMAS al llamar a Latex"));
    if (valor==2)
        QMessageBox::warning( this, tr("IMPRIMIR NOTAS"),
                                 tr("PROBLEMAS al llamar a 'dvips'"));
    if (valor==3)
        QMessageBox::warning( this, tr("IMPRIMIR NOTAS"),
                              tr("PROBLEMAS al llamar a ")+programa_imprimir());
  */
}

void notas::editlatex()
{
  generalatex();
  int valor=editalatex("notas");
  if (valor==1)
      QMessageBox::warning( this, tr("LATEX - NOTAS"),tr("PROBLEMAS al llamar al editor Latex"));
}

void notas::cambia_titulo(QString titulo)
{
    setWindowTitle(titulo);
}


void notas::informe() {
    QString fileName;
    fileName = ":/informes/notas.xml";

    QtRPT *report = new QtRPT(this);
    //report->recordCount << 1;
    if (!report->loadReport(fileName))
      {
        QMessageBox::information( this, tr("Imprimir Estado Contable"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
      }

    QObject::connect(report, &QtRPT::setDSInfo,
                     [&](DataSetInfo &dsInfo) {
        dsInfo.recordCount=1;
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
        Q_UNUSED(recNo);
        Q_UNUSED(reportPage);

        if (paramName == "NOTAS") paramValue =  ui.textEdit->toPlainText();

    });


    // report->printExec(true);
    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+(tr("notas.pdf"));
           report->printPDF(qfichero);
    }

}
