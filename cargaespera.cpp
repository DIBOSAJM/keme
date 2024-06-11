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

#include "cargaespera.h"
#include "funciones.h"
#include "basedatos.h"
#include <QSqlRecord>
#include <QMessageBox>

CustomSqlModelC::CustomSqlModelC(QObject *parent)
        : QSqlQueryModel(parent)
    {
     comadecimal=haycomadecimal(); decimales=haydecimales();
    }


QVariant CustomSqlModelC::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant CustomSqlModelC::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == 4 || index.column()==5)
	       {
   	        if (value.toDouble()<0.0001 && value.toDouble()>-0.0001)
                   return value.fromValue(vacio);
                else
                      {
                        return value.fromValue(formateanumerosep(value.toDouble(),comadecimal, decimales));
                      }
	       }
        }
        if (role == Qt::TextAlignmentRole && 
             (index.column() == 0 || index.column()==4 ||
              index.column() == 5 ))
            return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        return value;
    }



cargaespera::cargaespera() : QDialog() 
  {
    ui.setupUi(this);

    CustomSqlModelC *elmodelo = new CustomSqlModelC;
    model = elmodelo;

     model->setQuery( basedatos::instancia()->select8Borradororderasiento() );
     model->setHeaderData(0, Qt::Horizontal, tr("Asiento"));
     model->setHeaderData(1, Qt::Horizontal, tr("Usuario"));
     model->setHeaderData(2, Qt::Horizontal, tr("Cuenta"));
     model->setHeaderData(3, Qt::Horizontal, tr("Concepto"));
     model->setHeaderData(4, Qt::Horizontal, tr("DEBE"));
     model->setHeaderData(5, Qt::Horizontal, tr("HABER"));
     model->setHeaderData(6, Qt::Horizontal, tr("Documento"));
     model->setHeaderData(7, Qt::Horizontal, tr("C.I."));

     ui.elborrador->setModel(model);
     ui.elborrador->setAlternatingRowColors ( true);

     comadecimal=haycomadecimal(); decimales=haydecimales();
   // if (!conanalitica()) ui.elborrador->setColumnHidden(7,true);
     ui.elborrador->setColumnHidden(7,true);
     ui.elborrador->resizeColumnsToContents();

   connect(ui.BorrarButton,SIGNAL(clicked()),this,SLOT(eliminarasiento()));
   connect(ui.refrescapushButton,SIGNAL(clicked()),this,SLOT(refrescar()));


 }



void cargaespera::refrescar()
{
// QSqlQuery p=model->query();
// p.exec();
// model->clear();
// model->setQuery(p);

 model->setQuery( basedatos::instancia()->select8Borradororderasiento() );
 model->setHeaderData(0, Qt::Horizontal, tr("Asiento"));
 model->setHeaderData(1, Qt::Horizontal, tr("Usuario"));
 model->setHeaderData(2, Qt::Horizontal, tr("Cuenta"));
 model->setHeaderData(3, Qt::Horizontal, tr("Concepto"));
 model->setHeaderData(4, Qt::Horizontal, tr("DEBE"));
 model->setHeaderData(5, Qt::Horizontal, tr("HABER"));
 model->setHeaderData(6, Qt::Horizontal, tr("Documento"));
 model->setHeaderData(7, Qt::Horizontal, tr("C.I."));

 ui.elborrador->setModel(model);
 ui.elborrador->setAlternatingRowColors ( true);

 comadecimal=haycomadecimal(); decimales=haydecimales();
 // if (!conanalitica()) ui.elborrador->setColumnHidden(7,true);
 ui.elborrador->setColumnHidden(7,true);
 ui.elborrador->resizeColumnsToContents();


}


int cargaespera::asientoactual()
{
  if (!ui.elborrador->currentIndex().isValid()) return 0;
  int fila=ui.elborrador->currentIndex().row();
  return model->record(fila).value("asiento").toInt();
}


void cargaespera::eliminarasiento()
{
    qlonglong nasiento=asientoactual();
    QString asiento; asiento.setNum(nasiento);
    if (nasiento==0) return;

    if (QMessageBox::question(
            this,
            tr("Apuntes en espera"),
            tr("¿ Desea eliminar el asiento en espera seleccionado ?"))==QMessageBox::No )
        return ;

    basedatos::instancia()->deleteBorrador(asiento);

    refrescar();
}



cargaespera::~cargaespera()
{
 delete ui.elborrador;
 delete model;
}


void cargaespera::elimasientoactual()
{
    qlonglong nasiento=asientoactual();
    QString asiento; asiento.setNum(nasiento);
    if (nasiento==0) return;

    basedatos::instancia()->deleteBorrador(asiento);
}

void cargaespera::on_borratodo_pushButton_clicked()
{
    if (QMessageBox::question(
            this,
            tr("Apuntes en espera"),
            tr("¿ Desea eliminar todo el contenido ?"))==QMessageBox::No )
            return ;

        basedatos::instancia()->borra_borrador();

        refrescar();
}

