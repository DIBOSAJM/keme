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

#include "ed_venci_cta.h"
#include "basedatos.h"
#include "buscasubcuenta.h"

ed_venci_cta::ed_venci_cta() : QDialog() {
    ui.setupUi(this);

QStringList lista=basedatos::instancia()->listatiposdescripvenci();
ui.vencicomboBox->addItems (lista);

connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(aceptar()));
connect(ui.diafijocheckBox,SIGNAL(clicked()),this,SLOT(checkdiafijo()));
connect(ui.cta_tesorpushButton,SIGNAL( clicked()),this,SLOT(botoncta_tesorpulsado()));
connect(ui.cuenta_tesorlineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_tesor_finedicion()));
connect(ui.cuenta_tesorlineEdit,SIGNAL( textChanged(QString)),this,SLOT(cta_tesor_cambiada()));

}

void ed_venci_cta::pasa_cta_descrip(QString qcodigo, QString descrip)
{
    codigo=qcodigo;
    ui.cuentalabel->setText(tr("CUENTA: ")+ qcodigo);
    ui.descriplabel->setText(descrip.left(42));
    // cargamos datos de la tabla venci_cta
    QSqlQuery query=basedatos::instancia()->datos_venci_asignado_cta(qcodigo);
    // select venci, tesoreria, procesavenci, vto_dia_fijo
    if (query.isActive())
        if (query.next())
          {
           QString tipo=query.value(0).toString();
           int indice=0;
           for (int veces=0; veces< ui.vencicomboBox->count(); veces++)
           {
             if (ui.vencicomboBox->itemText(veces).section("//",0,0).trimmed() == tipo)
              { indice=veces; break; }
           }
           ui.vencicomboBox->setCurrentIndex(indice);

           ui.cuenta_tesorlineEdit->setText(query.value(1).toString());
           if (query.value(2).toBool())
               ui.procvtocheckBox->setChecked(true);
           if (query.value(3).toInt()>0)
              {
               ui.diafijocheckBox->setChecked(true);
               ui.vfijospinBox->setEnabled(true);
               ui.vfijospinBox->setValue(query.value(3).toInt());
              }
              else
                  {
                   ui.diafijocheckBox->setChecked(false);
                   ui.vfijospinBox->setEnabled(false);

                  }
          }

}


void ed_venci_cta::aceptar()
{
    QString tipo=ui.vencicomboBox->currentText().section("//",0,0).trimmed();
    int diafijo=ui.vfijospinBox->value();
    if (!ui.diafijocheckBox->isChecked()) diafijo=0;
    basedatos::instancia()->actu_venci_asignado_cta(codigo, tipo,
                                         ui.cuenta_tesorlineEdit->text(),
                                         ui.procvtocheckBox->isChecked(),
                                         diafijo);
    accept();

}


void ed_venci_cta::checkdiafijo()
{
   if (ui.diafijocheckBox->isChecked()) ui.vfijospinBox->setEnabled(true);
     else ui.vfijospinBox->setEnabled(false);
}


void ed_venci_cta::botoncta_tesorpulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuenta_tesorlineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.cuenta_tesorlineEdit->setText(cadena2);
       else ui.cuenta_tesorlineEdit->setText("");
    delete labusqueda;

}


void ed_venci_cta::cta_tesor_cambiada()
{
    QString cadena;
    if (existecodigoplan(ui.cuenta_tesorlineEdit->text(),&cadena))
       ui.descrip_cta_tesorlabel->setText(cadena);
     else
       ui.descrip_cta_tesorlabel->setText("");

}



void ed_venci_cta::cta_tesor_finedicion()
{
   ui.cuenta_tesorlineEdit->setText(expandepunto(ui.cuenta_tesorlineEdit->text(),anchocuentas()));
}

