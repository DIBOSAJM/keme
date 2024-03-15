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

#include "import_usuarios.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

import_usuarios::import_usuarios() : QDialog() {
    ui.setupUi(this);


  // cargamos ejercicios

    connect(ui.bdcomboBox,SIGNAL(activated(int)),this,SLOT(actucamposconexion(int)));
    connect(ui.AceptarpushButton,SIGNAL(clicked()),this,SLOT(procesar()));

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

void import_usuarios::pasaarchconex()
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
           QString linea,nombre;
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

void import_usuarios::actucamposconexion(int indice)
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


void import_usuarios::procesar()
{

   QString nombredb=ui.bdlineEdit->text();
   QString nombreusuario=ui.usuariolineEdit->text();
   QString clave=ui.clavelineEdit->text();
   QString host=ui.hostlineEdit->text();
   QString controlador=ui.controladorcomboBox->currentText();
   QString puerto=ui.puertolineEdit->text();

  if (!basedatos::instancia()->abre2db(nombredb,nombreusuario,clave,
               host, controlador, puerto, "segunda"))
     return;

    QSqlQuery query;
    query=basedatos::instancia()->select_usuarios ("segunda");
    if (query.isActive())
       while (query.next())
          {
           if (!basedatos::instancia()->existecodigousuario(query.value(0).toString()))
              {
                //  "select codigo, nombre, clave, nif, domicilio, poblacion,"
                //  "codigopostal, provincia, pais, tfno, email, observaciones,"
                //  "privilegios, imagen from usuarios";
               basedatos::instancia()->adcodigousuario(query.value(0).toString());
               basedatos::instancia()->grabarusuario(
                       query.value(0).toString(),
                       query.value(1).toString(),
                       query.value(2).toString(),
                       query.value(3).toString(),
                       query.value(4).toString(),
                       query.value(5).toString(),
                       query.value(6).toString(),
                       query.value(7).toString(),
                       query.value(8).toString(),
                       query.value(9).toString(),
                       query.value(10).toString(),
                       query.value(11).toString(),
                       query.value(12).toString());
               basedatos::instancia()->grabaimagenusuario(query.value(0).toString(),query.value(13).toString() );
              }
           }
        // falta cerrar base de datos "segunda"
        basedatos::instancia()->cierra_no_default("segunda");
        QMessageBox::information( this, tr("Importar usuarios"),
               tr("Se ha realizado la importación"));

}
