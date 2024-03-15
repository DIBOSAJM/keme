#include "papelera.h"
#include "ui_papelera.h"
#include "basedatos.h"
#include "funciones.h"
#include <QProgressDialog>
#include <QMessageBox>

papelera::papelera(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::papelera)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(3,300); // concepto
    cargar();
    connect(ui->borrar_todo_pushButton,SIGNAL(clicked(bool)),SLOT(borrar_todo()));
    connect(ui->refrescar_pushButton,SIGNAL(clicked(bool)),SLOT(refrescar()));
    connect(ui->recuperar_pushButton,SIGNAL(clicked(bool)),SLOT(recupera()));
}

papelera::~papelera()
{
    delete ui;
}

void papelera::refrescar()
{
    cargar();
}

void papelera::cargar()
{
    bool comadecimal=haycomadecimal();
    bool decimales=haydecimales();

    // deberíamos contar primero el número de registros
    int regs=basedatos::instancia()->selectCount_registros_papelera();
    //ui.mayortable->setRowCount(num);
    ui->tableWidget->setRowCount(regs);
    QSqlQuery q=basedatos::instancia()->select_ppdiario();
    int fila=0;
    if (q.isActive())
        while(q.next())
          {
            if (fila>=regs) break; // prevenimos violación de segmento
            QTableWidgetItem *newItemcta = new QTableWidgetItem(q.value(0).toString());
            ui->tableWidget->setItem(fila,0,newItemcta);
            QTableWidgetItem *newItem = new QTableWidgetItem(q.value(1).toDate().toString("dd/MM/yyyy"));
            ui->tableWidget->setItem(fila,1,newItem);
            QTableWidgetItem *newItemx = new QTableWidgetItem(q.value(2).toString());
            newItemx->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
            ui->tableWidget->setItem(fila,2,newItemx);  // asiento
            QTableWidgetItem *newItemp = new QTableWidgetItem(q.value(3).toString());
            ui->tableWidget->setItem(fila,3,newItemp);  // concepto
            double valdebe=q.value(4).toDouble();

            // ***************************************************************************************
            // ***************************************************************************************
            if (valdebe>0.0001 || valdebe<-0.0001)
              {
               QTableWidgetItem *newItem = new QTableWidgetItem(formateanumero(valdebe,comadecimal,decimales));
               newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
               ui->tableWidget->setItem(fila,4,newItem);  // debe
              }
              else
                   {
                    QTableWidgetItem *newItempp = new QTableWidgetItem("");
                    ui->tableWidget->setItem(fila,4,newItempp);
                   }
            double valhaber=q.value(5).toDouble();
            if (valhaber>0.0001 || valhaber<-0.0001)
              {
               QTableWidgetItem *newItemjj = new QTableWidgetItem(formateanumero(valhaber,comadecimal,decimales));
               newItemjj->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
               ui->tableWidget->setItem(fila,5,newItemjj);  // haber
              }
              else
                   {
                    QTableWidgetItem *newItemkk = new QTableWidgetItem("");
                    ui->tableWidget->setItem(fila,5,newItemkk);
                   }
            QTableWidgetItem *newItemj = new QTableWidgetItem(q.value(6).toString());
            ui->tableWidget->setItem(fila,6,newItemj); // documento
            QTableWidgetItem *newItem2 = new QTableWidgetItem(q.value(7).toString());
            ui->tableWidget->setItem(fila,7,newItem2); // diario
            QTableWidgetItem *newItem8 = new QTableWidgetItem(q.value(8).toString());
            ui->tableWidget->setItem(fila,8,newItem8); // diario
            fila++;

          }
}

void papelera::borrar_todo()
{
    if (QMessageBox::question(
             this,
             tr("PAPELERA"),
             tr("¿ Desea eliminar todo el contenido de la papelera ?"),
             tr("&Sí"), tr("&No"),
             QString(), 0, 1 ) > 0 )
                           return;

    QProgressDialog progreso("Eliminando contenido ...", 0, 0, 0);
    progreso.setWindowTitle(tr("PROCESANDO..."));
    progreso.setWindowModality(Qt::WindowModal);
    progreso.setMaximum(0);
    // progreso.setValue(30);
    progreso.setMinimumDuration ( 0 );
    // progreso.forceShow();
    progreso.show();
    QApplication::processEvents();

    basedatos::instancia()->borrar_papelera();
    cargar();
}

void papelera::recupera()
{
    QString asiento, ejercicio;

    if (ui->tableWidget->currentItem()==0) return;

    asiento=ui->tableWidget->item(ui->tableWidget->currentRow(),2)->text();
    ejercicio=ui->tableWidget->item(ui->tableWidget->currentRow(),8)->text();

    // preguntar
    if (QMessageBox::question(
             this,
             tr("PAPELERA"),
             tr("¿ Desea recuperar el asiento %1 ?").arg(asiento),
             tr("&Sí"), tr("&No"),
             QString(), 0, 1 ) > 0 )
                           return;
  // comprobar ejercicio no cerrado, período no bloqueado, cuenta no bloqueada
    if (ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio))
     {
       QMessageBox::warning( 0, QObject::tr("RECUPERAR ASIENTO"),
                             QObject::tr("ERROR: El apunte seleccionado corresponde a un ejercicio cerrado"));
       return;
     }

    if (basedatos::instancia()->ejercicio_bloqueado(ejercicio))
     {
       QMessageBox::warning( 0, QObject::tr("RECUPERAR ASIENTO"),
                             QObject::tr("ERROR: El apunte seleccionado corresponde a un ejercicio bloqueado"));
       return;
     }


    if (basedatos::instancia()->asiento_pp_en_periodo_bloqueado(asiento,ejercicio))
     {
       QMessageBox::warning( 0, QObject::tr("RECUPERAR ASIENTO"),
                             QObject::tr("ERROR: El asiento seleccionado corresponde a un periodo bloqueado"));
       return;
     }

    if (basedatos::instancia()->asiento_pp_con_cuentas_bloqueadas(asiento,ejercicio))
     {
       QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                             QObject::tr("ERROR: El asiento seleccionado posee cuentas bloqueadas"));
       return;
     }

  // procesamos recuperación y borrado asiento en papelera

    basedatos::instancia()->recupera_pp_asiento(asiento, ejercicio);

   cargar();

}
