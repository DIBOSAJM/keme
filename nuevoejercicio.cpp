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

#include "nuevoejercicio.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

nuevoejercicio::nuevoejercicio() : QDialog() {
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
move ( x, y ); */
// -------------------------------------------------------------

    QDate vfecha = basedatos::instancia()->selectMaxcierrecountcierreejercicios();

    ui.aperturadateEdit->setDate(vfecha);
    QDate vfecha2;
    vfecha2=vfecha.addDays(vfecha.daysInYear()-1);
    // vfecha.setYMD(QDate::currentDate().year(),12,31);
    ui.cierredateEdit->setDate(vfecha2);

   connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(guardarejercicio()));

}

void nuevoejercicio::guardarejercicio()
{
    if (ui.codigolineEdit->text().length()==0) 
        {
            QMessageBox::warning( this, tr("Nuevo ejercicio"),
            tr("ERROR: El código introducido no tiene contenido"));
           return;
        }


    if ( basedatos::instancia()->selectCodigoejercicioscodigo( ui.codigolineEdit->text() ) ) {
        QMessageBox::warning( this, tr("Nuevo ejercicio"),
        tr("ERROR: El código de ejercicio ya existe"));
        return;
    }

    if (ui.aperturadateEdit->date()>ui.cierredateEdit->date())
        {
            QMessageBox::warning( this, tr("Nuevo ejercicio"),
            tr("ERROR: Las fechas del ejercicio no son correctas"));
           return;
         }
    
    
    QSqlQuery query = basedatos::instancia()->selectAperturacierreejercicios();
    if ( query.isActive() )
    {
        while (query.next())
        {
            if (ui.aperturadateEdit->date()>=query.value(0).toDate() &&
	            ui.aperturadateEdit->date()<=query.value(1).toDate())
		    {
                QMessageBox::warning( this, tr("Nuevo ejercicio"),
                    tr("ERROR: La fecha de apertura está contenida en otro ejercicio"));
                return;
            } 
        }
    }

// Parece que se puede insertar  el registro
    basedatos::instancia()->insert6Ejercicios( ui.codigolineEdit->text() ,
                                               ui.aperturadateEdit->date() ,
                                               ui.cierredateEdit->date() ,
                                               false,
                                               false);
  accept();
}
