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

#include "selecasmod.h"
#include "edasientomodelo.h"
#include <QSqlQuery>
#include "basedatos.h"
#include "execasientomodelo.h"
#include "funciones.h"
#include "privilegios.h"
#include "tipos_automaticos.h"
#include "directorio.h"
#include <QMessageBox>
#include <QFileDialog>

selecasmod::selecasmod(QString qusuario) : QDialog() {
    ui.setupUi(this);

// -------------------------------------------------------------

 /*QSqlQuery query = basedatos::instancia()->selectAsientomodelocab_as_modeloorderasientomodelo();
 if ( query.isActive() ) {
          while ( query.next() ) 
                   ui.seleclistWidget->addItem (query.value(0).toString()) ;
          }*/

connect(ui.nuevopushButton,SIGNAL(clicked()),SLOT(nuevoasautomatico()));
connect(ui.editarpushButton,SIGNAL(clicked()),SLOT(editaasmodelo()));
connect(ui.ejecutarpushButton,SIGNAL(clicked()),SLOT(execasmodelo()));
connect(ui.exportarpushButton,SIGNAL(clicked()),SLOT(exportaasmodelo()));
connect(ui.importarpushButton,SIGNAL(clicked()),SLOT(importaasmodelo()));
connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borramodelo()));
connect(ui.tipospushButton,SIGNAL(clicked()),SLOT(tipos()));
connect(ui.aspiradorapushButton,SIGNAL(clicked()),SLOT(aspiradora()));

 // connect(ui.seleclistWidget,SIGNAL(currentRowChanged ( int )),this,SLOT(seleccioncambiada()));
 connect(ui.treeWidget,SIGNAL(doubleClicked(QModelIndex)) ,
           this,SLOT(execasmodelo()));

 QString privileg=basedatos::instancia()->privilegios(qusuario);

 if (privileg[edicion_automaticos]=='0')
    {
     ui.editarpushButton->setEnabled(false);
     ui.borrarpushButton->setEnabled(false);
     ui.importarpushButton->setEnabled(false);
     ui.nuevopushButton->setEnabled(false);
     ui.tipospushButton->setEnabled(false);
    }

 if (privileg[ejecutar_automaticos]=='0')
   {
     ui.ejecutarpushButton->setEnabled(false);
     ui.aspiradorapushButton->setEnabled(false);
   }

 if (privileg[exportar_automaticos]=='0')
     ui.exportarpushButton->setEnabled(false);

 usuario=qusuario;
 activa_msj_tabla=false;

 QStringList columnas;
 columnas << tr("ASIENTOS PREDEFINIDOS");
 ui.treeWidget->setHeaderLabels(columnas);
 ui.treeWidget->setAlternatingRowColors(true);

 cargaarbol();
 //ui.treeView->setModel(&modeloarbol);
 //ui.treeView->expandAll();
 //ui.treeView->resizeColumnToContents(0);
 //ui.treeView->setAlternatingRowColors ( true);
 // ui.treeView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

}

void selecasmod::activar_msj_tabla()
{
  activa_msj_tabla=true;
}

QString selecasmod::elecactual()
{
 // QString resultado="";
/*  if (ui.seleclistWidget->count()==0) return resultado;
  if (ui.seleclistWidget->currentRow()>=0)
 	return ui.seleclistWidget->currentItem()->text();
   else return resultado;*/

 if (ui.treeWidget->currentItem()!=0)
   {
     QString codigo=ui.treeWidget->currentItem()->text(0);
     if (lista_codigo.contains(codigo)) return QString();
     if( basedatos::instancia()->asientomodeloencab_as_modelo(codigo))
         return codigo;
       else return QString();
   }
  else return QString();

 /* QModelIndex indice=ui.treeView->currentIndex();
 if (!indice.isValid()) return QString();
 QList<QVariant> datos = modeloarbol.datosfiladeindice(indice);
 QString resultado=datos.at(0).toString();
 if( basedatos::instancia()->asientomodeloencab_as_modelo(resultado))
     return resultado;
   else return QString(); */
}


void selecasmod::refresca()
{
 /* ui.seleclistWidget->clear();
 QSqlQuery query = basedatos::instancia()->selectAsientomodelocab_as_modeloorderasientomodelo();
 if ( query.isActive() ) {
          while ( query.next() )
                   ui.seleclistWidget->addItem (query.value(0).toString()) ;
          } */
    lista_codigo.clear();
    lista_puntero_item.clear();
    ui.treeWidget->clear();
    cargaarbol();

 // modeloarbol.borraarbol();
 // modeloarbol.resetea();
 // ui.treeView->expandAll();
 // cargaarbol();
}

void selecasmod::nuevoasautomatico()
{
 edasientomodelo *e = new edasientomodelo();
 e->exec();
 delete(e);
 refresca();

}

void selecasmod::editaasmodelo()
{
  QString eleccion;
  eleccion=elecactual();
  if (eleccion.isEmpty()) return;
 edasientomodelo *e = new edasientomodelo();
 e->cargadatos(eleccion);
 e->exec();
 delete(e);
 refresca();
}


void selecasmod::execasmodelo()
{
 /*if (sobreescribeproxasiento())
    {
      QMessageBox::warning( this, tr("Tabla de asientos"),tr("ERROR: el número de próximo asiento en "
                           "configuración ya existe. Debe de ser sustituido por uno más elevado\n"));
      return;
     }*/

  QString eleccion;
  eleccion=elecactual();
  if (eleccion.isEmpty()) return;
 execasientomodelo *e = new execasientomodelo(usuario);
 e->cargadatos(eleccion);
 if (activa_msj_tabla) e->activar_msj_tabla();
 e->exec();
 delete(e);
}

void selecasmod::borramodelo()
{
  QString eleccion;
  eleccion=elecactual();
  if (eleccion.isEmpty()) return;

  if (QMessageBox::question(
            this,
            tr("¿ Borrar asiento automático ?"),
            tr("¿ Desea borrar '%1' ?").arg(eleccion )) == QMessageBox::Yes )
                          borraasientomodelo(eleccion);
 refresca();
}



void selecasmod::exportaasmodelo()
{
  QString eleccion;
  eleccion=elecactual();
  if (eleccion.isEmpty()) return;

  QString nombre;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajo());
  dir->filtrar("*.ast.xml");
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
  filtros << tr("Archivos con definición de asientos automáticos (*.ast.xml)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajobd()));
  dialogofich.setWindowTitle(tr("ARCHIVO PARA EXPORTAR ASIENTO AUTOMÁTICO"));
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

  if (nombre.right(8)!=".ast.xml") nombre=nombre+".ast.xml";
   QFile estado( adapta(nombre)  );
   if (estado.exists() && QMessageBox::question(this,
       tr("¿ Sobreescribir ? -- Exportar asiento automático"),
       tr("'%1' ya existe."
          "¿ Desea sobreescribirlo ?")
         .arg( nombre )) == QMessageBox::No )
     return ;
   if (exportarasmodelo(eleccion,nombre))
      {
       QMessageBox::information( this, tr("EXPORTAR ESTADO CONTABLE"),
                    tr("El archivo se ha generado correctamente"));
      }
     else
       QMessageBox::warning( this, tr("EXPORTAR ESTADO CONTABLE"),
                    tr("ERROR al generar el archivo"));

}


void selecasmod::importaasmodelo()
{
  QString cadfich;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  dir->filtrar("*.ast.xml");
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
  filtros << tr("Archivos con definición de asiento automático (*.ast.xml)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajobd()));
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

  int resultado=importarasmodelo(cadfich);
  if (resultado==1)
      {
       QMessageBox::information( this, tr("IMPORTAR ASIENTO AUTOMÁTICO"),
                    tr("El archivo seleccionado se ha importado correctamente"));
       refresca();
       return;
      }
  if (resultado==2)
      {
       QMessageBox::warning( this, tr("IMPORTAR ASIENTO AUTOMÁTICO"),
                    tr("ERROR: El asiento automático ya existe"));
       return;
      }
    else
      {
       QMessageBox::warning( this, tr("IMPORTAR ASIENTO AUTOMÁTICO"),
                    tr("ERROR al recuperar el fichero"));
       return;
      }

}


void selecasmod::tipos()
{
 tipos_automaticos *a = new tipos_automaticos();
 a->exec();
 delete(a);
}


void selecasmod::cargaarbol()
{
    /* QList<QVariant> datos;
    datos << tr("GENERAL");
    TreeItem *elementogeneral=new TreeItem(datos, modeloarbol.raiz());
    modeloarbol.raiz()->appendChild(elementogeneral); */
  QStringList lista= basedatos::instancia()->tipos_automaticos();
  for (int veces=0; veces<lista.count(); veces++)
     {
      QStringList datos;
      datos << lista.at(veces);
      lista_codigo << lista.at(veces);
      QTreeWidgetItem *elemento = new QTreeWidgetItem(ui.treeWidget,datos);
      elemento->setExpanded(true);
      lista_puntero_item << elemento;
      /* QList<QVariant> datos;
      datos << lista.at(veces);
      modeloarbol.raiz()->appendChild(new TreeItem(datos, modeloarbol.raiz()));*/
     }

//  TreeItem *elemento;
  QSqlQuery query = basedatos::instancia()->selectAsientomodelocab_as_modeloorderasientomodelo();
  if ( query.isActive() ) {
           while ( query.next() )
                   // ui.seleclistWidget->addItem (query.value(0).toString()) ;
              {
               // bool encontrada=false;
               for (int veces=0; veces<lista_codigo.count(); veces++)
                   {
                    if (lista_codigo.at(veces)==query.value(1).toString())
                       {
                        QStringList datos;
                        datos << query.value(0).toString();
                        // lista_puntero_item.at(veces);
                        // QTreeWidgetItem *elemento =
                        new QTreeWidgetItem(lista_puntero_item.at(veces),datos);
                        // elemento->setExpanded(true);
                        break;
                       }
                   }
               // elemento=modeloarbol.buscacadena(modeloarbol.raiz(),query.value(1).toString(),
               //                                 &encontrada);
               // QList<QVariant> datos;
               // datos << query.value(0).toString();
               // if (encontrada)
               //    {
               //     elemento->appendChild(new TreeItem(datos, elemento));
               //    }
                  // else elementogeneral->appendChild(new TreeItem(datos, elemento));
               }
           }

}


void selecasmod::aspiradora()
{
  QString eleccion;
  eleccion=elecactual();
  if (eleccion.isEmpty()) return;
 // setVisible(false);
 execasientomodelo *e = new execasientomodelo(usuario);
 e->cargadatos(eleccion);
 // e->exec();
 e->aspiradora();
 delete(e);
 // setVisible(true);
}
