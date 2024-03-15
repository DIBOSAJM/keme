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

#include "asigna_fichdoc.h"
#include "basedatos.h"
#include "funciones.h"

#include "directorio.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>


asigna_fichdoc::asigna_fichdoc(QString elasiento, QString qdocumento, QString ejercicio) : QDialog() {
    ui.setupUi(this);

etiqueta = new CustomQLabel;
etiqueta->setHidden(false);
etiqueta->setMinimumWidth(100);
etiqueta->setMinimumHeight(100);
etiqueta->setText(tr("Soltar archivo aquí"));
etiqueta->setAlignment(Qt::AlignHCenter);
etiqueta->setFrameStyle(QFrame::Panel | QFrame::Sunken);
ui.verticalLayout->addWidget(etiqueta);

connect(ui.visdocdefpushButton ,SIGNAL(clicked()),SLOT(visdocdef()));

connect(ui.borravisdocpushButton ,SIGNAL(clicked()),SLOT(suprimedocdef()));

connect(ui.fichdocpushButton ,SIGNAL(clicked()),SLOT(buscacopiadoc()));
connect(ui.visdocpushButton ,SIGNAL(clicked()),SLOT(visdoc()));

connect(ui.borraasdocpushButton ,SIGNAL(clicked()),SLOT(borraasignaciondoc()));
connect(ui.aceptarpushButton ,SIGNAL(clicked()),SLOT(botonaceptar()));

connect(etiqueta, SIGNAL(filedroped()),SLOT(botonaceptar()));

documento=qdocumento;
setWindowTitle(tr("Documento en asiento: ")+elasiento+" - "+documento);
asiento=elasiento;
ui.ejerciciolineEdit->setText(ejercicio);
}


void asigna_fichdoc::pasafichdoc(QString cadena)
{
    ui.docasignadolineEdit->setText(cadena);
}


void asigna_fichdoc::visdocdef()
{

  QString fichdoc=ui.docasignadolineEdit->text();

   if (fichdoc.isEmpty()) return;

   if (basedatos::instancia()->gestor_doc_bd()) {
       QString cadnum;
       if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
           else cadnum=fichdoc.section('-',0,0);
       qlonglong num=cadnum.toLongLong();
       QByteArray outByteArray;
       outByteArray=basedatos::instancia()->documento_bd(num);
       QString url=QStandardPaths::writableLocation(
                   QStandardPaths::TempLocation).append(QDir::separator()).append(fichdoc);
       QFile file(url);
       file.open(QIODevice::WriteOnly);

       //EL BLOB OBTENIDO DE LA CONSULTA LO PASAMOS AL ARCHIVO
       file.write(outByteArray);
       file.close();
       QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
   }
   else {
          // documento en fichero físico
          fichdoc=expanderutadocfich(fichdoc);
          QDesktopServices::openUrl(QUrl(fichdoc.prepend("file:"),QUrl::TolerantMode));
          /*if (!ejecuta(aplicacionabrirfich(extensionfich(fichdoc)),fichdoc))
             QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+fichdoc+tr(" con ")+
                             aplicacionabrirfich(extensionfich(fichdoc)));*/
        }

}


void asigna_fichdoc::suprimedocdef()
{
    QString fichdoc=ui.docasignadolineEdit->text();

   if (fichdoc.isEmpty()) return;

   if (QMessageBox::question(
          this,
          tr("Tabla de asientos"),
          tr("¿ Desea borrar el fichero asignado del documento ?"),
          tr("&Sí"), tr("&No"),
          QString(), 0, 1 ) ==1 )
                        return;

   bool gestor_bd=basedatos::instancia()->gestor_doc_bd();

   if (gestor_bd) {
       QString cadnum;
       if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
           else cadnum=fichdoc.section('-',0,0);
       qlonglong num=cadnum.toLongLong();
       basedatos::instancia()->borra_doc_gestor(num);
       ui.docasignadolineEdit->clear();
       basedatos::instancia()->borra_docdiario (asiento,ui.ejerciciolineEdit->text());
       return;
   }

  fichdoc=adapta(expanderutadocfich(fichdoc));
  QFile file(fichdoc);



   if (!file.remove())
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se ha podido borrar ")+ui.docasignadolineEdit->text());
    else ui.docasignadolineEdit->clear();

 // falta eliminar físicamente del campo del diario
 basedatos::instancia()->borra_docdiario (asiento,ui.ejerciciolineEdit->text());
}


void asigna_fichdoc::buscacopiadoc()
{
#ifdef NOMACHINE
  QString rutacarga=rutacargadocs().isEmpty() ? dirtrabajo():rutacargadocs());
  QDir direc(rutacarga);
  if (!direc.exists(rutacarga))
     {
       QMessageBox::warning( this, tr("Cargar documento"),
                             tr("ERROR: la ruta para cargar documentos no es correcta") );
       return;

     }
  directorio *dir = new directorio(this);
  dir->pasa_directorio(rutacarga);

  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      ui.fichdoclineEdit->setText(dir->ruta_actual());
    }
  delete(dir);
#else

  // QString fileName = QFileDialog::getOpenFileName(this, tr("Open a file"), "/public/files", tr("Teks File (*.txt)"));
  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setViewMode(QFileDialog::List);
  //dialogofich.setFileMode(QFileDialog::AnyFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Todos los archivos (*)");
  filtros << tr("Archivos jpg (*.jpg)");
  filtros << tr("Archivos jpg (*.jpeg)");
  filtros << tr("Archivos png (*.png)");
  filtros << tr("Archivos pdf (*.pdf)");
  dialogofich.setNameFilters(filtros);
  if (rutacargadocs().isEmpty())
     dialogofich.setDirectory(dirtrabajo());
    else
      dialogofich.setDirectory(rutacargadocs());
  dialogofich.setWindowTitle(tr("SELECCIÓN DE COPIA DE DOCUMENTO"));
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
           QString ruta=fileNames.at(0);
           if (eswindows())
             {
               ruta.replace("/",QDir::separator());
             }
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
           etiqueta->pasa_nombre_fich(ruta);
          }
     }
#endif
}


void asigna_fichdoc::visdoc()
{
   if (etiqueta->nombre_fich().isEmpty()) return;
   QString url=etiqueta->nombre_fich();
   QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

   /*if (!ejecuta(aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())),
                ui.fichdoclineEdit->text()))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+ui.fichdoclineEdit->text()+tr(" con ")+
                             aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())));*/

}

void asigna_fichdoc::borraasignaciondoc()
{
   etiqueta->borra_asignacion();

}

QString asigna_fichdoc::ruta_docs()
{
  return directorio_doc;
}

void asigna_fichdoc::botonaceptar()
{
    bool gestor_bd=basedatos::instancia()->gestor_doc_bd();

    if (etiqueta->nombre_fich().isEmpty()) return;

    if (gestor_bd) {
        // borramos documento antiguo si hay
        if (!ui.docasignadolineEdit->text().isEmpty()) {
           QString cadnum;
           QString fichdoc=ui.docasignadolineEdit->text();
           if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
              else cadnum=fichdoc.section('-',0,0);
           qlonglong num=cadnum.toLongLong();
           basedatos::instancia()->borra_doc_gestor(num);
        }

        // construimos nombre de fichero para guardarlo en campo
        qlonglong vnumdoc=basedatos::instancia()->select_prox_doc();
        QString cadnumdoc;
        cadnumdoc.setNum(vnumdoc);

        QString copiadocfich=cadnumdoc.trimmed();
        if (documento.length()>0)
            {
             copiadocfich+="-";
             documento.remove(' ');
             documento.remove('/');
             documento.remove('\\');
             documento.remove(':');
             documento.remove('*');
             documento.remove('?');
             documento.remove('|');
             documento.remove('<');
             documento.remove('>');
             copiadocfich+=documento;
            }
        copiadocfich.append('.');
        copiadocfich.append(extensionfich(etiqueta->nombre_fich()));

        QFile file(etiqueta->nombre_fich());
        file.open(QIODevice::ReadOnly);
        QByteArray inByteArray=file.readAll();
        basedatos::instancia()->inserta_doc_gestor(vnumdoc, copiadocfich, inByteArray);

        // incrementamos contador de documento
        vnumdoc++;
        cadnumdoc.setNum(vnumdoc);
        basedatos::instancia()->updateConfiguracionprox_documento(cadnumdoc);
        basedatos::instancia()->updatecopia_docdiario(asiento, copiadocfich,
                                                      ui.ejerciciolineEdit->text());
    }

    if (!gestor_bd)
       {
        // primero borramos documento antiguo si hay
        QProgressDialog progreso(tr("Cargando información del documento... "),0, 0, 0);
        progreso.setWindowTitle(tr("Cargando información ..."));
        progreso.setWindowModality(Qt::WindowModal);
        progreso.setMinimumDuration ( 0 );
        progreso.show();
        progreso.update();
        QApplication::processEvents();

        directorio_doc=etiqueta->nombre_fich();
        directorio_doc=directorio_doc.left(directorio_doc.lastIndexOf(QDir::separator()));


        QString fichdoc=ui.docasignadolineEdit->text();
        if (!fichdoc.isEmpty())
           {
            fichdoc=adapta(expanderutadocfich(fichdoc));
            QFile file(fichdoc);
            file.remove();
            QApplication::processEvents();
           }

        // ----------------------------------------------------------------
        qlonglong vnumdoc=basedatos::instancia()->select_prox_doc();
        QString cadnumdoc;
        cadnumdoc.setNum(vnumdoc);
        // construimos cadena con la ruta y nombre del archivo del documento
        QString caddestino=rutadocs();
        caddestino+=QDir::separator();
        // añadimos nombre de empresa para completar la ruta

        if (nombrebd().length()>0)
          {
           if (!es_sqlite()) caddestino.append(nombrebd());
              else
                   {
                     // el nombre de la base de datos guarda toda la ruta
                     QString nbd;
                     nbd=nombrebd().right(nombrebd().length()-nombrebd().lastIndexOf(QDir::separator())-1);
                     caddestino.append(nbd);
                   }
          }
        QString caddestinoprev;
        //if (eswindows()) caddestinoprev=QFile::encodeName(caddestino);
        //   else caddestinoprev=caddestino;
        caddestinoprev=caddestino;
        QDir d(caddestinoprev);
        QApplication::processEvents();
        if (!d.exists())
           {
            if (!d.mkdir(caddestinoprev))
               {
                QMessageBox::warning( this, tr("Asignar documento"),
                                      tr("ERROR: No se ha podido crear directorio"));
                return;
               }

            fijapermisos(caddestinoprev, "0770");
           }


        int numdir=(vnumdoc/100+1)*100;
        QString cadnumdir;
        cadnumdir.setNum(numdir);
        cadnumdir=cadnumdir.trimmed();
        caddestino+=QDir::separator();
        caddestino+=cadnumdir;
        //if (eswindows()) caddestinoprev=QFile::encodeName(caddestino);
        //   else caddestinoprev=caddestino;
        caddestinoprev=caddestino;
        d.setPath(caddestinoprev);
        if (!d.exists())
           {
            d.mkdir(caddestinoprev);
            fijapermisos(caddestinoprev, "0770");
           }
        QApplication::processEvents();

        caddestino+=QDir::separator();
        QString copiadocfich=cadnumdoc.trimmed();
        if (documento.length()>0)
            {
             copiadocfich+="-";
             documento.remove(' ');
             documento.remove('/');
             documento.remove('\\');
             documento.remove(':');
             documento.remove('*');
             documento.remove('?');
             documento.remove('|');
             documento.remove('<');
             documento.remove('>');
             copiadocfich+=documento;
            }
        copiadocfich.append('.');
        copiadocfich.append(extensionfich(etiqueta->nombre_fich()));
        caddestino.append(copiadocfich);
        // copiamos documento y actualizamos campo del diario
        // QMessageBox::warning( this, tr("pruebas"),"pruebas");
        bool copiado=false;
        if (eswindows())
           {
            QFile fichero(etiqueta->nombre_fich());
            copiado=fichero.copy(caddestino);
           }
          else
              {
               QFile fichero(etiqueta->nombre_fich());
               copiado=fichero.copy(caddestino);
               fijapermisos(caddestino, "0770");
              }
          if (!copiado)
            {
               QMessageBox::warning( this, tr("Asignar documento"),
                                     tr("ERROR: No se ha podido copiar archivo"));
               return;
            }
        // incrementamos contador de documento no ?
          QApplication::processEvents();
        vnumdoc++;
        cadnumdoc.setNum(vnumdoc);
        basedatos::instancia()->updateConfiguracionprox_documento(cadnumdoc);
        basedatos::instancia()->updatecopia_docdiario(asiento, copiadocfich,
                                                      ui.ejerciciolineEdit->text());
       }
  accept();
}
