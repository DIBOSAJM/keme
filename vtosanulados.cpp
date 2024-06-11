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

#include "vtosanulados.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

CustomSqlModelv::CustomSqlModelv(QObject *parent)
        : QSqlQueryModel(parent)
    {
     comadecimal=haycomadecimal(); sindecimales=!haydecimales();
    }


QVariant CustomSqlModelv::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant CustomSqlModelv::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == 2 || index.column() == 5)
               {
                return value.toDate().toString("dd-MM-yyyy");
               }
            if (index.column() == 3)
	       {
                 return value.fromValue(formateanumerosep(value.toDouble(),comadecimal, !sindecimales));
	       }
            if (index.column() == 6 )
               {
                if (value.toBool()) return value.fromValue(tr("DERECHO"));
                   else return value.fromValue(tr("OBLIGACIÓN"));
               }
        }
        if (role == Qt::TextAlignmentRole && 
             (index.column() == 0 || index.column()==3 ))
               return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        return value;
    }



vtosanulados::vtosanulados() : QDialog() 
  {
    ui.setupUi(this);


    QString ejercicio=ejerciciodelafecha(QDate::currentDate());
    if (ejercicio=="") ejercicio=ejercicio_igual_o_maxigual(QDate::currentDate());
    QDate fechaini=inicioejercicio(ejercicio);
    QDate fechafin=finejercicio(ejercicio);
    ui.inicialdateEdit->setDate(fechaini);
    ui.finaldateEdit->setDate(fechafin);

    CustomSqlModelv *elmodelo = new CustomSqlModelv;
    model = elmodelo;

     model->setQuery( basedatos::instancia()->select7Vencimientosfechasanuladoordernum( ui.inicialdateEdit->date() , ui.finaldateEdit->date() ) );
     model->setHeaderData(0, Qt::Horizontal, tr("Núm."));
     model->setHeaderData(1, Qt::Horizontal, tr("Cuenta ord."));
     model->setHeaderData(2, Qt::Horizontal, tr("Feha operación"));
     model->setHeaderData(3, Qt::Horizontal, tr("Importe"));
     model->setHeaderData(4, Qt::Horizontal, tr("Cuenta tesor."));
     model->setHeaderData(5, Qt::Horizontal, tr("Fecha vencim."));
     model->setHeaderData(6, Qt::Horizontal, tr("Derecho"));

     ui.latablavencipase->setModel(model);
     ui.latablavencipase->setAlternatingRowColors ( true);

     comadecimal=haycomadecimal(); sindecimales=!haydecimales();

     connect(ui.inicialdateEdit,SIGNAL(dateChanged(QDate)),SLOT(refrescar()));
     connect(ui.finaldateEdit,SIGNAL(dateChanged(QDate)),SLOT(refrescar()));
     connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borrarvto()));
     connect(ui.recuperarpushButton,SIGNAL(clicked()),SLOT(recuperavto()));

 }


vtosanulados::~vtosanulados()
{
 delete ui.latablavencipase;
 delete model;
}


void vtosanulados::refrescar()
{
    model->clear();
    model->setQuery( basedatos::instancia()->select7Vencimientosfechasanuladoordernum( ui.inicialdateEdit->date() , ui.finaldateEdit->date() ) );
}


void vtosanulados::borrarvto()
{
   QString cadena=vencimientoactual();
   if (cadena.length()==0) return;
   if (QMessageBox::question(
            this,
            tr("Vencimientos anulados"),
            tr("Esta operación dejará al pase del diario\ncorrespondiente sin vencimiento asignado\n"
           "¿ Desea continuar ?")) == QMessageBox::No )
        return ;
    borravenci(cadena);
    refrescar();
    return;
}


QString vtosanulados::vencimientoactual()
{
 if (!ui.latablavencipase->currentIndex().isValid()) return "";
 int fila=ui.latablavencipase->currentIndex().row();
 return model->data(model->index(fila,0),Qt::DisplayRole).toString();
}


void vtosanulados::recuperavto()
{
   QString cadena=vencimientoactual();
   if (cadena.length()==0) return;
   if (QMessageBox::question(
            this,
            tr("Vencimientos anulados"),
            tr("Va a suprimir del vencimiento seleccionado\n"
	"la marca de anulado, "
    "¿ Desea continuar ?")) == QMessageBox::No )
        return ;
    desanulavencimiento(cadena);
    refrescar();
    return;
}
