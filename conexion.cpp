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

#include "conexion.h"
#include <QtSql/QSqlDatabase>
#include "funciones.h"

conexion::conexion() : QDialog() {
    ui.setupUi(this);

// esta porción de código es para asegurarnos de que la ventana
// sale por el centro del escritorio
/*QDesktopWidget *desktop = QApplication::desktop();

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
move ( x, y );
*/
// -------------------------------------------------------------

    connect(ui.bdcomboBox,SIGNAL(activated(int)),this,SLOT(actucamposconexion(int)));

  QStringList list = QSqlDatabase::drivers();
   int veces=0;
   while (veces<list.size())
      {
       if ( !list.at(veces).contains("PSQL") && !list.at(veces).contains("MYSQL"))
        // && !list.at(veces).contains("SQLITE"))
             list.removeAt(veces);
          else veces++;
      }
    list.insert(0,"QSQLITE");

#ifdef SOLOPOSTGRES
    veces=0;
    while (veces<list.size())
       {
        // AÑADIMOS SQLITE para realizar pruebas
        if ( !list.at(veces).contains("PSQL") || list.at(veces).contains("PSQL7"))
              list.removeAt(veces);
           else veces++;
       }
#endif

    ui.controladorcomboBox->addItems(list);

}


void conexion::pasaarchconex(QString archivo)
{
 archconex=archivo;
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
           if (ui.bdcomboBox->count()>0) actucamposconexion(0);
         }
     }


}


QString conexion::controlador()
{
 return ui.controladorcomboBox->currentText();
}

QString conexion::basedatos()
{
 return ui.bdlineEdit->text();
}

QString conexion::usuario()
{
 return ui.usuariolineEdit->text();
}

QString conexion::clave()
{
 return ui.clavelineEdit->text();
}

QString conexion::host()
{
 return ui.hostlineEdit->text();
}

QString conexion::puerto()
{
 return ui.puertolineEdit->text();
}

void conexion::actulistaconex(void)
{
 // primero comprobamos si existe el nombre de la base de datos
 // si existe lo borramos
 for (int veces=0;veces<listaconex.size();veces++)
     {
      if (listaconex[veces].section('\t',0,0)==ui.bdlineEdit->text())
          {
           listaconex.removeAt(veces);
           break;
          }
     }
 QString linea;
 linea=ui.bdlineEdit->text();
 linea+="\t";
 linea+=ui.controladorcomboBox->currentText();
 linea+="\t";
 linea+=ui.usuariolineEdit->text();
 linea+="\t";
 linea+=ui.clavelineEdit->text();
 linea+="\t";
 linea+=ui.hostlineEdit->text();
 if (ui.puertolineEdit->text().length()>0)
    {
     linea+="\t";
     linea+=ui.puertolineEdit->text();
    }

 listaconex.insert(0,linea);
}

void conexion::actufichconex(void)
{
  // graba el contenido de la lista
   QString pasa;
   if (eswindows()) pasa=QFile::encodeName(archconex);
       else pasa=archconex;
  QFile fichero(pasa);
  if (! fichero.open( QIODevice::WriteOnly ) ) return;
  QTextStream stream( &fichero );
  stream.setEncoding(QStringConverter::Utf8);

  for (int veces=0;veces<listaconex.size();veces++)
     {
      stream << listaconex[veces] << "\n";
     }
  fichero.close();

}

void conexion::actucamposconexion(int indice)
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

