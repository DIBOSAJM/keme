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

#include "intercambianum.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "pideejercicio.h"
#include <QMessageBox>

intercambianum::intercambianum() : QDialog() {
    ui.setupUi(this);

  connect(ui.procederpushButton,SIGNAL(clicked()),SLOT(procede()));

}


void intercambianum::procede()
{
    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("DIARIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString elejercicio=ej->contenido();
    delete ej;
    // deben de existir los dos asientos y además no ser iguales
    if (ui.asientolineEdit1->text().remove(' ')==ui.asientolineEdit2->text().remove(' '))
       {
              QMessageBox::warning( this, tr("Intercambiar asientos"),
				   tr("Error, los códigos de asientos deben de ser distintos"));
             return;
       }
    if (!existeasiento(ui.asientolineEdit1->text(),elejercicio) ||
        !existeasiento(ui.asientolineEdit2->text(),elejercicio))
       {
              QMessageBox::warning( this, tr("Intercambiar asientos"),
				   tr("Error, asiento no existente"));
             return;
       }

    QSqlDatabase::database().transaction();

    QString temp = QString::number( (qlonglong)(5000000 + ui.asientolineEdit1->text().toLongLong()) );

    basedatos::instancia()->updateDiarioasientoasiento( temp, ui.asientolineEdit1->text(),
                                                        elejercicio );

    basedatos::instancia()->updateDiarioasientoasiento( ui.asientolineEdit1->text(),
                                                        ui.asientolineEdit2->text(),
                                                        elejercicio);

    basedatos::instancia()->updateDiarioasientoasiento( ui.asientolineEdit2->text(), temp,
                                                        elejercicio);

   QSqlDatabase::database().commit();

   QMessageBox::information( this, tr("INTERCAMBIO"),
		              tr("El proceso ha concluido."));
    accept();

}
