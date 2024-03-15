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

#include "exec_regul.h"
#include "basedatos.h"
#include "regul_sec.h"
#include "funciones.h"
#include "expresiones.h"
#include <QMessageBox>

exec_regul::exec_regul() : QDialog() {
    ui.setupUi(this);


     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("cab_regulariz");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modeloreg=modelo;
     ui.tabla->setModel(modelo);
     ui.tabla->setColumnWidth(0,60);
     ui.tabla->setColumnWidth(1,350);
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->show();

     // connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));
     connect(ui.ejecutarpushButton,SIGNAL(clicked()),this,SLOT(tablapulsada()));
     connect(ui.ejerciciocomboBox,SIGNAL(currentIndexChanged (QString )),this,
            SLOT(ejerciciocambiado()));

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

  ui.inicialdateEdit->setDate(inicioejercicio(ui.ejerciciocomboBox->currentText()));
  ui.finaldateEdit->setDate(finejercicio(ui.ejerciciocomboBox->currentText()));
  ui.asientodateEdit->setDate(finejercicio(ui.ejerciciocomboBox->currentText()));

}
 
exec_regul::~exec_regul()
{
  delete ui.tabla;
  delete modeloreg;
}



void exec_regul::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {        
          ejecuta(modeloreg->record(indiceactual.row()).value("codigo").toString());
          accept();
       }
}


void exec_regul::ejecuta(QString codigo)
{
    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();

    basedatos::instancia()->bloquea_para_regularizacion2();

 QSqlQuery query = basedatos::instancia()->carga_regul_sec(codigo);
 if ( query.isActive() ) {
   while (query.next())
     {
       QString descripcion=query.value(0).toString();
       QString cuenta_deudora=expandepunto(query.value(1).toString(),anchocuentas());
       QString cuenta_acreedora=expandepunto(query.value(2).toString(),anchocuentas());
       QString cuentas=query.value(3).toString();
       QString condicion=query.value(4).toString();
       QString diario=query.value(5).toString();
       // primero evaluamos la condicion
       // si se cumple procesamos, si no saltamos al siguiente
       QString qexpresion,qvalor;
       bool mayorque=false;
       bool menorque=false;
       bool mayorigualque=false;
       bool menorigualque=false;
       if (condicion.contains(">="))
          {
           mayorigualque=true;
           qexpresion=condicion.section(">=",0,0);
           qvalor=condicion.section(">=",1,1);
          }
          else
              {
               if (condicion.contains("<="))
                 {
                  menorigualque=true;
                  qexpresion=condicion.section("<=",0,0);
                  qvalor=condicion.section("<=",1,1);
                 }
                 else
                   {
                    if (condicion.contains("<"))
                       {
                        menorque=true;
                        qexpresion=condicion.section("<",0,0);
                        qvalor=condicion.section("<",1,1);
                       }
                       else
                           {
                            if (condicion.contains(">"))
                              {
                               mayorque=true;
                               qexpresion=condicion.section(">",0,0);
                               qvalor=condicion.section(">",1,1);
                              }
                           }
                   }
              }
      double valor_expresion=0;
      if (qexpresion.length()>0)
         {
           valor_expresion=valorexpresion(qexpresion,
                      ui.inicialdateEdit->date(),ui.finaldateEdit->date());
           double valor=convapunto(qvalor).toDouble();
           QString cadnumm; cadnumm.setNum(valor_expresion);

           if (mayorque)
              {
               if (valor_expresion<=valor) continue;
              }
           if (menorque)
              {
               if (valor_expresion>=valor) continue;
              }
           if (mayorigualque)
              {
               if (valor_expresion<valor) continue;
              }
           if (menorigualque)
              {
               if (valor_expresion>valor) continue;
              }
         }
      // calculamos el saldo que resulta de 'cuentas'
      double saldo=basedatos::instancia()->calculasaldocuentas(cuentas,ui.inicialdateEdit->date(),
                     ui.finaldateEdit->date());
      QString cuenta_reg;
      if (saldo>0.001)
         {
          if (!existesubcuenta(cuenta_deudora))
             {
              QMessageBox::warning( this, tr("FASE DE REGULARIZACIÓN"),
               tr("Fase no procesada. No existe la cuenta ")+cuenta_deudora);
              continue;
             }
          cuenta_reg=cuenta_deudora;    
         }
      if (saldo<-0.001)
         {
          if (!existesubcuenta(cuenta_acreedora))
             {
              QMessageBox::warning( this, tr("FASE DE REGULARIZACIÓN"),
               tr("Fase no procesada. No existe la cuenta ")+cuenta_acreedora);
              continue;
             }
          cuenta_reg=cuenta_acreedora;    
         }
      ejecuta_regularizacion(cuenta_reg,cuentas,descripcion,ui.inicialdateEdit->date(),
                             ui.finaldateEdit->date(),ui.asientodateEdit->date(),diario);
     } // fin del while
   } // fin del query active
  basedatos::instancia()->desbloquea_y_commit();
  QMessageBox::information( this, tr("REGULARIZACIÓN"),
       tr("La regularización ha finalizado"));

}

void exec_regul::ejerciciocambiado()
{
  ui.inicialdateEdit->setDate(inicioejercicio(ui.ejerciciocomboBox->currentText()));
  ui.finaldateEdit->setDate(finejercicio(ui.ejerciciocomboBox->currentText()));
  ui.asientodateEdit->setDate(finejercicio(ui.ejerciciocomboBox->currentText()));
}
