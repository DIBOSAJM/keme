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

#include "conex_rutas.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>
#include <QFileDialog>

conex_rutas::conex_rutas() : QDialog() {
    ui.setupUi(this);

connect(ui.bdcomboBox,SIGNAL(activated(int)),this,SLOT(actucamposconexion(int)));
connect(ui.compropushButton,SIGNAL(clicked()),this,SLOT(chequea_conexion()));
connect(ui.rutatrabajopushButton,SIGNAL(clicked()),this,SLOT(boton_ruta_trabajo()));
connect(ui.rutadocpushButton,SIGNAL(clicked(bool)),this,SLOT(boton_ruta_docs()));

QStringList list = QSqlDatabase::drivers();
int veces=0;
while (veces<list.size())
   {
    if ( !list.at(veces).contains("PSQL") && !list.at(veces).contains("MYSQL") &&
       !list.at(veces).contains("SQLITE"))
          list.removeAt(veces);
       else veces++;
   }

 ui.controladorcomboBox->addItems(list);
 pasaarchconex();

}


void conex_rutas::pasaarchconex()
{
 QString archconex=fichconex();
   QString pasa;
   if (eswindows()) pasa=QFile::encodeName(archconex);
       else pasa=archconex;
 QFile fichero(pasa);
  if (!fichero.exists()) return;
    else
     {
      QStringList nombres;
      if ( fichero.open( QIODevice::ReadOnly ) )
          {
           QTextStream stream( &fichero );
           stream.setEncoding(QStringConverter::Utf8);
           QString linea;
           while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            listaconex << linea;
            // analizamos la linea de texto
            nombres << linea.section('\t',0,0);
           }
           ui.bdcomboBox->addItems(nombres);
           ui.bdcomboBox->setEnabled(true);
         }
     }


}

void conex_rutas::actucamposconexion(int indice)
// slot para conectar al cambio en combo box
{
 QString linea;
 linea=listaconex[indice];
 ui.bdlineEdit->setText(linea.section('\t',0,0));
 int ind=ui.controladorcomboBox->findText(linea.section('\t',1,1));
 ui.controladorcomboBox->setCurrentIndex(ind);
 ui.usuariolineEdit->setText(linea.section('\t',2,2));
 ui.clavelineEdit->setText(linea.section('\t',3,3));
 ui.hostlineEdit->setText(linea.section('\t',4,4));
 ui.puertolineEdit->setText(linea.section('\t',5,5));
}


void conex_rutas::pasainfo(QString basedatos, QString ruta_trabajo , QString ruta_docs)
{
  ui.bdlineEdit->setText(basedatos);
  ui.ruta_trabajo_lineEdit->setText(ruta_trabajo);
  ui.ruta_docs_lineEdit->setText(ruta_docs);
}


void conex_rutas::chequea_conexion()
{
    if (!basedatos::instancia()->abre2db(ui.bdlineEdit->text(),
                                         ui.usuariolineEdit->text(),
                                         ui.clavelineEdit->text(),
                                         ui.hostlineEdit->text(),
                                         ui.controladorcomboBox->currentText(),
                                         ui.puertolineEdit->text(), "segunda"))
        QMessageBox::warning( this, tr("CONEXIÓN BASE DATOS"),
                              tr("La conexión ha fallado"));
     else
        {
         basedatos::instancia()->cierra_no_default("segunda");
         QMessageBox::information( this, tr("CONEXIÓN BASE DATOS"),
                               tr("La conexión es correcta"));
        }

}

void conex_rutas::recupera_info( QString *basedatos,
                                 QString *ruta_trabajo,
                                 QString *ruta_docs)
{
    *basedatos=ui.bdlineEdit->text();
    *ruta_trabajo=ui.ruta_trabajo_lineEdit->text();
    *ruta_docs=ui.ruta_docs_lineEdit->text();
}


void conex_rutas::boton_ruta_trabajo()
{
    QString fichero=ui.ruta_trabajo_lineEdit->text();
    if (fichero.isEmpty()) fichero=getenv("HOME");

/*
#ifdef NOMACHINE
  if (!ui.rutadocslineEdit->text().startsWith(getenv("HOME"))) return;
  directorio *dir = new directorio();
  dir->solo_directorios();
  dir->pasa_directorio(adapta(ui.rutadocslineEdit->text()));
  if (dir->exec() == QDialog::Accepted)
    {
      fichero=dir->directorio_actual();
      if (!fichero.startsWith(getenv("HOME"))) fichero=dirtrabajodefecto();
    }
  delete(dir);
#else
*/

    QFileDialog dialog(this);
    dialog.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
    dialog.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
    dialog.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
    dialog.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
    dialog.setLabelText ( QFileDialog::Reject, tr("Cancelar") );
     dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory (adapta(fichero));
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
        else return;
    fichero=fileNames.at(0);

// #endif

    if (fichero.endsWith(QDir::separator()))
         fichero=fichero.left(fichero.length()-1);
    ui.ruta_trabajo_lineEdit->setText(fichero);

}

void conex_rutas::boton_ruta_docs()
{
    QString fichero=ui.ruta_docs_lineEdit->text();
    if (fichero.isEmpty()) fichero=getenv("HOME");

/*
#ifdef NOMACHINE
  if (!ui.rutadocslineEdit->text().startsWith(getenv("HOME"))) return;
  directorio *dir = new directorio();
  dir->solo_directorios();
  dir->pasa_directorio(adapta(ui.rutadocslineEdit->text()));
  if (dir->exec() == QDialog::Accepted)
    {
      fichero=dir->directorio_actual();
      if (!fichero.startsWith(getenv("HOME"))) fichero=dirtrabajodefecto();
    }
  delete(dir);
#else
*/

    QFileDialog dialog(this);
    dialog.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
    dialog.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
    dialog.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
    dialog.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
    dialog.setLabelText ( QFileDialog::Reject, tr("Cancelar") );
     dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory (adapta(fichero));
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
        else return;
    fichero=fileNames.at(0);

// #endif

    if (fichero.endsWith(QDir::separator()))
         fichero=fichero.left(fichero.length()-1);
    ui.ruta_docs_lineEdit->setText(fichero);

}
