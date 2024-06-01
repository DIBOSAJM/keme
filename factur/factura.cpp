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

#include "factura.h"
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include "aux_express.h"
#include "datos_accesorios.h"
#include "edita_ref.h"
#include "buscaref.h"
#include "introci.h"
#include <QMessageBox>
#include "busca_externo.h"
#include "externos.h"
#include <QString>

factura::factura() : QDialog() {
    ui.setupUi(this);

coma=haycomadecimal();
decimales=haydecimales();

ui.fechadateEdit->setDate(QDate::currentDate());
ui.fechaopdateEdit->setDate(QDate::currentDate());
ui.fecha_asiento_dateEdit->setDate(QDate::currentDate());

ui.doccomboBox->addItems(basedatos::instancia()->listacoddocs());
ret_cambiado();
QString cuenta; double tipo;
basedatos::instancia()->carga_irpf(&cuenta,&tipo);
ui.tipo_ret_lineEdit->setText(formateanumero(tipo,coma,true));

QString cta_ant=basedatos::instancia()->cuenta_anticipo();
ui.cta_anticipos_lineEdit->setText(cta_ant);
ui.cta_anticipos_lineEdit->setToolTip(descripcioncuenta(cta_ant));

esconde_cta_anticipo();

ui.facturae_groupBox->hide();

ui.textEdit->setMinimumWidth(850);

imprimir=false;

connect(ui.doccomboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT (actudoc()));

connect(ui.tableWidget,SIGNAL( cellChanged ( int , int )),this,
       SLOT(contenidocambiado(int,int )));

connect(ui.cta_cli_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada()));
connect(ui.cta_cli_lineEdit,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion()));
connect(ui.ctapushButton,SIGNAL(clicked()),this,SLOT(buscactacli()));
connect(ui.edctapushButton,SIGNAL(clicked()),SLOT(datoscuenta()));
connect(ui.ret_checkBox,SIGNAL(stateChanged(int)),SLOT(ret_cambiado()));
connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(aceptar()));

connect(ui.tableWidget,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
       SLOT(posceldacambiada ( int, int, int, int  )));

connect(ui.BotonBorraLinea,SIGNAL(  clicked()),this,SLOT(borralinea( void )));
connect(ui.arribapushButton,SIGNAL(clicked()),SLOT(muevearriba()));
connect(ui.abajopushButton ,SIGNAL(clicked()),SLOT(mueveabajo()));

connect(ui.pdfpushButton ,SIGNAL(clicked()),SLOT(gen_pdf()));

connect(ui.lineatextEdit,SIGNAL(textChanged()),SLOT(texto_linea_cambiado()));

connect(ui.pedido_lineEdit,SIGNAL(textChanged(QString)),SLOT(pedido_cambiado()));
connect(ui.albaran_lineEdit,SIGNAL(textChanged(QString)),SLOT(albaran_cambiado()));
connect(ui.expediente_lineEdit,SIGNAL(textChanged(QString)),SLOT(expediente_cambiado()));
connect(ui.contrato_lineEdit,SIGNAL(textChanged(QString)),SLOT(contrato_cambiado()));

connect(ui.ci_pushButton,SIGNAL(clicked()),SLOT(procesabotonci()));

connect(ui.oculta_notas_lin_checkBox,SIGNAL(stateChanged(int)),SLOT(check_oculta_notas_lin()));

connect(ui.fpagopushButton, SIGNAL(clicked(bool)),SLOT(forma_pago()));

connect(ui.cta_anticipos_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(cta_anticipo_cambiada()));
connect(ui.cta_anticipos_lineEdit,SIGNAL(editingFinished()),this,SLOT(cta_anticipo_finedicion()));
connect(ui.busca_cta_anticipos_pushButton,SIGNAL(clicked()),this,SLOT(buscacta_anticipo()));

connect(ui.busca_externo_pushButton,SIGNAL(clicked(bool)),SLOT(buscar_externo()));
connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),SLOT(campo_externo_cambiado()));
connect(ui.quita_externo_pushButton,SIGNAL(clicked(bool)),SLOT(borra_externo()));
connect(ui.datos_externo_pushButton,SIGNAL(clicked(bool)),SLOT(ver_externo()));

connect(ui.facturae_checkBox,SIGNAL(stateChanged(int)),SLOT(check_facturae()));

connect(ui.ul_lin_fe_pushButton,SIGNAL(clicked(bool)),SLOT(copia_lin_face()));
connect(ui.fechadateEdit,SIGNAL(dateChanged(QDate)),SLOT(fecha_factura_cambiada()));

tipo_operacion=0;

actudoc();

// construir una lista con tipo de iva, re, baseimponible
QSqlQuery query =basedatos::instancia()->selectTodoTiposivanoclave("");
if (query.isActive())
    while (query.next())
       {
         codigoiva << query.value(0).toString();
         tipoiva << query.value(1).toDouble()/100;
         tipore << query.value(2).toDouble()/100;
         baseimponible << 0;
       }
 modo_consulta=false;
 ui.tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
 ui.tableWidget->setColumnWidth(0,90);
 ui.tableWidget->setColumnWidth(1,250);
 ui.tableWidget->setColumnWidth(2,90);
 ui.tableWidget->setColumnWidth(3,100);
 ui.tableWidget->setColumnWidth(4,90);
 ui.tableWidget->setColumnWidth(7,90);
 predefinida=false;

 lista_suplidos=basedatos::instancia()->lista_ref_suplidos();

 if (!((conanalitica() || conanalitica_parc()) && !basedatos::instancia()->analitica_tabla()))
    {
     ui.tableWidget->hideColumn(5);
     ui.ci_pushButton->hide();
    }

 if (!basedatos::instancia()->hay_externos())
    {
     ui.externo_label->hide();
     ui.externo_lineEdit->hide();
     ui.descrip_externo_lineEdit->hide();
     ui.busca_externo_pushButton->hide();
     ui.datos_externo_pushButton->hide();
     ui.quita_externo_pushButton->hide();
    }

}

QString factura::ltipoiva(QString codigo)
{
  int pos=codigoiva.indexOf(codigo);
  double valor=0;
  if (pos>=0)
      valor=tipoiva.at(pos);
  valor=valor*100;
  return formateanumero(valor,coma,decimales);
}

QString factura::ltipore(QString codigo)
{
    int pos=codigoiva.indexOf(codigo);
    double valor=0;
    if (pos>=0)
        valor=tipore.at(pos);
    valor=valor*100;
    return formateanumero(valor,coma,decimales);

}

void factura::fijatipoiva(QString codigo, double tporuno)
{
    int pos=codigoiva.indexOf(codigo);
    if (pos>=0)
        tipoiva.replace(pos,tporuno);
      else
          {
            codigoiva << codigo;
            tipoiva << tporuno;
            tipore << 0;
            baseimponible << 0;
          }

}


void factura::fijatipore(QString codigo, double tporuno)
{
    int pos=codigoiva.indexOf(codigo);
    if (pos>=0)
        tipore.replace(pos,tporuno);

}



void factura::actudoc()
{
 // actualizar serie
 // actualizar nombre documento
 QString logo;
 QString documento, cantidad, referencia, descripref, precio, totallin, bi;
 QString tipoiva, tipore, cuota, cuotare, totalfac, notas, venci, notastex;
 QString cif_empresa, cif_cliente, msnumero, msfecha, mscliente, msdescuento;
 bool rectificativo;
 QString totallineas;
 QString suplidos;
 QString lineas_doc;
 QString nombre_emisor;
 QString domicilio_emisor;
 QString cp_emisor;
 QString poblacion_emisor;
 QString provincia_emisor;
 QString pais_emisor;
 QString cif_emisor;
 QString id_registral;
 QString fichreport;
 QString concepto_sii;

 QString fe_idioma, fe_libro, fe_registro, fe_hoja, fe_folio, fe_seccion, fe_volumen, fe_datos_adic;
 QString cadret;
 QString clave_donacion, comunidad_autonoma;
 bool donacion_especie=false;
 bool donacion_2ejer=false;
 double porcent_deduc_autonomia=0;

 basedatos::instancia()->carga_tipo_doc(ui.doccomboBox->currentText(),
                                   &descripfact,
                                   &seriefact,
                                   &pie1,
                                   &pie2,
                                   &moneda,
                                   &codigo_moneda,
                                   &contabilizable,
                                   &rectificativo,
                                   &tipo_operacion,
                                   &documento, &cantidad, &referencia, &descripref,
                                   &precio, &totallin, &bi,
                                   &tipoiva, &tipore, &cuota, &cuotare, &totalfac,
                                   &notas, &venci, &notastex, &cif_empresa, &cif_cliente,
                                   &msnumero, &msfecha, &mscliente, &msdescuento,
                                   &totallineas, &suplidos,&lineas_doc, &nombre_emisor,
                                   &domicilio_emisor, &cp_emisor, &poblacion_emisor,
                                   &provincia_emisor, &pais_emisor, &cif_emisor,
                                   &id_registral, &fichreport, &concepto_sii,
                                   &fe_idioma, &fe_libro, &fe_registro, &fe_hoja, &fe_folio, &fe_seccion, &fe_volumen,
                                   &fe_datos_adic, &cadret,
                                   &clave_donacion, &comunidad_autonoma, &donacion_especie, &donacion_2ejer, &porcent_deduc_autonomia,
                                   &logo);
 ui.serielineEdit->setText(seriefact);
 ui.documentolabel->setText(descripfact);
 ui.pie1lineEdit->setText(pie1);
 ui.pie2lineEdit->setText(pie2);
 ui.textEdit->setText(notastex);
 ui.concepto_sii_lineEdit->setText(concepto_sii);
 QPixmap foto;
 if (logo.length()>0)
       {
        QByteArray byteshexa;
        byteshexa.append ( logo.toUtf8() );
        QByteArray bytes;
        bytes=bytes.fromHex ( byteshexa );
        foto.loadFromData ( bytes, "PNG");
       }
 ui.logo_label->setPixmap(foto);

}




void factura::contenidocambiado(int fila,int columna)
{

 if (ui.tableWidget->currentColumn()!=columna || ui.tableWidget->currentRow()!=fila) return;
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
// Referencia
// Descripción
// Cantidad
// precio
 // total
 // iva
if (columna==0) {
    //
    for (int veces=1;veces<ui.tableWidget->columnCount();veces++)
         if (ui.tableWidget->item(fila,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3 || veces==4 || veces==6 || veces==8 || veces==9)
                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.tableWidget->setItem(fila,veces,newItem);
             }
             else if (veces==2)
                   ui.tableWidget->item(fila,veces)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //
    QString descrip, tipoiva, tipoivare, cuenta;
    bool suplido,anticipo,precision;
    double pvp;
    QString codigo=ui.tableWidget->item(fila,columna)->text();
    if (codigo.length()==1)
      {
        buscaref *b = new buscaref(codigo);
        int cod=b->exec();
        QString cadena2=b->seleccioncuenta();
        delete(b);
        if (cod==QDialog::Accepted )
           ui.tableWidget->item(fila,columna)->setText(cadena2);
        codigo=cadena2;

      }
    if (!basedatos::instancia()->existecodigoref (codigo,&descrip) && !codigo.isEmpty())
      {
        QString TextoAviso = tr("El código suministrado no existe \n"
                     "¿ desea añadir la referencia ?");
        msgBox.setText(TextoAviso);
        msgBox.exec();
        if (msgBox.clickedButton() == aceptarButton)
          {
            basedatos::instancia()->insertref(codigo,QString());
            edita_ref *e = new edita_ref();
            e->pasacodigo(codigo);
            e->exec();
            delete(e);
            lista_suplidos.clear();
            lista_suplidos=basedatos::instancia()->lista_ref_suplidos();
          }
      }
    if (basedatos::instancia()->existecodigoref (codigo,&descrip))
       {

        basedatos::instancia()->carga_ref(codigo,
                                   &descrip,
                                   &tipoiva,
                                   &tipoivare,
                                   &cuenta,
                                   &pvp,
                                   &suplido, &anticipo, &precision);
        // Referencia
        // Descripción
        // Cantidad
        // precio
         // total
         // iva
        ui.tableWidget->item(fila,1)->setText(descrip);
        ui.tableWidget->item(fila,2)->setText("1");
        QString cadpvp;
        if (precision) cadpvp=formateanumero_ndec(pvp,coma,4);
           else cadpvp=formateanumero(pvp,coma,decimales);
        ui.tableWidget->item(fila,3)->setText(cadpvp);
        //ui.tableWidget->item(fila,4)->setText("0");
        if (tipo_operacion>1) tipoiva=tipoivaexento();
        QString codigoiva=ui.recheckBox->isChecked() ? tipoivare : tipoiva;
        if (anticipo)
            muestra_cta_anticipo();
        if (suplido)
           {
            ui.tableWidget->item(fila,7)->setText(QString());
            ui.tableWidget->item(fila,8)->setText(QString());
            ui.tableWidget->item(fila,9)->setText(QString());
           }
           else
               {
                ui.tableWidget->item(fila,7)->setText(codigoiva);
                ui.tableWidget->item(fila,8)->setText(ltipoiva(codigoiva));
                ui.tableWidget->item(fila,9)->setText(ltipore(codigoiva));
               }
        double total;
        total=convapunto(ui.tableWidget->item(fila,2)->text()).toDouble() *
              convapunto(ui.tableWidget->item(fila,3)->text()).toDouble();
        if (precision)
          ui.tableWidget->item(fila,6)->setText(formateanumero_ndec(redond(total,4),coma,4));
         else
            ui.tableWidget->item(fila,6)->setText(formateanumero(redond(total,2),coma,decimales));

       }
       else ui.tableWidget->item(fila,0)->setText("");
   }

if (columna==2 || columna==3 || columna==4 || columna==5)
  {    
    if (ui.tableWidget->item(fila,0)==NULL)
     { ui.tableWidget->item(fila,columna)->setText(""); return; }

    if (ui.tableWidget->item(fila,0)->text().isEmpty())
     { ui.tableWidget->item(fila,columna)->setText(""); return; }
    if (
        ui.tableWidget->item(fila,1)==NULL ||
        ui.tableWidget->item(fila,2)==NULL ||
        ui.tableWidget->item(fila,3)==NULL ||
        ui.tableWidget->item(fila,4)==NULL ||
        ui.tableWidget->item(fila,6)==NULL  ) return;
   if (ui.tableWidget->item(fila,0)->text().isEmpty())
    { ui.tableWidget->item(fila,columna)->setText(""); return; }
   float numero=convapunto(ui.tableWidget->item(fila,columna)->text()).toDouble();
   if (columna==2 && coma)
       ui.tableWidget->item(fila,2)->setText(coma ?
                                             convacoma(ui.tableWidget->item(fila,columna)->text()) :
                                             convapunto(ui.tableWidget->item(fila,columna)->text()));
   // la referencia tiene doble precisión ?
   bool dobleprecision=false;
   if (ui.tableWidget->item(fila,0)!=NULL)
        dobleprecision=basedatos::instancia()->ref_precision(ui.tableWidget->item(fila,0)->text());

   if (columna==3)
      {
       if (dobleprecision)
         ui.tableWidget->item(fila,3)->setText(formateanumero_ndec(numero,coma,4));
         else
           ui.tableWidget->item(fila,3)->setText(formateanumero(numero,coma,decimales));

      }
   if (columna==4)
       ui.tableWidget->item(fila,4)->setText(formateanumero(numero,coma,decimales));
    double total;
    int ndecimales=2;
    if (dobleprecision) ndecimales=4;
    total=redond(convapunto(ui.tableWidget->item(fila,2)->text()).toDouble() *
          convapunto(ui.tableWidget->item(fila,3)->text()).toDouble() *
          (1-convapunto(ui.tableWidget->item(fila,4)->text()).toDouble()/100),ndecimales);
    ui.tableWidget->item(fila,6)->setText(formateanumero_ndec(total,coma,ndecimales));
  }

if (columna==7 || columna==8 || columna==9)
 {
  // No admitimos cambios en tipo de IVA
    if (ui.tableWidget->item(fila,0)==NULL) return;
    QString descrip, tipoiva, tipoivare, cuenta;
    double pvp;
    bool suplido,anticipo,precision;
    QString codigo=ui.tableWidget->item(fila,0)->text();
    if (basedatos::instancia()->existecodigoref (codigo,&descrip))
       {

        basedatos::instancia()->carga_ref(codigo,
                                   &descrip,
                                   &tipoiva,
                                   &tipoivare,
                                   &cuenta,
                                   &pvp,
                                   &suplido, &anticipo, &precision);
        // Referencia
        // Descripción
        // Cantidad
        // precio
         // total
         // iva
        if (tipo_operacion>1) tipoiva=tipoivaexento();
        QString codigoiva=ui.recheckBox->isChecked() ? tipoivare : tipoiva;
        if (suplido)
           {
            ui.tableWidget->item(fila,7)->setText(QString());
            ui.tableWidget->item(fila,8)->setText(QString());
            ui.tableWidget->item(fila,9)->setText(QString());
           }
           else
               {
                ui.tableWidget->item(fila,7)->setText(codigoiva);
                ui.tableWidget->item(fila,8)->setText(ltipoiva(codigoiva));
                ui.tableWidget->item(fila,9)->setText(ltipore(codigoiva));
               }
       }
    else ui.tableWidget->item(fila,7)->setText("");

 }


actualizatotales();

// QMessageBox::warning( this, tr("facturas"),tr("PASAMOS DE totales"));

if (ui.tableWidget->currentColumn()==0)
   {
    if (ui.tableWidget->item(ui.tableWidget->currentRow(),0)==NULL) return;
    if (ui.tableWidget->item(ui.tableWidget->currentRow(),2)==NULL) return;
    ui.tableWidget->setCurrentCell(ui.tableWidget->currentRow(),2);
    return;
   }

if (ui.tableWidget->currentRow()>ui.tableWidget->rowCount()-2) return;

ui.tableWidget->disconnect(SIGNAL(currentCellChanged(int,int,int,int)));
ui.tableWidget->setCurrentCell(ui.tableWidget->currentRow()+1,0);
connect(ui.tableWidget,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
       SLOT(posceldacambiada ( int, int, int, int  )));

return;

}


void factura::actualizatotales()
{
    double base, cuotaIVA, cuotaRE, suplidos;
    base=0;
    cuotaIVA=0;
    cuotaRE=0;
    suplidos=0;
    for (int veces=0; veces<baseimponible.size(); veces++) baseimponible[veces]=0;
    for (int veces=0;veces<ui.tableWidget->rowCount();veces++)
       {        
        if (ui.tableWidget->item(veces,0)==NULL) continue;        
        if (ui.tableWidget->item(veces,6)==NULL) continue; // total
        if (lista_suplidos.contains(ui.tableWidget->item(veces,0)->text()) &&
                !ui.tableWidget->item(veces,0)->text().isEmpty())
           {
            suplidos+=convapunto(ui.tableWidget->item(veces,6)->text()).toDouble();
            continue;
           }
        if (ui.tableWidget->item(veces,7)==NULL) continue;
        // actualizar listas con baseimponible
        bool encontrado=false;
        for (int veces2=0; veces2<baseimponible.size(); veces2++)
           {
            if (codigoiva.at(veces2)==ui.tableWidget->item(veces,7)->text())
              {
                baseimponible[veces2]=baseimponible.at(veces2)+
                                      convapunto(ui.tableWidget->item(veces,6)->text()).toDouble();
                base+=convapunto(ui.tableWidget->item(veces,6)->text()).toDouble();
                cuotaIVA+=convapunto(ui.tableWidget->item(veces,6)->text()).toDouble()*tipoiva.at(veces2);
                cuotaRE+=convapunto(ui.tableWidget->item(veces,6)->text()).toDouble()*tipore.at(veces2);
                encontrado=true;
              }
           }
        // si no encontramos, suponemos IVA exento
        if (!encontrado) base+=convapunto(ui.tableWidget->item(veces,6)->text()).toDouble();
       }
    gsuplidos=suplidos;
    ui.bilineEdit->setText(formateanumero(redond(base,2),coma,decimales));
    ui.cuotaIVAlineEdit->setText(formateanumero(redond(cuotaIVA,2),coma,decimales));
    ui.cuotaRElineEdit->setText(formateanumero(redond(cuotaRE,2),coma,decimales));
    ui.totallineEdit->setText(formateanumero(redond(base+cuotaIVA+cuotaRE+suplidos,2),coma,decimales));
    double tipo=convapunto(ui.tipo_ret_lineEdit->text()).toDouble();
    if (ui.ret_checkBox->isChecked())
      {
       ui.ret_lineEdit->setText(formateanumero(redond(tipo*base/100,2),coma,decimales));
       ui.totallineEdit->setText(formateanumero(redond(suplidos+base+cuotaIVA+cuotaRE-tipo*base/100,2),coma,decimales));
      }
}


void factura::ctabasecambiada()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

    QString cadena;
    if (ui.cta_cli_lineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
        if (!existecodigoplan(ui.cta_cli_lineEdit->text(),&cadena))
          {
         // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
                 {
                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.cta_cli_lineEdit->text());
                   lasubcuenta2->exec();
                   delete(lasubcuenta2);
                   if (!existecodigoplan(ui.cta_cli_lineEdit->text(),&cadena))
                        ui.cta_cli_lineEdit->setText("");
                 }
                  else
                    ui.cta_cli_lineEdit->setText("");
           }
        }
   if (esauxiliar(ui.cta_cli_lineEdit->text()))
       ui.clientelabel->setText(descripcioncuenta(ui.cta_cli_lineEdit->text()));
     else ui.clientelabel->setText("");
}

void factura::ctabasefinedicion()
{
  QString cadena;
  ui.cta_cli_lineEdit->setText(expandepunto(ui.cta_cli_lineEdit->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
  if (ui.cta_cli_lineEdit->text().length()>3 && cod_longitud_variable()) {
        if (!existecodigoplan(ui.cta_cli_lineEdit->text(),&cadena))
          {
         // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
                 {
                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.cta_cli_lineEdit->text());
                   lasubcuenta2->exec();
                   delete(lasubcuenta2);
                   if (!esauxiliar(ui.cta_cli_lineEdit->text()))
                        ui.cta_cli_lineEdit->setText("");
                 }
                  else
                    ui.cta_cli_lineEdit->setText("");
           }
        if (esauxiliar(ui.cta_cli_lineEdit->text()))
          ui.clientelabel->setText(descripcioncuenta(ui.cta_cli_lineEdit->text()));
         else ui.clientelabel->setText("");
        }

}


void factura::buscactacli()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cta_cli_lineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.cta_cli_lineEdit->setText(cadena2);
       else ui.cta_cli_lineEdit->setText("");
    delete(labusqueda);
}


void factura::datoscuenta()
{
    QString cuenta=ui.cta_cli_lineEdit->text();
    if (cuenta.isEmpty()) return;
    if (!existesubcuenta(cuenta)) return;
    datos_accesorios *d = new datos_accesorios();
    d->cargacodigo( cuenta );
    d->exec();
    delete(d);
}


void factura::ret_cambiado()
{
    if (ui.ret_checkBox->isChecked())
       {
        ui.ret_label->setVisible(true);
        ui.tipo_ret_lineEdit->setVisible(true);
        ui.ret_lineEdit->setVisible(true);
        ui.porcent_label->setVisible(true);
       }
      else
          {
           ui.ret_label->setVisible(false);
           ui.tipo_ret_lineEdit->setVisible(false);
           ui.ret_lineEdit->setVisible(false);
           ui.porcent_label->setVisible(false);
          }
    actualizatotales();
    /*
 double base=convapunto(ui.bilineEdit->text()).toDouble();
 double cuotaIVA=convapunto(ui.cuotaIVAlineEdit->text()).toDouble();
 double cuotaRE=convapunto(ui.cuotaRElineEdit->text()).toDouble();
 if (ui.ret_checkBox->isChecked())
    {
     double tipo=convapunto(ui.tipo_ret_lineEdit->text()).toDouble();
     ui.ret_lineEdit->setText(formateanumero(tipo*base/100,coma,decimales));
     ui.totallineEdit->setText(formateanumerosep(base+cuotaIVA+cuotaRE-tipo*base/100,coma,decimales));
     ui.ret_label->setVisible(true);
     ui.tipo_ret_lineEdit->setVisible(true);
     ui.ret_lineEdit->setVisible(true);
     ui.porcent_label->setVisible(true);
    }
   else
       {
        ui.totallineEdit->setText(formateanumerosep(base+cuotaIVA+cuotaRE,coma,decimales));
        ui.ret_label->setVisible(false);
        ui.tipo_ret_lineEdit->setVisible(false);
        ui.ret_lineEdit->setVisible(false);
        ui.porcent_label->setVisible(false);
       }
*/
}

void factura::latexdoc(QString serie, QString numero)
{

    QString fichero=latex_doc(  serie,numero);
    if (fichero.isEmpty())
    {
     QMessageBox::warning( this, tr("Edición de documentos"),
                           tr("ERROR: No se ha podido generar Latex"));
     return;
    }

    if (!genera_pdf_latex(fichero))
       {
        QMessageBox::information( this, tr("FACTURA EN PDF"),
                                  tr("ERROR: no ha sido posible generar el archivo PDF"));
        return;
       }
    QString archivopdf=fichero;
    archivopdf.truncate(archivopdf.length()-4);
    archivopdf.append(".pdf");
    QString url=archivopdf;
    QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

    /* if (!ejecuta(aplicacionabrirfich(extensionfich(archivopdf)),archivopdf))
          QMessageBox::warning( this, tr("FACTURA EN PDF"),
                              tr("No se puede abrir ")+archivopdf+tr(" con ")+
                              aplicacionabrirfich(extensionfich(archivopdf)) + "\n" +
                              tr("Verifique el diálogo de preferencias")); */

}

bool factura::imprime_informe(QString *serie, QString *factura) {
    *serie=ui.serielineEdit->text();
    *factura=ui.facturalineEdit->text();
    return imprimir;
}


void factura::terminar(bool impri)
{

    if (predefinida)
       {
        guarda_predef();
        accept();
        return;
       }

    if (modo_consulta)
     {
       if (impri) imprimir=true; //latexdoc(ui.serielineEdit->text(), ui.facturalineEdit->text());
       accept();
       return;
     }
    // si el campo serielineEdit está vacío, mensaje y no permitir la grabación


    if (!existesubcuenta(ui.cta_cli_lineEdit->text()))
    {
        QMessageBox::warning( this, tr("GUARDAR FACTURA"),
                            tr("ERROR: Es necesario suministrar un código de cuenta correcto"));
        return;

    }

    // Comenzamos transacción: tablas series_fact, facturas, detalle_fact (añadir)
    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();

    basedatos::instancia()->bloquea_para_facturas();

   // Está el campo facturalineEdit vacío ?
   //
   //   Sí --->  aumentamos campo de número (tabla series_fact) y asignamos a facturalineEdit;
    //           incrementamos valor campo proxnum, para código de serie (codigo) en series_fact

   if (ui.facturalineEdit->text().isEmpty())
    {
     qlonglong numero=basedatos::instancia()->proxnum_serie(ui.serielineEdit->text());
     QString cadnum; cadnum.setNum(numero);
     ui.facturalineEdit->setText(cadnum);
    }

   // comprobamos si existe cabecera de factura
    long clave_cabecera;
    QString cadsuplidos;
    cadsuplidos.setNum(gsuplidos,'f',2);
    if (basedatos::instancia()->existe_fact(ui.serielineEdit->text(), ui.facturalineEdit->text()))
       {
        // actualizamos
        clave_cabecera=basedatos::instancia()->modificacabecerafactura(
                                            ui.serielineEdit->text(),
                                            ui.facturalineEdit->text(),
                                            ui.cta_cli_lineEdit->text(),
                                            ui.fecha_asiento_dateEdit->date(),
                                            ui.fechadateEdit->date(),
                                            ui.fechaopdateEdit->date(),
                                            false, // contabilizado
                                            contabilizable,
                                            ui.ret_checkBox->isChecked(),
                                            ui.recheckBox->isChecked(),
                                            convapunto(ui.tipo_ret_lineEdit->text()),
                                            convapunto(ui.ret_lineEdit->text()),
                                            ui.doccomboBox->currentText(),
                                            ui.textEdit->toPlainText(),
                                            ui.pie1lineEdit->text(),
                                            ui.pie2lineEdit->text(),
                                            "0",
                                            ui.cta_anticipos_lineEdit->text(),
                                            ui.externo_lineEdit->text(),
                                            ui.concepto_sii_lineEdit->text(),
                                            ui.rol1_lineEdit->text(),
                                            ui.rol2_lineEdit->text(),
                                            ui.rol3_lineEdit->text(),
                                            cadsuplidos,
                                            convapunto(ui.totallineEdit->text())
                                            );
        // borramos registros de detalle
        basedatos::instancia()->borralineas_doc(clave_cabecera);
       }
      else
         {
          // insertamos
          clave_cabecera=basedatos::instancia()->nuevacabecerafactura(
                  ui.serielineEdit->text(),
                  ui.facturalineEdit->text(),
                  ui.cta_cli_lineEdit->text(),
                  ui.fecha_asiento_dateEdit->date(),
                  ui.fechadateEdit->date(),
                  ui.fechaopdateEdit->date(),
                  false, // contabilizado
                  contabilizable,
                  ui.ret_checkBox->isChecked(),
                  ui.recheckBox->isChecked(),
                  convapunto(ui.tipo_ret_lineEdit->text()),
                  convapunto(ui.ret_lineEdit->text()),
                  ui.doccomboBox->currentText(),
                  ui.textEdit->toPlainText(),
                  ui.pie1lineEdit->text(),
                  ui.pie2lineEdit->text(),
                  "0",
                  ui.cta_anticipos_lineEdit->text(),
                  ui.externo_lineEdit->text(),
                  ui.concepto_sii_lineEdit->text(),
                      ui.rol1_lineEdit->text(),
                      ui.rol2_lineEdit->text(),
                      ui.rol3_lineEdit->text(),
                      cadsuplidos,
                      convapunto(ui.totallineEdit->text())
                     );

         }
    // añadimos registros de detalle
    int ultimo=ultimafila();
    for (int veces=0; veces<ultimo; veces++)
       {
        if (ui.tableWidget->item(veces,0)==NULL) continue;
        QString tipo,re;
        cadvalorestipoiva(ui.tableWidget->item(veces,7)->text(), &tipo, &re);
        basedatos::instancia()->insert_lin_doc(clave_cabecera,
                                       ui.tableWidget->item(veces,0)->text(),   // codigo
                                       ui.tableWidget->item(veces,1)->text(),   // descrip
                                       convapunto(ui.tableWidget->item(veces,2)->text()),   // cantidad
                                       convapunto(ui.tableWidget->item(veces,3)->text()),   // precio
                                       ui.tableWidget->item(veces,7)->text(), // clave_iva
                                       tipo, // tipo_iva
                                       re, // recargo equivalencia
                                       convapunto(ui.tableWidget->item(veces,4)->text()), //dto
                                       ui.tableWidget->item(veces,5)->text(),
                                       texto_linea[veces], pedido_linea[veces], albaran_linea[veces],
                                       expediente_linea[veces], contrato_linea[veces]);
       }
    // terminamos
    basedatos::instancia()->desbloquea_y_commit();
    if (impri) imprimir=true; //latexdoc(ui.serielineEdit->text(), ui.facturalineEdit->text());
    accept();

}

void factura::aceptar()
{
    terminar(false); // no pdf
}


void factura::gen_pdf()
{
    terminar(true); // pdf
}


void factura::pasa_cabecera_doc(QString serie,
                       QString numero,
                       QString cuenta,
                       QDate fecha_fac,
                       QDate fecha_asiento,
                       QDate fecha_op,
                       bool contabilizable,
                       bool con_ret,
                       bool re,
                       QString tipo_ret,
                       QString retencion,
                       QString tipo_doc,
                       QString notas,
                       QString pie1,
                       QString pie2,
                       QString anticipos,
                       QString externo,
                       QString concepto_sii, QString c_a_rol1, QString c_a_rol2, QString c_a_rol3)
{
    bool esfacturae=false;
    if (!c_a_rol1.isEmpty() || !c_a_rol2.isEmpty() || !c_a_rol3.isEmpty()) esfacturae=true;
    if (esfacturae) ui.facturae_checkBox->setChecked(true);
    for (int veces=0; veces<ui.doccomboBox->count(); veces++)
        {
         if (ui.doccomboBox->itemText(veces)==tipo_doc)
            {
             ui.doccomboBox->setCurrentIndex(veces);
             break;
            }
        }

    ui.serielineEdit->setText(serie);
    ui.facturalineEdit->setText(numero);
    ui.externo_lineEdit->setText(externo);
    ui.cta_cli_lineEdit->setText(cuenta);
    ui.fechadateEdit->setDate(fecha_fac);
    ui.fecha_asiento_dateEdit->setDate(fecha_asiento);
    ui.fechaopdateEdit->setDate(fecha_op);
    contabilizable=contabilizable;
    ui.ret_checkBox->setChecked(con_ret);
    ui.recheckBox->setChecked(re);
    ui.concepto_sii_lineEdit->setText(concepto_sii);
    ui.rol1_lineEdit->setText(c_a_rol1);
    ui.rol2_lineEdit->setText(c_a_rol2);
    ui.rol3_lineEdit->setText(c_a_rol3);

    if (tipo_ret.toFloat()<0.0001 && tipo_ret.toFloat()>-0.001)
       {
        double qtipo;
        basedatos::instancia()->carga_irpf(&cuenta,&qtipo);
        ui.tipo_ret_lineEdit->setText(formateanumero(qtipo,coma,true));
        ui.ret_lineEdit->setText(coma ? convacoma(0) : 0);
       }
      else
     {
      ui.tipo_ret_lineEdit->setText(coma ? convacoma(tipo_ret) : tipo_ret);
      ui.ret_lineEdit->setText(coma ? convacoma(retencion) : retencion);
     }
    ui.textEdit->setText(notas);
    ui.pie1lineEdit->setText(pie1);
    ui.pie2lineEdit->setText(pie2);
    ui.cta_anticipos_lineEdit->setText(anticipos);

    ui.doccomboBox->setEnabled(false);

    // averiguamos clave
    int clave=basedatos::instancia()->clave_doc_fac(serie,numero);

    QSqlQuery q;
    q = basedatos::instancia()->select_lin_doc(clave);
    int veces=0;
    if (q.isActive())
       while (q.next())
        {
        for (int veces2=0;veces2<ui.tableWidget->columnCount();veces2++)
             if (ui.tableWidget->item(veces,veces2)==0)
                 {
                  QTableWidgetItem *newItem = new QTableWidgetItem("");
                  if (veces2==2 || veces2==3 || veces2==4 || veces2==6)
                      newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.tableWidget->setItem(veces,veces2,newItem);
                 }
        // clave, codigo, descripcion, cantidad, precio, clave_iva, tipo_iva, tipo_re, dto
        ui.tableWidget->item(veces,0)->setText(q.value(1).toString());

        // la referencia es de doble precisión ?
        bool dobleprecision=false;
        if (basedatos::instancia()->ref_precision(q.value(1).toString())) dobleprecision=true;
        if (basedatos::instancia()->ref_con_anticipo(q.value(1).toString()))
            muestra_cta_anticipo();

        ui.tableWidget->item(veces,1)->setText(q.value(2).toString());
        ui.tableWidget->item(veces,2)->setText(coma ? convacoma(q.value(3).toString())
            : q.value(3).toString());
        double cantidad=q.value(3).toDouble();
        double precio=q.value(4).toDouble();
        double totalneto=redond(cantidad*precio*(1-q.value(8).toDouble()/100),2);
        if (dobleprecision) totalneto=redond(cantidad*precio*(1-q.value(8).toDouble()/100),4);
        if (dobleprecision)
           ui.tableWidget->item(veces,3)->setText(formateanumero_ndec(q.value(4).toDouble(),coma,4));
          else
            ui.tableWidget->item(veces,3)->setText(formateanumero(q.value(4).toDouble(),coma,decimales));
        ui.tableWidget->item(veces,4)->setText(formateanumero(q.value(8).toDouble(),coma,decimales));
        if (dobleprecision)
           ui.tableWidget->item(veces,6)->setText(formateanumero_ndec(totalneto,coma,4));
          else
            ui.tableWidget->item(veces,6)->setText(formateanumero(totalneto,coma,decimales));

        ui.tableWidget->item(veces,7)->setText(q.value(5).toString());
        // tipo iva y re lugares 7 y 8
        ui.tableWidget->item(veces,8)->setText(formateanumero(q.value(6).toDouble(),
                                                              coma,decimales));
       //  QMessageBox::warning( this, tr("facturas"),formateanumero(q.value(6).toDouble(),
       //                                                           coma,decimales));

        ui.tableWidget->item(veces,9)->setText(formateanumero(q.value(7).toDouble(),
                                                              coma,decimales));
        texto_linea[veces]=q.value(9).toString();
        pedido_linea[veces]=q.value(11).toString();
        albaran_linea[veces]=q.value(12).toString();
        expediente_linea[veces]=q.value(13).toString();
        contrato_linea[veces]=q.value(14).toString();

        if (!pedido_linea[veces].isEmpty() || !albaran_linea[veces].isEmpty() ||
                !expediente_linea[veces].isEmpty() || !contrato_linea[veces].isEmpty())
           ui.facturae_checkBox->setChecked(true);

        ui.tableWidget->item(veces,5)->setText(q.value(10).toString());
        // actualizar listas con tipos de iva y re
        fijatipoiva(q.value(5).toString(),q.value(6).toDouble()/100);
        fijatipore(q.value(5).toString(),q.value(7).toDouble()/100);
        veces++;

        }
    ui.tableWidget->setCurrentCell(0,0);
    ui.lineatextEdit->setText(texto_linea[0]);
    actualizatotales();
}


void factura::solo_cabecera_doc(QString serie,
                       QString numero,
                       QString cuenta,
                       QDate fecha_fac,
                       QDate fecha_asiento,
                       QDate fecha_op,
                       bool contabilizable,
                       bool con_ret,
                       bool re,
                       QString tipo_ret,
                       QString retencion,
                       QString tipo_doc
                       )
{
    ui.serielineEdit->setText(serie);
    ui.facturalineEdit->setText(numero);
    ui.cta_cli_lineEdit->setText(cuenta);
    ui.fecha_asiento_dateEdit->setDate(fecha_asiento);
    ui.fechadateEdit->setDate(fecha_fac);
    ui.fechaopdateEdit->setDate(fecha_op);
    contabilizable=contabilizable;
    ui.ret_checkBox->setChecked(con_ret);
    ui.recheckBox->setChecked(re);
    ui.tipo_ret_lineEdit->setText(coma ? convacoma(tipo_ret) : tipo_ret);
    ui.ret_lineEdit->setText(coma ? convacoma(retencion) : retencion);
    for (int veces=0; veces<ui.doccomboBox->count(); veces++)
        {
         if (ui.doccomboBox->itemText(veces)==tipo_doc)
            {
             ui.doccomboBox->setCurrentIndex(veces);
             break;
            }
        }

    ui.doccomboBox->setEnabled(false);
    actudoc();
}


int factura::ultimafila()
{
  int veces=0;
  while (veces< ui.tableWidget->rowCount ())
    {
      if (ui.tableWidget->item(veces,0)==0) break;
      if (ui.tableWidget->item(veces,0)->text().length()==0) break;
      veces++;
    }
  if (veces==ui.tableWidget->rowCount()) return veces-1;
 return veces;
}


void factura::posceldacambiada ( int row, int column, int prevrow, int prevcol )
{
   contenidocambiado(prevrow,prevcol);
   int ultima=ultimafila();
   /*if (row<=ultima)
      {
       ui.lineatextEdit->setText(texto_linea[row]);
       return;
      }*/
    if (ui.tableWidget->item(row,0)==0) ui.tableWidget->setCurrentCell(ultima,0);
      else
          if (ui.tableWidget->item(row,0)->text().length()==0)
                ui.tableWidget->setCurrentCell(ultima,0);
    ui.lineatextEdit->setText(texto_linea[row]);

    ui.pedido_lineEdit->setText(pedido_linea[row]);
    ui.albaran_lineEdit->setText(albaran_linea[row]);
    ui.expediente_lineEdit->setText(expediente_linea[row]);
    ui.contrato_lineEdit->setText(contrato_linea[row]);
}


void factura::borralinea( void )
{
 int fila=ui.tableWidget->currentRow();
 ui.tableWidget->removeRow(fila);

 for (int veces=fila;veces<99;veces++)
   {
     texto_linea[veces]=texto_linea[veces+1];
     pedido_linea[veces]=pedido_linea[veces+1];
     albaran_linea[veces]=albaran_linea[veces+1];
     expediente_linea[veces]=expediente_linea[veces+1];
     contrato_linea[veces]=contrato_linea[veces+1];
   }
 texto_linea[99].clear();

 actualizatotales();
}

void factura::muevearriba()
{
  if (ui.tableWidget->currentRow()==0) return;

  ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
  ui.tableWidget->disconnect(SIGNAL(  currentCellChanged ( int , int, int, int )));
  // guardamos contenidos de la fila superior
  QString cols[ui.tableWidget->columnCount()];
  int pos=ui.tableWidget->currentRow();
  if (ui.tableWidget->item(pos,0)==0) return;

  if (ui.tableWidget->item(pos,0)->text().isEmpty()) return;

  for (int veces=0;veces<ui.tableWidget->columnCount();veces++)
  {
    cols[veces]=ui.tableWidget->item(pos-1,veces)->text();
  }
  QString guardatexto=texto_linea[pos-1];

  // asignamos valores a la línea previa
  for (int veces=0;veces<ui.tableWidget->columnCount();veces++)
     ui.tableWidget->item(pos-1,veces)->setText(
                ui.tableWidget->item(pos,veces)->text());
  texto_linea[pos-1]=texto_linea[pos];

  // asignamos valores a la línea siguiente
  for (int veces=0;veces<ui.tableWidget->columnCount();veces++)
     ui.tableWidget->item(pos,veces)->setText(cols[veces]);
  texto_linea[pos]=guardatexto;

  ui.lineatextEdit->setText(texto_linea[ui.tableWidget->currentRow()]);

  connect(ui.tableWidget,SIGNAL( cellChanged ( int , int )),this,
     SLOT(contenidocambiado(int,int )));
  connect(ui.tableWidget,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(posceldacambiada ( int, int, int, int  )));

  int fila=pos-1;
  ui.tableWidget->setCurrentCell(fila,0);
}



// *************************************************************************************
void factura::mueveabajo()
{
    if (ui.tableWidget->currentRow()>=ui.tableWidget->rowCount()-1) return;
    if (ui.tableWidget->item(ui.tableWidget->currentRow()+1,0)==0) return;
        else if (ui.tableWidget->item(ui.tableWidget->currentRow()+1,0)->text().length()==0) return;
    if (ui.tableWidget->item(ui.tableWidget->currentRow(),0)==0) return;

  // guardamos contenidos de la fila inferior
  QString cols[ui.tableWidget->columnCount()];

  for (int veces=0;veces<ui.tableWidget->columnCount();veces++)
    cols[veces]=ui.tableWidget->item(ui.tableWidget->currentRow()+1,veces)->text();

  QString guardatexto=texto_linea[ui.tableWidget->currentRow()+1];

  ui.tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
  ui.tableWidget->disconnect(SIGNAL(  currentCellChanged ( int , int, int, int )));

  // asignamos valores a la fila inferior
  for (int veces=0;veces<ui.tableWidget->columnCount();veces++)
    ui.tableWidget->item(ui.tableWidget->currentRow()+1,veces)->setText(
                ui.tableWidget->item(ui.tableWidget->currentRow(),veces)->text());

  texto_linea[ui.tableWidget->currentRow()+1]=texto_linea[ui.tableWidget->currentRow()];

  // asignamos valores (guardados) a la fila en curso
  for (int veces=0;veces<ui.tableWidget->columnCount();veces++)
    ui.tableWidget->item(ui.tableWidget->currentRow(),veces)->setText(cols[veces]);

  texto_linea[ui.tableWidget->currentRow()]=guardatexto;

  ui.lineatextEdit->setText(texto_linea[ui.tableWidget->currentRow()]);

  connect(ui.tableWidget,SIGNAL( cellChanged ( int , int )),this,
     SLOT(contenidocambiado(int,int )));
  connect(ui.tableWidget,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(posceldacambiada ( int, int, int, int  )));

  int fila=ui.tableWidget->currentRow()+1;
  ui.tableWidget->setCurrentCell(fila,0);

}


void factura::carga_lineas_doc(QString serie,
                       QString numero)
{
    // averiguamos clave
    int clave=basedatos::instancia()->clave_doc_fac(serie,numero);

    QSqlQuery q;
    q = basedatos::instancia()->select_lin_doc(clave);
    int veces=ultimafila();
    // if (veces!=0) veces++;
    if (q.isActive())
       while (q.next())
        {
        for (int veces2=0;veces2<ui.tableWidget->columnCount();veces2++)
             if (ui.tableWidget->item(veces,veces2)==0)
                 {
                  QTableWidgetItem *newItem = new QTableWidgetItem("");
                  if (veces2==2 || veces2==3 || veces2==4 || veces2==6)
                      newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.tableWidget->setItem(veces,veces2,newItem);
                 }
        // clave, codigo, descripcion, cantidad, precio, clave_iva, tipo_iva, tipo_re, dto
        if (basedatos::instancia()->ref_con_anticipo(q.value(1).toString()))
            muestra_cta_anticipo();
        bool dobleprecision=false;
        if (basedatos::instancia()->ref_precision(q.value(1).toString()))
            dobleprecision=true;
        ui.tableWidget->item(veces,0)->setText(q.value(1).toString());
        ui.tableWidget->item(veces,1)->setText(q.value(2).toString());
        QString qcantidad=q.value(3).toString();
        if (qcantidad.contains(".000")) qcantidad.remove(".000");
        ui.tableWidget->item(veces,2)->setText(coma ? convacoma(qcantidad)
            : qcantidad);
        double cantidad=q.value(3).toDouble();
        double precio=q.value(4).toDouble();
        double totalneto=redond(cantidad*precio*(1-q.value(8).toDouble()/100),2);
        if (dobleprecision)
            ui.tableWidget->item(veces,3)->setText(formateanumero_ndec(q.value(4).toDouble(),coma,4));
           else
            ui.tableWidget->item(veces,3)->setText(formateanumero(q.value(4).toDouble(),coma,decimales));
        ui.tableWidget->item(veces,4)->setText(formateanumero(q.value(8).toDouble(),coma,decimales));
        if (dobleprecision)
           ui.tableWidget->item(veces,6)->setText(formateanumero_ndec(totalneto,coma,4));
          else
            ui.tableWidget->item(veces,6)->setText(formateanumero(totalneto,coma,decimales));
        ui.tableWidget->item(veces,7)->setText(q.value(5).toString());
        // tipo iva y re lugares 7 y 8
        ui.tableWidget->item(veces,8)->setText(formateanumero(q.value(6).toDouble(),
                                                              coma,decimales));
       //  QMessageBox::warning( this, tr("facturas"),formateanumero(q.value(6).toDouble(),
       //                                                           coma,decimales));

        ui.tableWidget->item(veces,9)->setText(formateanumero(q.value(7).toDouble(),
                                                              coma,decimales));
        texto_linea[veces]=q.value(9).toString();
        ui.tableWidget->item(veces,5)->setText(q.value(10).toString());
        // actualizar listas con tipos de iva y re
        fijatipoiva(q.value(5).toString(),q.value(6).toDouble()/100);
        fijatipore(q.value(5).toString(),q.value(7).toDouble()/100);

        veces++;

        }
    actualizatotales();
    ui.tableWidget->setCurrentCell(0,0);
}


void factura::addnotas(QString notas)
{
    ui.textEdit->append("\n");
    ui.textEdit->append(notas);
}

void factura::activa_consulta()
{
  setWindowTitle(tr("DOCUMENTO: MODO CONSULTA"));
  modo_consulta=true;
  ui.doccomboBox->setEnabled(false);
  ui.serielineEdit->setReadOnly(true);
  ui.fechadateEdit->setReadOnly(true);
  ui.fechaopdateEdit->setReadOnly(true);
  ui.fecha_asiento_dateEdit->setReadOnly(true);
  ui.cta_cli_lineEdit->setReadOnly(true);
  ui.ctapushButton->setEnabled(false);
  ui.ret_checkBox->setEnabled(false);
  ui.recheckBox->setEnabled(false);
  ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui.BotonBorraLinea->setEnabled(false);
  ui.abajopushButton->setEnabled(false);
  ui.arribapushButton->setEnabled(false);
  ui.textEdit->setReadOnly(true);
  ui.pie1lineEdit->setReadOnly(true);
  ui.pie2lineEdit->setReadOnly(true);
  ui.ci_pushButton->setEnabled(false);
  ui.lineatextEdit->setReadOnly(true);
  ui.fpagopushButton->setEnabled(false);
  ui.cta_anticipos_lineEdit->setEnabled(false);
  ui.busca_cta_anticipos_pushButton->setEnabled(false);

  ui.busca_externo_pushButton->setEnabled(false);
  ui.externo_lineEdit->setReadOnly(true);
  ui.quita_externo_pushButton->setEnabled(false);

  ui.concepto_sii_lineEdit->setReadOnly(true);
  ui.facturae_checkBox->setEnabled(false);

  ui.rol1_lineEdit->setReadOnly(true);
  ui.rol2_lineEdit->setReadOnly(true);
  ui.rol3_lineEdit->setReadOnly(true);

  ui.pedido_lineEdit->setReadOnly(true);
  ui.albaran_lineEdit->setReadOnly(true);
  ui.expediente_lineEdit->setReadOnly(true);
  ui.contrato_lineEdit->setReadOnly(true);

}


void factura::pasa_fact_predef(QString codigo, QString descrip,
                               QString serie,
                               bool contabilizable,
                               bool con_ret,
                               bool re,
                               QString tipo_ret,
                               QString tipo_doc,
                               QString notas,
                               QString pie1,
                               QString pie2
                                            , QString concepto_sii)
{
    predefinida=true;
    codigopredef=codigo;
    ui.documentolabel->setText(descrip);
    ui.fechalabel->hide();
    ui.fechadateEdit->hide();
    ui.fechaoplabel->hide();
    ui.fechaopdateEdit->hide();
    ui.fecha_asiento_label->hide();
    ui.fecha_asiento_dateEdit->hide();
    ui.ctaclientelabel->hide();
    ui.cta_cli_lineEdit->hide();
    ui.ctapushButton->hide();
    ui.ctapushButton->hide();
    ui.edctapushButton->hide();
    ui.clientelabel->hide();
    ui.pdfpushButton->hide();
    ui.fpagopushButton->hide();
    ui.externo_label->hide();
    ui.externo_lineEdit->hide();
    ui.busca_externo_pushButton->hide();
    ui.datos_externo_pushButton->hide();
    ui.quita_externo_pushButton->hide();
    ui.descrip_externo_lineEdit->hide();

    ui.facturae_checkBox->hide();

    ui.facturalineEdit->setText(codigo);
    ui.facturalineEdit->setCursorPosition(0);
    ui.serielineEdit->setText(serie);
    contabilizable=contabilizable;
    ui.ret_checkBox->setChecked(con_ret);
    ui.recheckBox->setChecked(re);
    // ui.tipo_ret_lineEdit->setText(coma ? convacoma(tipo_ret) : tipo_ret);
    for (int veces=0; veces<ui.doccomboBox->count(); veces++)
        {
         if (ui.doccomboBox->itemText(veces)==tipo_doc)
            {
             ui.doccomboBox->setCurrentIndex(veces);
             break;
            }
        }
    ui.textEdit->setText(notas);
    ui.pie1lineEdit->setText(pie1);
    ui.pie2lineEdit->setText(pie2);
    ui.concepto_sii_lineEdit->setText(concepto_sii);

    QSqlQuery q;
    q = basedatos::instancia()->select_lin_doc_predef(codigopredef);
    int veces=ultimafila();
    // if (veces!=0) veces++;
    if (q.isActive())
       while (q.next())
        {
        for (int veces2=0;veces2<ui.tableWidget->columnCount();veces2++)
             if (ui.tableWidget->item(veces,veces2)==0)
                 {
                  QTableWidgetItem *newItem = new QTableWidgetItem("");
                  if (veces2==2 || veces2==3 || veces2==4 || veces2==6)
                      newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.tableWidget->setItem(veces,veces2,newItem);
                 }
        // codigo_doc, codigo, descripcion, cantidad, precio, clave_iva, tipo_iva, tipo_re, dto
        ui.tableWidget->item(veces,0)->setText(q.value(1).toString());
        bool precision=basedatos::instancia()->ref_precision(q.value(1).toString());
        ui.tableWidget->item(veces,1)->setText(q.value(2).toString());
        QString qcantidad=q.value(3).toString();
        if (qcantidad.contains(".000")) qcantidad.remove(".000");
        ui.tableWidget->item(veces,2)->setText(coma ? convacoma(qcantidad)
            : qcantidad);
        double cantidad=q.value(3).toDouble();
        double precio=q.value(4).toDouble();
        double totalneto=cantidad*precio*(1-q.value(8).toDouble()/100);
        if (precision)
           ui.tableWidget->item(veces,3)->setText(formateanumero_ndec(q.value(4).toDouble(),coma,4));
          else
            ui.tableWidget->item(veces,3)->setText(formateanumero(q.value(4).toDouble(),coma,decimales));
        ui.tableWidget->item(veces,4)->setText(formateanumero(q.value(8).toDouble(),coma,decimales));
        if (precision)
           ui.tableWidget->item(veces,6)->setText(formateanumero_ndec(redond(totalneto,4),coma,4));
          else
            ui.tableWidget->item(veces,6)->setText(formateanumero(redond(totalneto,2),coma,decimales));
        ui.tableWidget->item(veces,7)->setText(q.value(5).toString());
        // tipo iva y re lugares 7 y 8
        ui.tableWidget->item(veces,8)->setText(formateanumero(q.value(6).toDouble(),
                                                              coma,decimales));
       //  QMessageBox::warning( this, tr("facturas"),formateanumero(q.value(6).toDouble(),
       //                                                           coma,decimales));

        ui.tableWidget->item(veces,9)->setText(formateanumero(q.value(7).toDouble(),
                                                              coma,decimales));
        texto_linea[veces]=q.value(9).toString();
        ui.tableWidget->item(veces,5)->setText(q.value(10).toString());
        // actualizar listas con tipos de iva y re
        fijatipoiva(q.value(5).toString(),q.value(6).toDouble()/100);
        fijatipore(q.value(5).toString(),q.value(7).toDouble()/100);

        veces++;

        }
    actualizatotales();



}

void factura::guarda_predef()
{
    // Comenzamos transacción: tablas facturas_predef, lin_fact_predef


   // comprobamos si existe cabecera de factura predefinida ***
    if (basedatos::instancia()->existe_fact_predef(codigopredef))
       {
        // actualizamos
        basedatos::instancia()->modificacabecerafactura_predef(
                                            codigopredef,
                                            contabilizable,
                                            ui.ret_checkBox->isChecked(),
                                            ui.recheckBox->isChecked(),
                                            convapunto(ui.tipo_ret_lineEdit->text()),
                                            ui.doccomboBox->currentText(),
                                            ui.textEdit->toPlainText(),
                                            ui.pie1lineEdit->text(),
                                            ui.pie2lineEdit->text(),
                                            ui.concepto_sii_lineEdit->text()
                                            );
        // borramos registros de detalle
        basedatos::instancia()->borralineas_fac_predef(codigopredef);
       }
    // añadimos registros de detalle

    int ultimo=ultimafila();
    for (int veces=0; veces<ultimo; veces++)
       {
        if (ui.tableWidget->item(veces,0)==NULL) continue;
        QString tipo,re;
        cadvalorestipoiva(ui.tableWidget->item(veces,7)->text(), &tipo, &re);
        basedatos::instancia()->insert_lin_doc_predef(codigopredef,
                                       ui.tableWidget->item(veces,0)->text(),   // codigo
                                       ui.tableWidget->item(veces,1)->text(),   // descrip
                                       convapunto(ui.tableWidget->item(veces,2)->text()),   // cantidad
                                       convapunto(ui.tableWidget->item(veces,3)->text()),   // precio
                                       ui.tableWidget->item(veces,7)->text(), // clave_iva
                                       tipo, // tipo_iva
                                       re, // recargo equivalencia
                                       convapunto(ui.tableWidget->item(veces,4)->text()),
                                       ui.tableWidget->item(veces,5)->text(),
                                       texto_linea[veces]); // dto.
       }
    // terminamos

}


void factura::texto_linea_cambiado()
{
   texto_linea[ui.tableWidget->currentRow()]=ui.lineatextEdit->toPlainText();
}


void factura::pedido_cambiado()
{
  pedido_linea[ui.tableWidget->currentRow()]=ui.pedido_lineEdit->text();
}

void factura::albaran_cambiado()
{
  albaran_linea[ui.tableWidget->currentRow()]=ui.albaran_lineEdit->text();
}

void factura::expediente_cambiado()
{
  expediente_linea[ui.tableWidget->currentRow()]=ui.expediente_lineEdit->text();
}

void factura::contrato_cambiado()
{
  contrato_linea[ui.tableWidget->currentRow()]=ui.contrato_lineEdit->text();
}

void factura::procesabotonci()
{
 int fila=ui.tableWidget->currentRow();
 if (fila<0 || fila>ui.tableWidget->rowCount()) return;
 if (ui.tableWidget->item(fila,0)==NULL) return;
 if (ui.tableWidget->item(fila,0)->text().isEmpty()) return;

 introci *c = new introci();
 c->pasaci(ui.tableWidget->item(fila,5)->text());
 int resultado=c->exec();
 if (resultado==QDialog::Accepted)
   ui.tableWidget->item(fila,5)->setText(c->cadenaci());
 delete(c);
}


void factura::check_oculta_notas_lin()
{
    if (ui.oculta_notas_lin_checkBox->isChecked())
        ui.lineatextEdit->hide();
    else
        ui.lineatextEdit->show();
}

void factura::forma_pago()
{
    QString forma_pago;
    bool mostrar_cuenta_propia, mostrar_cuenta_cliente;
    basedatos::instancia()->cuenta_aux_forma_pago(ui.cta_cli_lineEdit->text(),&forma_pago,&mostrar_cuenta_propia,
                                                  &mostrar_cuenta_cliente);
    QString cad_forma_pago;
    if (!forma_pago.isEmpty())
      {
       cad_forma_pago=tr("Forma de pago: ")+forma_pago;
       if (mostrar_cuenta_propia)
          {
           QString cta_tesoreria=basedatos::instancia()->cta_tesoreria_venci(ui.cta_cli_lineEdit->text());
           if (!cta_tesoreria.isEmpty())
             {
               // consultamos iban asociado a la cuenta
               QString ccc, iban, cedente1, cedente2,nriesgo, sufijo;
               basedatos::instancia()->datos_cuenta_banco(cta_tesoreria, &ccc, &iban, &cedente1, &cedente2,
                                                          &nriesgo, &sufijo);
               cad_forma_pago+=tr(" - IBAN: ");
               cad_forma_pago+=iban;
               cad_forma_pago+="\n";
             }
          }
       if (mostrar_cuenta_cliente)
         {
           if (!ui.ctaclientelabel->text().isEmpty())
             {
               // consultamos iban asociado a la cuenta
               QString ccc, iban, cedente1, cedente2,nriesgo, sufijo;
               basedatos::instancia()->datos_cuenta_banco(ui.cta_cli_lineEdit->text(), &ccc, &iban, &cedente1, &cedente2,
                                                          &nriesgo, &sufijo);
               cad_forma_pago+=tr(" - IBAN: ");
               cad_forma_pago+=iban;
               cad_forma_pago+="\n";

             }
         }
      }
    QString cadena;
    int vto_dia_fijo;
    // bool asoc_auxiliar=false;
    bool procesavenci=false;
    bool domiciliable=false;
    //QString cta_tesoreria;
    QString cta_ordenante=ui.cta_cli_lineEdit->text();
    QString cta_tesoreria;
    if (basedatos::instancia()->hayvenciasociado(cta_ordenante)
        && ! basedatos::instancia()->cta_tesoreria_venci(cta_ordenante).isEmpty())
      {
      cadena=basedatos::instancia()->cadvenci_subcuenta(cta_ordenante);
      vto_dia_fijo=basedatos::instancia()->dia_fijo_vto_subcuenta(cta_ordenante);
      //cta_tesoreria=basedatos::instancia()->cta_tesoreria_venci(cta_ordenante);
      //procesavenci=basedatos::instancia()->hayprocvenci(cta_ordenante);
      // asoc_auxiliar=true;
      }
    // QMessageBox::warning( this, tr("Tabla de apuntes"),"contenido:"+cadena);
     if (cadena.isEmpty())
       basedatos::instancia()->hay_venci_cta_asociado(cta_ordenante,
                                                       &cadena,
                                                       &cta_tesoreria, &procesavenci,
                                                       &vto_dia_fijo);
    QString concepto;
    if (!cadena.isEmpty())
     {
      domiciliable=basedatos::instancia()->cuenta_dom_bancaria(cta_ordenante);
      //if (forzarcontado) cadena="0";
      int numregistros=cadena.count(',')+1;

     double importe=convapunto(ui.totallineEdit->text()).toDouble();
     double importetotal=importe;
     importe=importe/numregistros;
     QString cadimporte;
     cadimporte.setNum(importe,'f',2);
     double arrastre=0;
     double resto=0;
     QString cadnumregistros; cadnumregistros.setNum(numregistros);
     concepto+= (numregistros >0 && numregistros<2) ? tr("Vencimiento: \n") : tr("Vencimientos: \n");
     for (int veces=0;veces<numregistros;veces++)
       {
        QString cadvez; cadvez.setNum(veces+1);
        if (veces==numregistros-1) // estamos en el último registro
         {
           resto=importetotal-arrastre;
           cadimporte.setNum(resto,'f',2);
         }
          else
           arrastre+=cadimporte.toDouble();
        // QMessageBox::warning( this, tr("Tabla de apuntes"),cadena.section(',',veces,veces));
        int dias=cadena.section(',',veces,veces).toInt();
        //QSqlDatabase::database().transaction();
        //basedatos::instancia()->bloquea_para_nuevo_vencimiento();
        //qlonglong vnum = basedatos::instancia()->proximovencimiento();
        //QString cadnum; cadnum.setNum(vnum+1);
        // ***********************************************************************
        QDate fechavenci;
        // fechavenci=fecha_operacion;
        fechavenci=ui.fechadateEdit->date();
        // QMessageBox::warning( this, tr("Tabla de apuntes"),
        //                      fecha_factura.toString("dd-MM-yyyy"));
        fechavenci=fechavenci.addDays(dias);
        int mesvenci=fechavenci.month();
        int anyovenci=fechavenci.year();
        if (vto_dia_fijo>0)
           {
             if (mesvenci==2 && vto_dia_fijo>28) vto_dia_fijo=28;
             QList<int> meses30; meses30 << 11 << 4 << 6 << 9;
             if (meses30.contains(mesvenci) && vto_dia_fijo>30 )
                 vto_dia_fijo=30;
           }
        if (fechavenci.day()<vto_dia_fijo && vto_dia_fijo>0)
            fechavenci.setDate(anyovenci,mesvenci,vto_dia_fijo);
        if (fechavenci.day()>vto_dia_fijo && vto_dia_fijo>0)
           {
            fechavenci=fechavenci.addMonths(1) ;
            mesvenci=fechavenci.month();
            anyovenci=fechavenci.year();
            fechavenci.setDate(anyovenci,mesvenci,vto_dia_fijo);
           }
        //QString cadnumvto; cadnumvto.setNum(vnum);
        //basedatos::instancia()->updateConfiguracionprox_vencimiento(cadnum);
        // QString num, QString cta_ordenante, QDate fecha_operacion, QString importe,
        // QString cuenta_tesoreria, QDate fecha_vencimiento, bool derecho, bool pendiente,
        // bool anulado, QString pase_diario_operacion, QString pase_diario_vencimiento,
        // QString concepto

        //concepto+=documento;
        concepto+=" - ";
        concepto+=cadvez+"/"+cadnumregistros;
        concepto+=" ";
        concepto+=fechavenci.toString("dd/MM/yyyy");
        concepto+=": ";
        concepto+=formateanumerosep(cadimporte.toDouble(),coma,decimales);
        concepto+=" \n";
        //if (!domiciliable)
        //    concepto+=" "+descripcioncuenta(cta_ordenante);
        /*basedatos::instancia()->insertVencimientos ( cadnumvto , cta_ordenante,
                                                     fecha_operacion ,
                                                     cadimporte ,
                                                     cta_tesoreria,
                                                     fechavenci,
                                                     derecho,
                                                     true , false , pase, "0",
                                                     concepto ); */
        // basedatos::instancia()->desbloquea_y_commit();
       }
    }
    ui.textEdit->setText(cad_forma_pago + "\n"+ concepto);
}

void factura::esconde_cta_anticipo()
{
    ui.cta_anticipos_label->hide();
    ui.cta_anticipos_lineEdit->hide();
    ui.busca_cta_anticipos_pushButton->hide();
}

void factura::muestra_cta_anticipo()
{
    ui.cta_anticipos_label->show();
    ui.cta_anticipos_lineEdit->show();
    ui.busca_cta_anticipos_pushButton->show();

}




void factura::cta_anticipo_cambiada()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

    QString cadena;
    if (ui.cta_anticipos_lineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
        if (!existecodigoplan(ui.cta_anticipos_lineEdit->text(),&cadena))
          {
         // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
                 {
                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.cta_anticipos_lineEdit->text());
                   lasubcuenta2->exec();
                   delete(lasubcuenta2);
                   if (!existecodigoplan(ui.cta_anticipos_lineEdit->text(),&cadena))
                        ui.cta_anticipos_lineEdit->setText("");
                 }
                  else
                    ui.cta_anticipos_lineEdit->setText("");
           }
        }
   if (esauxiliar(ui.cta_anticipos_lineEdit->text()))
       ui.cta_anticipos_lineEdit->setToolTip(descripcioncuenta(ui.cta_anticipos_lineEdit->text()));
     else ui.cta_anticipos_lineEdit->setToolTip("");
}

void factura::cta_anticipo_finedicion()
{
  QString cadena;
  ui.cta_anticipos_lineEdit->setText(expandepunto(ui.cta_anticipos_lineEdit->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
  if (ui.cta_anticipos_lineEdit->text().length()>3 && cod_longitud_variable()) {
        if (!existecodigoplan(ui.cta_anticipos_lineEdit->text(),&cadena))
          {
         // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
                 {
                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.cta_anticipos_lineEdit->text());
                   lasubcuenta2->exec();
                   delete(lasubcuenta2);
                   if (!esauxiliar(ui.cta_anticipos_lineEdit->text()))
                        ui.cta_anticipos_lineEdit->setText("");
                 }
                  else
                    ui.cta_anticipos_lineEdit->setText("");
           }
        if (esauxiliar(ui.cta_anticipos_lineEdit->text()))
          ui.cta_anticipos_lineEdit->setToolTip(descripcioncuenta(ui.cta_anticipos_lineEdit->text()));
         else ui.cta_anticipos_lineEdit->setToolTip("");
        }

}


void factura::buscacta_anticipo()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cta_anticipos_lineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.cta_anticipos_lineEdit->setText(cadena2);
       else ui.cta_anticipos_lineEdit->setText("");
    delete(labusqueda);
}


void factura::buscar_externo()
{
   busca_externo *b = new busca_externo();
   int rdo=b->exec();
   if (rdo==QDialog::Accepted)
     {
       ui.externo_lineEdit->setText(b->codigo_elec());
     }
     else ui.externo_lineEdit->clear();
   delete(b);
}

void factura::campo_externo_cambiado()
{
 QString codigo=ui.externo_lineEdit->text();
 if (basedatos::instancia()->existe_externo(codigo))
   {
     ui.descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(codigo));
     ui.cta_cli_lineEdit->setText(basedatos::instancia()->cuenta_externo(codigo));
   }
   else ui.descrip_externo_lineEdit->clear();
}

void factura::borra_externo()
{
  ui.externo_lineEdit->clear();
}

void factura::ver_externo()
{
    externos *e = new externos();
    e->pasa_codigo(ui.externo_lineEdit->text());
    e->exec();
    delete(e);
}

void factura::check_facturae()
{
    if (ui.facturae_checkBox->isChecked())
       {
        ui.textEdit->setMinimumWidth(600);

        ui.facturae_groupBox->show();
       }
      else
          {
           ui.facturae_groupBox->hide();
           ui.textEdit->setMinimumWidth(850);
          }
}


void factura::copia_lin_face()
{
 int linea=ui.tableWidget->currentRow();
 if (ui.tableWidget->currentRow()>0 && ui.tableWidget->currentRow()<100)
    {
     ui.pedido_lineEdit->setText(pedido_linea[linea-1]);
     ui.albaran_lineEdit->setText(albaran_linea[linea-1]);
     ui.expediente_lineEdit->setText(expediente_linea[linea-1]);
     ui.contrato_lineEdit->setText(contrato_linea[linea-1]);
    }
}

void factura::fecha_factura_cambiada() {
    ui.fechaopdateEdit->setDate(ui.fechadateEdit->date());
    ui.fecha_asiento_dateEdit->setDate(ui.fechadateEdit->date());

}
