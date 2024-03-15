/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

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

#include "bloqueaper.h"
#include "funciones.h"
#include "basedatos.h"
#include <QDateEdit>
#include <QCheckBox>

bloqueaper::bloqueaper(QString qejercicio) : QDialog() {
    ui.setupUi(this);

 ejercicio=qejercicio;
 QString cadena=tr("PERÍODOS EJERCICIO ");
 cadena+=ejercicio;
 ui.label->setText(cadena);
 QStringList columnas;
 columnas << tr("INICIO") << tr("FIN") << tr("HABILITADO") ;
 ui.tableWidget->setHorizontalHeaderLabels(columnas);


// cargamos datos de la tabla
 QSqlQuery query = basedatos::instancia()->selectCodigoiniciofinperiodosejercicioorrderinicio(ejercicio);
 int veces=0;
 if (query.isActive())
 while ( query.next() )
    {
     hfilas.prepend(query.value(0).toString());
     ui.tableWidget->insertRow(veces);
     QDateEdit  *fecha = new QDateEdit();
     fecha->setDisplayFormat("dd/MM/yyyy");
     fecha->setDate(query.value(1).toDate());
     fecha->setReadOnly(true);
     ui.tableWidget->setCellWidget(veces,0,fecha);
     QDateEdit  *fecha2 = new QDateEdit();
     fecha2->setDisplayFormat("dd/MM/yyyy");
     fecha2->setDate(query.value(2).toDate());
     fecha2->setReadOnly(true);
     ui.tableWidget->setCellWidget(veces,1,fecha2);
     QCheckBox *checkbox2 = new QCheckBox(tr("Inhabilitar"), this);
     ui.tableWidget->setCellWidget ( veces, 2, checkbox2 );
     if (query.value(3).toBool()) checkbox2->setChecked(true);

    }
 ui.tableWidget->setVerticalHeaderLabels(hfilas);
 connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(aceptar()));

}


void bloqueaper::aceptar()
{

 for (int veces=0;veces<ui.tableWidget->rowCount(); veces++)
     {
      QCheckBox *checkbox = (QCheckBox*)ui.tableWidget->cellWidget(veces,2);
      bool inhabilitado=checkbox->isChecked();
      basedatos::instancia()->actuperiodos(ejercicio, hfilas[veces],inhabilitado);
     }
 accept();
}
