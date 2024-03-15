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

#include "edita_ref.h"
#include "basedatos.h"
#include "funciones.h"
#include "buscasubcuenta.h"

edita_ref::edita_ref() : QDialog() {
    ui.setupUi(this);

    QString clavedefecto=clave_iva_defecto();
    double tipo,re;
    QString qdescrip;
    if (clavedefecto.length()>0 && existeclavetipoiva(clavedefecto,&tipo,&re,&qdescrip))
       {
        ui.ivacomboBox->insertItem ( 0,clavedefecto) ;
        ui.ivarecomboBox->insertItem ( 0,clavedefecto) ;
       }


    QSqlQuery query = basedatos::instancia()->selectTodoTiposivanoclave(clavedefecto);

    if ( query.isActive() )
       {
         while (query.next())
            {
             ui.ivacomboBox->insertItem ( -1,query.value(0).toString()) ;
             ui.ivarecomboBox->insertItem ( -1,query.value(0).toString()) ;
            }
         }
    ui.ivacomboBox->insertItem(-1,tr("EXENTO"));
    ui.ivarecomboBox->insertItem(-1,tr("EXENTO"));
    coma=haycomadecimal();
    decimales=haydecimales();

    connect(ui.aceptarpushButton, SIGNAL(clicked()), SLOT(terminar()));
    connect(ui.ivacomboBox,SIGNAL(currentIndexChanged(int)), SLOT(ivacombocambiado()));
    connect(ui.ivarecomboBox,SIGNAL(currentIndexChanged(int)), SLOT(ivarecombocambiado()));
    connect(ui.pvplineEdit,SIGNAL(editingFinished()),SLOT(pvpfinedic()));
    connect(ui.ctapushButton,SIGNAL(clicked()),this,SLOT(buscacta()));
    connect(ui.cuentalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuentacambiada()));
    connect(ui.cuentalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaexpandepunto()));
    connect(ui.suplidocheckBox,SIGNAL(stateChanged(int)),this,SLOT(suplidos_check()));
    connect(ui.anticipo_checkBox,SIGNAL(stateChanged(int)),this,SLOT(anticipos_check()));
    ivacombocambiado();
    ivarecombocambiado();
}

void edita_ref::pasacodigo(QString codigo)
{
    QString descrip, tipoiva, tipoivare, cuenta;
    double pvp;
    bool suplido, anticipo, precision;
    basedatos::instancia()->carga_ref(codigo,
                                   &descrip,
                                   &tipoiva,
                                   &tipoivare,
                                   &cuenta,
                                   &pvp,
                                   &suplido,
                                   &anticipo, &precision);
    ui.codigolineEdit->setText(codigo);
    ui.descriplineEdit->setText(descrip);
    ui.cuentalineEdit->setText(cuenta);
    ui.suplidocheckBox->setChecked(suplido);
    ui.anticipo_checkBox->setChecked(anticipo);
    ui.doble_precision_checkBox->setChecked(precision);

    for (int veces=0; veces<ui.ivacomboBox->count(); veces++)
        if (ui.ivacomboBox->itemText(veces)==tipoiva)
           {
             ui.ivacomboBox->setCurrentIndex(veces);
             break;
           }

    for (int veces=0; veces<ui.ivarecomboBox->count(); veces++)
        if (ui.ivarecomboBox->itemText(veces)==tipoivare)
           {
             ui.ivarecomboBox->setCurrentIndex(veces);
             break;
           }

    if (precision) ui.pvplineEdit->setText(formateanumero_ndec(pvp,coma, 4));
     else
       ui.pvplineEdit->setText(formateanumero(pvp,coma, decimales));
}


void edita_ref::terminar()
{
    basedatos::instancia()->guarda_ref(ui.codigolineEdit->text(),
                                   ui.descriplineEdit->text(),
                                   ui.ivacomboBox->currentText(),
                                   ui.ivarecomboBox->currentText(),
                                   ui.cuentalineEdit->text(),
                                   convapunto(ui.pvplineEdit->text()),
                                   ui.suplidocheckBox->isChecked(),
                                   ui.anticipo_checkBox->isChecked(),
                                   ui.doble_precision_checkBox->isChecked());
    accept();
}

void edita_ref::ivacombocambiado()
{
    QString tipo,re;
    QString descrip;
    existeclaveiva(ui.ivacomboBox->currentText(),
                       &tipo,&re,&descrip);

    ui.descripIVAlabel->setText(descrip);

}

void edita_ref::ivarecombocambiado()
{
    QString tipo,re;
    QString descrip;

    existeclaveiva(ui.ivarecomboBox->currentText(),
                       &tipo,&re,&descrip);

    ui.descripIVARElabel->setText(descrip);

}

void edita_ref::pvpfinedic()
{
    QString precio=ui.pvplineEdit->text();
    ui.pvplineEdit->setText(coma ? convacoma(precio) : convapunto(precio));
}


void edita_ref::buscacta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentalineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.cuentalineEdit->setText(cadena2);
       else ui.cuentalineEdit->setText("");
    delete labusqueda;

}


void edita_ref::cuentacambiada()
{
    if(existesubcuenta(ui.cuentalineEdit->text()) || ui.anticipo_checkBox->isChecked())
       {
        ui.aceptarpushButton->setEnabled(true);
        ui.descripctalabel->setText(descripcioncuenta(ui.cuentalineEdit->text()));
       }
     else
         {
          ui.aceptarpushButton->setEnabled(false);
          ui.descripctalabel->clear();
         }
}


void edita_ref::ctaexpandepunto()
{
  ui.cuentalineEdit->setText(expandepunto(ui.cuentalineEdit->text(),anchocuentas()));
}

void edita_ref::suplidos_check()
{
    if (ui.suplidocheckBox->isChecked())
       {
        ui.anticipo_checkBox->setChecked(false);
        ui.ivacomboBox->setEnabled(false);
        ui.ivarecomboBox->setEnabled(false);
       }
       else
           {
            ui.ivacomboBox->setEnabled(true);
            ui.ivarecomboBox->setEnabled(true);
            cuentacambiada();
           }
}


void edita_ref::anticipos_check()
{
    if (ui.anticipo_checkBox->isChecked())
       {
         ui.suplidocheckBox->setChecked(false);
         ui.cuentalineEdit->clear();
         ui.cuentalineEdit->setEnabled(false);
         ui.ctapushButton->setEnabled(false);
         cuentacambiada();
        // ui.cuenta_label->setText(tr("Cuenta de tesorería:"));
       }
       else
           {
            // ui.cuenta_label->setText(tr("Cuenta asignada:"));
            ui.cuentalineEdit->setEnabled(true);
            ui.ctapushButton->setEnabled(true);
            cuentacambiada();
           }
}
