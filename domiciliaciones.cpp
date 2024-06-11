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

#include "domiciliaciones.h"
#include "domiciliacion.h"
#include <QSqlRecord>
#include "basedatos.h"
#include "funciones.h"
#include <QMessageBox>

CustomSqlModel_doms::CustomSqlModel_doms(QObject *parent)
        : QSqlTableModel(parent)
    {
     comadecimal=haycomadecimal(); decimales=haydecimales();
    }


QVariant CustomSqlModel_doms::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QString CustomSqlModel_doms::nombre_columna(int columna)
{
    return QSqlQueryModel::headerData(columna, Qt::Horizontal, Qt::DisplayRole).toString();
}



QVariant CustomSqlModel_doms::data(const QModelIndex &index, int role) const
    {
     QString vacio;
     QVariant value = QSqlQueryModel::data(index, role);
     if (value.isValid() && role == Qt::DisplayRole) {
        if (index.column() == 3 )
           {
            return value.toDate().toString("dd-MM-yyyy");
            }
     }
     if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column()==4) {
               //importe
                if (value.toDouble()<0.0001 && value.toDouble()>-0.0001)
                       return value.fromValue(vacio);
                    else
                          {
                            if (comadecimal)
                               {
                                if (decimales)
                                    return formateanumerosep(value.toDouble(),comadecimal,true);
                                else
                                    {
                                     return formateanumerosep(value.toDouble(),comadecimal,false);
                                    }
                               }
                             else
                               {
                                if (decimales)
                                    return formateanumerosep(value.toDouble(),comadecimal,true);
                                else
                                    {
                                     return formateanumerosep(value.toDouble(),comadecimal,false);
                                    }
                               }
                           }
            }
            if (index.column() == 5 || index.column() ==6 || index.column() ==7)
               {
                if (value.toBool()) return value.fromValue(tr("*"));
                   else return value.fromValue(tr(""));
               }
        }
        if (role == Qt::TextAlignmentRole &&
             (index.column() == 3 || index.column()>4))
               return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignCenter));
        if (role == Qt::TextAlignmentRole &&
             (index.column() == 0 || index.column()==4))
               return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));

        return value;
    }




domiciliaciones::domiciliaciones(QString qusuario) : QDialog() {
    ui.setupUi(this);

modelo= new CustomSqlModel_doms;
usuario=qusuario;
connect(ui.nuevapushButton,SIGNAL(clicked()),this,SLOT(nueva()));
connect(ui.editarpushButton,SIGNAL(clicked()),this,SLOT(editar()));
connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(borrar()));
refrescar();
}

void domiciliaciones::para_pagos()
{
    pagos=true;
    refrescar();
}


void domiciliaciones::nueva()
{
   domiciliacion *n = new domiciliacion(usuario);
   if (pagos) n->para_pago();
   n->exec();
   delete (n);
   refrescar();
}


void domiciliaciones::refrescar()
{
/*    modelo->setTable("domiciliaciones");
    modelo->setSort(0,Qt::DescendingOrder);
    if (pagos)
       modelo->setFilter("pagos");
      else
        modelo->setFilter("not pagos");*/

    if (!pagos)
        modelo->setTable("vista_domiciliaciones");
      else
          modelo->setTable("vista_transferencias");


    modelo->select();
   /* modelo->setHeaderData(0, Qt::Horizontal, tr("ID."));
    modelo->setHeaderData(1, Qt::Horizontal, tr("CUENTA"));
    modelo->setHeaderData(2, Qt::Horizontal, tr("DESCRIPCIÓN"));
    modelo->setHeaderData(9, Qt::Horizontal, tr("FECHA"));
    modelo->setHeaderData(11, Qt::Horizontal, tr("C19"));
    modelo->setHeaderData(12, Qt::Horizontal, tr("C58"));*/
    // modelo->setHeaderData(13, Qt::Horizontal, tr("Abonada"));

    /*for (int veces=10; veces<15; veces++)
      {
       if (modelo->nombre_columna(veces)=="emitido19")  modelo->setHeaderData(veces, Qt::Horizontal, tr("C19"));
       if (modelo->nombre_columna(veces)=="emitido58")  modelo->setHeaderData(veces, Qt::Horizontal, tr("C18"));
       if (modelo->nombre_columna(veces)=="emitidosepa1914")  modelo->setHeaderData(veces, Qt::Horizontal, tr("SEPA19"));
       if (modelo->nombre_columna(veces)=="procesada")  modelo->setHeaderData(veces, Qt::Horizontal, tr("Abonada"));
      } */
    ui.tableView->setModel(modelo);
    ui.tableView->setColumnWidth(0,80);
    // ui.tableView->setColumnWidth(1,410);
    ui.tableView->setColumnWidth(2,300);
    //ui.tableView->setColumnWidth(11,40);
    //ui.tableView->setColumnWidth(12,40);
    //ui.tableView->setColumnWidth(13,60);

    // for (int veces=3;veces<11; veces++) ui.tableView->hideColumn(veces);
    // ui.tableView->showColumn(9);
    ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );
    ui.tableView->show();
}


void domiciliaciones::editar()
{
  QModelIndex indiceactual=ui.tableView->currentIndex();
  if (!indiceactual.isValid()) return;

  QString id_dom=modelo->record(indiceactual.row()).value("id").toString();
  domiciliacion *n = new domiciliacion(usuario);
  if (pagos) n->para_pago();
  n->carga_domiciliacion(id_dom);
  n->exec();
  delete (n);
  refrescar();

}


void domiciliaciones::borrar()
{
    QModelIndex indiceactual=ui.tableView->currentIndex();
    if (!indiceactual.isValid()) return;

    QString id_dom=modelo->record(indiceactual.row()).value("id").toString();


    if (QMessageBox::question(this,
        tr("Domiciliaciones"),
        tr("¿ Desea eliminar la dociciliacion '%1' ?").arg(id_dom)) == QMessageBox::No )
      return ;

    // QMessageBox::information( this, tr("Domiciliaciones"),tr("Sigue la operación"));

    basedatos::instancia()->elimina_domiciliacion(id_dom);

    refrescar();

}
