/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

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

#include "procesamemoria.h"
#include "basedatos.h"
#include "funciones.h"
#include "directorio.h"
#include <QSqlQuery>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>

procesamemoria::procesamemoria() : QDialog() {
    ui.setupUi(this);

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

 connect(ui.buscapushButton,SIGNAL( clicked()),this,SLOT(buscafich()));
 connect(ui.fichlineEdit,SIGNAL(textChanged(QString)),this,SLOT(campocambiado()));
 connect(ui.procesarpushButton,SIGNAL( clicked()),this,SLOT(procesafich()));

 cad_ejercicio=tr("EJERCICIO");
 cad_fechaconst=tr("FECHACONST");

 apertura=tr("APERTURA");
 cierre=tr("CIERRE");
 ciudad=tr("CIUDAD");
 provincia=tr("PROVINCIA");
 domicilio=tr("DOMICILIO");
 cpostal=tr("CPOSTAL");
 objeto=tr("OBJETO");
 actividad=tr("ACTIVIDAD");
 idregistral=tr("IDREGISTRAL");
 cif=tr("CIF");
 empresa=tr("EMPRESA");

}



QString procesamemoria::ejercicio()
{
   return ui.ejerciciocomboBox->currentText();
}


void procesamemoria::buscafich()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajobd());
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      ui.fichlineEdit->setText(dir->ruta_actual());
    }
  delete(dir);
#else


  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Archivos de texto plano (*.tex *.htm *.html)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(dirtrabajobd());
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
             QString nombre =fileNames.at(0);
             ui.fichlineEdit->setText(nombre);
          }
     }
#endif
}

void procesamemoria::campocambiado()
{
 if (ui.fichlineEdit->text().length()>0) ui.procesarpushButton->setEnabled(true);
    else ui.procesarpushButton->setEnabled(false);
}


void procesamemoria::procesafich()
{
  QStringList variables;
  QStringList equivalencias;
  QFile fichero(adapta(ui.fichlineEdit->text()));
  if (!fichero.exists())
    {
     QMessageBox::warning( this, tr("PROCESAR MEMORIA"),
              tr("ERROR: Problemas al abrir el archivo"));
     return;
    }
  QTextStream stream( &fichero );
  stream.setEncoding(QStringConverter::Utf8);
  QString linea;
  int lineas=0;
   if ( fichero.open( QIODevice::ReadOnly ) ) 
     {
         while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            lineas++;
           }
         fichero.close();
      }

 QProgressDialog progreso(tr("Procesando información ..."), 0, 1, lineas);
 progreso.setWindowModality(Qt::WindowModal);

  QString fichmod=dirtrabajo();
  int pos=ui.fichlineEdit->text().lastIndexOf(QDir::separator());
  if (pos<0) pos=0;
  QString cadfich2=ui.fichlineEdit->text().mid(pos);
  fichmod+=cadfich2.insert(cadfich2.lastIndexOf('.'),ui.ejerciciocomboBox->currentText());
  QFile escritura( adapta(fichmod) );

  if ( !escritura.open( QIODevice::WriteOnly ) )
    {
     QMessageBox::warning( this, tr("PROCESAR MEMORIA"),
              tr("ERROR: Problemas al escribir el archivo de destino"));
     return;
    }
  QTextStream streamescritura( &escritura );
  streamescritura.setEncoding(QStringConverter::Utf8);

  pos=0;
   if ( fichero.open( QIODevice::ReadOnly ) ) 
     {
         while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            pos++;
            progreso.setValue(pos);
            if (linea.contains("(-"+cad_ejercicio+"-)"))
               {
                linea.replace("(-"+cad_ejercicio+"-)",ui.ejerciciocomboBox->currentText());
               }
            if (linea.contains("(-"+cad_fechaconst+"-)"))
               {
                linea.replace("(-"+cad_fechaconst+"-)",fechaliteral(basedatos::instancia()->fechaconst()));
               }
            if (linea.contains("(-"+apertura+"-)"))
               {
                linea.replace("(-"+apertura+"-)",fechaliteral(inicioejercicio(ui.ejerciciocomboBox->currentText())));
               }
            if (linea.contains("(-"+cierre+"-)"))
               {
                linea.replace("(-"+cierre+"-)",fechaliteral(finejercicio(ui.ejerciciocomboBox->currentText())));
               }
            if (linea.contains("(-"+empresa+"-)"))
               {
                linea.replace("(-"+empresa+"-)",basedatos::instancia()->selectEmpresaconfiguracion ());
               }
            if (linea.contains("(-"+ciudad+"-)"))
               {
                linea.replace("(-"+ciudad+"-)",basedatos::instancia()->ciudad());
               }
            if (linea.contains("(-"+provincia+"-)"))
               {
                linea.replace("(-"+provincia+"-)",basedatos::instancia()->provincia());
               }
            if (linea.contains("(-"+domicilio+"-)"))
               {
                linea.replace("(-"+domicilio+"-)",basedatos::instancia()->domicilio());
               }
            if (linea.contains("(-"+cpostal+"-)"))
               {
                linea.replace("(-"+cpostal+"-)",basedatos::instancia()->cpostal());
               }
            if (linea.contains("(-"+objeto+"-)"))
               {
                linea.replace("(-"+objeto+"-)",basedatos::instancia()->objeto());
               }
            if (linea.contains("(-"+actividad+"-)"))
               {
                linea.replace("(-"+actividad+"-)",basedatos::instancia()->actividad());
               }
            if (linea.contains("(-"+idregistral+"-)"))
               {
                linea.replace("(-"+idregistral+"-)",basedatos::instancia()->idregistral());
               }
            if (linea.contains("(-"+cif+"-)"))
               {
                linea.replace("(-"+cif+"-)",basedatos::instancia()->cif());
               }
            // Buscamos tokens
            while (linea.contains("(-"))
               {
                 int posini=linea.indexOf("(-")+2;
                 int posfin=linea.indexOf("-)");
                 if (posini>posfin) break;
                 QString token=linea.mid(posini,posfin-posini);
                 // esto sirve para guardar cadenas simplificadas de estados contables
                 // en variables está la simplificación
                 // en equivalencias está el nombre completo del estado contable
                 if (token.contains(';'))
                    {
                     QString var=token.left(token.indexOf(';'));
                     QString equiv=token.mid(token.indexOf(';')+1);
                     if (var.length()==0 || equiv.length()==0) break;
                     variables << var;
                     equivalencias << equiv;
                     linea.clear();
                    }
                 // aquí analizamos el contenido de un nodo de estado contable
                 // formato:  nombre:nodo,ejercicio
                 if (token.contains(':'))
                   {
                    // “(-bal:TOTAL,1,2008-)”  El segundo parámetro es el de la parte del estado
                     // en la tabla estado hay un campo que se llama parte1 (bool)
                    if (token.indexOf(':')>token.indexOf(',')) break;
                    QString estado=token.left(token.indexOf(':'));
                    QString nodo=token.mid(token.indexOf(':')+1,token.indexOf(',')-token.indexOf(':')-1);
                    QString final=token.mid(token.indexOf(',')+1);
                    QString cadparte1=final.left(final.indexOf(','));
                    QString ejercicio=final.mid(final.indexOf(',')+1);
                    int posicion=variables.lastIndexOf(estado);
                    if (posicion>=0) estado=equivalencias.at(posicion);
                    bool parte1=cadparte1.contains('1');
                    // QMessageBox::information( this, tr("PROCESAR MEMORIA"),
                    //    estado+" -- "+nodo+" -- "+cadparte1+" -- "+ejercicio);
                    QString cadres=basedatos::instancia()->cadvalorestado(estado,nodo,ejercicio,parte1);
                    linea.replace("(-"+token+"-)",cadres);
                   }
               }
            streamescritura << linea << "\n";
           }
         fichero.close();
      }
  escritura.close();

  QMessageBox::information( this, tr("PROCESAR MEMORIA"),
      tr("Se ha procesado el contenido de la memoria\n"
         "El archivo de destino se ha guardado en:\n")+fichmod);
  accept();
}
