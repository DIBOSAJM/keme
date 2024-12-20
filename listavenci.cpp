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

#include "listavenci.h"
#include "funciones.h"
#include "basedatos.h"
#include <QSqlRecord>
#include "buscasubcuenta.h"
#include "procesavencimiento.h"
#include "pide_info_unif.h"
#include "editarasiento.h"
#include "consultavencipase.h"
#include "busca_externo.h"
#include <QMessageBox>

CustomSqlModel5::CustomSqlModel5(QObject *parent)
        : QSqlTableModel(parent)
    {
     comadecimal=haycomadecimal(); decimales=haydecimales();
    }


QVariant CustomSqlModel5::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant CustomSqlModel5::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);

        QString nombrecol = QSqlQueryModel::headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column()== 2|| index.column() == 5
                || nombrecol=="fecha_realizacion" || nombrecol=="forzar_a_fecha"
                || nombrecol=="liq_iva_fecha")
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
            if (index.column() == 6)
	       {
                if (value.toBool()) return value.fromValue(tr("DERECHO"));
                   else return value.fromValue(tr("OBLIGACION"));
               }
            if (nombrecol=="pendiente" || nombrecol=="anulado" ||
                nombrecol=="impagado" || nombrecol=="domiciliable"
                || nombrecol=="forzar_liq_iva")
               {
                if (value.toBool()) return value.fromValue(tr("SI"));
                   else return value.fromValue(tr("NO"));
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




listavenci::listavenci(QString qusuario) : QDialog() {
    ui.setupUi(this);


    QString ejercicio=ejerciciodelafecha(QDate::currentDate());
    if (ejercicio=="") ejercicio=ejercicio_igual_o_maxigual(QDate::currentDate());
    QDate fechaini=inicioejercicio(ejercicio);
    QDate fechafin=finejercicio(ejercicio);
    ui.inicialdateEdit->setDate(fechaini);
    ui.finaldateEdit->setDate(fechafin);
    ui.venciinidateEdit->setDate(fechaini);
    ui.vencifindateEdit->setDate(fechafin);

    CustomSqlModel5 *elmodelo = new CustomSqlModel5;
    model = elmodelo;

     //model->setTable("vencimientos");
     model->setTable("vistavenci");
     model->setHeaderData(0, Qt::Horizontal, tr("Núm."));
     model->setHeaderData(1, Qt::Horizontal, tr("Ordenante"));
     model->setHeaderData(2, Qt::Horizontal, tr("Fecha"));
     model->setHeaderData(3, Qt::Horizontal, tr("Importe"));
     model->setHeaderData(4, Qt::Horizontal, tr("Tesorería"));
     model->setHeaderData(5, Qt::Horizontal, tr("Vencimiento"));
     model->setHeaderData(6, Qt::Horizontal, tr("Tipo"));
     ui.latablavencipase->setEditTriggers (QAbstractItemView::NoEditTriggers);
     // ui.latablavencipase->setEditable(false);
     ui.latablavencipase->setSortingEnabled(true);

     // ui.latablavencipase->setModel(model);
     ui.latablavencipase->setAlternatingRowColors ( true);

     ui.latablavencipase->setColumnHidden(7,true);
     ui.latablavencipase->setColumnHidden(8,true);
     ui.latablavencipase->setColumnHidden(9,true);
     ui.latablavencipase->setColumnHidden(10,true);
     ui.latablavencipase->setColumnHidden(11,true);

     comadecimal=haycomadecimal(); decimales=haydecimales();
     // model->select();
     refrescar();

     ui.latablavencipase->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui.latablavencipase->setSelectionMode(QAbstractItemView::ExtendedSelection);

     ui.latablavencipase->sortByColumn(0,Qt::AscendingOrder);

   connect(ui.anotaciongroupBox,SIGNAL(toggled(bool)),this,SLOT(estadogrupofanotcambiado()));
   connect(ui.vencigroupBox,SIGNAL(toggled(bool)),this,SLOT(estadogrupovencicambiado()));

  connect(ui.cuentacheckBox,SIGNAL(stateChanged(int)),this,SLOT(cuentacheckBoxcambiado( int )));
  connect(ui.cuentalineEdit,SIGNAL(editingFinished ()),this,SLOT(trasctaordenante()));
  connect(ui.buscapushButton,SIGNAL( clicked()),this,SLOT(buscacuenta()));
  connect(ui.cuentalineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaordenantecambiada()));

  connect(ui.cuentatcheckBox,SIGNAL(stateChanged(int)),this,SLOT(cuentatcheckBoxcambiado( int )));
  connect(ui.cuentatlineEdit,SIGNAL(editingFinished ()),this,SLOT(trasctatordenante()));
  connect(ui.buscatpushButton,SIGNAL( clicked()),this,SLOT(buscacuentat()));
  connect(ui.cuentatlineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctatordenantecambiada()));

  connect(ui.externo_checkBox,SIGNAL(stateChanged(int)),this,SLOT(externo_checkBoxcambiado(int)));
  connect(ui.busca_externo_pushButton,SIGNAL(clicked(bool)),this,SLOT(busca_ext()));
  connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(externo_cambiado()));

  connect(ui.refrescapushButton,SIGNAL( clicked()),this,SLOT(refrescar()));

  connect(ui.infopushButton,SIGNAL( clicked()),this,SLOT(infovencipase()));

  connect(ui.procesapushButton,SIGNAL( clicked()),this,SLOT(procesar()));
  connect(ui.copia2_pushButton,SIGNAL( clicked()),this,SLOT(copia_informe2()));
  connect(ui.borrarpushButton,SIGNAL( clicked()),this,SLOT(borrar()));

  connect(ui.unificarpushButton,SIGNAL( clicked()),this,SLOT(unificarproc()));
  connect(ui.imprimirpushButton,SIGNAL( clicked()),this,SLOT(imprimir()));
  connect(ui.informe2pushButton,SIGNAL( clicked()),this,SLOT(imprimir2()));

  connect(ui.consulta_asientopushButton,SIGNAL(clicked()),this,SLOT(consulta_asiento()));
  connect(ui.ed_asientopushButton,SIGNAL(clicked()),this,SLOT(edita_asiento()));
  connect(ui.conspagopushButton,SIGNAL(clicked()),this,SLOT(consulta_asiento_pago()));
  connect(ui.editapagopushButton,SIGNAL(clicked()),this,SLOT(edita_asiento_pago()));
  connect(ui.edvencipushButton,SIGNAL(clicked()),this,SLOT(editavenci()));
  connect(ui.copiarpushButton,SIGNAL(clicked()),this,SLOT(copiar()));
  connect(ui.latablavencipase,SIGNAL(clicked(QModelIndex)),this,SLOT(infocuentalin(QModelIndex)));
  connect(ui.importe_desdelineEdit,SIGNAL(textChanged(QString)),this,SLOT(importe_desde_cambiado()));
  connect(ui.importe_hastalineEdit,SIGNAL(textChanged(QString)),this,SLOT(importe_hasta_cambiado()));

  connect(ui.pendientescheckBox,SIGNAL(stateChanged(int)),this,SLOT(checkpendientes_procesados_cambiado()));
  connect(ui.procesadoscheckBox,SIGNAL(stateChanged(int)),this,SLOT(checkpendientes_procesados_cambiado()));

  usuario=qusuario;

  if (!basedatos::instancia()->hay_externos()) {
      ui.externo_checkBox->hide();
      ui.externo_lineEdit->hide();
      ui.busca_externo_pushButton->hide();
      ui.externo_descrip_lineEdit->hide();
  }

}


listavenci::~listavenci()
{
 delete ui.latablavencipase;
 delete model;
}

void listavenci::pasa_int_fechas_vencimiento(QDate fechaini, QDate fechafin, int margen)
{
    QDate ini=fechaini.addDays(-margen);
    QDate fin=fechafin.addDays(margen);
    ui.anotaciongroupBox->setChecked(false);
    ui.vencigroupBox->setChecked(true);
    ui.venciinidateEdit->setDate(ini);
    ui.vencifindateEdit->setDate(fin);
}

void listavenci::pasa_cta_tesoreria(QString cuenta)
{
  ui.cuentatcheckBox->setChecked(true);
  ui.cuentatlineEdit->setText(cuenta);
}

void listavenci::refrescar()
{
 // QSqlQuery p=model->query(); 
 // p.exec();
 // model->clear();
 // model->setTable("vencimientos");
 asignafiltro();
 // model->clear();
 // model->select();
 calculasuma();
 ui.latablavencipase->setCurrentIndex(model->index(0,0));
 infocuentalin(ui.latablavencipase->currentIndex());
 ui.selec_label->setText("- -");
}


void listavenci::estadogrupofanotcambiado()
{
  if (ui.anotaciongroupBox->isChecked())
     ui.vencigroupBox->setChecked(false);
  else
     ui.vencigroupBox->setChecked(true);
}


void listavenci::estadogrupovencicambiado()
{
  if (ui.vencigroupBox->isChecked())
      ui.anotaciongroupBox->setChecked(false);
  else
      ui.anotaciongroupBox->setChecked(true);
}


void listavenci::cuentacheckBoxcambiado( int activado )
{
    if (activado) 
      { 
        ui.cuentalineEdit->setEnabled(true);
        ui.descriplineEdit->setEnabled(true);
        ui.buscapushButton->setEnabled(true);
     }
    else
      { 
        ui.cuentalineEdit->setText("");
        ui.descriplineEdit->setText("");
        ui.cuentalineEdit->setEnabled(false);
        ui.descriplineEdit->setEnabled(false);
        ui.buscapushButton->setEnabled(false);
     }
}


void listavenci::trasctaordenante()
{
  QString cadena;
  cadena=expandepunto(ui.cuentalineEdit->text(),anchocuentas());
  ui.cuentalineEdit->setText(cadena);
}


void listavenci::ctaordenantecambiada()
{
    QString descrip;
   if(ui.cuentalineEdit->text().length()==anchocuentas())
     {
       if (existecodigoplan(ui.cuentalineEdit->text(),&descrip))
          ui.descriplineEdit->setText(descrip);
       else
          ui.descriplineEdit->setText("");
     }
     else ui.descriplineEdit->setText("");
}



void listavenci::buscacuenta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentalineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();

    if (cod==QDialog::Accepted && cadena2.length()==anchocuentas()) ui.cuentalineEdit->setText(cadena2);
       else ui.cuentalineEdit->setText("");
    delete labusqueda;
}


void listavenci::asignafiltro()
{
  QString cadfiltro="";
  if (ui.anotaciongroupBox->isChecked())
     {
      cadfiltro="fecha_operacion>='";
      cadfiltro+=ui.inicialdateEdit->date().toString("yyyy-MM-dd");
      cadfiltro+="' and fecha_operacion<='";
      cadfiltro+=ui.finaldateEdit->date().toString("yyyy-MM-dd");
      cadfiltro+="'";
     }
  if (ui.pendientescheckBox->isChecked() && !ui.procesadoscheckBox->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" pendiente";
    }
  if (ui.procesadoscheckBox->isChecked() && !ui.pendientescheckBox->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" NOT pendiente";
    }
  if (!ui.procesadoscheckBox->isChecked() && !ui.pendientescheckBox->isChecked())
   {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" pendiente and NOT pendiente";
   }

  if (ui.cobroscheckBox->isChecked() && !ui.pagoscheckBox->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" derecho";
    }
  if (ui.pagoscheckBox->isChecked() && !ui.cobroscheckBox->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" not derecho";
    }
  if (!ui.pagoscheckBox->isChecked() && !ui.cobroscheckBox->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" derecho and not derecho";
    }
  if (ui.vencigroupBox->isChecked())
     {
      bool solo_procesadas=ui.procesadoscheckBox->isChecked() &&
                           !ui.pendientescheckBox->isChecked();
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+= solo_procesadas ? " fecha_realizacion>='" : " fecha_vencimiento>='";
      cadfiltro+=ui.venciinidateEdit->date().toString("yyyy-MM-dd");
      cadfiltro+=solo_procesadas ? "' and fecha_realizacion<='" : "' and fecha_vencimiento<='";
      cadfiltro+=ui.vencifindateEdit->date().toString("yyyy-MM-dd");
      cadfiltro+="'";
     }
  if (ui.cuentacheckBox->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND ";
      if (!cod_longitud_variable() && ui.cuentalineEdit->text().length()<anchocuentas())
         {
          if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
              cadfiltro += "cta_ordenante like '"+ ui.cuentalineEdit->text().left(-1).replace("'","''") +"%' ";
              }
             else
                 {
                  cadfiltro += "position('";
                  cadfiltro += ui.cuentalineEdit->text().left(-1).replace("'","''");
                  cadfiltro += "' in cta_ordenante)=1";
                 }
         }
         else
          {
           cadfiltro+="cta_ordenante='";
           cadfiltro+=ui.cuentalineEdit->text();
           cadfiltro+="'";
          }
    }
  if (ui.cuentatcheckBox->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" cuenta_tesoreria='";
      cadfiltro+=ui.cuentatlineEdit->text();
      cadfiltro+="'";
    }

  if (ui.externo_checkBox->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" externo='";
      cadfiltro+=ui.externo_lineEdit->text();
      cadfiltro+="'";
    }

  if (ui.domiciliablesradioButton->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" domiciliable";
    }
  if (ui.nodomiciliablesradioButton->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" not domiciliable";
    }



  if (ui.domiciliadosradioButton->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" domiciliacion>0";
    }
  if (ui.nodomiciliadosradioButton->isChecked())
    {
      if (cadfiltro.length()>0) cadfiltro+=" AND";
      cadfiltro+=" domiciliacion=0";
    }

  double desde=convapunto(ui.importe_desdelineEdit->text()).toDouble();
  double hasta=convapunto(ui.importe_hastalineEdit->text()).toDouble();

  if (!(desde>-0.001 && desde<0.001 && hasta>-0.001 && hasta<0.001))
    {
      QString caddesde; caddesde.setNum(desde,'f',2);
      QString cadhasta; cadhasta.setNum(hasta,'f',2);
      cadfiltro+=" AND importe>="; cadfiltro+=caddesde;
      cadfiltro+=" AND importe<="; cadfiltro+=cadhasta;
    }


  cadfiltro+=" AND not anulado";
  // model->clear();
  // model->setTable("vencimientos");
  model->setFilter (cadfiltro);
  model->select();
  ui.latablavencipase->setModel(model);
  ui.latablavencipase->show();
  // QMessageBox::information( this, tr("Vencimientos"),cadfiltro);

}


QString listavenci::vencimientoactual()
{
  QString resultado="";
  if (!ui.latablavencipase->currentIndex().isValid()) return resultado;
  int fila=ui.latablavencipase->currentIndex().row();
  return model->record(fila).value("num").toString();
}


void listavenci::infovencipase()
{
   QString cadena=vencimientoactual();
   int fila=ui.latablavencipase->currentIndex().row();
   if (cadena.length()>0)
      {
       QString asiento=numasientovto(cadena);
       QString cad2="";
       //if (numeracionrelativa()) cad2="Asiento correspondiente: "+numrelativa(asiento);
       cad2="Asiento correspondiente: "+asiento;
       if (ui.procesadoscheckBox->isChecked())
          {
             cad2+=tr("\nAsiento vencimiento procesado: ");
             cad2+=numasientovtovto(cadena);
          }
       cad2+=tr("\nCuenta tesorería: ");
       cad2+=descripcioncuenta(model->record(fila).value("cuenta_tesoreria").toString());
       cad2+=tr("\nCuenta ordenante: ");
       cad2+=descripcioncuenta(model->record(fila).value("cta_ordenante").toString());
       QMessageBox::information( this, tr("Vencimientos"),cad2);
      }
}


void listavenci::calculasuma()
{
    double total = basedatos::instancia()->selectSumimportevendimientosfiltroderecho( model->filter() , true);
  
    total -= basedatos::instancia()->selectSumimportevendimientosfiltroderecho( model->filter() , false);

    ui.sumaimportelineEdit->setText(formateanumero(total,comadecimal,decimales));
}


void listavenci::procesar()
{
    QItemSelectionModel *seleccion;
    seleccion=ui.latablavencipase->selectionModel();
    if (seleccion->hasSelection())
       {
         QList<QModelIndex> listanum;
         QList<QModelIndex> listacta_ordenante;
         QList<QModelIndex> listafecha_operacion;
         QList<QModelIndex> listaimporte;
         QList<QModelIndex> listacuenta_tesoreria;
         QList<QModelIndex> listafecha_vencimiento;
         QList<QModelIndex> listaderecho;
         listanum= seleccion->selectedRows ( 0 );
         listacta_ordenante=seleccion->selectedRows (1);
         listafecha_operacion=seleccion->selectedRows (2);
         listaimporte=seleccion->selectedRows (3);
         listacuenta_tesoreria=seleccion->selectedRows (4);
         listafecha_vencimiento=seleccion->selectedRows (5);
         listaderecho=seleccion->selectedRows (6);

         // comprobar misma cuenta tesorería y fecha vencimiento
         if (listanum.size()>1)
          {
             if (QMessageBox::question(
                      this,
                      tr("Vencimientos"),
                      tr("¿ Desea procesar por lotes los vencimientos seleccionados ?")) == QMessageBox::No)
                  return ;
            for (int i = 0; i < listacta_ordenante.size(); ++i)
              {
                QString cadena=model->datagen(listanum.at(i),Qt::DisplayRole).toString();
                if (vencimientoprocesado(cadena))
                    continue;
                // existe cuenta de cargo ?
                QString qctatesoreria=model->datagen(listacuenta_tesoreria.at(i),Qt::DisplayRole).toString();
                if (!existesubcuenta(qctatesoreria)) continue;
                procesavencimiento *procvenci=new procesavencimiento(usuario);
                procvenci->cargadatos(cadena);
                procvenci->botonprocesarpulsado();
                delete procvenci;
              }
            // QMessageBox::information( this, tr("Vencimientos"),tr("Se han procesado los vencimientos"));
            refrescar();
            return;
          }
      }

    // ------------------------------------------------
    // procesamos el vencimiento como uno normal

   QString cadena=vencimientoactual();
   if (cadena.length()==0) return;
   // está ya procesado ??: pendiente==false ??
   if (vencimientoprocesado(cadena))
     {
       QMessageBox::warning( this, tr("Vencimientos"),tr("El vencimiento ya está procesado"));
       return;
     }
    procesavencimiento *procvenci=new procesavencimiento(usuario);
    procvenci->cargadatos(cadena);
    int resultado=procvenci->exec();
    if (resultado!=QDialog::Accepted)
       {
         QMessageBox::information( this, tr("Vencimientos"),tr("El procesado del vencimiento se ha cancelado"));
       }
    delete procvenci;
    refrescar();
}


void listavenci::anular()
{
   QString cadena=vencimientoactual();
   if (cadena.length()==0) return;
  // para marcar como anulado, no debe de estar procesado el vencimiento
   if (vencimientoprocesado(cadena))
     {
       QMessageBox::warning( this, tr("Vencimientos"),tr("No se puede anular un vencimiento procesado"));
       return;
     }
   if (QMessageBox::question(
            this,
            tr("Vencimientos"),
            tr("¿ Desea anular el vencimiento seleccionado ?")) == QMessageBox::No )
        return ;
   anulavencimiento(cadena);
   refrescar();
   return;
}


void listavenci::borrar()
{

    QItemSelectionModel *seleccion;
    seleccion=ui.latablavencipase->selectionModel();
    if (seleccion->hasSelection())
       {
         QList<QModelIndex> listanum;
         listanum= seleccion->selectedRows ( 0 );

         // comprobar misma cuenta tesorería y fecha vencimiento
         if (listanum.size()>1)
          {
             if (QMessageBox::question(
                      this,
                      tr("Vencimientos"),
                      tr("¿ Desea borrar por lotes los vencimientos seleccionados ?")) == QMessageBox::No )
                  return ;
            for (int i = 0; i < listanum.size(); ++i)
              {
                QString cadasiento;
                QString cadena=model->datagen(listanum.at(i),Qt::DisplayRole).toString();
                if (vencimientoprocesado(cadena))
                   {
                      // cadasiento=numasientovtovto(vencimientoactual());
                      QDate fechaasiento =basedatos::instancia()->fecha_pase_diario_vto (cadena);
                      cadasiento=basedatos::instancia()->numasientovtovto(cadena);
                      QString ejercicio=ejerciciodelafecha(fechaasiento);
                      if (ejerciciocerrado(ejercicio))
                      {
                          QMessageBox::warning( this, tr("Vencimientos"),
                             tr("ERROR: el asiento procesado del vencimiento %1 pertenece a ejercicio cerrado").arg(cadena));
                          continue;
                      }
                      if (basedatos::instancia()->ejercicio_bloqueado(ejercicio))
                      {
                          QMessageBox::warning( this, tr("Vencimientos"),
                             tr("ERROR: el asiento procesado del vencimiento %1 pertenece a ejercicio bloqueado").arg(cadena));
                          continue;
                      }
                       // borramos el asiento del vencimiento procesado
                      borraasientos(cadasiento,cadasiento,ejercicio);
                   }
                borravenci(cadena);
              }
            // QMessageBox::information( this, tr("Vencimientos"),tr("Se han procesado los vencimientos"));
            refrescar();
            return;
          }
      }

    // ------------------------------------------------
    // procesamos el vencimiento como uno normal


   QString cadasiento="";
   QString cadena=vencimientoactual();
   if (cadena.length()==0) return;
   if (vencimientoprocesado(cadena))
      {
         // cadasiento=numasientovtovto(vencimientoactual());
         QDate fechaasiento =basedatos::instancia()->fecha_pase_diario_vto (vencimientoactual());
         cadasiento=basedatos::instancia()->numasientovtovto(cadena);
         QString ejercicio=ejerciciodelafecha(fechaasiento);
         if (ejerciciocerrado(ejercicio)) {
             QMessageBox::warning( this, tr("Vencimientos"),
                tr("ERROR: el asiento procesado pertenece a ejercicio cerrado"));
             return;
         }
         if (basedatos::instancia()->ejercicio_bloqueado(ejercicio)) {
             QMessageBox::warning( this, tr("Vencimientos"),
                tr("ERROR: el asiento procesado pertenece a ejercicio bloqueado"));
             return;
         }
          // borramos el asiento del vencimiento procesado
         borraasientos(cadasiento,cadasiento,ejercicio);
      }
   borravenci(cadena);
   refrescar();
   return;
}



void listavenci::cuentatcheckBoxcambiado( int activado )
{
    if (activado) 
      { 
        ui.cuentatlineEdit->setEnabled(true);
        ui.descriptlineEdit->setEnabled(true);
        ui.buscatpushButton->setEnabled(true);
     }
    else
      { 
        ui.cuentatlineEdit->setText("");
        ui.descriptlineEdit->setText("");
        ui.cuentatlineEdit->setEnabled(false);
        ui.descriptlineEdit->setEnabled(false);
        ui.buscatpushButton->setEnabled(false);
     }
}


void listavenci::trasctatordenante()
{
  QString cadena;
  cadena=expandepunto(ui.cuentatlineEdit->text(),anchocuentas());
  ui.cuentatlineEdit->setText(cadena);
}


void listavenci::externo_checkBoxcambiado( int activado ) {
    if (activado)
      {
        ui.externo_lineEdit->setEnabled(true);
        ui.externo_descrip_lineEdit->setEnabled(true);
        ui.busca_externo_pushButton->setEnabled(true);
     }
    else
      {
        ui.externo_lineEdit ->setText("");
        ui.externo_descrip_lineEdit->setText("");
        ui.externo_lineEdit->setEnabled(false);
        ui.externo_descrip_lineEdit->setEnabled(false);
        ui.busca_externo_pushButton->setEnabled(false);
     }
}


void listavenci::ctatordenantecambiada()
{
    QString descrip;
   if(ui.cuentatlineEdit->text().length()==anchocuentas())
     {
       if (existecodigoplan(ui.cuentatlineEdit->text(),&descrip))
          ui.descriptlineEdit->setText(descrip);
       else
          ui.descriptlineEdit->setText("");
     }
     else ui.descriptlineEdit->setText("");
}



void listavenci::buscacuentat()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentatlineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();

    if (cod==QDialog::Accepted && cadena2.length()==anchocuentas()) ui.cuentatlineEdit->setText(cadena2);
       else ui.cuentatlineEdit->setText("");
    delete labusqueda;
}

void listavenci::infocuentalin(QModelIndex primaryKeyIndex)
{
   if (!primaryKeyIndex.isValid())
      {
       ui.codigolabel->setText("");
       ui.descriplabel->setText("");
       return;
      }
   int fila=primaryKeyIndex.row();
   QString cad = model->data(model->index(fila,1),Qt::DisplayRole).toString();
   //if (ui.latablavencipase->)
   QString externo=basedatos::instancia()->externo_pase(model->data(model->index(fila,9),Qt::DisplayRole).toString());
   if (externo.isEmpty()) {
       ui.codigolabel->setText(cad);
       ui.descriplabel->setText(descripcioncuenta(cad));
   }
   else {
       ui.codigolabel->setText(externo);
       ui.descriplabel->setText(basedatos::instancia()->razon_externo(externo));
   }

   // verificamos suma de agrupación

   QItemSelectionModel *seleccion;
   seleccion=ui.latablavencipase->selectionModel();
   if (!seleccion->hasSelection()) return;
   QList<QModelIndex> listaimporte;
   QList<QModelIndex> listaderecho;
   listaimporte=seleccion->selectedRows (3);
   listaderecho=seleccion->selectedRows (6);
   double saldo=0;
   for (int i = 0; i < listaimporte.size(); ++i)
      {
       QString cadimporte = model->datagen(listaimporte.at(i),Qt::DisplayRole).toString();
       if (model->datagen(listaderecho.at(i),Qt::DisplayRole).toBool())
          {
           saldo += cadimporte.toDouble();
          }
       else
          {
           saldo -= cadimporte.toDouble();
       }

      }
   ui.selec_label->setText(formateanumerosep(saldo,comadecimal,decimales));

}


void listavenci::unificarproc()
{
 QItemSelectionModel *seleccion;
 seleccion=ui.latablavencipase->selectionModel();
 if (seleccion->hasSelection())
    {
      QList<QModelIndex> listanum;
      QList<QModelIndex> listacta_ordenante;
      QList<QModelIndex> listafecha_operacion;
      QList<QModelIndex> listaimporte;
      QList<QModelIndex> listacuenta_tesoreria;
      QList<QModelIndex> listafecha_vencimiento;
      QList<QModelIndex> listaderecho;
      listanum= seleccion->selectedRows ( 0 );
      listacta_ordenante=seleccion->selectedRows (1);
      listafecha_operacion=seleccion->selectedRows (2);
      listaimporte=seleccion->selectedRows (3);
      listacuenta_tesoreria=seleccion->selectedRows (4);
      listafecha_vencimiento=seleccion->selectedRows (5);
      listaderecho=seleccion->selectedRows (6);

      int columna_externo=0;
      for (int veces=7; veces<model->columnCount(); veces++)
          if (model->headerData(veces,Qt::Horizontal).toString()==tr("externo"))
              columna_externo=veces;
      QList<QModelIndex> listaexterno;
      listaexterno = seleccion->selectedRows ( columna_externo );
      //QString cadnumero; cadnumero.setNum(columna_externo);
      //QMessageBox::information( this, tr("Vencimientos"),cadnumero);

      //seleccion->record(0).value("externo").toString(),

      // comprobar misma cuenta tesorería y fecha vencimiento
      if (listanum.size()<2)
       {
        QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: se han "
                             "seleccionado menos de dos vencimientos"));
        return;
       }
      QString qexterno=model->datagen(listaexterno.at(0),Qt::DisplayRole).toString();
      QString qctatesoreria=model->datagen(listacuenta_tesoreria.at(0),Qt::DisplayRole).toString();
      QDate qvto=model->datagen(listafecha_vencimiento.at(0),Qt::DisplayRole).toDate();
/*      if (!fechacorrespondeaejercicio(qvto))
         {
            QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: en la "
                        "selección hay fechas que no corresponden a ajercicios definidos"));
            return;
         } */
      if (vencimientoprocesado(model->datagen(listanum.at(0),Qt::DisplayRole).toString()))
          {
            QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: en la "
                        "selección hay vencimientos que ya han sido procesados"));
            return;
          }

      for (int i = 1; i < listanum.size(); ++i)
           {
            if (qexterno!=model->datagen(listaexterno.at(i),Qt::DisplayRole).toString())
            {
              QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: los "
                          "externos no coinciden"));
              return;
            }
            /*if (qctatesoreria!=model->datagen(listacuenta_tesoreria.at(i),Qt::DisplayRole).toString())
               {
                 QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: las "
                             "cuentas de tesorería no coinciden"));
                 return;
               }*/
            /* if (qvto!=model->datagen(listafecha_vencimiento.at(i),Qt::DisplayRole).toDate())
               {
                 QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: las "
                             "fechas de vencimiento no coinciden"));
                 return;
               } */
            if (vencimientoprocesado(model->datagen(listanum.at(i),Qt::DisplayRole).toString()))
               {
                 QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: en la "
                        "selección hay vencimientos que ya han sido procesados"));
                 return;
               }
            /*if (!fechacorrespondeaejercicio(model->datagen(listafecha_vencimiento.at(i),
                                                                Qt::DisplayRole).toDate()))
               {
                QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: en la "
                        "selección hay fechas que no corresponden a ajercicios definidos"));
                return;
               } */
           }
        pide_info_unif p;
        p.asignaetiqueta(tr("FECHA COMÚN DE VENCIMIENTO:"));
        p.pasafecha(qvto);
        p.asigna_concepto(tr("Procesado de vencimientos"));
        p.pasa_externo(qexterno);
        p.pasa_cuenta_tesoreria(qctatesoreria);
        int resultado=p.exec();
        qvto=p.fecha();
        QString ejercicio=ejerciciodelafecha(qvto);
        QString concepto_resumen=p.recupera_concepto();
        QString documento=p.recupera_documento();
        bool borrador=p.borrador();

        if (resultado != QDialog::Accepted) return;

        // Si importe gasto>0; existe cuenta de gasto ?
        if (!p.importe_gasto().isEmpty()) {
          if (!existesubcuenta(p.cuenta_gasto())) {
             QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: la cuenta de gasto "
                                                            "suministrada no es correcta"));
             return;

          }
        }

        // Si externo no vacío; existe externo ?
        if (!p.externo().isEmpty()) {
            if (!basedatos::instancia()->existe_externo(p.externo())) {
                QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: el código de externo "
                                                               "suministrado no es correcto"));
                return;
            }
        }
        qexterno=p.externo();

        // Existe cuenta de banco y no está vacía ?
        if (p.cuenta_tesoreria().isEmpty() || !existesubcuenta(p.cuenta_tesoreria())) {
            QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: la cuenta de "
                                                           "tesorería no existe"));
            return;
        }
        qctatesoreria=p.cuenta_tesoreria();

        if (!fechacorrespondeaejercicio(qvto) || fechadejerciciocerrado(qvto))
         {
            QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: la fecha"
                                                            "suministrada para la selección no es correcta"));
            return;
         }

      if (basedatos::instancia()->fecha_periodo_bloqueado(qvto))
       {
        QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR: La fecha del vencimiento corresponde "
                                                              "a un periodo bloqueado"));
        return;
       }

      if (basedatos::instancia()->existe_bloqueo_cta(qctatesoreria,qvto))
      {
         QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR: La cuenta %1 "
                                        "está bloqueada").arg(qctatesoreria));
         return;
      }


      for (int i = 0; i < listacta_ordenante.size(); ++i)
           {
            if (basedatos::instancia()->existe_bloqueo_cta(
                    model->datagen(listacta_ordenante.at(i),Qt::DisplayRole).toString(),qvto))
              {
                 QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR: La cuenta %1 "
                                        "está bloqueada").arg(model->datagen(
                                                listacta_ordenante.at(i),Qt::DisplayRole).toString()));
               return;
              }
           }

      if (QMessageBox::question(
            this,
            tr("Vencimientos"),
            tr("Se van a procesar los vencimientos selecccionados.\n¿Desea continuar?")) == QMessageBox::No )
            return ;
  // generamos asiento y actualizamos vencimientos en una sola transacción

    QSqlDatabase::database().transaction();

    basedatos::instancia()->bloquea_para_listavenci();
    QString cadnumasiento;

    qlonglong vnum = basedatos::instancia()->proximoasiento(ejercicio);

    QString cadnum; cadnum.setNum(vnum+1);
    cadnumasiento.setNum(vnum);
    basedatos::instancia()->update_ejercicio_prox_asiento(ejercicio, cadnum);

    QString cadnumpase;
    qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
    // esto sería para cada pase del asiento; incrementar pase
    cadnumpase.setNum(pase);


      double saldo=0;
      for (int i = 0; i < listanum.size(); ++i)
           {
            QString debe, haber;
            QString cadimporte = model->datagen(listaimporte.at(i),Qt::DisplayRole).toString();
            if (model->datagen(listaderecho.at(i),Qt::DisplayRole).toBool())
               {
                debe = "0";
                haber = cadimporte;
                saldo += cadimporte.toDouble();
               }
            else
               {
                debe = cadimporte;
                haber = "0";
                saldo -= cadimporte.toDouble();
	        }
            QString concepto = "Vto. ";
            concepto += descripcioncuenta(model->datagen(listacta_ordenante.at(i),Qt::DisplayRole).toString());
            concepto += " de ";
            concepto += model->datagen(listafecha_operacion.at(i),Qt::DisplayRole).toDate().toString("dd.MM.yyyy");
            QString pase_diario_vencimiento=cadnumpase;
            basedatos::instancia()->insertDiario10( cadnumasiento, cadnumpase, qvto.toString("yyyy-MM-dd"),
                                                    model->datagen(listacta_ordenante.at(i),
                                                    Qt::DisplayRole).toString() , debe, haber,
                                                    concepto, documento, usuario, ejercicio, qexterno,borrador);

           //-------------------------------------------------------------------------------------------------

            if (model->datagen(listaderecho.at(i),Qt::DisplayRole).toBool()) {
                basedatos::instancia()->updateSaldossubcuentasaldocargocodigo( model->datagen(listacta_ordenante.at(i),Qt::DisplayRole).toString() , model->datagen(listaimporte.at(i),Qt::DisplayRole).toString() , false );
                // restar
            }
            else {
               basedatos::instancia()->updateSaldossubcuentasaldocargocodigo( model->datagen(listacta_ordenante.at(i),Qt::DisplayRole).toString() , model->datagen(listaimporte.at(i),Qt::DisplayRole).toString() , true );
                // sumar
            }
           //-------------------------------------------------------------------------------------------------
           //-------------------------------------------------------------------------------------------------
           basedatos::instancia()->haz_venci_procesado(model->datagen(listanum.at(i),Qt::DisplayRole).toString(),
                                                      pase_diario_vencimiento);
           basedatos::instancia()->updateVencimientos4num( qctatesoreria ,
                                                    qvto , false,
                                                    cadnumpase,"","",
                                                    model->datagen(listanum.at(i),Qt::DisplayRole).toString() );

           //---------------------------------------------------------------------------------------------
           pase++;
           cadnumpase.setNum(pase);
          }
      // queda pasar la cuenta de tesorería
      // tenemos en cuenta los gastos
      double gastos_op=0;
      if (!p.importe_gasto().isEmpty()) {
          gastos_op=convapunto(p.importe_gasto()).toDouble();
      }

      saldo-=gastos_op;

      QString debe, haber;
      if (saldo>0)
        {
            debe.setNum(saldo,'f',2);
            haber = "0";
        }
        else
        {
            debe = "0";
            haber.setNum(-saldo,'f',2);
        }

        basedatos::instancia()->insertDiario10( cadnumasiento, cadnumpase,
                                                qvto.toString("yyyy-MM-dd"), qctatesoreria,
                                                debe, haber, concepto_resumen, documento,
                                                usuario,ejercicio, qexterno, borrador );


   //---------------------------------------------------------------------------------------------------------
    if (saldo>0) {
        QString cnum; cnum.setNum(saldo,'f',2);
        basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(qctatesoreria, cnum, true);
        //sumar
    }
    else {
        if (saldo<0) saldo=-1*saldo;
        // restar
        QString cnum; cnum.setNum(saldo,'f',2);
        basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(qctatesoreria, cnum, false);
    }

   //--------------------------------------------------------------------------------------------------------
   // apunte del gasto
    pase++;
    cadnumpase.setNum(pase);
    if (gastos_op>0.001 || gastos_op<-0.001) {
        if (gastos_op>0)
          {
              debe.setNum(gastos_op,'f',2);
              haber = "0";
          }
          else
          {
              debe = "0";
              haber.setNum(-gastos_op,'f',2);
          }

          basedatos::instancia()->insertDiario10( cadnumasiento, cadnumpase,
                                                  qvto.toString("yyyy-MM-dd"), p.cuenta_gasto(),
                                                  debe, haber, concepto_resumen, documento,
                                                  usuario,ejercicio, qexterno, borrador );
          if (gastos_op>0) {
              QString cnum; cnum.setNum(gastos_op,'f',2);
              basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(p.cuenta_gasto(), cnum, true);
              //sumar
          }
          else {
              if (gastos_op<0) saldo=-1*gastos_op;
              // restar
              QString cnum; cnum.setNum(saldo,'f',2);
              basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(p.cuenta_gasto(), cnum, false);
          }

    }


   //-------------------------------------------------------------------------------------------------------
    pase++;
    cadnumpase.setNum(pase);
    basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);

    // fin de la transacción
    basedatos::instancia()->desbloquea_y_commit();

    // accept() ;
    refrescar();


    QMessageBox::information( this, tr("Vencimientos"),
                             tr("Se han procesado los vencimientos seleccionados en un asiento contable"));
      // QMessageBox::warning( this, tr("HOLA"),valores);
    }
    else
         {
          QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR en unificar procesados: no se ha "
                             "seleccionado ningún vencimiento"));
          return;
         }

}




void listavenci::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("vencimientos.tex");
   QFile fichero(adapta(qfichero));

    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";
    
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    if (ui.anotaciongroupBox->isChecked()) 
       {
         stream << tr("FECHA DE ANOTACIÓN: ");
         stream << ui.inicialdateEdit->date().toString("dd.MM.yyyy");
         stream << tr(" A ") << ui.finaldateEdit->date().toString("dd.MM.yyyy");
       }
   else
       {
           stream << tr("FECHA DE VENCIMIENTO:");
           stream << ui.venciinidateEdit->date().toString("dd.MM.yyyy");
           stream << tr(" A ") << ui.vencifindateEdit->date().toString("dd.MM.yyyy");
        }
    stream << "}}";
    stream << "\\end{center}";

    if (ui.domiciliablesradioButton->isChecked())
      {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Registros domiciliables") << "}}";
        stream << "\\end{center}";
      }

    if (ui.nodomiciliablesradioButton->isChecked())
      {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Registros no domiciliables") << "}}";
        stream << "\\end{center}";
      }

    if (ui.domiciliadosradioButton->isChecked())
      {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Registros domiciliados") << "}}";
        stream << "\\end{center}";
      }

    if (ui.nodomiciliadosradioButton->isChecked())
      {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Registros no domiciliados") << "}}";
        stream << "\\end{center}";
      }

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|r|c|c|r|c|c|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{6}{|c|} {\\textbf{";
   QString cadena=tr("VENCIMIENTOS ");
   if (ui.pendientescheckBox->isChecked()) cadena+=tr("PENDIENTES, ");
   if (ui.procesadoscheckBox->isChecked()) cadena+=tr("PROCESADOS, ");
   if (ui.cobroscheckBox->isChecked()) cadena+=tr("DE COBROS");
   if (ui.pagoscheckBox->isChecked()) cadena+=tr(", DE PAGOS");
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // ---------------------------------------------------------------------------------------------------
    stream << "{\\scriptsize{" << tr("Núm.Vto.") << "}} & ";
    stream << "{\\scriptsize{" << tr("Cuenta ordenante") << "}} & ";
    stream << "{\\scriptsize{" << tr("Fecha operación") << "}} & ";
    stream << "{\\scriptsize{" << tr("Importe operación") << "}} & ";
    stream << "{\\scriptsize{" << tr("Cuenta tesorería") << "}} & ";
    stream << "{\\scriptsize{" << tr("Fecha vencimiento") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // --------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "\\multicolumn{6}{|c|} {\\textbf{";
   cadena=tr("VENCIMIENTOS ");
   if (ui.pendientescheckBox->isChecked()) cadena+=tr("PENDIENTES, ");
   if (ui.procesadoscheckBox->isChecked()) cadena+=tr("PROCESADOS, ");
   if (ui.cobroscheckBox->isChecked()) cadena+=tr("DE COBROS");
   if (ui.pagoscheckBox->isChecked()) cadena+=tr(", DE PAGOS");
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    stream << "{\\scriptsize{" << tr("Núm.Vto.") << "}} & ";
    stream << "{\\scriptsize{" << tr("Cuenta ordenante") << "}} & ";
    stream << "{\\scriptsize{" << tr("Fecha operación") << "}} & ";
    stream << "{\\scriptsize{" << tr("Importe operación") << "}} & ";
    stream << "{\\scriptsize{" << tr("Cuenta tesorería") << "}} & ";
    stream << "{\\scriptsize{" << tr("Fecha vencimiento") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ----------------------------------------------------------------------------------------------------
    int pos=0; 
    double total=0;
    int filas=model->rowCount();
    while (pos<filas)
          {
             double importe=model->record(pos).value("importe").toDouble();
             if (!model->record(pos).value("derecho").toBool()) importe=-importe;
             total+=importe;
             stream << "{\\scriptsize " << model->record(pos).value("num").toString() << "} & {\\scriptsize ";

             stream << model->record(pos).value("cta_ordenante").toString() << "} & {\\scriptsize ";
             stream << model->record(pos).value("fecha_operacion").toDate().toString("dd-MM-yyyy")
                     << "} & {\\scriptsize ";
             stream << formateanumerosep(importe,comadecimal,decimales) << "} & {\\scriptsize ";
             stream << model->record(pos).value("cuenta_tesoreria").toString() << "} & {\\scriptsize ";
             stream << model->record(pos).value("fecha_vencimiento").toDate().toString("dd-MM-yyyy")
                 << "} \\\\ \n  " << "\\hline\n";
             pos++;
          }
    stream  << "\\multicolumn{3}{|r|}{" << tr("TOTAL VENCIMIENTOS:") << "} & {\\scriptsize " <<
             formateanumerosep(total,comadecimal,decimales) ;
    stream << "} &  & \\\\ \n"  << "\\hline\n";

    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}


void listavenci::imprimir()
{
    QString fichero=tr("vencimientos");

    generalatex();

   int valor=imprimelatex2(fichero);
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir vencimientos"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir vencimientos"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir vencimientos"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void listavenci::generalatex2()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("vencimientos2.tex");
   QFile fichero(adapta(qfichero));

    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{landscape}" << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    if (ui.anotaciongroupBox->isChecked())
       {
         stream << tr("FECHA DE ANOTACIÓN: ");
         stream << ui.inicialdateEdit->date().toString("dd.MM.yyyy");
         stream << tr(" A ") << ui.finaldateEdit->date().toString("dd.MM.yyyy");
       }
   else
       {
           stream << tr("FECHA DE VENCIMIENTO:");
           stream << ui.venciinidateEdit->date().toString("dd.MM.yyyy");
           stream << tr(" A ") << ui.vencifindateEdit->date().toString("dd.MM.yyyy");
        }
    stream << "}}";
    stream << "\\end{center}";

    if (ui.domiciliablesradioButton->isChecked())
      {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Registros domiciliables") << "}}";
        stream << "\\end{center}";
      }

    if (ui.nodomiciliablesradioButton->isChecked())
      {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Registros no domiciliables") << "}}";
        stream << "\\end{center}";
      }

    if (ui.domiciliadosradioButton->isChecked())
      {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Registros domiciliados") << "}}";
        stream << "\\end{center}";
      }

    if (ui.nodomiciliadosradioButton->isChecked())
      {
        stream << "\\begin{center}" << "\n";
        stream << "{\\textbf {";
        stream << tr("Registros no domiciliados") << "}}";
        stream << "\\end{center}";
      }

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|r|p{2cm}|c|p{6cm}|c|r|c|c|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{8}{|c|} {\\textbf{";
   QString cadena=tr("VENCIMIENTOS ");
   if (ui.pendientescheckBox->isChecked()) cadena+=tr("PENDIENTES, ");
   if (ui.procesadoscheckBox->isChecked()) cadena+=tr("PROCESADOS, ");
   if (ui.cobroscheckBox->isChecked()) cadena+=tr("DE COBROS");
   if (ui.pagoscheckBox->isChecked()) cadena+=tr(", DE PAGOS");
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    // ---------------------------------------------------------------------------------------------------
    stream << "{\\scriptsize{" << tr("Núm.Vto.") << "}} & ";
    stream << "{\\scriptsize{" << tr("Documento") << "}} & "; // ************************
    stream << "{\\scriptsize{" << tr("Cuenta") << "}} & ";
    stream << "{\\scriptsize{" << tr("Nombre") << "}} & ";
    stream << "{\\scriptsize{" << tr("Fecha operación") << "}} & ";
    stream << "{\\scriptsize{" << tr("Importe operación") << "}} & ";
    stream << "{\\scriptsize{" << tr("Cuenta tesorería") << "}} & ";
    stream << "{\\scriptsize{" << tr("Fecha vencimiento") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // --------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "\\multicolumn{8}{|c|} {\\textbf{";
   cadena=tr("VENCIMIENTOS ");
   if (ui.pendientescheckBox->isChecked()) cadena+=tr("PENDIENTES, ");
   if (ui.procesadoscheckBox->isChecked()) cadena+=tr("PROCESADOS, ");
   if (ui.cobroscheckBox->isChecked()) cadena+=tr("DE COBROS");
   if (ui.pagoscheckBox->isChecked()) cadena+=tr(", DE PAGOS");
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
    stream << "{\\scriptsize{" << tr("Núm.Vto.") << "}} & ";
    stream << "{\\scriptsize{" << tr("Documento") << "}} & "; // ************************
    stream << "{\\scriptsize{" << tr("Cuenta") << "}} & ";
    stream << "{\\scriptsize{" << tr("Nombre") << "}} & ";
    stream << "{\\scriptsize{" << tr("Fecha operación") << "}} & ";
    stream << "{\\scriptsize{" << tr("Importe operación") << "}} & ";
    stream << "{\\scriptsize{" << tr("Cuenta tesorería") << "}} & ";
    stream << "{\\scriptsize{" << tr("Fecha vencimiento") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ----------------------------------------------------------------------------------------------------
    int pos=0;
    double total=0;
    int filas=model->rowCount();
    while (pos<filas)
          {
             double importe=model->record(pos).value("importe").toDouble();
             if (!model->record(pos).value("derecho").toBool()) importe=-importe;
             total+=importe;
             stream << "{\\scriptsize " << model->record(pos).value("num").toString() << "} & {\\scriptsize ";

             stream << filtracad(basedatos::instancia()->documento_pase(
                     model->record(pos).value("pase_diario_operacion").toString())) << "} & {\\scriptsize ";

             QString externo=basedatos::instancia()->externo_pase(model->record(pos).value("pase_diario_operacion").toString());

             if (externo.isEmpty()) {
                 stream << model->record(pos).value("cta_ordenante").toString() << "} & {\\scriptsize ";

                 stream << filtracad(descripcioncuenta(model->record(pos).value("cta_ordenante").toString()))
                         << "} & {\\scriptsize ";

             }
             else {
                 stream << filtracad(externo) << "} & {\\scriptsize ";

                 stream << filtracad(basedatos::instancia()->razon_externo(externo))
                         << "} & {\\scriptsize ";

             }

             stream << model->record(pos).value("fecha_operacion").toDate().toString("dd-MM-yyyy")
                     << "} & {\\scriptsize ";
             stream << formateanumerosep(importe,comadecimal,decimales) << "} & {\\scriptsize ";
             stream << model->record(pos).value("cuenta_tesoreria").toString() << "} & {\\scriptsize ";
             stream << model->record(pos).value("fecha_vencimiento").toDate().toString("dd-MM-yyyy")
                 << "} \\\\ \n  " << "\\hline\n";
             pos++;
          }
    stream  << "\\multicolumn{5}{|r|}{" << tr("TOTAL VENCIMIENTOS:") << "} & {\\scriptsize " <<
             formateanumerosep(total,comadecimal,decimales) ;
    stream << "} &  & \\\\ \n"  << "\\hline\n";

    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "\\end{landscape}\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}

void listavenci::copia_informe2() {
    QString cadcopy;
    cadcopy=filtracad(nombreempresa()) + "\n" ;
    if (ui.anotaciongroupBox->isChecked())
       {
         cadcopy+= tr("FECHA DE ANOTACIÓN: ");
         cadcopy+= ui.inicialdateEdit->date().toString("dd.MM.yyyy");
         cadcopy+= tr(" A ") + ui.finaldateEdit->date().toString("dd.MM.yyyy") +"\n";
       }
   else
       {
           cadcopy+=tr("FECHA DE VENCIMIENTO:");
           cadcopy+= ui.venciinidateEdit->date().toString("dd.MM.yyyy");
           cadcopy+= tr(" A ") + ui.vencifindateEdit->date().toString("dd.MM.yyyy")+"\n";
        }

   cadcopy+=tr("VENCIMIENTOS ");
   if (ui.pendientescheckBox->isChecked()) cadcopy+=tr("PENDIENTES, ");
   if (ui.procesadoscheckBox->isChecked()) cadcopy+=tr("PROCESADOS, ");
   if (ui.cobroscheckBox->isChecked()) cadcopy+=tr("DE COBROS");
   if (ui.pagoscheckBox->isChecked()) cadcopy+=tr(", DE PAGOS");
   cadcopy += "\n";
    // ---------------------------------------------------------------------------------------------------
    cadcopy +=  tr("Núm.Vto.") + "\t";
    cadcopy +=  tr("Documento") + "\t"; // ************************
    cadcopy +=  tr("Cuenta") + "\t";
    cadcopy +=  tr("Nombre") + "\t";
    cadcopy +=  tr("Fecha operación") + "\t";
    cadcopy +=  tr("Importe operación") + "\t";
    cadcopy +=  tr("Cuenta tesorería") + "\t";
    cadcopy +=  tr("Fecha vencimiento") + "\t";
    cadcopy +=  tr("Fecha realización");
    cadcopy +=  "\n";
    // --------------------------------------------------------------------------------------------------
    int pos=0;
    double total=0;
    int filas=model->rowCount();
    while (pos<filas)
          {
             double importe=model->record(pos).value("importe").toDouble();
             if (!model->record(pos).value("derecho").toBool()) importe=-importe;
             total+=importe;
             cadcopy+= model->record(pos).value("num").toString() + "\t";

             cadcopy+= filtracad(basedatos::instancia()->documento_pase(
                     model->record(pos).value("pase_diario_operacion").toString())) + "\t";

             QString externo=basedatos::instancia()->externo_pase(model->record(pos).value("pase_diario_operacion").toString());

             if (externo.isEmpty()) {
                 cadcopy +=  model->record(pos).value("cta_ordenante").toString() + "\t";

                 cadcopy += filtracad(descripcioncuenta(model->record(pos).value("cta_ordenante").toString()))
                         + "\t";

             }
             else {
                 cadcopy += filtracad(externo) + "\t";

                 cadcopy += filtracad(basedatos::instancia()->razon_externo(externo))
                         + "\t";

             }

             cadcopy += model->record(pos).value("fecha_operacion").toDate().toString("dd-MM-yyyy")
                     + "\t";
             cadcopy += formateanumerosep(importe,comadecimal,decimales) + "\t";
             cadcopy += model->record(pos).value("cuenta_tesoreria").toString() + "\t";
             cadcopy += model->record(pos).value("fecha_vencimiento").toDate().toString("dd-MM-yyyy") + "\t";
             cadcopy += model->record(pos).value("fecha_realizacion").toDate().toString("dd-MM-yyyy")
                 + "\n";
             pos++;
          }
    cadcopy += "\t\t\t\t" + tr("TOTAL:") + "\t" +
             formateanumerosep(total,comadecimal,decimales) ;
    cadcopy + "\n";
    QClipboard *cb = QApplication::clipboard();
    cb->setText(cadcopy);
    QMessageBox::information( this, tr("Vencimientos"),
                              tr("Se ha pasado el contenido al portapapeles") );

}


void listavenci::imprimir2()
{
    QString fichero=tr("vencimientos2");

    generalatex2();

   int valor=imprimelatex2(fichero);
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir vencimientos"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir vencimientos"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir vencimientos"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void listavenci::consulta_asiento()
{
    int fila=ui.latablavencipase->currentIndex().row();
    QString asiento=basedatos::instancia()->selectAsientodiariopase(
            model->record(fila).value("pase_diario_operacion").toString());
    QString ejercicio;
    ejercicio=ejerciciodelafecha(basedatos::instancia()->select_fecha_diariopase(
            model->record(fila).value("pase_diario_operacion").toString()));
    consultarasiento(asiento,usuario,ejercicio);
    // refrescar();
}

void listavenci::edita_asiento()
{
    int fila=ui.latablavencipase->currentIndex().row();
    QString asiento=basedatos::instancia()->selectAsientodiariopase(
            model->record(fila).value("pase_diario_operacion").toString());
    QString ejercicio;
    ejercicio=ejerciciodelafecha(basedatos::instancia()->select_fecha_diariopase(
            model->record(fila).value("pase_diario_operacion").toString()));
    editarasiento(asiento,usuario,ejercicio);
    refrescar();
}

void listavenci::consulta_asiento_pago()
{
    int fila=ui.latablavencipase->currentIndex().row();
    if (model->record(fila).value("pase_diario_vencimiento").toInt()==0) return;
    QString asiento=basedatos::instancia()->selectAsientodiariopase(
            model->record(fila).value("pase_diario_vencimiento").toString());
    QString ejercicio;
    ejercicio=ejerciciodelafecha(basedatos::instancia()->select_fecha_diariopase(
            model->record(fila).value("pase_diario_vencimiento").toString()));
    consultarasiento(asiento,usuario,ejercicio);
}


void listavenci::edita_asiento_pago()
{
    int fila=ui.latablavencipase->currentIndex().row();
    if (model->record(fila).value("pase_diario_vencimiento").toInt()==0) return;
    QString asiento=basedatos::instancia()->selectAsientodiariopase(
            model->record(fila).value("pase_diario_vencimiento").toString());
    QString ejercicio;
    ejercicio=ejerciciodelafecha(basedatos::instancia()->select_fecha_diariopase(
            model->record(fila).value("pase_diario_vencimiento").toString()));
    editarasiento(asiento,usuario,ejercicio);
    refrescar();
}


void listavenci::editavenci()
{
    int fila=ui.latablavencipase->currentIndex().row();

    if (!model->record(fila).value("pendiente").toBool()) return;

    QSqlQuery q = basedatos::instancia()->selec_diario_vr(
            model->record(fila).value("pase_diario_operacion").toString());

    // "select asiento, debe, haber, cuenta, fecha from diario where pase=";
   if (q.isActive())
       if (q.next())
       {
        consultavencipase *losvenci=new consultavencipase(
                          model->record(fila).value("pase_diario_operacion").toString(),
                          q.value(0).toString(),
                          q.value(1).toString(),
                          q.value(2).toString(),
                          q.value(3).toString(),
                          q.value(4).toDate());
        losvenci->exec();
        delete(losvenci);
        refrescar();
       }
}


void listavenci::copiar()
{
   QString cadena;
   cadena+= filtracad(nombreempresa());
   cadena+= "\n";
   cadena+= "\n";
    if (ui.anotaciongroupBox->isChecked())
       {
         cadena+=  tr("FECHA DE ANOTACIÓN: ");
         cadena+="\t";
         cadena+= ui.inicialdateEdit->date().toString("dd/MM/yyyy");
         cadena+="\t";
         cadena+= tr("A");
         cadena+="\t";
         cadena+=ui.finaldateEdit->date().toString("dd/MM/yyyy");
         cadena+= "\n";
       }
   else
       {
         cadena+=tr("FECHA DE VENCIMIENTO:");
         cadena+="\t";
         cadena+=ui.venciinidateEdit->date().toString("dd/MM/yyyy");
         cadena+="\t";
         cadena+=tr("A");
         cadena+="\t";
         cadena+=ui.vencifindateEdit->date().toString("dd/MM/yyyy");
         cadena+= "\n";
        }
    cadena += "\n";
   QString cad=tr("VENCIMIENTOS ");
   if (ui.pendientescheckBox->isChecked()) cad+=tr("PENDIENTES, ");
   if (ui.procesadoscheckBox->isChecked()) cad+=tr("PROCESADOS, ");
   if (ui.cobroscheckBox->isChecked()) cad+=tr("DE COBROS");
   if (ui.pagoscheckBox->isChecked()) cad+=tr(", DE PAGOS");
   cadena+=cad;
   cadena+="\n";
    // ---------------------------------------------------------------------------------------------------
    cadena+= tr("Núm.Vto.") + "\t";
    cadena+=tr("Documento")  + "\t";
    cadena+=tr("Cuenta")  + "\t";
    cadena+=tr("Nombre")  + "\t";
    cadena+=tr("Fecha operación") + "\t";
    cadena+=tr("Importe operación") + "\t";
    cadena+=tr("Cuenta tesorería") + "\t";
    cadena+=tr("Fecha vencimiento") + "\n";
    // --------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------
    int pos=0;
    double total=0;
    int filas=model->rowCount();
    while (pos<filas)
          {
             double importe=model->record(pos).value("importe").toDouble();
             if (!model->record(pos).value("derecho").toBool()) importe=-importe;
             total+=importe;
             cadena+=model->record(pos).value("num").toString() + "\t";

             cadena+=basedatos::instancia()->documento_pase(
                     model->record(pos).value("pase_diario_operacion").toString());
             cadena+="\t";

             cadena+=model->record(pos).value("cta_ordenante").toString();
             cadena+="\t";

             cadena+=filtracad(descripcioncuenta(model->record(pos).value("cta_ordenante").toString()));
             cadena+="\t";

             cadena+=model->record(pos).value("fecha_operacion").toDate().toString("dd-MM-yyyy");
             cadena+="\t";

             cadena+=formateanumero(importe,comadecimal,decimales);
             cadena+="\t";

             cadena+=model->record(pos).value("cuenta_tesoreria").toString();
             cadena+="\t";

             cadena+=model->record(pos).value("fecha_vencimiento").toDate().toString("dd-MM-yyyy");
             cadena+="\t";

             cadena+="\n";
             pos++;
          }
    cadena += tr("TOTAL VENCIMIENTOS:") + "\t" + formateanumero(total,comadecimal,decimales);
    cadena += "\n";

    QClipboard *cb = QApplication::clipboard();

    cb->setText(cadena);
    QMessageBox::information( this, tr("Consulta de vencimientos"),
                              tr("Se ha pasado el contenido al portapapeles") );

}


void listavenci::esconde_domiciliables()
{
  ui.domgroupBox->hide();
  ui.dom2groupBox->hide();
}

void listavenci::importe_desde_cambiado()
{
 QString contenido=ui.importe_desdelineEdit->text();
 if (comadecimal && contenido.contains('.'))
    {
     contenido=contenido.replace('.',',');
     ui.importe_desdelineEdit->setText(contenido);
    }
 if (!comadecimal && contenido.contains(','))
    {
     contenido=contenido.replace(',','.');
     ui.importe_desdelineEdit->setText(contenido);
    }

 ui.importe_hastalineEdit->setText(ui.importe_desdelineEdit->text());
}


void listavenci::importe_hasta_cambiado()
{
 QString contenido=ui.importe_hastalineEdit->text();
 if (comadecimal && contenido.contains('.'))
    {
     contenido=contenido.replace('.',',');
     ui.importe_hastalineEdit->setText(contenido);
    }
 if (!comadecimal && contenido.contains(','))
    {
     contenido=contenido.replace(',','.');
     ui.importe_hastalineEdit->setText(contenido);
    }

}


void listavenci::checkpendientes_procesados_cambiado()
{
    if (!ui.pendientescheckBox->isChecked() && ui.procesadoscheckBox->isChecked())
      {
        ui.vencigroupBox->setTitle(tr("Fecha realización"));
      }
      else
          ui.vencigroupBox->setTitle(tr("Fecha vencimiento"));

}


void listavenci::busca_ext() {
    busca_externo *b = new busca_externo();
    int resultado=b->exec();
    if (resultado==QDialog::Accepted)
       {
        ui.externo_lineEdit->setText(b->codigo_elec());
        //ui.descrip_externo_lineEdit->setText(b->nombre_elec());

       }
    delete(b);
}

void listavenci::externo_cambiado() {
    ui.externo_descrip_lineEdit->setText(basedatos::instancia()->razon_externo(ui.externo_lineEdit->text()));
}
