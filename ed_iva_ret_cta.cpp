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

#include "ed_iva_ret_cta.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"

ed_iva_ret_cta::ed_iva_ret_cta() : QDialog() {
    ui.setupUi(this);

cargacomboIVA();
cargacombooperacionret();
comadecimal=haycomadecimal();

connect(ui.cuentaIVAlineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_IVA_finedicion()));
connect(ui.ctaIVApushButton,SIGNAL( clicked()),this,SLOT(botonctaivapulsado()));
connect(ui.cuentaIVAlineEdit,SIGNAL( textChanged(QString)),this,SLOT(cta_IVA_cambiada()));

connect(ui.ctaBaseIVAlineEdit,SIGNAL(editingFinished()),this,SLOT(cta_base_IVA_finedicion()));
connect(ui.ctaBaseIVApushButton,SIGNAL(clicked(bool)),this,SLOT(botoncta_base_ivapulsado()));
connect(ui.ctaBaseIVAlineEdit,SIGNAL(textChanged(QString)),this,SLOT(cta_base_IVA_cambiada()));

connect(ui.cuentata_ret_asiglineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_ret_asig_finedicion()));
connect(ui.ctaretpushButton,SIGNAL( clicked()),this,SLOT(botonctaretpulsado()));
connect(ui.cuentata_ret_asiglineEdit,SIGNAL(textChanged(QString)),this, SLOT(cta_ret_asig_cambiada()));

connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(guardar()));
}


void ed_iva_ret_cta::cargacomboIVA()
{
  // Tenemos que cargar clave IVA por defecto y resto de IVA'S en el combo box
    QString pasalinea;
    QString Clavedefecto, laclave;
    double tipo;
    double re;
    bool comadecimal=haycomadecimal();
    QString qdescrip;
    QString convert;
    Clavedefecto=clave_iva_defecto();
    if (Clavedefecto.length()>0 && existeclavetipoiva(Clavedefecto,&tipo,&re,&qdescrip))
        {
        pasalinea=Clavedefecto;
        if (!conigic()) pasalinea+=tr(" // IVA:");
            else pasalinea+=tr(" // IGIC:");
        convert.setNum(tipo,'f',2);
        if (comadecimal) pasalinea+=convacoma(convert);
            else pasalinea+=convert;
        pasalinea+=tr(" RE:");
        convert.setNum(re,'f',2);
        if (comadecimal) pasalinea+=convacoma(convert);
            else pasalinea+=convert;
        pasalinea += " : ";
        pasalinea += qdescrip;
        ui.tipoIVAcomboBox->insertItem ( 0,pasalinea) ;
        }
    // ahora cargamos el resto de los tipos de IVA

 QSqlQuery query = basedatos::instancia()->selectTodoTiposivanoclave(Clavedefecto);

 if ( query.isActive() )
    {
      while (query.next())
         {
          laclave=query.value(0).toString();
          tipo=query.value(1).toDouble();
          re=query.value(2).toDouble();
               qdescrip=query.value(3).toString();
          pasalinea=laclave;
          pasalinea+=tr(" // IVA:");
          convert.setNum(tipo,'f',2);
          if (comadecimal) pasalinea+=convacoma(convert);
              else pasalinea+=convert;
          pasalinea+=tr(" RE:");
          convert.setNum(re,'f',2);
          if (comadecimal) pasalinea+=convacoma(convert);
              else pasalinea+=convert;
          pasalinea += " : ";
          pasalinea += qdescrip;
          ui.tipoIVAcomboBox->insertItem ( -1,pasalinea) ;
         }
      }

}

void ed_iva_ret_cta::cargacombooperacionret()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_ret();
  // ui.operacionretcomboBox->addItems (lista);
  ui.tipo_oper_ret_asigcomboBox->addItems(lista);
}


void ed_iva_ret_cta::pasa_cta_descrip(QString qcodigo, QString descrip)
{
    codigo=qcodigo;
    ui.cuentalabel->setText(tr("CUENTA: ")+ qcodigo);
    ui.descriplabel->setText(descrip.left(42));
    // cargamos datos de la tabla venci_cta
    QSqlQuery query=basedatos::instancia()->datos_iva_ret_asignado_cta(qcodigo);
    /* "ivaasoc bool default 0,"
     "cuentaiva      varchar(40),"
     "tipoiva        varchar(20),"
     "conceptodiario varchar(155),"
     "cuenta_ret_asig varchar(40) default '',"
     "es_ret_asig_arrend bool "
     "tipo_ret_asig numeric(5,2) default 0,"
     "tipo_oper_ret_asig varchar(80)," */

    if (query.isActive())
        if (query.next())
          {
           if (query.value(0).toBool())
             {
               ui.ivaasocgroupBox->setChecked(true);
               ui.cuentaIVAlineEdit->setText(query.value(1).toString());
               QString tipo=query.value(2).toString();
               int indice=0;
               for (int veces=0; veces< ui.tipoIVAcomboBox->count(); veces++)
               {
                 if (ui.tipoIVAcomboBox->itemText(veces).section("//",0,0).trimmed() == tipo)
                  { indice=veces; break; }
               }
               ui.tipoIVAcomboBox->setCurrentIndex(indice);
             }
           ui.conceptodiariolineEdit->setText(query.value(3).toString());
           if (!query.value(4).toString().isEmpty()) ui.retgroupBox->setChecked(true);
           ui.cuentata_ret_asiglineEdit->setText(query.value(4).toString());
           ui.es_ret_asigcheckBox->setChecked(query.value(5).toBool());
           QString ret=query.value(6).toString();
           ui.porc_ret_asig_lineEdit->setText(ret);
           QString tipo_oper_ret_asig=query.value(7).toString();
           int indice=0;
           while (indice<ui.tipo_oper_ret_asigcomboBox->count())
             {
              if (ui.tipo_oper_ret_asigcomboBox->itemText(indice).left(1)==tipo_oper_ret_asig.left(1)) break;
              indice++;
             }
           if (indice>=ui.tipo_oper_ret_asigcomboBox->count()) indice=0;
           ui.tipo_oper_ret_asigcomboBox->setCurrentIndex(indice);
           ui.ctaBaseIVAlineEdit->setText(query.value(8).toString());

          }

}


void ed_iva_ret_cta::cta_IVA_finedicion()
{
    ui.cuentaIVAlineEdit->setText(expandepunto(ui.cuentaIVAlineEdit->text(),anchocuentas()));
}


void ed_iva_ret_cta::botonctaivapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentaIVAlineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.cuentaIVAlineEdit->setText(cadena2);
       else ui.cuentaIVAlineEdit->setText("");
    delete labusqueda;
}


void ed_iva_ret_cta::cta_IVA_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.cuentaIVAlineEdit->text(),&cadena))
    ui.descripctaIVAlabel->setText(cadena);
  else
    ui.descripctaIVAlabel->setText("");
}

void ed_iva_ret_cta::cta_ret_asig_finedicion()
{
    ui.cuentata_ret_asiglineEdit->setText(expandepunto(ui.cuentata_ret_asiglineEdit->text(),anchocuentas()));

}


void ed_iva_ret_cta::botonctaretpulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentata_ret_asiglineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.cuentata_ret_asiglineEdit->setText(cadena2);
       else ui.cuentata_ret_asiglineEdit->setText("");
    delete labusqueda;
}


void ed_iva_ret_cta::cta_ret_asig_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.cuentata_ret_asiglineEdit->text(),&cadena))
    ui.cta_ret_label->setText(cadena);
  else
    ui.cta_ret_label->setText("");
}

void ed_iva_ret_cta::guardar()
{
    /* "ivaasoc bool default 0,"
     "cuentaiva      varchar(40),"
     "tipoiva        varchar(20),"
     "conceptodiario varchar(155),"
     "cuenta_ret_asig varchar(40) default '',"
     "es_ret_asig_arrend bool "
     "tipo_ret_asig numeric(5,2) default 0,"
     "tipo_oper_ret_asig varchar(80)," */
    QString tipo_ret_asig=ui.porc_ret_asig_lineEdit->text();
    if (tipo_ret_asig.isEmpty()) tipo_ret_asig="0";
       else
            {
             if (convapunto(tipo_ret_asig).toDouble()<0.001) tipo_ret_asig="0";
                else tipo_ret_asig=convapunto(tipo_ret_asig);
            }
    basedatos::instancia()->actu_iva_ret_asignado_cta(
            codigo,
            ui.ivaasocgroupBox->isChecked(),
            ui.cuentaIVAlineEdit->text(),
            ui.tipoIVAcomboBox->currentText().section("//",0,0).trimmed(),
            ui.conceptodiariolineEdit->text(),
            ui.retgroupBox->isChecked() ? ui.cuentata_ret_asiglineEdit->text() : "",
            ui.es_ret_asigcheckBox->isChecked(),
            tipo_ret_asig,
            ui.tipo_oper_ret_asigcomboBox->currentText().left(1),
            ui.ctaBaseIVAlineEdit->text());

//    QMessageBox::warning( this, tr("DDD"),
//                      ui.tipo_oper_ret_asigcomboBox->currentText().left(1));

    accept();
}


void ed_iva_ret_cta::cta_base_IVA_finedicion()
{
    ui.ctaBaseIVAlineEdit->setText(expandepunto(ui.ctaBaseIVAlineEdit->text(),anchocuentas()));
}


void ed_iva_ret_cta::botoncta_base_ivapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.ctaBaseIVAlineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.ctaBaseIVAlineEdit->setText(cadena2);
       else ui.ctaBaseIVAlineEdit->setText("");
    delete labusqueda;
}


void ed_iva_ret_cta::cta_base_IVA_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.ctaBaseIVAlineEdit->text(),&cadena))
    ui.descripctaBaseIVAlabel->setText(cadena);
  else
    ui.descripctaBaseIVAlabel->setText("");
}
