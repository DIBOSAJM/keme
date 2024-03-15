/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

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

#include "numeracion.h"
#include "funciones.h"
#include "basedatos.h"

numeracion::numeracion() : QDialog()
 {
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
move ( x, y ); */
// -------------------------------------------------------------

   connect(ui.aceptarpushButton,SIGNAL( clicked()),this,
           SLOT(aceptar()));

 }



void numeracion::pasaejercicio( QString qejercicio )
{
  ejercicio=qejercicio;
  QString cadena;
  cadena=tr("Numeración, ejercicio: ");
  cadena+=ejercicio;
  setWindowTitle(cadena);
  // pasar prox_asiento y primero_apertura a plantilla
  QString prox_asiento;
  bool primero_apertura;
  basedatos::instancia()->datosnumeracion_ejercicio (qejercicio,
                                          &prox_asiento,
                                          &primero_apertura);
  ui.proxasientolineEdit->setText(prox_asiento);
  ui.primero_aperturacheckBox->setChecked(primero_apertura);
}

void numeracion::aceptar()
{
   basedatos::instancia()->pasanumeracion_ejercicio (ejercicio,
                                                    ui.proxasientolineEdit->text(),
                                                    ui.primero_aperturacheckBox->isChecked());
   accept();
}
