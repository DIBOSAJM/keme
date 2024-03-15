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

#include "asigna_dir_trab.h"
#include "conex_rutas.h"
#include "funciones.h"
#include "basedatos.h"
#include <QProgressDialog>
#include <QMessageBox>

asigna_dir_trab::asigna_dir_trab() : QDialog() {
    ui.setupUi(this);

ui.conextableWidget->setColumnWidth(0,150);
ui.conextableWidget->setColumnWidth(1,230);
ui.conextableWidget->setColumnWidth(2,230);
/*
for (int fila=0; fila<ui.conextableWidget->rowCount(); fila++)
    for (int col=0; col<ui.conextableWidget->columnCount(); col++)
        {
         QTableWidgetItem *newItem = new QTableWidgetItem("");
         ui.conextableWidget->setItem(fila,col,newItem);

        }
*/
// cargamos conexiones en array [9,10]


connect(ui.conexionpushButton,SIGNAL(clicked()),SLOT(conexion()));
connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(boton_aceptar()));
connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borra_conex()));
connect(ui.nuevapushButton,SIGNAL(clicked(bool)),SLOT(nueva_conex()));

}

void asigna_dir_trab::borra_conex()
{
    int fila=ui.conextableWidget->currentRow();
    if (fila<0 || fila >= ui.conextableWidget->rowCount()) return;
    QString cadcodigo; cadcodigo.setNum(fila+1);
    if (QMessageBox::question(this,
                    tr("Consolidación"),
                    tr("¿ Desea borrar rutas asociadas a la conexión '%1' ?")
                      .arg( cadcodigo ),
                    tr("&Sí"), tr("&No"),
                              QString(), 0, 1 ) )
                  return ;
    ui.conextableWidget->item(fila,0)->setText(QString());
    ui.conextableWidget->item(fila,1)->setText(QString());
    ui.conextableWidget->item(fila,2)->setText(QString());

}


void asigna_dir_trab::conexion()
{
    int fila=ui.conextableWidget->currentRow();
    if (fila<0 || fila >= ui.conextableWidget->rowCount()) return;

    conex_rutas *c = new conex_rutas();
    // pasamos info conexión
    // nombrebase, descrip, fecha_actu, hora_actu, usuario, clave, host,
    // puerto, controlador, ultimo_apunte, import_tipos_diario
    /* pasainfo(QString controlador, QString basedatos, QString usuario,
             QString clave, QString host, QString puerto,
             QString codigo, QString descripcion, bool infodiario) */
    QString codigo; codigo.setNum(fila+1);
    c->pasainfo(ui.conextableWidget->item(fila,0)->text(),ui.conextableWidget->item(fila,1)->text(),
                ui.conextableWidget->item(fila,2)->text());

    int resultado=c->exec();
    if (resultado==QDialog::Rejected) return;

    QString basedatos,ruta_trabajo, ruta_docs;
    c->recupera_info(&basedatos,&ruta_trabajo, &ruta_docs);
    delete(c);
    ui.conextableWidget->item(fila,0)->setText(basedatos);
    ui.conextableWidget->item(fila,1)->setText(ruta_trabajo);
    ui.conextableWidget->item(fila,2)->setText(ruta_docs);
}



void asigna_dir_trab::boton_aceptar()
{

  accept();
}

void asigna_dir_trab::nueva_conex()
{
    int fila=ui.conextableWidget->rowCount();
    ui.conextableWidget->insertRow(fila);
    for (int col=0; col<ui.conextableWidget->columnCount(); col++)
        {
         QTableWidgetItem *newItem = new QTableWidgetItem("");
         ui.conextableWidget->setItem(fila,col,newItem);
        }
    ui.conextableWidget->setCurrentCell(fila,0);
    conexion();
}


void asigna_dir_trab::pasa_listas_info(QStringList conex_rutas, QStringList rutas_trabajo, QStringList rutas_docs)
{
    for (int veces=0; veces<conex_rutas.count(); veces++)
       {
         ui.conextableWidget->insertRow(veces);
         QTableWidgetItem *newItem = new QTableWidgetItem(conex_rutas.at(veces));
         ui.conextableWidget->setItem(veces,0,newItem);

         QTableWidgetItem *newItem2 = new QTableWidgetItem(rutas_trabajo.at(veces));
         ui.conextableWidget->setItem(veces,1,newItem2);

         QTableWidgetItem *newItem3 = new QTableWidgetItem(rutas_docs.at(veces));
         ui.conextableWidget->setItem(veces,2,newItem3);

       }
}

void asigna_dir_trab::recupera_listas_info(QStringList *conex_rutas, QStringList *rutas_trabajo, QStringList *rutas_docs)
{
    for (int veces=0; veces<ui.conextableWidget->rowCount();veces++)
       {
        *conex_rutas<<ui.conextableWidget->item(veces,0)->text();
        *rutas_trabajo<<ui.conextableWidget->item(veces,1)->text();
        *rutas_docs<<ui.conextableWidget->item(veces,2)->text();
       }
}
