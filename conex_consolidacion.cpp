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

#include "conex_consolidacion.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

conex_consolidacion::conex_consolidacion() : QDialog() {
    ui.setupUi(this);

connect(ui.bdcomboBox,SIGNAL(activated(int)),this,SLOT(actucamposconexion(int)));
connect(ui.compropushButton,SIGNAL(clicked()),this,SLOT(chequea_conexion()));

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


void conex_consolidacion::pasaarchconex()
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

void conex_consolidacion::actucamposconexion(int indice)
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


void conex_consolidacion::pasainfo(QString controlador, QString basedatos, QString usuario,
                                   QString clave, QString host, QString puerto,
                                   QString codigo, QString descripcion, bool infodiario)
{
  int indice=ui.controladorcomboBox->findText(controlador);
  if (indice>=0) ui.controladorcomboBox->setCurrentIndex(indice);
  ui.bdlineEdit->setText(basedatos);
  ui.usuariolineEdit->setText(usuario);
  ui.clavelineEdit->setText(clave);
  ui.hostlineEdit->setText(host);
  ui.puertolineEdit->setText(puerto);
  ui.codigolineEdit->setText(codigo);
  ui.descriplineEdit->setText(descripcion);
  ui.diarioscheckBox->setChecked(infodiario);
}


void conex_consolidacion::chequea_conexion()
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

void conex_consolidacion::recupera_info(QString *controlador, QString *basedatos,
                                        QString *usuario,
                                        QString *clave, QString *host, QString *puerto,
                                        QString *codigo, QString *descripcion,
                                        bool *infodiario)
{
    *controlador=ui.controladorcomboBox->currentText();
    *basedatos=ui.bdlineEdit->text();
    *usuario=ui.usuariolineEdit->text();
    *host=ui.hostlineEdit->text();
    *puerto=ui.puertolineEdit->text();
    *clave=ui.clavelineEdit->text();
    *codigo=ui.descriplineEdit->text();
    *descripcion=ui.descriplineEdit->text();
    *infodiario=ui.diarioscheckBox->isChecked();
}
