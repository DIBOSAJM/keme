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

#include "consultavencipase.h"
#include "funciones.h"
#include "basedatos.h"
#include "nuevo_vencimiento.h"
#include <QSqlRecord>
#include <QMessageBox>

CustomSqlModel4::CustomSqlModel4(QObject *parent)
        : QSqlQueryModel(parent)
    {
     comadecimal=haycomadecimal(); decimales=haydecimales();
    }


QVariant CustomSqlModel4::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant CustomSqlModel4::data(const QModelIndex &index, int role) const
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
   	        if (value.toDouble()<0.0001 && value.toDouble()>-0.0001)
                   return value.fromValue(vacio);
                else
                      {
                        return value.fromValue(formateanumerosep(value.toDouble(),comadecimal, decimales));
                      }
	       }
           if (index.column() == 8 || index.column() ==9)
              {
               if (value.toBool())  return "*";
                 else return "";
              }
        }
        if (role == Qt::TextAlignmentRole && 
             (index.column() == 0 || index.column()==3 ))
            return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        if (role == Qt::TextAlignmentRole && 
             (index.column() != 0 && index.column()!=3 ))
            return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignCenter));
        return value;
    }




consultavencipase::consultavencipase(QString qpase, QString asiento, QString debe, QString haber,
                                     QString qcuentadiario, QDate qfechadiario, QString qexterno) : QDialog() {
    ui.setupUi(this);

    CustomSqlModel4 *elmodelo = new CustomSqlModel4;
    model = elmodelo;
     model->setQuery( basedatos::instancia()->select9Vencimientospase_diario_operacion(qpase) );
     model->setHeaderData(0, Qt::Horizontal, tr("Núm."));
     model->setHeaderData(1, Qt::Horizontal, tr("Ordenante"));
     model->setHeaderData(2, Qt::Horizontal, tr("Fecha"));
     model->setHeaderData(3, Qt::Horizontal, tr("Importe"));
     model->setHeaderData(4, Qt::Horizontal, tr("Tesorería"));
     model->setHeaderData(5, Qt::Horizontal, tr("Vencimiento"));
     model->setHeaderData(6, Qt::Horizontal, tr("Concepto"));
     model->setHeaderData(10, Qt::Horizontal, tr("Externo"));

     ui.latablavencipase->setModel(model);
     ui.latablavencipase->setAlternatingRowColors ( true);
     ui.latablavencipase->hideColumn(7); // derecho

     comadecimal=haycomadecimal(); decimales=haydecimales();


    eldebe=convapunto(debe);
    elhaber=convapunto(haber);
    cuentadiario=qcuentadiario;
    fechadiario=qfechadiario;
    elpase=qpase;
    externo=qexterno;
   ui.AsientolineEdit->setText(asiento);
   // if (numeracionrelativa()) ui.asrellineEdit->setText(numrelativa(asiento));
   ui.PaselineEdit->setText(qpase);
   calculasinvencimiento();
   connect(ui.AdicionpushButton,SIGNAL(clicked()),this,SLOT(nuevovencimiento()));
   connect(ui.BorrarpushButton,SIGNAL(clicked()),this,SLOT(borravencimiento()));
   connect(ui.EditarpushButton,SIGNAL(clicked()),this,SLOT(editavencimiento()));

}


void consultavencipase::calculasinvencimiento()
{
    double haber=elhaber.toDouble();
    QString cadimporte;
    if (haber<0.0001 && haber>-0.0001) cadimporte=eldebe;
       else 
              {
	       if (haber<0) cadimporte=elhaber.right(elhaber.length()-1);
	       else cadimporte="-"+elhaber;
              }
    // ----------------------------------------------------------------------------------------------
    double elimporte;
    double sustraer = basedatos::instancia()->selectSumimportevencimientospase_diario_operacion(elpase);
    double queda;

    if (sustraer>0.0001)
    {
		elimporte=cadimporte.toDouble();
		if (elimporte<0) sustraer=sustraer*-1;
		queda=elimporte-sustraer;
		if (elimporte<0 && queda>0) queda=0;
		if (elimporte>0 && queda<0) queda=0;
	    cadimporte=cadimporte.setNum(queda,'f',2);
	}
    // -----------------------------------------------------------------------------------------------
   ui.SinvencilineEdit->setText(formateanumerosep(cadimporte.toDouble(),comadecimal,decimales));
   sinvenci=cadimporte;
}

consultavencipase::~consultavencipase()
{
 delete ui.latablavencipase;
 delete model;
}

void consultavencipase::refrescar()
{
    QSqlQuery p=model->query();
    delete(model);
    CustomSqlModel4 *elmodelo = new CustomSqlModel4;
    model = elmodelo;

     model->setQuery( basedatos::instancia()->select9Vencimientospase_diario_operacion(elpase) );
     model->setHeaderData(0, Qt::Horizontal, tr("Núm."));
     model->setHeaderData(1, Qt::Horizontal, tr("Ordenante"));
     model->setHeaderData(2, Qt::Horizontal, tr("Fecha"));
     model->setHeaderData(3, Qt::Horizontal, tr("Importe"));
     model->setHeaderData(4, Qt::Horizontal, tr("Tesorería"));
     model->setHeaderData(5, Qt::Horizontal, tr("Vencimiento"));
     model->setHeaderData(6, Qt::Horizontal, tr("Concepto"));
     model->setHeaderData(10, Qt::Horizontal, tr("Externo"));

     ui.latablavencipase->setModel(model);
     ui.latablavencipase->setAlternatingRowColors ( true);
     ui.latablavencipase->hideColumn(7);
 // QSqlQuery p=model->query(); 
 // p.exec();
 // model->clear();
 // model->setQuery(p);
}

void consultavencipase::nuevovencimiento()
{
    if ((conanalitica() && (escuentadegasto(cuentadiario) ||
                            escuentadeingreso(cuentadiario))) ||
            (conanalitica_parc() && codigo_en_analitica(cuentadiario)))
     {
        QMessageBox::warning( this, tr("Añadir vencimientos"),
                        tr("ERROR: Las cuentas de analítica no pueden poseer vencimientos"));
        return;
     }

    nuevo_vencimiento *venci=new nuevo_vencimiento();
    venci->pasaInfo(cuentadiario,
		    fechadiario,
            sinvenci, externo);
    int resultado; resultado=venci->exec();
    if (resultado==QDialog::Accepted)
      {
	venci->guardanuevovencimiento(elpase);
      }
    
   delete venci;
   calculasinvencimiento();
   // refrescar la tabla
   refrescar();
}


void consultavencipase::borravencimiento()
{
    QString elvencimiento;
    elvencimiento=vencimientoactual();
    if (elvencimiento=="")
       {
         QMessageBox::warning( this, tr("GESTIÓN VENCIMIENTOS"),
         tr("Para borrar un vencimiento debe de seleccionarlo en la tabla"));
         return;
       }

  if (!pendiente(elvencimiento))
       {
         QMessageBox::warning( this, tr("GESTIÓN VENCIMIENTOS"),
         tr("No se puede borrar un vencimiento procesado"));
         return;
       }

  if (QMessageBox::question(
            this,
            tr("GESTIÓN VENCIMIENTOS"),
            tr("¿ Desea eliminar el vencimiento %1 ?")
                .arg( elvencimiento )) == QMessageBox::No)
        return ;

    basedatos::instancia()->deleteVencimientosnum(vencimientoactual());
  refrescar();
  calculasinvencimiento();
}




QString consultavencipase::vencimientoactual()
{
  QString resultado="";
  if (!ui.latablavencipase->currentIndex().isValid()) return resultado;
  int fila=ui.latablavencipase->currentIndex().row();
  return model->record(fila).value("num").toString();
}


void consultavencipase::editavencimiento()
{
    QString elvencimiento;
    elvencimiento=vencimientoactual();
    if (elvencimiento=="")
       {
         QMessageBox::warning( this, tr("GESTIÓN VENCIMIENTOS"),
         tr("Para editar un vencimiento debe de seleccionarlo en la tabla"));
         return;
       }
  if (!pendiente(elvencimiento))
       {
         QMessageBox::warning( this, tr("GESTIÓN VENCIMIENTOS"),
         tr("No se puede editar un vencimiento procesado"));
         return;
       }


   int fila=ui.latablavencipase->currentIndex().row();
   QString cadimporte=model->record(fila).value("importe").toString();
   // QMessageBox::warning( this, tr("Tabla de apuntes"),model->record(fila).value("derecho").toString());
    if (!model->record(fila).value("derecho").toBool())
	cadimporte="-"+cadimporte;
    nuevo_vencimiento *venci=new nuevo_vencimiento();
    venci->pasaInfo(cuentadiario,
		    fechadiario,
		    cadimporte);
    venci->pasaInfo2(model->record(fila).value("cuenta_tesoreria").toString(),
		     model->record(fila).value("fecha_vencimiento").toDate(),
                     model->record(fila).value("concepto").toString(),
                     model->record(fila).value("domiciliable").toBool(),
                     model->record(fila).value("externo").toString());
    int resultado; resultado=venci->exec();
    if (resultado==QDialog::Accepted)
      {
	venci->cambiavencimiento(model->record(fila).value("num").toString());
      }
    
    delete venci;
   refrescar();
   calculasinvencimiento();

}


bool consultavencipase::vencimientoscubiertos()
{
  double valor=convapunto(ui.SinvencilineEdit->text()).toDouble();
  if (valor>-0.001 && valor<0.001) return true;
  return false;
}


bool consultavencipase::pendiente( QString venci )
{
    return basedatos::instancia()->selectsiPendientevencimientosnum(venci);
}
