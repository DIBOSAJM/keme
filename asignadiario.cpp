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

#include "asignadiario.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"

asignadiario::asignadiario() : QDialog() {
    ui.setupUi(this);

  QStringList diarios;

 diarios << diario_apertura();
 diarios << diario_cierre();
 diarios << diario_regularizacion();
 diarios << diario_no_asignado();

    QSqlQuery query = basedatos::instancia()->selectCodigoDiariosordercodigo();
    while (query.next())
       {
           diarios << query.value(0).toString();
       }

  ui.diariocomboBox->addItems(diarios);

  connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(procesar()));


}


void asignadiario::pasanumasiento( QString qasiento, QString qejercicio)
{
 asiento=qasiento;
 ejercicio=qejercicio;
 ui.numtextLabel->setText(asiento);
}


void asignadiario::procesar()
{
    QString qdiario;
    if (ui.diariocomboBox->currentText()!=diario_no_asignado()) 
        qdiario = ui.diariocomboBox->currentText().left(-1).replace("'","''");
    else
        qdiario = "";
    basedatos::instancia()->updateDiariodiarioasiento(qdiario,asiento,ejercicio);
    accept();
}
