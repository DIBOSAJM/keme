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

#include <QApplication>

#include "main_factur.h"
#include <qtextcodec.h>


int main(int argc, char *argv[])
{
        // Q_INIT_RESOURCE(application);

  QApplication app(argc, argv);
  //app.addLibraryPath ( "/usr/lib/qt4/plugins"); // añadido para trabajar con QT-Creator 2.0
  //QTextCodec::setCodecForTr( QTextCodec::codecForName("utf8") );

  main_factur mainWin;
  if (mainWin.oknosalir())
       {
        mainWin.show();
       } else { return 0;  }
  return app.exec();
 }
