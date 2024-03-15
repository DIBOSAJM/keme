/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#include "preferencias.h"
#include "funciones.h"
#include <QFileInfo>
#include <QCloseEvent>
#include "directorio.h"
#include <QMessageBox>
#include <qfiledialog.h>
#include "asigna_dir_trab.h"

preferencias::preferencias() : QDialog() {
    ui.setupUi(this);

// esta porción de código es para asegurarnos de que la ventana
// sale por el centro del escritorio
/* QDesktopWidget *desktop = QApplication::desktop();

int screenWidth, width; 
int screenHeight, height;
int x, y;
QSize windowSize;

screenWidth = desktop->width(); // ancho de la pantalla
screenHeight = desktop->height(); // alto de la pantalla

windowSize = size(); // tamaño de nuestra ventana
width = windowSize.width(); 
height = windowSize.height();

x = (screenWidth - width) / 2;
y = (screenHeight - height) / 2;
y -= 50;

// mueve la ventana a las coordenadas deseadas
move ( x, y );*/
// -------------------------------------------------------------

    connect(ui.filtrargroupBox,SIGNAL(toggled(bool)),this,SLOT(filtrarporcambiado()));
    connect(ui.personalizadogroupBox,SIGNAL(toggled(bool)),this,SLOT(filtrarinicambiado()));
    connect(ui.okButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui.dirtrabpushButton,SIGNAL(clicked()),this,SLOT(dirtrab_pulsado()));

    connect(ui.rutadocpushButton,SIGNAL(clicked()),this,SLOT(rutadoc_pulsado()));
    connect(ui.cargadocpushButton,SIGNAL(clicked()),this,SLOT(cargadoc_pulsado()));

    connect(ui.copiapdfpushButton,SIGNAL(clicked()),this,SLOT(copiapdf_pulsado()));

    connect(ui.rutasegpushButton,SIGNAL(clicked()),this,SLOT(ruta_seg_pulsado()));
    connect(ui.rutalibrospushButton,SIGNAL(clicked()),this,SLOT(ruta_libros_pulsado()));

    connect(ui.rutaspushButton,SIGNAL(clicked(bool)),this,SLOT(rutas_conexion()));


}


void preferencias::filtrarporcambiado()
{
    if (ui.filtrargroupBox->isChecked())
	ui.personalizadogroupBox->setChecked(false);
}

void preferencias::filtrarinicambiado()
{
    if (ui.personalizadogroupBox->isChecked())
	ui.filtrargroupBox->setChecked(false);
}

void preferencias::accept()
{
  QFileInfo fdirectorio( ui.directoriolineEdit->text() );
  QDir ddirectorio( ui.directoriolineEdit->text() );

  // si no existe el directorio lo intenta crear
  if (! ddirectorio.exists() )
  {
    // si es capaz de crear el directorio
    if ( ddirectorio.mkdir(ui.directoriolineEdit->text()) )
    {
      // creado el directorio
      QMessageBox::information(this, tr("Preferencias") , tr("No existía y se ha creado correctamente el directorio %1").arg( ui.directoriolineEdit->text() ) );
      QDialog::accept();
    }
    else
    {
      // no creado el directorio
      QMessageBox::warning(this, tr("Preferencias") , tr ("No se ha podido crear el directorio %1").arg( ddirectorio.absolutePath() ) );
    }
  }
  // si existe el directorio pero no es escribible
  else {
    if ( fdirectorio.isWritable() ) {
      // el directorio es correcto
      QDialog::accept();
    }
    else {
      // el directorio no se puede escribir
      QMessageBox::warning(this, tr("Preferencias") , tr ("No hay permisos de escritura en el directorio %1").arg( ui.directoriolineEdit->text() ) );
    }
  }

// --------------------------------------------------------------------------------------------------------------

  if (ui.rutadocslineEdit->text().isEmpty()) return;

  QFileInfo fdirectorio2( ui.rutadocslineEdit->text() );
  QDir ddirectorio2( ui.rutadocslineEdit->text() );

  // si no existe el directorio lo intenta crear
  if (! ddirectorio2.exists() )
  {
    // si es capaz de crear el directorio
    if ( ddirectorio2.mkdir(ui.rutadocslineEdit->text()) )
    {
      // creado el directorio
      QMessageBox::information(this, tr("Preferencias") ,
                               tr("No existía y se ha creado correctamente el directorio %1").arg(
                                       ui.rutadocslineEdit->text() ) );
      QDialog::accept();
    }
    else
    {
      // no creado el directorio
      QMessageBox::warning(this, tr("Preferencias") , tr ("No se ha podido crear el directorio %1").arg(
              ddirectorio2.absolutePath() ) );
    }
  }
  // si existe el directorio pero no es escribible
  else {
    if ( fdirectorio2.isWritable() ) {
      // el directorio es correcto
      QDialog::accept();
    }
    else {
      // el directorio no se puede escribir
      QMessageBox::warning(this, tr("Preferencias") ,
                           tr ("No hay permisos de escritura en el directorio %1").arg(
                                   ui.rutadocslineEdit->text() ) );
    }
  }


}

void preferencias::pasaeditorlatex(QString qeditor)
{
    ui.editortexlineEdit->setText(qeditor);
}

void preferencias::pasa_ruta_seg(QString qruta)
{
    ui.rutaseglineEdit->setText(qruta);
}

QString preferencias::ruta_seg()
{
  if (ui.rutaseglineEdit->text().isEmpty()) return QString();
#ifdef NOMACHINE
  return ui.rutaseglineEdit->text().startsWith(getenv("HOME")) ?
          ui.rutaseglineEdit->text() : getenv("HOME");
#else
  return ui.rutaseglineEdit->text();
#endif
}

void preferencias::pasadirtrabajo(QString directorio)
{
    ui.directoriolineEdit->setText(directorio);
}

void preferencias::pasavisordvi(QString visor)
{
    ui.visorlineEdit->setText(visor);
}

void preferencias::pasaaplicimprimir(QString aplic)
{
    ui.imprimirlineEdit->setText(aplic);
}

void preferencias::pasanavegador(QString navegador)
{
    //ui.navegadorlineEdit->setText(navegador);
}

void preferencias::pasafiltropor(int filtro)
{
    if (filtro<1 || filtro>3) 
    {
     ui.filtrargroupBox->setChecked(false);
     return;
    }
    ui.filtrargroupBox->setChecked(true);
    if (filtro==EJERCICIOACTUAL) ui.ejercicioradioButton->setChecked(true);
    if (filtro==ULTIMOS30DIAS) ui.u30diasradioButton->setChecked(true);
    if (filtro==ULTIMOS30ASIENTOS) ui.u30asientosradioButton->setChecked(true);
}

void preferencias::pasaordenarpor(int orden)
{
    if (orden==FECHA) ui.fecharadioButton->setChecked(true);
    else ui.numradioButton->setChecked(true);
}

void preferencias::pasaestilonumerico(int estilo)
{
    if (estilo==COMADECIMAL) ui.comaradioButton->setChecked(true);
    else ui.puntoradioButton->setChecked(true);
}

void preferencias::pasasindecimales(bool sindecimales)
{
 ui.sindecimalescheckBox->setChecked(sindecimales);
}


void preferencias::pasafiltrodiariopersonalizado(QString filtro)
{
    if (filtro.length()>0)
    {
	ui.personalizadogroupBox->setChecked(true);
      ui.personalizadolineEdit->setText(filtro);
    }
    else
        ui.personalizadogroupBox->setChecked(false);
}

void preferencias::pasafiltrartex(bool filtrartext)
{
 ui.filtratexcheckBox->setChecked(filtrartext);
}

void preferencias::pasahaydockautomaticos(bool activo)
{
 if (!activo) ui.automaticoscheckBox->setChecked(false);
}

void preferencias::pasahaydockestados(bool activo)
{
 if (!activo) ui.estadoscheckBox->setChecked(false);
}

bool preferencias::sihaydockautomaticos()
{
  return ui.automaticoscheckBox->isChecked();
}

bool preferencias::sihaydockestados()
{
  return ui.estadoscheckBox->isChecked();
}

void preferencias::pasaidioma(int qidioma)
{
 if (qidioma==0) ui.espradioButton->setChecked(true);
 if (qidioma==1) ui.catradioButton->setChecked(true);
 if (qidioma==2) ui.galradioButton->setChecked(true);
 if (qidioma==3) ui.ingradioButton->setChecked(true);
 if (qidioma==4) ui.portradioButton->setChecked(true);
 if (qidioma==5) ui.euskeraradioButton->setChecked(true);
}

void preferencias::pasa_repetidos(bool rep)
{
    ui.repetidos_checkBox->setChecked(rep);
}

QString preferencias::dirtrabajo()
{
#ifdef NOMACHINE

    return ui.directoriolineEdit->text().startsWith(getenv("HOME")) ?
            ui.directoriolineEdit->text() : getenv("HOME") ;
#else
    return ui.directoriolineEdit->text();
#endif
}

QString preferencias::rutadocs()
{
#ifdef NOMACHINE
  return ui.rutadocslineEdit->text().startsWith(getenv("HOME")) ?
          ui.rutadocslineEdit->text() : getenv("HOME");
#else
  return ui.rutadocslineEdit->text();
#endif
}

QString preferencias::rutacopiapdf()
{
 if (ui.copiapdflineEdit->text().isEmpty()) return QString();
#ifdef NOMACHINE
  return ui.copiapdflineEdit->text().startsWith(getenv("HOME")) ?
          ui.copiapdflineEdit->text() : getenv("HOME");
#else
  return ui.copiapdflineEdit->text();
#endif
}

QString preferencias::visordvi()
{
    return ui.visorlineEdit->text();
}

QString preferencias::aplicimprimir()
{
    return ui.imprimirlineEdit->text();
}

QString preferencias::navegador()
{
    // return ui.navegadorlineEdit->text();
    return QString();
}

QString preferencias::ruta_curl() {
  return ui.curl_lineEdit->text();
}

QString preferencias::editorlatex()
{
    return ui.editortexlineEdit->text();
}

int preferencias::filtropor()
{
    if (!ui.filtrargroupBox->isChecked()) return NADA;
    if (ui.ejercicioradioButton->isChecked()) return EJERCICIOACTUAL;
    if (ui.u30diasradioButton->isChecked()) return ULTIMOS30DIAS;
    if (ui.u30asientosradioButton->isChecked()) return ULTIMOS30ASIENTOS;
    return NADA;
}

int preferencias::ordenarpor()
{
    if (ui.fecharadioButton->isChecked()) return FECHA;
    return ASIENTO;
}

int preferencias::estilonumerico()
{
    if (ui.comaradioButton->isChecked()) return COMADECIMAL;
    return PUNTODECIMAL;
}

bool preferencias::sindecimales()
{
 if (ui.sindecimalescheckBox->isChecked()) return true; 
 return false;
}

QString preferencias::filtrodiariopersonalizado()
{
    if (ui.personalizadogroupBox->isChecked())
      return ui.personalizadolineEdit->text();
    return "";
}

bool preferencias::esfiltrartex()
{
  return ui.filtratexcheckBox->isChecked();
}

bool preferencias::hay_control_docs_repetidos()
{
  return ui.repetidos_checkBox->isChecked();
}

int preferencias::qidioma()
{
 if (ui.catradioButton->isChecked()) return 1;
 if (ui.galradioButton->isChecked()) return 2;
 if (ui.ingradioButton->isChecked()) return 3;
 if (ui.portradioButton->isChecked()) return 4;
 if (ui.euskeraradioButton->isChecked()) return 5;
 return 0;
}


QString preferencias::ext1()
{
 return ui.ext1lineEdit->text();
}

QString preferencias::vis1()
{
 return ui.vis1lineEdit->text();
}

QString preferencias::ext2()
{
   return ui.ext2lineEdit->text();
}

QString preferencias::vis2()
{
  return ui.vis2lineEdit->text();
}

QString preferencias::ext3()
{
 return ui.ext3lineEdit->text();
}

QString preferencias::vis3()
{
 return ui.vis3lineEdit->text();
}

QString preferencias::ext4()
{
 return ui.ext4lineEdit->text();
}

QString preferencias::vis4()
{
    return ui.vis4lineEdit->text();
}

QString preferencias::url_actu()
{
    return ui.url_actu_lineEdit->text();
}

QString preferencias::msj_url_actu()
{
   return ui.msj_url_lineEdit->text();
}

void preferencias::pasarutadocs(QString cadena)
{
    ui.rutadocslineEdit->setText(cadena);
}

void preferencias::pasarutacopiapdf(QString cadena)
{
    ui.copiapdflineEdit->setText(cadena);
}

void preferencias::pasa_ruta_curl(QString qruta) {
   ui.curl_lineEdit->setText(qruta);
}

void preferencias::pasaext1(QString cadena)
{
    ui.ext1lineEdit->setText(cadena);
}

void preferencias::pasavis1(QString cadena)
{
    ui.vis1lineEdit->setText(cadena);
}

void preferencias::pasaext2(QString cadena)
{
    ui.ext2lineEdit->setText(cadena);
}

void preferencias::pasavis2(QString cadena)
{
    ui.vis2lineEdit->setText(cadena);
}

void preferencias::pasaext3(QString cadena)
{
    ui.ext3lineEdit->setText(cadena);
}

void preferencias::pasavis3(QString cadena)
{
    ui.vis3lineEdit->setText(cadena);
}

void preferencias::pasaext4(QString cadena)
{
    ui.ext4lineEdit->setText(cadena);
}

void preferencias::pasavis4(QString cadena)
{
    ui.vis4lineEdit->setText(cadena);
}

QString preferencias::rutacargadocs()
{
#ifdef NOMACHINE
  return ui.rutacargadocslineEdit->text().startsWith(getenv("HOME")) ?
          ui.rutacargadocslineEdit->text() : getenv("HOME");
#else
  return ui.rutacargadocslineEdit->text();
#endif
}

void preferencias::pasarutacargadocs(QString cadena)
{
  ui.rutacargadocslineEdit->setText(cadena);
}


void preferencias::pasarutalibros(QString cadena)
{
  ui.rutalibroslineEdit->setText(cadena);
}

QString preferencias::rutalibros()
{
 if (ui.rutalibroslineEdit->text().isEmpty()) return QString();
#ifdef NOMACHINE
  return ui.rutalibroslineEdit->text().startsWith(getenv("HOME")) ?
          ui.rutalibroslineEdit->text(): getenv("HOME");
#else
  return ui.rutalibroslineEdit->text();
#endif
}


void preferencias::pasaguardausuario(bool guarda)
{
 ui.usuariocheckBox->setChecked(guarda);
}

void preferencias::pasaguardaconexion(bool guarda)
{
 ui.empresacheckBox->setChecked(guarda);
}

bool preferencias::seguardausuario()
{
  return ui.usuariocheckBox->isChecked();
}

bool preferencias::seguardaempresa()
{
  return ui.empresacheckBox->isChecked();
}


void preferencias::dirtrab_pulsado()
{
    QString fichero=dirtrabajo();

#ifdef NOMACHINE
  if (!ui.directoriolineEdit->text().startsWith(getenv("HOME"))) return;
  directorio *dir = new directorio();
  dir->solo_directorios();
  dir->pasa_directorio(adapta(ui.directoriolineEdit->text()));
  if (dir->exec() == QDialog::Accepted)
    {
      fichero=dir->directorio_actual();
      if (!fichero.startsWith(getenv("HOME"))) fichero=dirtrabajodefecto();
    }
  delete(dir);
#else

    QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory (adapta(ui.directoriolineEdit->text()));

     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
      else return;
    fichero=fileNames.at(0);
#endif

    if (fichero.endsWith(QDir::separator()))
        fichero=fichero.left(fichero.length()-1);
    ui.directoriolineEdit->setText(fichero);
}


void preferencias::rutadoc_pulsado()
{
    QString fichero=ui.rutadocslineEdit->text();


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


    QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory (adapta(ui.rutadocslineEdit->text()));
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
        else return;
    fichero=fileNames.at(0);

#endif

    if (fichero.endsWith(QDir::separator()))
         fichero=fichero.left(fichero.length()-1);
    ui.rutadocslineEdit->setText(fichero);
}


void preferencias::cargadoc_pulsado()
{
    QString fichero=ui.rutacargadocslineEdit->text();

#ifdef NOMACHINE
  if (!ui.rutacargadocslineEdit->text().startsWith(getenv("HOME"))) return;
  directorio *dir = new directorio();
  dir->solo_directorios();
  dir->pasa_directorio(adapta(ui.rutacargadocslineEdit->text()));
  if (dir->exec() == QDialog::Accepted)
    {
      fichero=dir->directorio_actual();
      if (!fichero.startsWith(getenv("HOME"))) fichero=dirtrabajodefecto();
    }
  delete(dir);
#else

    QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory (adapta(ui.rutacargadocslineEdit->text()));
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
       else return;
    fichero=fileNames.at(0);
#endif

    if (fichero.endsWith(QDir::separator()))
           fichero=fichero.left(fichero.length()-1);
    ui.rutacargadocslineEdit->setText(fichero);
}


void preferencias::copiapdf_pulsado()
{
    QString fichero=ui.copiapdflineEdit->text();

#ifdef NOMACHINE
  if (!ui.copiapdflineEdit->text().startsWith(getenv("HOME"))) return;
  directorio *dir = new directorio();
  dir->solo_directorios();
  dir->pasa_directorio(adapta(ui.copiapdflineEdit->text()));
  if (dir->exec() == QDialog::Accepted)
    {
      fichero=dir->directorio_actual();
      if (!fichero.startsWith(getenv("HOME"))) fichero=dirtrabajodefecto();
    }
  delete(dir);
#else
    QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
    QString paso=dirtrabajo();
    if (!ui.copiapdflineEdit->text().isEmpty()) paso=ui.copiapdflineEdit->text();
    dialog.setDirectory (adapta(paso));
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
       else return;
    fichero=fileNames.at(0);
#endif


    if (fichero.endsWith(QDir::separator()))
       fichero=fichero.left(fichero.length()-1);
    ui.copiapdflineEdit->setText(fichero);
}


void preferencias::ruta_seg_pulsado()
{
    QString fichero=ui.rutaseglineEdit->text();

#ifdef NOMACHINE
  if (!ui.rutaseglineEdit->text().startsWith(getenv("HOME"))) return;
  directorio *dir = new directorio();
  dir->solo_directorios();
  dir->pasa_directorio(adapta(ui.rutaseglineEdit->text()));
  if (dir->exec() == QDialog::Accepted)
    {
      fichero=dir->directorio_actual();
      if (!fichero.startsWith(getenv("HOME"))) fichero=dirtrabajodefecto();
    }
  delete(dir);
#else

    QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory (adapta(ui.rutaseglineEdit->text()));
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
       else return;
    fichero=fileNames.at(0);
#endif

    if (fichero.endsWith(QDir::separator()))
        fichero=fichero.left(fichero.length()-1);
    ui.rutaseglineEdit->setText(fichero);
}


void preferencias::ruta_libros_pulsado()
{
    QString fichero=ui.rutalibroslineEdit->text();

#ifdef NOMACHINE
  if (!ui.rutalibroslineEdit->text().startsWith(getenv("HOME"))) return;
  directorio *dir = new directorio();
  dir->solo_directorios();
  dir->pasa_directorio(adapta(ui.rutalibroslineEdit->text()));
  if (dir->exec() == QDialog::Accepted)
    {
      fichero=dir->directorio_actual();
      if (!fichero.startsWith(getenv("HOME"))) fichero=dirtrabajodefecto();
    }
  delete(dir);
#else

    QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory (adapta(ui.rutalibroslineEdit->text()));
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
       else return;
    fichero=fileNames.at(0);
#endif

    if (fichero.endsWith(QDir::separator()))
           fichero=fichero.left(fichero.length()-1);
    ui.rutalibroslineEdit->setText(fichero);
}

void preferencias::rutas_conexion()
{
  // QStringList conex_rutas, rutas_trabajo, rutas_docs;
  asigna_dir_trab *a = new asigna_dir_trab();
  a->pasa_listas_info(conex_rutas, rutas_trabajo, rutas_docs);

  int resultado=a->exec();
  if (resultado == QDialog::Accepted)
     {
      QStringList rconex_rutas, rrutas_trabajo, rrutas_docs;
      a->recupera_listas_info(&rconex_rutas, &rrutas_trabajo, &rrutas_docs);
      conex_rutas=rconex_rutas;
      rutas_trabajo=rrutas_trabajo;
      rutas_docs=rrutas_docs;
     }
  delete(a);
}


void preferencias::pasa_listas_info(QStringList qconex_rutas, QStringList qrutas_trabajo, QStringList qrutas_docs)
{
    conex_rutas=qconex_rutas;
    rutas_trabajo=qrutas_trabajo;
    rutas_docs=qrutas_docs;
}

void preferencias::recupera_listas_info(QStringList *qconex_rutas, QStringList *qrutas_trabajo, QStringList *qrutas_docs)
{
    *qconex_rutas=conex_rutas;
    *qrutas_trabajo=rutas_trabajo;
    *qrutas_docs=rutas_docs;

}

void preferencias::pasa_url_actu(QString cadena)
{
    ui.url_actu_lineEdit->setText(cadena);
}

void preferencias::pasa_msj_url_actu(QString cadena)
{
   ui.msj_url_lineEdit->setText(cadena);
}

void preferencias::pasa_visualizador_extendido(bool visexten) {
    ui.visualizadorcheckBox->setChecked(visexten);
}

bool preferencias::visualizador_extendido() {
    return ui.visualizadorcheckBox->isChecked();
}


void preferencias::pasa_import_remota(bool importrem) {
    ui.imp_remota_checkBox->setChecked(importrem);
}

bool preferencias::import_remota() {
    return ui.imp_remota_checkBox->isChecked();
}
