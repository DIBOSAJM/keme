/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C) José Manuel Díez Botella

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

#include "buscasubcuenta_input.h"
#include "funciones.h"
#include "basedatos.h"
#include "asignainputcta.h"
#include <QMessageBox>

buscasubcuenta_input::buscasubcuenta_input() : QDialog() {
    ui.setupUi(this);

ui.ctastableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
ui.ctastableWidget->setColumnWidth(1,300);

connect(ui.SubcuentaLineEdit,SIGNAL(textChanged(QString)),this,SLOT(subcuenta_cambiada()));
connect(ui.TituloLineEdit,SIGNAL(textChanged(QString)),this,SLOT(titulo_cambiado()));
connect(ui.CIBoton,SIGNAL(clicked()),this,SLOT(procesaimputaciones()));
connect(ui.ctastableWidget,SIGNAL(cellDoubleClicked ( int , int )),this,SLOT(procesaimputaciones()));
connect(ui.copiarpushButton,SIGNAL(clicked()),this,SLOT(copia_cuenta()));
connect(ui.borramodelopushButton,SIGNAL(clicked()),this,SLOT(elimina_copia()));

// cargarmos info inicial en la tabla
QString clave_gastos, clave_ingresos,filtro;
clave_gastos=basedatos::instancia()->clavegastos();
clave_ingresos=basedatos::instancia()->claveingresos();

if (conanalitica())
  {

    int partes=clave_gastos.count(',');
    if (clave_gastos.length()>0)
       {
        for (int veces=0;veces<=partes;veces++)
           {
            if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
             filtro += "codigo like '"+ clave_gastos.section(',',veces,veces).replace("'","''") +"%'";
            }
            else {
             filtro+="position('";
             filtro+=clave_gastos.section(',',veces,veces).replace("'","''");
             filtro+="' in codigo)=1";
            }
            if (veces!=partes)          // filtrogastos+=")";
               filtro+=" or ";
            }
        }

    partes=clave_ingresos.count(',');
    if (clave_ingresos.length()>0)
       {
        for (int veces=0;veces<=partes;veces++)
           {
            if (veces==0 && filtro.length()>0) filtro+=" or ";
            if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
             filtro += "codigo like '"+ clave_ingresos.section(',',veces,veces).replace("'","''") +"%'";
            }
            else {
             filtro+="position('";
             filtro+=clave_ingresos.section(',',veces,veces).replace("'","''");
             filtro+="' in codigo)=1";
            }
            if (veces!=partes) filtro+=" or ";
            }
        }
  }

if (conanalitica_parc())
  {
    QString cadcodigos=basedatos::instancia()->select_codigos_en_ctas_analitica();
    cadcodigos.remove(' ');
    QStringList lista;
    lista = cadcodigos.split(",");
    for (int veces=0;veces<lista.size();veces++)
       {
        if (veces>0) filtro +=" or ";
        if (basedatos::instancia()->cualControlador() == basedatos::SQLITE)
           filtro += "codigo like '" + lista.at(veces) + "%'";
        else
           {
            filtro+="position('";
            filtro+=lista.at(veces);
            filtro+="' in codigo)=1";
           }

       }
  }

// QMessageBox::information( this, tr("subcuentas imputaciones"),filtro);


 if (filtro.length()>0)
   {
     QSqlQuery query = basedatos::instancia()->listacuentasauxiliares(filtro);
     if (query.isActive())
       while (query.next())
        {
         codigo << query.value(0).toString();
         descripcion << query.value(1).toString();
        }
     codigo_origen=codigo;
     descripcion_origen=descripcion;
     // rellenamos la tabla con la información
     for (int fila=0; fila<codigo.size();fila++)
        {
          ui.ctastableWidget->insertRow(fila);
          ui.ctastableWidget->setItem(fila,0,new QTableWidgetItem(codigo.at(fila)));
          ui.ctastableWidget->setItem(fila,1,new QTableWidgetItem(descripcion.at(fila)));


          QSqlQuery query = basedatos::instancia()->selectSumasignacion100input_cta(codigo.at(fila));
          QLabel  *etiqueta = new QLabel();
          etiqueta->setAlignment(Qt::AlignHCenter);
          QString contenido="0";
          if ( (query.isActive()) && (query.first()) )
             {
              double valor=query.value(0).toDouble();
              if (valor>99.999 && valor<100.001)
                {
                  etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
                  ui.ctastableWidget->setCellWidget(fila,2,etiqueta);
                  contenido="1";
                }
             }
          if (contenido=="0")
            {
              etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
              ui.ctastableWidget->setCellWidget(fila,2,etiqueta);
            }

          ui.ctastableWidget->setItem(fila,2,new QTableWidgetItem(""));
        }
   }

}


void buscasubcuenta_input::subcuenta_cambiada()
{
    ui.TituloLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui.TituloLineEdit->clear();
    connect(ui.TituloLineEdit,SIGNAL(textChanged(QString)),this,SLOT(titulo_cambiado()));
    // escondemos todas las filas que no cumplan la condición
    bool comience=ui.ComienceRadioButton->isChecked();
    for (int veces=0; veces<ui.ctastableWidget->rowCount();veces++)
       {
        if (comience)
           {
             if (ui.ctastableWidget->item(veces,0)->text().startsWith(ui.SubcuentaLineEdit->text()))
                 ui.ctastableWidget->showRow(veces);
             else
                 ui.ctastableWidget->hideRow(veces);
           }
         else
        {
          if (ui.ctastableWidget->item(veces,0)->text().contains(ui.SubcuentaLineEdit->text()))
              ui.ctastableWidget->showRow(veces);
          else
              ui.ctastableWidget->hideRow(veces);
        }
       }
}

void buscasubcuenta_input::titulo_cambiado()
{
    ui.SubcuentaLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui.SubcuentaLineEdit->clear();
    connect(ui.SubcuentaLineEdit,SIGNAL(textChanged(QString)),this,SLOT(subcuenta_cambiada()));
    // escondemos todas las filas que no cumplan la condición
    bool comience=ui.ComienceRadioButton->isChecked();
    for (int veces=0; veces<ui.ctastableWidget->rowCount();veces++)
       {
        if (comience)
           {
             if (ui.ctastableWidget->item(veces,1)->text().startsWith(ui.TituloLineEdit->text()))
                 ui.ctastableWidget->showRow(veces);
             else
                 ui.ctastableWidget->hideRow(veces);
           }
         else
        {
          if (ui.ctastableWidget->item(veces,1)->text().contains(ui.TituloLineEdit->text()))
              ui.ctastableWidget->showRow(veces);
          else
              ui.ctastableWidget->hideRow(veces);
        }
       }
}


QString buscasubcuenta_input::seleccioncuenta( int *fila )
{
/* if ((ui.SubcuentaLineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) ||
    (cod_longitud_variable() && esauxiliar(ui.SubcuentaLineEdit->text())))
    {
     if (existesubcuenta(ui.SubcuentaLineEdit->text()))
         return ui.SubcuentaLineEdit->text();
     else return QString();
    } */
 *fila=0;
 if (ui.ctastableWidget->rowCount()==0) return QString();
 if (ui.ctastableWidget->currentRow()>=0 &&
     ui.ctastableWidget->currentRow()< ui.ctastableWidget->rowCount())
     {
      *fila=ui.ctastableWidget->currentRow();
      return ui.ctastableWidget->item(ui.ctastableWidget->currentRow(),0)->text();
     }
   else
       return ui.ctastableWidget->item(0,0)->text();
 return QString();
}



void buscasubcuenta_input::procesaimputaciones()
{
  int fila;
  QString cuenta=seleccioncuenta(&fila);
  bool sobreescribir=false;
  if (!ui.modelolineEdit->text().isEmpty())
     {
      QSqlQuery query = basedatos::instancia()->selectSumasignacion100input_cta(codigo.at(fila));
      if ( (query.isActive()) && (query.first()) )
         {
          double valor=query.value(0).toDouble();
          if (valor>99.999 && valor<100.001)
             {
              if (!QMessageBox::question(this,
                    tr("ANALÍTICA"),
                    tr("¿ Sobreescribir imputaciones ?"),
                    tr("&Sí"), tr("&No"),
                    QString(), 0, 1 ))
                {
                  sobreescribir=true;
                }
                else sobreescribir=false;
             } else sobreescribir=true;
         }

     }
  if (cuenta.isEmpty()) return;
  if (sobreescribir) basedatos::instancia()->clona_input(ui.modelolineEdit->text(),cuenta);
  asignainputcta *a = new asignainputcta();
  a->pasacta( cuenta );
  a->exec();
  delete(a);

  QSqlQuery query = basedatos::instancia()->selectSumasignacion100input_cta(codigo.at(fila));
  if ( (query.isActive()) && (query.first()) )
     {
      double valor=query.value(0).toDouble();
      if (valor>99.999 && valor<100.001)
        {
          marca(fila);
        }
       else desmarca(fila);
     } else desmarca(fila);
}


void buscasubcuenta_input::marca(int fila)
{
 QLabel *etiqueta=(QLabel*)ui.ctastableWidget->cellWidget(fila,2);
 etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
 // ui.ctastableWidget->item(fila,2)->setText("1");
}


void buscasubcuenta_input::desmarca(int fila)
{
 QLabel *etiqueta=(QLabel*)ui.ctastableWidget->cellWidget(fila,2);
 etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
 // ui.ctastableWidget->item(fila,2)->setText("0");
}



void buscasubcuenta_input::copia_cuenta()
{
  int fila;
  QString cuenta=seleccioncuenta(&fila);
  if (cuenta.isEmpty()) return;
  QSqlQuery query = basedatos::instancia()->selectSumasignacion100input_cta(codigo.at(fila));
  if ( (query.isActive()) && (query.first()) )
    {
     double valor=query.value(0).toDouble();
     if (valor>99.999 && valor<100.001)
        ui.modelolineEdit->setText(cuenta);
    }
}

void buscasubcuenta_input::elimina_copia()
{
  ui.modelolineEdit->clear();
}
