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

#include "ejercicios.h"
#include "nuevoejercicio.h"
#include "funciones.h"
#include "basedatos.h"
#include "editafechacierre.h"
#include "numeracion.h"
#include "periodos.h"
#include "pidenombre.h"
#include <QSqlRecord>
#include <qmessagebox.h>
#include <QProgressDialog>


CustomSqlModel3::CustomSqlModel3(QObject *parent)
        : QSqlQueryModel(parent)
    {
    }


QVariant CustomSqlModel3::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant CustomSqlModel3::data(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == 1 || index.column() == 2)
               {
                return value.toDate().toString("dd-MM-yyyy");
               }
            if (index.column() == 3 || index.column() == 6)
               {
                if (value.toBool()) return value.fromValue(tr("SI"));
                   else return value.fromValue(tr("NO"));
               }
        }
        return value;
    }



ejercicios::ejercicios() : QDialog() {
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

    CustomSqlModel3 *elmodelo = new CustomSqlModel3;
    model = elmodelo;

     model->setQuery( basedatos::instancia()->select6ejerciciosorderapertura() );
     model->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     model->setHeaderData(1, Qt::Horizontal, tr("APERTURA"));
     model->setHeaderData(2, Qt::Horizontal, tr("CIERRE"));
     model->setHeaderData(3, Qt::Horizontal, tr("CERRADO"));
     model->setHeaderData(4, Qt::Horizontal, tr("PROX_ASIENTO"));
     model->setHeaderData(5, Qt::Horizontal, tr("PROX_RECEP."));
     model->setHeaderData(6, Qt::Horizontal, tr("BLOQUEADO"));

     ui.ejerciciostableView->setModel(model);
     ui.ejerciciostableView->setAlternatingRowColors ( true);

     connect(ui.nuevopushButton,SIGNAL(clicked()),SLOT(nuevejercicio()));
     connect(ui.eliminarpushButton,SIGNAL(clicked()),SLOT(borraejercicio()));
     connect(ui.fcierrepushButton,SIGNAL(clicked()),SLOT(editacierre()));
     connect(ui.periodospushButton,SIGNAL(clicked()),SLOT(editaperiodos()));
     connect(ui.numeracionpushButton,SIGNAL(clicked()),SLOT(procesanumeracion()));
     connect(ui.recepcionpushButton,SIGNAL(clicked()),SLOT(procesanum_recepcion()));
     connect(ui.bloquea_pushButton,SIGNAL(clicked()),SLOT(on_bloquea_pushButton_clicked()));
}

ejercicios::~ejercicios()
{
  delete ui.ejerciciostableView;
  delete model;
}


void ejercicios::refrescar()
{
    delete(model);
    CustomSqlModel3 *elmodelo = new CustomSqlModel3;
    model = elmodelo;

     model->setQuery( basedatos::instancia()->select6ejerciciosorderapertura() );
     model->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     model->setHeaderData(1, Qt::Horizontal, tr("APERTURA"));
     model->setHeaderData(2, Qt::Horizontal, tr("CIERRE"));
     model->setHeaderData(3, Qt::Horizontal, tr("CERRADO"));
     model->setHeaderData(4, Qt::Horizontal, tr("PROX_ASIENTO"));
     model->setHeaderData(5, Qt::Horizontal, tr("PROX_RECEP."));
     model->setHeaderData(6, Qt::Horizontal, tr("BLOQUEADO"));
     ui.ejerciciostableView->setModel(model);
     ui.ejerciciostableView->setAlternatingRowColors ( true);

 // QSqlQuery p=model->query(); 
 // p.exec();
 // model->clear();
 // model->setQuery(p);
}

void ejercicios::nuevejercicio()
{
  nuevoejercicio *nejercicio = new nuevoejercicio();
  nejercicio->exec();
  delete nejercicio;
  refrescar();
}

void ejercicios::borraejercicio()
{
    QString elcodigo;
    elcodigo=ejercicioactual();

    if (elcodigo=="")
       {
         QMessageBox::warning( this, tr("Ejercicios"),
         tr("Para borrar un ejercicio debe de seleccionarlo en la lista"));
         return;
       }

    if (QMessageBox::question(this,
        tr("Borrar ejercicio"),
        tr("Se eliminarán todos los datos del ejercicio '%1' "
           "¿ Confirmar ?")
          .arg(elcodigo),
        tr("&Sí"), tr("&No"),
                                 QString(), 0, 1 ))
      return ;

    QDate inicio=inicioejercicio(elcodigo);
    QDate fin=finejercicio(elcodigo);

    QProgressDialog progress(tr("Borrando ejercicio '%1'...").arg(elcodigo),
                             tr("Abortar"), 0, 11, this);
    progress.setWindowModality(Qt::WindowModal);


    // eliminamos registro de amortcontable
    basedatos::instancia()->deleteAmortcontableejercicio( elcodigo );

    progress.setValue(1);

    // eliminamos registros de amortfiscalycontable
    basedatos::instancia()->deleteAmortfiscalycontableejercicio( elcodigo );

    progress.setValue(2);

    // borramos registros de libroiva
    QSqlQuery query = basedatos::instancia()->pase_libroiva();
    if (query.isActive())
    {
        while (query.next())
        {
            QDate lafecha=basedatos::instancia()->fecha_pase_diario(query.value(0).toString());
            if (lafecha>=inicio && lafecha<=fin)
                // borramos registro del libroiva
                basedatos::instancia()->borra_reg_libroiva(query.value(0).toString());
        }
    }

    progress.setValue(3);

    // borramos registros de vencimientos
    query = basedatos::instancia()->num_pase_vencimientos();
    if (query.isActive())
    {
        while (query.next())
        {
            QDate lafecha=basedatos::instancia()->fecha_pase_diario(query.value(1).toString());
            if (lafecha>=inicio && lafecha<=fin)
                // borramos registro del libroiva
                basedatos::instancia()->borra_reg_vencimientos(query.value(0).toString());
        }
    }

    progress.setValue(4);

    // borramos registros de presupuestos
    basedatos::instancia()->borra_ej_presupuesto(elcodigo);

    progress.setValue(5);

    // borramos registros de ajustes_conci
    basedatos::instancia()->borra_ej_ajustesconci(inicio,fin);

    progress.setValue(6);

    // borramos registros de cuenta_ext_conci
    basedatos::instancia()->borra_ej_cuenta_ext_conci(inicio,fin);

    // borramos datos de tabla diario analítica
    query = basedatos::instancia()->clave_ci_diario_ci();
    if (query.isActive())
    {
        while (query.next())
        {
          QDate fecha;
          fecha=basedatos::instancia()->fecha_clave_ci_diario(query.value(0).toString());
          if (fecha>=inicio && fecha<=fin)
             // borramos registros en diario_ci
              basedatos::instancia()->borra_clave_ci_diario_ci(query.value(0).toString());
        }
    }

    progress.setValue(7);

    // borramos registros de retenciones
    query = basedatos::instancia()->num_pase_retenciones();
    if (query.isActive())
    {
        while (query.next())
        {
            QDate lafecha=basedatos::instancia()->fecha_pase_diario(query.value(0).toString());
            if (lafecha>=inicio && lafecha<=fin)
                // borramos registro del libroiva
                basedatos::instancia()->borra_reg_retenciones(query.value(0).toString());
        }
    }

    progress.setValue(9);

    // borramos registros del diario entre fechas
    basedatos::instancia()->borra_diario_entrefechas(inicio,fin);

    progress.setValue(10);

    /*
    QString cadena;
    int fila=ui.ejerciciostableView->currentIndex().row();

    if (basedatos::instancia()->selectasientodiariofechas( model->datagen(model->index(fila,1),Qt::DisplayRole).toDate() , model->datagen(model->index(fila,2),Qt::DisplayRole).toDate() )) {
        QMessageBox::warning( this, tr("Borrar ejercicios"),
            tr("ERROR: El ejercicio seleccionado posee apuntes en el diario"));
        return;
    }

    if (basedatos::instancia()->selectEjerciciopresupuestoejercicio( elcodigo )) {
        QMessageBox::warning( this, tr("Borrar ejercicios"),
            tr("ERROR: El ejercicio seleccionado posee apuntes en presupuestos"));
        return;
    } */


    query = basedatos::instancia()->clave_fechafac_facturas();
    if (query.isActive())
    {
        while (query.next())
        {
          QDate fecha;
          fecha=query.value(1).toDate();
          if (fecha>=inicio && fecha<=fin)
            {
             // borramos registros en lin_fact
              basedatos::instancia()->borra_lin_fact_clave(query.value(0).toString());
             // borramos cabecera en facturas
              basedatos::instancia()->borra_facturas_clave(query.value(0).toString());
            }
        }
    }

    progress.setValue(11);

  // Bueno parece que se puede borrar el ejercicio
    basedatos::instancia()->deletePeriodosejercicio(elcodigo);
  basedatos::instancia()->deleteEjercicioscodigo(elcodigo);

  refrescar();
}




QString ejercicios::ejercicioactual()
{

 if (!ui.ejerciciostableView->currentIndex().isValid()) return "";
 int fila=ui.ejerciciostableView->currentIndex().row();
 return model->data(model->index(fila,0),Qt::DisplayRole).toString();

}

QString ejercicios::num_recepcion_actual()
{
    if (!ui.ejerciciostableView->currentIndex().isValid()) return "";
    int fila=ui.ejerciciostableView->currentIndex().row();
    return model->record(fila).value("prox_nrecepcion").toString();

}

bool ejercicios::bloqueado_actual() {
    if (!ui.ejerciciostableView->currentIndex().isValid()) return "";
    int fila=ui.ejerciciostableView->currentIndex().row();
    return model->record(fila).value("bloqueado").toBool();
}

void ejercicios::editacierre()
{
    QString elcodigo;
    elcodigo=ejercicioactual();
    if (elcodigo=="")
       {
         QMessageBox::warning( this, tr("Ejercicios"),
         tr("Para editar la fecha de cierre de un ejercicio debe de seleccionarlo de la lista"));
         return;
       }

 // model->record(fila).value("num").toString();
 int fila=ui.ejerciciostableView->currentIndex().row();
 if (model->record(fila).value("cerrado").toBool() ||
      model->record(fila).value("cerrando").toBool())
    {
         QMessageBox::warning( this, tr("Ejercicios"),
         tr("No es posible editar la fecha de cierre de un ejercicio cerrado"));
         return;
    }

  editafechacierre *ef = new editafechacierre();
  ef->pasafecha(model->record(fila).value("cierre").toDate());
  ef->pasaapertura(model->record(fila).value("apertura").toDate());
  ef->pasaejercicio(model->record(fila).value("codigo").toString());
  ef->exec();
  delete ef;
  refrescar();
}


void ejercicios::editaperiodos()
{
    QString elcodigo;
    elcodigo=ejercicioactual();
    if (elcodigo=="")
       {
         QMessageBox::warning( this, tr("Ejercicios"),
         tr("Para editar periodos de un ejercicio debe de seleccionarlo de la lista"));
         return;
       }

 periodos *p = new periodos(elcodigo);
 p->exec();
 delete p;

}


void ejercicios::procesanumeracion()
{
    QString elcodigo;
    elcodigo=ejercicioactual();
    if (elcodigo=="")
       {
         QMessageBox::warning( this, tr("Ejercicios"),
         tr("Para editar un ejercicio debe de seleccionarlo de la lista"));
         return;
       }
    numeracion *n = new numeracion();
    n->pasaejercicio(elcodigo);
    n->exec();
    delete n;
    refrescar();

}


void ejercicios::procesanum_recepcion()
{
    QString elcodigo;
    elcodigo=ejercicioactual();
    if (elcodigo=="")
       {
         QMessageBox::warning( this, tr("Ejercicios"),
         tr("Para editar un ejercicio debe de seleccionarlo de la lista"));
         return;
       }
    pidenombre *n = new pidenombre();
    n->asignacontenido(num_recepcion_actual());
    n->asignaetiqueta(tr("Próximo número de recepción:"));
    // n->pasaejercicio(elcodigo);
    n->exec();
    QString cad=n->contenido();
    basedatos::instancia()->pasa_prox_nrecepcion_ejercicio (elcodigo, cad);

    delete n;

    refrescar();

}

void ejercicios::on_bloquea_pushButton_clicked()
{
    ui.bloquea_pushButton->disconnect(SIGNAL(clicked()));
    QString elcodigo;
    elcodigo=ejercicioactual();
    if (elcodigo=="")
       {
         QMessageBox::warning( this, tr("Ejercicios"),
         tr("Para editar un ejercicio debe de seleccionarlo de la lista"));
         return;
       }
    basedatos::instancia()->actu_bloquea_ejercicio(elcodigo,!bloqueado_actual());

    connect(ui.bloquea_pushButton,SIGNAL(clicked()),SLOT(on_bloquea_pushButton_clicked()));
      refrescar();
}
