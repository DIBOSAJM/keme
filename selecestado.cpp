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

#include "selecestado.h"
#include <QSqlQuery>
#include "basedatos.h"
#include "generadorestados.h"
#include "calcestado.h"
#include "funciones.h"
#include "privilegios.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

selecestado::selecestado(QString qusuario) : QDialog() {
    ui.setupUi(this);


 QSqlQuery query = basedatos::instancia()->selectTitulocabeceraestadosordertitulo();

 if ( query.isActive() ) {
          while ( query.next() ) 
                   ui.seleclistWidget->addItem (query.value(0).toString()) ;
	  }
connect(ui.nuevopushButton,SIGNAL(clicked()),SLOT(nuevogenestado()));
connect(ui.editarpushButton,SIGNAL(clicked()),SLOT(edgenestado()));
connect(ui.ejecutarpushButton,SIGNAL(clicked()),SLOT(calculaestado()));
connect(ui.exportarpushButton,SIGNAL(clicked()),SLOT(exportestado()));
connect(ui.importarpushButton,SIGNAL(clicked()),SLOT(importestado()));
connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borrarestado()));

connect(ui.seleclistWidget,SIGNAL(itemDoubleClicked ( QListWidgetItem *)) ,
           this,SLOT(calculaestado()));
 // connect(ui.seleclistWidget,SIGNAL(currentRowChanged ( int )),this,SLOT(seleccioncambiada()));
 // connect(ui.seleclistWidget,SIGNAL(itemDoubleClicked ( QListWidgetItem *)) ,
 //          this,SLOT(execasmodelo()));
 usuario=qusuario;

 QString privileg=basedatos::instancia()->privilegios(qusuario);

 if (privileg[edicion_estados]=='0')
    {
     ui.editarpushButton->setEnabled(false);
     ui.borrarpushButton->setEnabled(false);
     ui.nuevopushButton->setEnabled(false);
     ui.importarpushButton->setEnabled(false);
    }

 if (privileg[ejecutar_estados]=='0')
   {
    ui.ejecutarpushButton->setEnabled(false);
   }
}

QString selecestado::elecactual()
{
  QString resultado="";
  if (ui.seleclistWidget->count()==0) return resultado;
  if (ui.seleclistWidget->currentRow()>=0)
 	return ui.seleclistWidget->currentItem()->text();
   else return resultado;
}


void selecestado::nuevogenestado()
{
 generadorestados *g = new generadorestados();
 g->exec();
 delete(g);
 refresca();
}

void selecestado::edgenestado()
{
   QProgressDialog progreso(tr("Cargando Estado Contable ..."), 0, 0, 0);
   progreso.setMinimumDuration(0);
   progreso.setMaximum(0);
   //progreso.setValue(0);
   progreso.setWindowTitle("Estados Contables");
   progreso.setWindowModality(Qt::WindowModal);
   progreso.show();
   //progreso.setValue(1);
   QCoreApplication::processEvents();
   //QMessageBox::warning( this, tr("ESTADO CONTABLE"),
   //             tr("carga estado contable"));
   generadorestados *estado=new generadorestados();
   estado->cargaestado(elecactual());
   progreso.close();
   estado->exec();
   delete(estado);
   refresca();
}

void selecestado::calculaestado()
{
  if (basedatos::instancia()->gestiondeusuarios() && !privilegios[ejecutar_estados]) return;
  QString eleccion=elecactual();
 calcestado *c = new calcestado();
 c->cargaestado(eleccion);
 c->exec();
 delete(c);
}

void selecestado::borrarestado()
{
  QString eleccion=elecactual();
  if (QMessageBox::question(
            this,
            tr("¿ Borrar estado contable ?"),
          tr("¿ Desea borrar '%1' ?").arg(eleccion )) == QMessageBox::Yes )
                     {
                          borraestado(eleccion);
                     }
 refresca();
}


void selecestado::exportestado()
{

  QString eleccion=elecactual();

  QString nombre;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  dir->filtrar("*.est.xml");
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
  filtros << tr("Archivos con definición de estado contable (*.est.xml)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("ARCHIVO PARA EXPORTAR ESTADO CONTABLE"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
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

  if (nombre.right(8)!=".est.xml") nombre=nombre+".est.xml";
   QFile estado( adapta(nombre)  );
   if (estado.exists() && QMessageBox::question(this,
       tr("¿ Sobreescribir ? -- Exportar estado contable"),
       tr("'%1' ya existe."
          "¿ Desea sobreescribirlo ?")
         .arg( nombre )) == QMessageBox::No )
     return ;
   if (exportarestado(eleccion,nombre))
      {
       QMessageBox::information( this, tr("EXPORTAR ESTADO CONTABLE"),
                    tr("El archivo se ha generado correctamente"));
      }
     else
       QMessageBox::warning( this, tr("EXPORTAR ESTADO CONTABLE"),
                    tr("ERROR al generar el archivo"));

}


void selecestado::importestado()
{
  QString cadfich;


#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  dir->filtrar("*.est.xml");
  if (dir->exec() == QDialog::Accepted)
    {
      cadfich=dir->ruta_actual();
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
  filtros << tr("Archivos con definición de estado contable (*.est.xml)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajobd())+QDir::separator()+"zz_Estados_Contables");
  // dialogofich.setDirectory("qrc:/estados/estados/");
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
           cadfich=fileNames.at(0);
          }
     }
#endif

  if (cadfich.isEmpty()) return;

  int resultado=importarestado(cadfich);
  refresca();
  if (resultado==1)
      {
       QMessageBox::information( this, tr("IMPORTAR ESTADOS CONTABLES"),
                    tr("El archivo seleccionado se ha importado correctamente"));
       return;
      }
  if (resultado==2)
      {
       QMessageBox::warning( this, tr("IMPORTAR ESTADOS CONTABLES"),
                    tr("ERROR: El estado contable ya existe"));
       return;
      }
    else
      {
       QMessageBox::warning( this, tr("IMPORTAR ESTADOS CONTABLES"),
                    tr("ERROR al recuperar el fichero"));
       return;
      }

}


void selecestado::refresca()
{
 ui.seleclistWidget->clear();

 QSqlQuery query = basedatos::instancia()->selectTitulocabeceraestadosordertitulo();

 if ( query.isActive() ) {
          while ( query.next() )
                   ui.seleclistWidget->addItem (query.value(0).toString()) ;
          }

}
