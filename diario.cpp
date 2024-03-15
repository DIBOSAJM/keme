/* ----------------------------------------------------------------------------------
    KEME-Contabilidad ; aplicación para llevar contabilidades

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

#include "diario.h" 
#include "funciones.h"
#include "basedatos.h"
// #include "editarasiento.h"
#include <QSqlField>
#include <QMessageBox>
#include "privilegios.h"
#include "editafechaasiento.h"
#include "edit_conc_doc.h"
#include "cambiacuentapase.h"
#include "ivarepercutido.h"
#include "ivasoportado.h"
#include "exento.h"
#include "eib.h"
#include "retencion.h"

CustomSqlModel::CustomSqlModel(QObject *parent)
        : QSqlQueryModel(parent)
    {
     // primerasiento1=numeracionrelativa();
     comadecimal=true; sindecimales=false;
     tabla_ci=basedatos::instancia()->analitica_tabla();

    }

void CustomSqlModel::pasainfo(bool qcomadecimal, bool qsindecimales)
{
 comadecimal=qcomadecimal;
 sindecimales=qsindecimales;
}

/* void CustomSqlModel::chknumeracion()
{
  primerasiento1=numeracionrelativa();
}
*/

// qlonglong primas=0;

/* void asignaprimas(qlonglong qprimas)
{
 primas=qprimas;
}
*/

QVariant CustomSqlModel::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant CustomSqlModel::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == 2)
               {
                // número de asiento
                /* if (primerasiento1)
                    {
                      if (primerasiento1) asignaprimas(primasejerciciodelafecha(
                              record(index.row()).value("fecha").toDate()));
                      QString cad2;
                      cad2.setNum(value.toLongLong()-primas+1);
                      return cad2;
                    } */
               }
            if (index.column() == 1)
               {
                // if (primerasiento1) asignaprimas(primasejerciciodelafecha(value.toDate()));
                return value.toDate().toString("dd-MM-yyyy");
               }
            if (index.column() == 4 || index.column()==5)
	       {
   	        if (value.toDouble()<0.0001 && value.toDouble()>-0.0001)
                   return value.fromValue(vacio);
                else
                      {
                        if (comadecimal)
                           {
                            if (!sindecimales)
                                return formateanumerosep(value.toDouble(),comadecimal,true);
                                //return value.fromValue(puntuanumerocomadec(value.toString()));
                            else
                                {
                                 // QString cad=puntuanumerocomadec(value.toString());
                                 // cad.truncate(cad.length()-3);
                                 // return value.fromValue(cad);
                                 return formateanumerosep(value.toDouble(),comadecimal,false);
                                }
                           }
                         else
                           {
                            if (!sindecimales)
                                return formateanumerosep(value.toDouble(),comadecimal,true);
                                // return value.fromValue(puntuanumeropuntodec(value.toString()));
                            else
                                {
                                 // QString cad=puntuanumeropuntodec(value.toString());
                                 // cad.truncate(cad.length()-3);
                                 // return value.fromValue(cad);
                                 return formateanumerosep(value.toDouble(),comadecimal,false);
                                }
                           }
                       }
	       }
            if (index.column() == 11)
               {
                if (tabla_ci)
                   {
                    if (value.toLongLong()>0)
                        return QString("*");
                     else return QString();
                   }
               }

            if (index.column() == 15)
               {
                    if (value.toBool())
                        return QString("*");
                     else return QString();
               }

            if (index.column() == 14)
               {
                     if (!(value.toLongLong()>0))
                        return QString();
                    // else return value.toString();
               }
        }
        if (role == Qt::TextAlignmentRole && 
             (index.column() == 4 || index.column()==5 ||
              index.column() == 2 || index.column()==9 || index.column()==14))
               return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        if (role == Qt::TextAlignmentRole && index.column()==1)
            return QVariant::fromValue(int(Qt::AlignCenter));

        if (role == Qt::ForegroundRole)
           {
            if (record(index.row()).value("revisado").toBool())
                     return QVariant::fromValue(QBrush(Qt::darkGreen ));
           // else return qVariantFromValue(QBrush(Qt::darkRed));
           }
        return value;
    }



diario::diario() : QWidget() {
  ui.setupUi(this);

// esta porción de código es para asegurarnos de que la ventana
// sale por el centro del escritorio
  /*
QDesktopWidget *desktop = QApplication::desktop();

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
model=NULL;
  ui.latabladiario->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui.latabladiario->setSelectionMode(QAbstractItemView::ContiguousSelection);
  ui.fechadateEdit->setDate(QDate::currentDate());
  comadecimal=true; sindecimales=false;
                        //objeto del que sale la señal
  connect(ui.latabladiario,SIGNAL(clicked(QModelIndex)),this,SLOT(infocuentapase(QModelIndex)));
  // connect(ui.latabladiario,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editarasientod()));
  connect(ui.iniciopushButton,SIGNAL(clicked()),SLOT(inicio()));
  connect(ui.finpushButton,SIGNAL(clicked()),SLOT(fin()));
  connect(ui.fecha_pushButton,SIGNAL(clicked(bool)),SLOT(editafechaasien()));
  connect(ui.concepto_pushButton,SIGNAL(clicked(bool)),SLOT(edcondoc()));
  connect(ui.cuenta_pushButton,SIGNAL(clicked(bool)),SLOT(cambiacuenpase()));
  connect(ui.iva_pushButton,SIGNAL(clicked(bool)),SLOT(ed_registro_iva()));
  connect(ui.ret_pushButton,SIGNAL(clicked(bool)),SLOT(ed_registro_ret()));
  // ui.fotolabel->setFixedHeight(60);
  if (!privilegios[edi_asiento]) {
      ui.fecha_pushButton->setEnabled(false);
      ui.concepto_pushButton->setEnabled(false);
      ui.cuenta_pushButton->setEnabled(false);
      ui.iva_pushButton->setEnabled(false);
      ui.ret_pushButton->setEnabled(false);
  } else {
      ui.fecha_pushButton->setEnabled(true);
      ui.concepto_pushButton->setEnabled(true);
      ui.cuenta_pushButton->setEnabled(true);
      ui.iva_pushButton->setEnabled(true);
      ui.ret_pushButton->setEnabled(true);

  }

}

void diario::activaconfiltro(QString filtro,bool qcomadecimal, bool qsindecimales, QString qusuario)
{
        CustomSqlModel *elmodelo = new CustomSqlModel;
        comadecimal=qcomadecimal;
        sindecimales=qsindecimales;
        guardafiltro=filtro;
        model = elmodelo;
        model->pasainfo(qcomadecimal, qsindecimales);

        model->setQuery( basedatos::instancia()->select11Diariofiltro(filtro) );
        model->setHeaderData(0, Qt::Horizontal, tr("cuenta"));
        model->setHeaderData(1, Qt::Horizontal, tr("fecha"));
        model->setHeaderData(2, Qt::Horizontal, tr("asiento"));
        model->setHeaderData(3, Qt::Horizontal, tr("concepto"));
        model->setHeaderData(4, Qt::Horizontal, tr("debe"));
        model->setHeaderData(5, Qt::Horizontal, tr("haber"));
        model->setHeaderData(6, Qt::Horizontal, tr("documento"));
        model->setHeaderData(7, Qt::Horizontal, tr("diario"));
        model->setHeaderData(8, Qt::Horizontal, tr("usuario"));
        model->setHeaderData(9, Qt::Horizontal, tr("apunte"));
        model->setHeaderData(10, Qt::Horizontal, tr("ci"));
        model->setHeaderData(11, Qt::Horizontal, tr("ci"));
        model->setHeaderData(12, Qt::Horizontal, tr("fichero doc"));
        model->setHeaderData(13, Qt::Horizontal, tr("ejercicio"));
        model->setHeaderData(14, Qt::Horizontal, tr("recepción"));
        model->setHeaderData(15, Qt::Horizontal, tr("revisado"));

        ui.latabladiario->setModel(model);
        if ((!conanalitica() && !conanalitica_parc())
            || basedatos::instancia()->analitica_tabla()) ui.latabladiario->setColumnHidden(10,true);
           else ui.latabladiario->setColumnHidden(10,false);
        if (!basedatos::instancia()->analitica_tabla()) ui.latabladiario->setColumnHidden(11,true);
          else ui.latabladiario->setColumnHidden(11,false);

        if (!basedatos::instancia()->gestiondeusuarios()) ui.latabladiario->setColumnHidden(8,true);
          else ui.latabladiario->setColumnHidden(8,false);

        ui.latabladiario->setAlternatingRowColors ( true);
        // infocuentapase();
  usuario=qusuario;
  // ui.latabladiario->setSortingEnabled(true);
  // ui.latabladiario->sortByColumn(1,Qt::AscendingOrder);

}

void diario::cabecera_campo_orden(int campo)
{
    // cuenta, fecha, asiento, concepto, debe,
    // haber, documento, diario, usuario, pase,ci,
    // clave_ci, copia_doc, ejercicio
    if (model==NULL) return;
    model->setHeaderData(0, Qt::Horizontal, tr("cuenta"));
    model->setHeaderData(1, Qt::Horizontal, tr("fecha"));
    model->setHeaderData(2, Qt::Horizontal, tr("asiento"));
    model->setHeaderData(3, Qt::Horizontal, tr("concepto"));
    model->setHeaderData(4, Qt::Horizontal, tr("debe"));
    model->setHeaderData(5, Qt::Horizontal, tr("haber"));
    model->setHeaderData(6, Qt::Horizontal, tr("documento"));
    model->setHeaderData(7, Qt::Horizontal, tr("diario"));
    model->setHeaderData(8, Qt::Horizontal, tr("usuario"));
    model->setHeaderData(9, Qt::Horizontal, tr("apunte"));
    model->setHeaderData(10, Qt::Horizontal, tr("ci"));
    model->setHeaderData(11, Qt::Horizontal, tr("ci"));
    model->setHeaderData(12, Qt::Horizontal, tr("fichero doc"));
    model->setHeaderData(13, Qt::Horizontal, tr("ejercicio"));
    model->setHeaderData(14, Qt::Horizontal, tr("recepción"));
    model->setHeaderData(15, Qt::Horizontal, tr("revisado"));

    switch (campo)
    {
      case 0: model->setHeaderData(0, Qt::Horizontal, tr("cuenta*"));
          break;
      case 1: model->setHeaderData(1, Qt::Horizontal, tr("fecha*"));
          break;
     case 2: model->setHeaderData(2, Qt::Horizontal, tr("asiento*"));;
         break;
     case 3: model->setHeaderData(3, Qt::Horizontal, tr("concepto*"));
         break;
     case 4: model->setHeaderData(4, Qt::Horizontal, tr("debe*"));
         break;
     case 5: model->setHeaderData(5, Qt::Horizontal, tr("haber*"));
         break;
     case 6: model->setHeaderData(6, Qt::Horizontal, tr("documento*"));
         break;
     case 7: model->setHeaderData(7, Qt::Horizontal, tr("diario*"));
         break;
     case 8: model->setHeaderData(8, Qt::Horizontal, tr("usuario*"));
         break;
     case 9: model->setHeaderData(9, Qt::Horizontal, tr("apunte*"));
         break;
     case 10: model->setHeaderData(10, Qt::Horizontal, tr("ci*"));
         break;
     case 11: model->setHeaderData(11, Qt::Horizontal, tr("ci*"));
         break;
     case 12: model->setHeaderData(12, Qt::Horizontal, tr("fichero doc*"));
         break;
     case 13: model->setHeaderData(13, Qt::Horizontal, tr("ejercicio*"));
         break;
     case 14: model->setHeaderData(14, Qt::Horizontal, tr("recepcion*"));
        break;
    case 15:  model->setHeaderData(15, Qt::Horizontal, tr("revisado*"));

    }

}

/*void diario::pasafiltro(QString filtro,bool qcomadecimal,bool qsindecimales)
{
 model->pasainfo(qcomadecimal,qsindecimales);
 sindecimales=qsindecimales;
 guardafiltro=filtro;
 model->setQuery( basedatos::instancia()->select11Diariofiltro(filtro) );

}*/

bool diario::pasafiltro(QString filtro,bool qcomadecimal,bool qsindecimales)
{
 model->pasainfo(qcomadecimal,qsindecimales);
 sindecimales=qsindecimales;
 guardafiltro=filtro;
 bool correcto;
 QSqlQuery q=basedatos::instancia()->selectDiariofiltro_nomsj_error (filtro, &correcto);
 if (correcto)
    model->setQuery( q );
   else return false;
 return true;
}


void diario::refresca()
{
 if (!conanalitica()) ui.latabladiario->setColumnHidden(10,true);
     else ui.latabladiario->setColumnHidden(10,false);
 if (!basedatos::instancia()->analitica_tabla()) ui.latabladiario->setColumnHidden(11,true);
     else ui.latabladiario->setColumnHidden(11,false);
 if (!basedatos::instancia()->gestiondeusuarios()) ui.latabladiario->setColumnHidden(8,true);
    else ui.latabladiario->setColumnHidden(8,false);
 //model->chknumeracion();
 QSqlQuery p=model->query(); 
 p.exec();
 model->clear();
 model->setQuery(p);
}

void diario::irfinal()
{
 // QModelIndex indice=ui.latabladiario->currentIndex ();
 ui.latabladiario->setCurrentIndex(modelo()->index(modelo()->rowCount()-1,0));
 ui.latabladiario->scrollToBottom();
 ui.latabladiario->setFocus();
}


void diario::irafila(int fila)
{
 if (fila>=modelo()->rowCount()) irfinal();
   else
       ui.latabladiario->selectRow(fila);
}

int diario::fila_actual()
{
    if (!ui.latabladiario->currentIndex().isValid()) return 0;
    QModelIndex indice=ui.latabladiario->currentIndex ();
    return indice.row();
}

int diario::anchocolumna(int col)
{
 return ui.latabladiario->columnWidth(col);
}

void diario::pasaanchocolumna(int columna,int ancho)
{
  ui.latabladiario->setColumnWidth(columna,ancho);
}

void diario::pasaanchos(int ancho[])
{
  for (int veces=0;veces<15;veces++) pasaanchocolumna(veces,ancho[veces]);
}

void diario::muestratabla(void)
{

    if ((!conanalitica() && !conanalitica_parc())
        || basedatos::instancia()->analitica_tabla()) ui.latabladiario->setColumnHidden(10,true);
       else ui.latabladiario->setColumnHidden(10,false);
    if (!basedatos::instancia()->analitica_tabla()) ui.latabladiario->setColumnHidden(11,true);
      else ui.latabladiario->setColumnHidden(11,false);
    if (!basedatos::instancia()->gestiondeusuarios()) ui.latabladiario->setColumnHidden(8,true);
      else ui.latabladiario->setColumnHidden(8,false);
 ui.latabladiario->show();
}


QModelIndex diario::indiceactual(void)
{
 return ui.latabladiario->currentIndex();
}

void diario::situate(QModelIndex indice)
{
 ui.latabladiario->setCurrentIndex (indice);
}

void diario::pasafiltroedlin(QString filtro)
{
  ui.filtrolineEdit->setText(filtro);
}

void diario::infocuentapase(QModelIndex primaryKeyIndex)
{
   int fila=primaryKeyIndex.row();
   QString cad = model->data(model->index(fila,0),Qt::DisplayRole).toString();
   ui.codigolineEdit->setText(cad);
   ui.descriplineEdit->setText(descripcioncuenta(cad));

   QString ejercicio=ejerciciodelafecha(model->datagen(model->index(fila,1),Qt::DisplayRole).toDate());
   double saldo=saldocuentaendiarioejercicio(cad,ejercicio);
   QString cadnum;
   cadnum.setNum(saldo,'f',2);
   cadnum.remove('-');

   if (comadecimal) cadnum=puntuanumerocomadec(cadnum);
       else cadnum=puntuanumeropuntodec(cadnum);

   if (saldo<0.001) { ui.haberlineEdit->setText(cadnum); ui.debelineEdit->setText(""); }
   if (saldo>0.001) { ui.debelineEdit->setText(cadnum); ui.haberlineEdit->setText(""); }

}

QString diario::subcuentaactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return "";
 int fila=ui.latabladiario->currentIndex().row();
 return  model->record(fila).value("cuenta").toString();;
}

QString diario::externoactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return "";
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("externo").toString();
}


qlonglong diario::asientoactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return 0;
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("asiento").toLongLong();
 // return model->record(fila).field(2).defaultValue().toLongLong();
 // return model->data(model->index(fila,2),Qt::DisplayRole).toLongLong();
}

bool diario::apunterevisado()
{
  if (!ui.latabladiario->currentIndex().isValid()) return false;
  int fila=ui.latabladiario->currentIndex().row();
  return model->record(fila).value("revisado").toBool();
}

QString diario::ciactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return "";
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("ci").toString();
}


QDate diario::fechapaseactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return QDate::currentDate();
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("fecha").toDate();
}

qlonglong diario::paseactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return 0;
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("pase").toLongLong();
}

double diario::debe()
{
 if (!ui.latabladiario->currentIndex().isValid()) return 0;
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("debe").toDouble();
}

double diario::haber()
{
 if (!ui.latabladiario->currentIndex().isValid()) return 0;
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("haber").toDouble();
}

CustomSqlModel *diario::modelo()
{
  return model;
}


QTableView *diario::tabladiario()
{
 return ui.latabladiario;
}


QString diario::conceptoactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return "";
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("concepto").toString();
}

QString diario::codigo_var_evpn_pymes()
{
 if (!ui.latabladiario->currentIndex().isValid()) return "";
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("codigo_var_evpn_pymes").toString();
}

QString diario::documentoactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return "";
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("documento").toString();
}

diario::~diario()
{
 delete model;
}



void diario::editarasientod()
{
  /* QString privileg=basedatos::instancia()->privilegios(usuario);

 if (privileg[asientos]=='0')
    {
     return;
    }

 int fila=ui.latabladiario->currentIndex().row();
 QString cad = model->record(fila).value("asiento").toString();
 QString ejercicio = model->record(fila).value("ejercicio").toString();
   // int fila=primaryKeyIndex.row();
   // QString cad = model->data(model->index(fila,2),Qt::DisplayRole).toString();
   // cad sería el número de asiento
   // queda editarlo
   editarasiento(cad,usuario,ejercicio);
   // refrescamos el diario
   // refresca();
   pasafiltro(guardafiltro,comadecimal,sindecimales);
   muestratabla();
   // irfinal();
   irafila(fila);*/
}

QString diario::fichdocumentoactual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return "";
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("copia_doc").toString();
}

void diario::asignaimagen(QPixmap imagen)
{
    ui.fotolabel->setFixedHeight(30);
    ui.fotolabel->setFixedWidth(27);

    ui.fotolabel->setPixmap(imagen);
}

void diario::resetimagen()
{
    ui.fotolabel->setFixedHeight(5);
    ui.fotolabel->setFixedWidth(0);
}


qlonglong diario::clave_ci_actual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return 0;
 int fila=ui.latabladiario->currentIndex().row();
 return model->record(fila).value("clave_ci").toLongLong();
}

QDate diario::fechasel()
{
  return ui.fechadateEdit->date();
}


int diario::columna_actual()
{
 if (!ui.latabladiario->currentIndex().isValid()) return -1;

 return ui.latabladiario->currentIndex().column();
}


void diario::inicio()
{
    ui.latabladiario->selectRow(0);
    /*ui.latabladiario->setCurrentIndex(modelo()->index(0,0));
    ui.latabladiario->scrollToBottom();
    ui.latabladiario->setFocus();*/

}

void diario::fin()
{
 ui.latabladiario->selectRow(modelo()->rowCount()-1);
}

void diario::esconde_filtro()
{
   ui.filtrolineEdit->hide();
   ui.filtrolabel->hide();
   ui.iniciopushButton->hide();
   ui.finpushButton->hide();
   ui.fechadateEdit->hide();
   ui.fotolabel->hide();

   ui.fecha_pushButton->hide();
   ui.concepto_pushButton->hide();
   ui.cuenta_pushButton->hide();
   ui.iva_pushButton->hide();
   ui.ret_pushButton->hide();

}

void diario::muestra_filtro()
{
   ui.filtrolineEdit->show();
   ui.filtrolabel->show();
   ui.iniciopushButton->show();
   ui.finpushButton->show();
   ui.fechadateEdit->show();
   ui.fotolabel->show();

   ui.fecha_pushButton->show();
   ui.concepto_pushButton->show();
   ui.cuenta_pushButton->show();
   ui.iva_pushButton->show();
   ui.ret_pushButton->show();

}


void diario::editafechaasien()
{
    QString elasiento;
    qlonglong nasiento=asientoactual();
    if (nasiento>0) elasiento.setNum(nasiento);
    if (elasiento.isEmpty())
       {
         QMessageBox::warning( this, tr("CAMBIAR FECHA EN ASIENTO"),
         tr("Para cambiar una fecha debe de seleccionar un apunte en el diario"));
         return;
       }
  QDate qfecha=fechapaseactual();

  editafechaasiento *e = new editafechaasiento();

  e->pasadatos( elasiento, qfecha );

  e->exec();
  delete(e);

  int guardafila=fila_actual();
  bool correcto=false;
  QSqlQuery q=basedatos::instancia()->selectDiariofiltro_nomsj_error (guardafiltro, &correcto);
  if (correcto)
     model->setQuery( q );
  irafila(guardafila);
}


void diario::edcondoc()
{
  qlonglong pase=paseactual();
  QString qapunte;
  if (pase>0) qapunte.setNum(pase);
  if (qapunte=="")
       {
         QMessageBox::warning( this, tr("EDITAR CONCEPTO / DOCUMENTO"),
         tr("Para cambiar datos debe de seleccionar un apunte en el diario"));
         return;
       }
  QDate qfecha=fechapaseactual();

  edit_conc_doc *e = new edit_conc_doc();
  e->pasadatos( qapunte, qfecha, conceptoactual(), documentoactual() );
  e->todo_el_asiento();
  e->exec();
  delete(e);

  int guardafila=fila_actual();
  bool correcto=false;
  QSqlQuery q=basedatos::instancia()->selectDiariofiltro_nomsj_error (guardafiltro, &correcto);
  if (correcto)
     model->setQuery( q );
  irafila(guardafila);
}

void diario::cambiacuenpase()
{
    QString elasiento;
    qlonglong nasiento=asientoactual();
    if (nasiento>0) elasiento.setNum(nasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("CAMBIAR CUENTA EN PASE"),
         tr("Para cambiar una subcuenta debe de seleccionar un pase en el diario"));
         return;
       }


  cambiacuentapase *c = new cambiacuentapase();

  qlonglong pase=paseactual();
  QString qapunte; qapunte.setNum(pase);
  QString codigo=subcuentaactual();
  QDate qfecha=fechapaseactual();
  double ddebe=debe();
  QString qdebe; qdebe.setNum(ddebe);
  double dhaber=haber();
  QString qhaber; qhaber.setNum(dhaber);

  c->pasainfo( qapunte, codigo, qfecha, qdebe, qhaber );
  c->exec();
  delete(c);

  int guardafila=fila_actual();
  bool correcto=false;
  QSqlQuery q=basedatos::instancia()->selectDiariofiltro_nomsj_error (guardafiltro, &correcto);
  if (correcto)
     model->setQuery( q );
  irafila(guardafila);
}

void diario::ed_registro_iva() {
    qlonglong pase=paseactual();
    QString qapunte;
    if (pase>0) qapunte.setNum(pase);
    if (qapunte=="")
         {
           QMessageBox::warning( this, tr("EDITAR CONCEPTO / DOCUMENTO"),
                                 tr("Para proceder hay que seleccionar un apunte en el diario"));
           return;
         }

    if (basedatos::instancia()->paseenlibroiva_isp(qapunte)) {
        QMessageBox::warning( this, tr("EDITAR REGISTRO DE IVA"),
        tr("Éste tipo de apuntes solo es editable en tabla de asientos"));
        return;
    }

    QString codigo=subcuentaactual();

// ---------------------------------------------------------------------------------------------------

QString cadrectif,cadautofact,cadagrario;
    QSqlQuery query2 = basedatos::instancia()->select14Libroivapase(qapunte);
    if (query2.isActive())
      if (query2.next()) {
        QString cadsoportado="0";
        if (query2.value(10).toBool()) cadsoportado="1";
        if (query2.value(12).toBool()==true )
           cadrectif="1";
        if (query2.value(13).toBool()==true )
           cadautofact="1";
        if (query2.value(24).toBool()==true )
            cadagrario="1";
        if (escuentadeivasoportado(codigo)) {
            ivasoportado *i = new ivasoportado(comadecimal,!sindecimales);

            i->pasadatos2( query2.value(0).toString(), formateanumero(query2.value(1).toDouble(),comadecimal,!sindecimales),
                                 query2.value(2).toString(),
                                 formateanumero(query2.value(3).toDouble(),comadecimal,!sindecimales), codigo,
                                 formateanumero(query2.value(5).toDouble(),comadecimal,!sindecimales),
                                 query2.value(6).toString(),query2.value(14).toDate(),
                                 formateanumero(query2.value(11).toDouble()*100,comadecimal,!sindecimales),
                                 cadrectif,
                                 query2.value(15).toDate(),query2.value(16).toString(),
                                 formateanumero(query2.value(17).toDouble(),comadecimal,!sindecimales),
                                 query2.value(18).toString(), query2.value(19).toString(),
                                 query2.value(20).toString(), query2.value(21).toString(),
                                 query2.value(22).toBool() ? "1":"",
                                 formateanumero(query2.value(23).toDouble()*100,comadecimal,!sindecimales), cadagrario,
                                 query2.value(25).toString(), query2.value(26).toString(),
                                 query2.value(29).toBool() ? "1" : "", query2.value(31).toBool() ? "1" : "",
                                 query2.value(32).toBool(),
                                 query2.value(35).toBool(), query2.value(36).toBool(),
                                 query2.value(37).toString());
            i->desactiva_base_tipo();

            if (i->exec()==QDialog::Accepted) {
                QString cuentabase, baseimponible, qclaveiva, qtipoiva, cuentaiva, cuotaiva;
                QString ctafra;
                QDate qfechafra;
                QString soportado, prorrata, qrectificativa; QDate fechaop;
                QString claveop, bicoste, frectif, numfact;
                QString facini, facfinal, bieninversion, afectacion, qagrario, qnombre;
                QString qcif, import, cajaiva;
                bool qno347, arrto_ret, arrto_sin_ret;
                QString dua;
                i->recuperadatos( &cuentabase, &baseimponible, &qclaveiva,
                                    &qtipoiva, &cuentaiva, &cuotaiva,
                                    &ctafra, &qfechafra, &soportado,
                                    &prorrata, &qrectificativa, &fechaop,
                                    &claveop,
                                    &bicoste,
                                    &frectif, &numfact,
                                    &facini, &facfinal, &bieninversion,
                                    &afectacion, &qagrario, &qnombre,
                                    &qcif, &import, &cajaiva, &qno347,
                                    &arrto_ret, &arrto_sin_ret, &dua);

                 int numero_facturas=numfact.toInt();
                 basedatos::instancia()->updateLibroiva(qapunte, cuentabase, baseimponible, qclaveiva,
                                qtipoiva, "", cuotaiva, ctafra,
                                qfechafra, soportado=="1", false, false, prorrata,
                                qrectificativa=="1", false,
                                fechaop, claveop, bicoste,
                                frectif, numero_facturas, facini, facfinal,
                                false, false, bieninversion=="1",
                                false, afectacion, qagrario=="1",
                                qnombre, qcif,
                                import=="1", false, false,
                                false, cajaiva=="1", qno347, false,
                                false, arrto_ret, arrto_sin_ret,dua,"");



            }
            delete(i);
        }
        else {
              if (escuentadeivarepercutido(codigo)) {
                  ivarepercutido *i = new ivarepercutido(comadecimal,!sindecimales);
                  i->pasadatos2( query2.value(0).toString(),
                                 formateanumero(query2.value(1).toDouble(),comadecimal,!sindecimales),
                                 query2.value(2).toString(),
                                 formateanumero(query2.value(3).toDouble(),comadecimal,!sindecimales),
                                 formateanumero(query2.value(4).toDouble(),comadecimal,!sindecimales), codigo,
                                 formateanumero(query2.value(5).toDouble(),comadecimal,!sindecimales),
                                 query2.value(6).toString(),
                                 query2.value(14).toDate(), cadrectif,
                                 query2.value(15).toDate(),query2.value(16).toString(),
                                 formateanumero(query2.value(17).toString().toDouble(),comadecimal,!sindecimales),
                                 query2.value(18).toString(), query2.value(19).toString(),
                                 query2.value(20).toString(), query2.value(21).toString(),
                                 query2.value(25).toString(), query2.value(26).toString(),
                                 query2.value(31).toString(),
                                 query2.value(35).toBool() ? "1" : "",query2.value(36).toBool() ? "1" : "");
                  if (i->exec()==QDialog::Accepted) {
                      QString cuentabase, baseimponible, qclaveiva;
                      QString qtipoiva, qtipore, cuentaiva, cuotaiva, ctafra;
                      QDate qfechafra;
                      QString soportado, qrectif;
                      QDate fechaop;
                      QString claveop, bicoste, frectif, numfact, facini, facfinal, nombre, cif;
                      QString caja, arrto_ret, arrto_sin_ret;
                      i->recuperadatos( &cuentabase, &baseimponible, &qclaveiva,
                                        &qtipoiva, &qtipore, &cuentaiva,
                                        &cuotaiva, &ctafra, &qfechafra,
                                        &soportado, &qrectif,
                                        &fechaop, &claveop,
                                        &bicoste,
                                        &frectif, &numfact,
                                        &facini, &facfinal, &nombre,
                                        &cif, &caja, &arrto_ret, &arrto_sin_ret);
                      int numero_facturas=numfact.toInt();
                      basedatos::instancia()->updateLibroiva(qapunte, cuentabase, baseimponible, qclaveiva,
                                     qtipoiva, qtipore, cuotaiva, ctafra,
                                     qfechafra, soportado=="1", false, false, "",
                                     qrectif=="1", false,
                                     fechaop, claveop, bicoste,
                                     frectif, numero_facturas, facini, facfinal,
                                     false, false, false,
                                     false, "", false,
                                     nombre, cif,
                                     false, false, false,
                                     false, caja=="1", false, false,
                                     false, arrto_ret=="1", arrto_sin_ret=="1","","");
                  }
                  delete(i);

              }
               else {
                  if (query2.value(39).toBool() || query2.value(40).toBool()) {
                  // eib ó eis
                  eib *e = new eib(comadecimal,!sindecimales);
                  //pasadatos( QString cuentabase, QString baseimponible, QString ctafra,
                  //          QDate qfechafra, QDate qfechaop, QString claveop )
                  e->pasadatos(query2.value(0).toString(),formateanumero(query2.value(1).toDouble(),comadecimal,!sindecimales),
                               query2.value(6).toString(),query2.value(14).toDate(),query2.value(15).toDate(), query2.value(16).toString());
                  e->esconde_externo();
                  if (query2.value(40).toBool()) e->selec_prservicios();

                  if (e->exec() ==QDialog::Accepted) {;
                      QString cuentabase, baseimponible, ctafra;
                      QDate qfechafra, qfechaop;
                      QString claveop, prservicios;
                      e->recuperadatos( &cuentabase, &baseimponible,  &ctafra,
                                  &qfechafra, &qfechaop, &claveop,
                                  &prservicios);
                      basedatos::instancia()->updateLibroiva(qapunte, cuentabase, baseimponible, "",
                                     "0", "0", "0", ctafra,
                                     qfechafra, false, false, prservicios!="1", "",
                                     "0", false,
                                     qfechaop, claveop, "",
                                     "",0 , "", "",
                                     false, prservicios=="1", false,
                                     false, "", false,
                                     "", "",
                                     false, false, false,
                                     false, "", false, false,
                                     false, false, false,"","");
                  }
                  delete(e);
                  }
                   else {
                   // exento
                   QString registro_donacion;
                   if (query2.value(41).toBool()) {
                      QJsonObject reg_donacion;
                      reg_donacion.insert("clave",query2.value(42).toString());
                      reg_donacion.insert("especie",query2.value(43).toBool());
                      reg_donacion.insert("recurrente",query2.value(44).toBool());
                      reg_donacion.insert("ca",query2.value(45).toString());
                      reg_donacion.insert("deduccion_ca",query2.value(46).toDouble());
                      QJsonDocument doc(reg_donacion);
                      registro_donacion=doc.toJson();
                     }
                   exento *e =new exento(comadecimal,!sindecimales);
                   e->pasadatos(query2.value(0).toString(),formateanumero(query2.value(1).toDouble(),comadecimal,!sindecimales),
                               query2.value(6).toString(),
                               query2.value(14).toDate(),query2.value(15).toDate(), query2.value(16).toString(),
                               cadrectif,query2.value(18).toString(),query2.value(19).toString(),
                               query2.value(20).toString(), query2.value(21).toString(),
                               query2.value(22).toBool() ? "1":"",query2.value(38).toBool() ? "1" : "",
                               query2.value(25).toString(), query2.value(26).toString(), query2.value(29).toBool() ? "1":"",
                               query2.value(30).toBool() ? "1":"",!query2.value(27).toBool() ? "1" : "",
                               query2.value(33).toBool() ? "1" : "",
                               (query2.value(28).toBool() && !query2.value(10).toBool()) ? "1" : "",registro_donacion, query2.value(37).toString());
                   if (cadsoportado!="1") e->fuerzaemitidas();
                    if (e->exec() ==QDialog::Accepted) {
                        QString cuentabase, baseimponible,  ctafra;
                        QDate qfechafra, fechaop;
                        QString claveop, rectificativa, frectif;
                        QString numfact;
                        QString facini, facfinal, bieninversion;
                        QString op_no_sujeta, nombre, cif;
                        QString importacion, exportacion;
                        QString exenta_no_deduc, vta_fuera_tac, emitida_isp_int;
                        QString dua;

                        e->recuperadatos(&cuentabase, &baseimponible,  &ctafra,
                                         &qfechafra, &fechaop,
                                         &claveop, &rectificativa, &frectif,
                                         &numfact,
                                         &facini, &facfinal, &bieninversion,
                                         &op_no_sujeta, &nombre, &cif,
                                         &importacion, &exportacion,
                                         &exenta_no_deduc, &vta_fuera_tac, &emitida_isp_int, &registro_donacion, &dua);
                        int numero_facturas=numfact.toInt();
                        basedatos::instancia()->updateLibroiva(qapunte, cuentabase, baseimponible, "",
                                       "0", "0", "0", ctafra,
                                       qfechafra, !e->eslibroemitidas(), false, false, "",
                                       rectificativa=="1", false,
                                       fechaop, claveop, "",
                                       frectif, numero_facturas, facini, facfinal,
                                       false, false, bieninversion=="1",
                                       op_no_sujeta=="1", "", false,
                                       nombre, cif,
                                       importacion=="1", exportacion=="1", exenta_no_deduc=="1",
                                       emitida_isp_int=="1", false, false, vta_fuera_tac=="1",
                                       false, false, false,dua, registro_donacion);
                    }
                    delete(e);
                  }
              }
        }
    }

}

void diario::ed_registro_ret() {
    qlonglong pase=paseactual();
    QString qapunte;
    if (pase>0) qapunte.setNum(pase);
    if (qapunte=="")
         {
           QMessageBox::warning( this, tr("EDITAR CONCEPTO / DOCUMENTO"),
                                 tr("Para proceder hay que seleccionar un apunte en el diario"));
           return;
         }

    QString codigo=subcuentaactual();

    QSqlQuery query2 = basedatos::instancia()->datos_ret_pase (qapunte);
    if (query2.isActive())
        if (query2.next())
         {
            // SELECT pase, cta_retenido, arrendamiento, clave_ret,
            // base_ret, tipo_ret, retencion, ing_cta, ing_cta_rep,
            // nombre, cif, provincia
            QString cta_retenido=query2.value(1).toString();
            bool ret_arrendamiento=query2.value(2).toBool();
            QString clave=query2.value(3).toString();
            QString base_percepciones=query2.value(4).toString();
            QString tipo_ret=query2.value(5).toString();
            QString qretencion=query2.value(6).toString();
            QString ing_cta=query2.value(7).toString();
            QString ing_cta_repercutido=query2.value(8).toString();
            QString nombre_ret=query2.value(9).toString();
            QString cif_ret= query2.value(10).toString();
            QString provincia=query2.value(11).toString();

            retencion *r = new retencion(comadecimal,!sindecimales);
            r->pasadatos_all(codigo,
                               cta_retenido, ret_arrendamiento,
                               clave, base_percepciones,
                               tipo_ret, qretencion,
                               ing_cta, ing_cta_repercutido,
                               nombre_ret, cif_ret, provincia);
            if (r->exec() ==QDialog::Accepted) {
                r->recuperadatos(&cta_retenido, &ret_arrendamiento,
                                 &clave, &base_percepciones,
                                 &tipo_ret, &qretencion,
                                 &ing_cta, &ing_cta_repercutido,
                                 &nombre_ret, &cif_ret, &provincia);

                basedatos::instancia()->modifica_retencion (qapunte,
                                                  cta_retenido, ret_arrendamiento,
                                                  clave, base_percepciones,
                                                  tipo_ret, qretencion,
                                                  ing_cta, ing_cta_repercutido,
                                                  nombre_ret, cif_ret, provincia);
            }
            delete(r);
        }
}
