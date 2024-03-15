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

#include "periodos.h"
#include "funciones.h"
#include "basedatos.h"
#include <QDateEdit>
#include <QMessageBox>

periodos::periodos(QString qejercicio) : QDialog() {
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
move ( x, y );*/
// -------------------------------------------------------------

 ejercicio=qejercicio;
 QString cadena=tr("PERÍODOS EJERCICIO ");
 cadena+=ejercicio;
 ui.label->setText(cadena);
 QStringList columnas;
 columnas << tr("NOMBRE") << tr("INICIO") << tr("FIN");
 ui.tableWidget->setHorizontalHeaderLabels(columnas);


// cargamos datos de la tabla
 QSqlQuery query = basedatos::instancia()->selectCodigoiniciofinperiodosejercicioorrderinicio(ejercicio);
 int veces=0;
 if (query.isActive())
 while ( query.next() )
    {
     ui.tableWidget->insertRow(veces);
     QTableWidgetItem *newItem = new QTableWidgetItem(query.value(0).toString());
     ui.tableWidget->setItem(veces,0,newItem);
     QDateEdit  *fecha = new QDateEdit();
     fecha->setDisplayFormat("dd/MM/yyyy");
     fecha->setDate(query.value(1).toDate());
     ui.tableWidget->setCellWidget(veces,1,fecha);
     QDateEdit  *fecha2 = new QDateEdit();
     fecha2->setDisplayFormat("dd/MM/yyyy");
     fecha2->setDate(query.value(2).toDate());
     ui.tableWidget->setCellWidget(veces,2,fecha2);

    }

 connect(ui.mesespushButton,SIGNAL(clicked()),this,SLOT(asignameses()));
 connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(grabar()));
 connect(ui.borratodopushButton,SIGNAL(clicked()),this,SLOT(borrartodo()));
 connect(ui.insertapushButton,SIGNAL(clicked()),this,SLOT(insertafila()));
 connect(ui.borrafilapushButton,SIGNAL(clicked()),this,SLOT(borrafila()));

}

void periodos::asignameses()
{
  
  QDate fechaini=inicioejercicio(ejercicio);
  for (int veces=0;veces<12;veces++)
     {
      ui.tableWidget->insertRow(veces);
      QTableWidgetItem *newItem = new QTableWidgetItem(cadmes(veces+1));
      ui.tableWidget->setItem(veces,0,newItem);
      QDateEdit  *fecha = new QDateEdit();
      fecha->setDisplayFormat("dd/MM/yyyy");
      fecha->setDate(fechaini.addMonths(veces));
      ui.tableWidget->setCellWidget(veces,1,fecha);
      QDateEdit  *fecha2 = new QDateEdit();
      fecha2->setDisplayFormat("dd/MM/yyyy");
      QDate fechafinmes=fechaini.addMonths(veces+1);
      fechafinmes=fechafinmes.addDays(-1);
      fecha2->setDate(fechafinmes);
      ui.tableWidget->setCellWidget(veces,2,fecha2);
     }
}

void periodos::grabar()
{

 basedatos::instancia()->deletePeriodosejercicio(ejercicio);

 for (int veces=0;veces<ui.tableWidget->rowCount(); veces++)
     {
      QDateEdit *fechaini=(QDateEdit*)ui.tableWidget->cellWidget(veces,1);
      QDateEdit *fechafin=(QDateEdit*)ui.tableWidget->cellWidget(veces,2);
      QString codigo;
      if (ui.tableWidget->item(veces,0)==NULL) continue;
      if (ui.tableWidget->item(veces,0)->text().length()==0) continue;
      codigo=ui.tableWidget->item(veces,0)->text().left(-1).replace("'","''");

      basedatos::instancia()->insertPeriodos(ejercicio, codigo, fechaini->date(), fechafin->date());
     }
 accept();
}

void periodos::borrartodo()
{
 ui.tableWidget->clearContents();
 ui.tableWidget->setRowCount(0);
}

void periodos::insertafila()
{
  if (ui.tableWidget->rowCount()>=15)
     {
      QMessageBox::warning( this, tr("Períodos"),
           tr("No se puede exceder el número de 15 períodos"));
     }
  int fila=ui.tableWidget->currentRow();
  ui.tableWidget->insertRow(fila);
  QTableWidgetItem *newItem = new QTableWidgetItem("");
  ui.tableWidget->setItem(fila,0,newItem);
  QDateEdit  *fecha = new QDateEdit();
  fecha->setDisplayFormat("dd/MM/yyyy");
  fecha->setDate(QDate::currentDate ());
  ui.tableWidget->setCellWidget(fila,1,fecha);
  QDateEdit  *fecha2 = new QDateEdit();
  fecha2->setDisplayFormat("dd/MM/yyyy");
  fecha2->setDate(QDate::currentDate ());
  ui.tableWidget->setCellWidget(fila,2,fecha2);
}

void periodos::borrafila()
{
 int fila=ui.tableWidget->currentRow();
 if (fila>=0 && ui.tableWidget->rowCount()>fila)
    ui.tableWidget->removeRow(fila);
}
