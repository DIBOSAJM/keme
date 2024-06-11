/* ----------------------------------------------------------------------------------
    KEME-Contabilidad ; aplicación para llevar contabilidades

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

#include "subcuentas.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>
#include "buscasubcuenta.h"
#include "ajuste_imagen.h"
#include "directorio.h"
#include <QFileDialog>
#include "vies_msj.h"
#include "graf_recorte.h"

#define CODPAIS "ES"

subcuentas::subcuentas() : QDialog() {
  ui.setupUi(this);

if (basedatos::instancia()->registros_plan()>100)
  modolista=true;
else modolista=false;

ui.treeWidget->setColumnCount(2);
QStringList columnas;
columnas << tr("CÓDIGO") << tr("DESCRIPCIÓN");
ui.treeWidget->setHeaderLabels(columnas);


if (!modolista)
  {
    ui.tableView->hide();
    cargaarbol();
    primer_elemento_ini();
  }
  else
     {
      ui.treeWidget->hide();
      QSqlTableModel *modelo = new QSqlTableModel;
      modeloreg=modelo;
      refrescar("");
     }

  // cargaarbol();
  cargacomboIVA();
  cargacombovenci();
  cargacomboclaveidfiscal();
  cargacombopaises();
  cargacombooperacionret();
  cargacombo_formas_pago();

  cargacombos_tipoProveedor();

  ui.tabWidget->setTabEnabled(2,false);

 /* ui.treeView->setModel(&modeloarbol);
  ui.treeView->expandAll();
  ui.treeView->resizeColumnToContents(0); */
  ui.treeWidget->resizeColumnToContents(0);
  ui.treeWidget->setAlternatingRowColors ( true);
  /*
  ui.treeView->setEditTriggers ( QAbstractItemView::NoEditTriggers );
  */

  ui.codigolineEdit->setFocus();

  ui.vfijospinBox->setEnabled(false);

  if (basedatos::instancia()->hay_criterio_caja_config())
      ui.caja_ivacheckBox->hide();

  if (!basedatos::instancia()->tipo_proveedor_config())
      ui.tabWidget->setTabEnabled(3,false);


  connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado(QString)));
  connect(ui.codigolineEdit,SIGNAL(editingFinished()),this,SLOT(codigofinedicion()));
  connect(ui.treeWidget,SIGNAL( clicked (QModelIndex )),this,SLOT(filaclicked()));

  connect(ui.tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(fila_tabla_clicked()));

  connect(ui.descripcionlineEdit,SIGNAL( textChanged(QString)),this,SLOT(descripcambiada()));
  connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(botonaceptar()));
  connect(ui.borrarpushButton,SIGNAL( clicked()),this,SLOT(botonborrar()));
  connect(ui.datosgroupBox,SIGNAL( clicked(bool)),this,SLOT(grupodatospulsado()));

  connect(ui.fotopushButton,SIGNAL( clicked()),this,SLOT(importafoto()));

  connect(ui.borrafotopushButton,SIGNAL( clicked()),this,SLOT(borrafoto()));

  connect(ui.ctaIVApushButton,SIGNAL( clicked()),this,SLOT(botonctaivapulsado()));
  connect(ui.cta_tesorpushButton,SIGNAL( clicked()),this,SLOT(botoncta_tesorpulsado()));
  connect(ui.ctaretpushButton,SIGNAL( clicked()),this,SLOT(botonctaretpulsado()));

  connect(ui.ctaBaseIVAlineEdit,SIGNAL(editingFinished()),this,SLOT(cta_base_IVA_finedicion()));
  connect(ui.ctaBaseIVApushButton,SIGNAL(clicked(bool)),this,SLOT(botoncta_base_ivapulsado()));
  connect(ui.ctaBaseIVAlineEdit,SIGNAL(textChanged(QString)),this,SLOT(cta_base_IVA_cambiada()));

  connect(ui.cuentaIVAlineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_IVA_finedicion()));
  connect(ui.cuenta_tesorlineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_tesor_finedicion()));

  connect(ui.venciasocgroupBox, SIGNAL(clicked(bool)), this, SLOT(venciasoccambiado()));

  connect(ui.tiporetlineEdit,SIGNAL(editingFinished()),this,SLOT(tiporet_finedicion()));

  connect(ui.diafijocheckBox,SIGNAL(clicked()),this,SLOT(checkdiafijo()));

  connect(ui.cuentata_ret_asiglineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_ret_asig_finedicion()));

  connect(ui.codigopushButton, SIGNAL(clicked()),this, SLOT(botoncodigo_pulsado()));

  connect(ui.UE_pushButton,SIGNAL(clicked(bool)),this,SLOT(boton_ue_pulsado()));

  connect(ui.refreshpushButton,SIGNAL(clicked(bool)),this,SLOT(refresca_cuentas()));

  ui.check_ciflabel->hide();
  ui.check_ccclabel->hide();
  ui.check_ibanlabel->hide();
}

void subcuentas::cargaarbol()
{
 if (!cod_longitud_variable())
  {
   QSqlQuery query = basedatos::instancia()->selectCodigodescripcionplancontablefiltro("");
   if ( query.isActive() ) {
          while ( query.next() )
              {
                insertacuentaarbol_inicial(query.value(0).toString(),query.value(1).toString());
	      }

   }
  }
  else
      {
       // creamos primero el árbol con cuentas agregadoras
       QSqlQuery query = basedatos::instancia()->selectCodigodescripcionplancontableauxiliarordercodigo(false);
       if ( query.isActive() ) {
           while ( query.next() )
              {
                insertacuentaarbol_inicial(query.value(0).toString(),query.value(1).toString());
	      }
          }
       // insertamos ahora las cuentas axiliares
       query = basedatos::instancia()->selectCodigodescripcionplancontableauxiliarordercodigo(true);
       if ( query.isActive() ) {
           while ( query.next() )
              {
                insertacuentaarbol_inicial(query.value(0).toString(),query.value(1).toString());
	      }
          }
      }
}

void subcuentas::insertacuentaarbol(QString cadena1,QString cadena2)
{
    QString cadactual;
    cadactual=cadena1;
    bool seencuentra=false;
    QTreeWidgetItem *elemento;
    // QListViewItem *elemento;
    QStringList datos;
    datos << cadena1 << cadena2;
    for (int veces=1; veces<cadactual.length(); veces++)
        {
          QString extract;
          extract=cadactual.left(cadactual.length()-veces);
          bool encontrada=false;
          elemento=buscacadena(extract,&encontrada);
          if (encontrada)
            {
              if (elemento!=0) {
              QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento,datos);
              // new QTreeWidgetItem(elemento,datos);
              elemento4->setExpanded(true);
              ui.treeWidget->setCurrentItem(elemento4);
              seencuentra=true;
              break;
             }
            }
         }
     if (!seencuentra)
        {
         //QMessageBox::information( this, tr("Edición de plan contable"),
         //                             tr("no se encuentra ")+ datos.at(0));
         QTreeWidgetItem *elemento = new QTreeWidgetItem(ui.treeWidget,datos);
         elemento->setExpanded(true);
         ui.treeWidget->setCurrentItem(elemento);
        }
}


/*void subcuentas::insertacuentaauxiliararbol(QString cadena1,QString cadena2)
{
  QString cadactual;
	        cadactual=cadena1;
	        bool seencuentra=false;
                TreeItem *elemento;
	        // QListViewItem *elemento;
	        for (int veces=1; veces<cadactual.length(); veces++)
		{
		    QString extract;
		    extract=cadactual.left(cadactual.length()-veces);
                    bool encontrada=false;
                    elemento=modeloarbol.buscacadena(modeloarbol.raiz(),extract,&encontrada);
                    if (esauxiliar(extract)) encontrada=false;
		    if (encontrada)
		        {
                         QList<QVariant> datos;
                         datos << cadena1 << cadena2;
                         TreeItem *qelem=new TreeItem(datos, elemento);
                          elemento->appendChild(qelem);
                          modeloarbol.actu_elemento(elemento);
                          modeloarbol.resetea();
		          seencuentra=true;
		          break;
	                        }
		}
	        if (!seencuentra)
		{
                            QList<QVariant> datos;
                            datos << cadena1 << cadena2;
                            modeloarbol.raiz()->appendChild(new TreeItem(datos, modeloarbol.raiz()));
		}
}
*/

void subcuentas::codigocambiado(QString codigo)
{
 if (codigo.length()==0)
   {
    ui.datosgroupBox->setChecked(false);
    ui.datosgroupBox->setEnabled(false);
    resetdatosaccesorios();
   }

 if (ui.datosgroupBox->isEnabled())
     {
      if (ui.datosgroupBox->isChecked())
          {
            resetdatosaccesorios();
            ui.datosgroupBox->setChecked(false);
          }
     }
 if ((codigo.length()==anchocuentas()) || (codigo.length()>3 && cod_longitud_variable())) 
   {
    ui.descripcionlineEdit->setEnabled(true);
    if (!cod_longitud_variable()) ui.descripcionlineEdit->setFocus();
   }
     else
          {
           ui.descripcionlineEdit->setText("");
           ui.descripcionlineEdit->setEnabled(false);
           ui.borrarpushButton->setEnabled(false);
           // return;
          }

 bool encontrada=false;

 QTreeWidgetItem *elemento;
 elemento=NULL;

 if (!modolista)
   {
    QString buscar=codigo;
    if (buscar.contains(".") && !cod_longitud_variable()) buscar=expandepunto(codigo,anchocuentas());
    elemento=buscacadena(buscar,&encontrada);
   }
  else
       {
        // filtramos lista
        // encontrada=existecuenta
        refrescar(filtrocodigo());
        encontrada=existesubcuenta(codigo);
       }

 if (basedatos::instancia()->escuenta_de_ret_ing(codigo))
     ui.tabWidget->setTabEnabled(2,true);

 if (encontrada)
     {
      //QModelIndex indice;
      if (!modolista)
        {
          ui.treeWidget->setCurrentItem(elemento);
         //indice = modeloarbol.creaindice(elemento);
         //ui.treeView->setCurrentIndex(indice);
        }
      if (codigo.length()==anchocuentas() || esauxiliar(codigo))
          {
           // es cuenta de retención -->activar pestaña en datos
          // if (basedatos::instancia()->escuenta_de_ret_irpf(codigo))
          //    ui.tabWidget->setTabEnabled(2,true);

           ui.descripcionlineEdit->setText(descripcioncuenta(codigo));
           ui.aceptarpushButton->setEnabled(false);
           ui.borrarpushButton->setEnabled(true);
           // habilitar grupo de datossubcuenta
           ui.datosgroupBox->setEnabled(true);
           
           // ver si hay datossubcuenta, activar groupbox y cargarcampos

           QString razon;
           QString ncomercial;
           QString cif;
           // QString nifcomunitario; eliminado
           QString nifrprlegal;
           QString web;
           QString claveidfiscal;
           QString domicilio;
           QString poblacion;
           QString codigopostal;
           QString provincia;
           QString pais;
           QString tfno;
           QString fax;
           QString email;
           QString observaciones;
           QString ccc;
           QString cuota;
           QString imagen;
           bool venciasoc;
           QString codvenci;
           QString tesoreria;
           bool ivaasoc;
           bool procvto;
           QString cuentaiva;
           QString cta_base_IVA;

           QString tipoiva;
           QString conceptodiario;
           QString paisdat;
           QString tipo_ret;
           QString tipo_operacion_ret;
           int vdiafijo=0;
           bool ret_arrendamiento;

           QString cuenta_ret_asig; bool es_ret_asig_arrend;
           QString tipo_ret_asig; QString tipo_oper_ret_asig;
           bool dom_bancaria=false;
           QString iban,nriesgo,cedente1,cedente2,bic,sufijo;
           QString ref_mandato; QDate firma_mandato;
           bool caja_iva=false;
           QString formapago;
           // pais_dat, qtipo_ret, ret_arrendamiento, tipo_operacion_ret
           if (existendatosaccesorios(codigo, &razon, &ncomercial, &cif, &nifrprlegal, &domicilio,
			       &poblacion,&codigopostal, &provincia, &pais, &tfno,
                               &fax, &email, &observaciones, &ccc, &cuota,
                               &venciasoc, &codvenci, &tesoreria, &ivaasoc, &cuentaiva, &cta_base_IVA, &tipoiva,
                               &conceptodiario, &web, &claveidfiscal, &procvto,
                               &paisdat, &tipo_ret, &tipo_operacion_ret, &ret_arrendamiento,
                               &vdiafijo, &cuenta_ret_asig, &es_ret_asig_arrend, &tipo_ret_asig,
                               &tipo_oper_ret_asig,
                               &dom_bancaria, &iban, &nriesgo, &cedente1, &cedente2, &bic,
                               &sufijo,  &ref_mandato, &firma_mandato, &caja_iva, &formapago,
                               &imagen))

            {
             desactivaconexdatos();
             ui.datosgroupBox->setChecked(true);
             ui.tabWidget->setEnabled(true);
             // Pasamos datos a campos
	     ui.NombreLineEdit->setText(razon);
             ui.ncomerciallineEdit->setText(ncomercial);
	     ui.DomicilioLineEdit->setText(domicilio);
	     ui.PoblacionLineEdit->setText(poblacion);
	     ui.ProvinciaLineEdit->setText(provincia);
	     ui.CpLineEdit->setText(codigopostal);
	     ui.NifLineEdit->setText(cif);
             if (!ui.NifLineEdit->text().isEmpty())
               {
                if (isNifCifNie(ui.NifLineEdit->text())>0)
                   ui.check_ciflabel->show();
               }

             ui.NifReplegalLineEdit->setText(nifrprlegal);
             // ui.PaisLineEdit->setText(pais);
             ui.TfnoLineEdit->setText(tfno);
             ui.FaxLineEdit->setText(fax);
             ui.EmailLineEdit->setText(email);
             ui.ObservacionesLineEdit->setText(observaciones);
             ui.ccclineEdit->setText(ccc);
             if (!ui.ccclineEdit->text().isEmpty())
               {
                if (ccc_correcto(ui.ccclineEdit->text())) {
                   ui.check_ccclabel->show();
                   // comprobamos si el campo iban tiene contenido
                   if (ui.ibanlineEdit->text().isEmpty()) {
                       ui.ibanlineEdit->setText("ES"+digito_cotrol_IBAN("ES00"+ui.ccclineEdit->text().remove('-'))+ui.ccclineEdit->text().remove('-'));
                   }
                }
               }

             ui.cuotalineEdit->setText(cuota);
             ui.weblineEdit->setText(web);
             ui.caja_ivacheckBox->setChecked(caja_iva);
             // buscamos código de operación
             // si vacío asignamos 1
             if (claveidfiscal.isEmpty()) claveidfiscal="1";
             int indice=0;
             while (indice<ui.claveidfiscalcomboBox->count())
                   {
                    QString texto=ui.claveidfiscalcomboBox->itemText(indice);
                    QString extracto=texto.section("//",0,0).trimmed();
                    if (extracto==claveidfiscal) break;
                    indice++;
                   }
             if (indice>ui.claveidfiscalcomboBox->count()) indice--;
             ui.claveidfiscalcomboBox->setCurrentIndex(indice);

             ui.paislineEdit->setText(paisdat);
             // buscamos código de país
             // si vacío asignamos CODPAIS
             if (pais.length()!=2) pais=CODPAIS;
             indice=0;
             while (indice<ui.paiscomboBox->count())
                   {
                    QString texto=ui.paiscomboBox->itemText(indice);
                    QString extracto=texto.section(" ",0,0).trimmed();
                    if (extracto==pais) break;
                    indice++;
                   }
             if (indice>ui.paiscomboBox->count()) indice--;
             ui.paiscomboBox->setCurrentIndex(indice);

             indice=0;
             while (indice<ui.forma_pago_comboBox->count())
                   {
                    if (ui.forma_pago_comboBox->itemText(indice)==formapago) break;
                    indice++;
                   }
             if (indice>ui.forma_pago_comboBox->count()) indice=0;
             ui.forma_pago_comboBox->setCurrentIndex(indice);

             if (venciasoc)
               {
                 ui.venciasocgroupBox->setChecked(true);
                 // buscamos codvenci en el combo vencicomboBox
                 ui.diafijocheckBox->setEnabled(true);
                 if (vdiafijo>0)
                    {
                     // ui.diafijocheckBox->setEnabled(true);
                     ui.diafijocheckBox->setChecked(true);
                     ui.vfijospinBox->setEnabled(true);
                     ui.vfijospinBox->setValue(vdiafijo);
                    }
                    else
                       {
                        // ui.diafijocheckBox->setEnabled(false);
                        ui.vfijospinBox->setEnabled(false);
                        ui.diafijocheckBox->setChecked(false);
                       }
                 int indice=0;
                 while (indice<ui.vencicomboBox->count())
                   {
                    QString texto=ui.vencicomboBox->itemText(indice);
                    QString extracto=texto.section("//",0,0).trimmed();
                    if (extracto==codvenci) break;
                    indice++;
                   }
                 if (indice>ui.vencicomboBox->count()) indice--;
                 ui.vencicomboBox->setCurrentIndex(indice);
                 ui.cuenta_tesorlineEdit->setText(tesoreria);
                 ui.descrip_cta_tesorlabel->setText(descripcioncuenta(tesoreria));
               }
                else
                     {
                      ui.cuenta_tesorlineEdit->clear();
                      ui.venciasocgroupBox->setChecked(false);
                     }
             ui.procvtocheckBox->setChecked(procvto);
             if (ivaasoc)
                {
                 ui.ivaasocgroupBox->setChecked(true);
                 // buscamos tipoiva en tipoIVAcomboBox
                 int indice=0;
                 while (indice<ui.tipoIVAcomboBox->count())
                   {
                    QString texto=ui.tipoIVAcomboBox->itemText(indice);
                    QString extracto=texto.section("//",0,0).trimmed();
                    if (extracto==tipoiva) break;
                    indice++;
                   }
                 if (indice>ui.tipoIVAcomboBox->count()) indice--;
                 ui.tipoIVAcomboBox->setCurrentIndex(indice);
                 ui.cuentaIVAlineEdit->setText(cuentaiva);
                 ui.descripctaIVAlabel->setText(descripcioncuenta(cuentaiva));
                 ui.ctaBaseIVAlineEdit->setText(cta_base_IVA);
                 ui.descripctaBaseIVAlabel->setText(descripcioncuenta(cta_base_IVA));
                }
                else
                   {
                    ui.ivaasocgroupBox->setChecked(false);
                    ui.cuentaIVAlineEdit->clear();
                   }
             ui.conceptodiariolineEdit->setText(conceptodiario);
             // &paisdat, &tipo_ret, &tipo_operacion_ret, &ret_arrendamiento,
             ui.tiporetlineEdit->setText(haycomadecimal() ? convacoma(tipo_ret):tipo_ret);
             indice=0;
             while (indice<ui.operacionretcomboBox->count())
               {
                if (ui.operacionretcomboBox->itemText(indice).left(1)==tipo_operacion_ret.left(1)) break;
                indice++;
               }
             if (indice>=ui.operacionretcomboBox->count()) indice=0;
             ui.operacionretcomboBox->setCurrentIndex(indice);
             ui.alquilercheckBox->setChecked(ret_arrendamiento);
             if (ret_arrendamiento) ui.operacionretcomboBox->setEnabled(false);
                else ui.operacionretcomboBox->setEnabled(true);
             // QString cuenta_ret_asig; bool es_ret_asig_arrend;
             // QString tipo_ret_asig; QString tipo_oper_ret_asig;
             ui.retgroupBox->setChecked(!cuenta_ret_asig.isEmpty());

             ui.cuentata_ret_asiglineEdit->setText(cuenta_ret_asig);
             ui.cta_ret_label->setText(descripcioncuenta(cuenta_ret_asig));
             ui.es_ret_asigcheckBox->setChecked(es_ret_asig_arrend);
             if (es_ret_asig_arrend) ui.tipo_oper_ret_asigcomboBox->setEnabled(false);
             else {if (ui.retgroupBox->isChecked()) ui.tipo_oper_ret_asigcomboBox->setEnabled(true);}
             ui.porc_ret_asig_lineEdit->setText(haycomadecimal() ? convacoma(tipo_ret_asig):tipo_ret_asig);
             indice=0;
             while (indice<ui.tipo_oper_ret_asigcomboBox->count())
               {
                if (ui.tipo_oper_ret_asigcomboBox->itemText(indice).left(1)==tipo_oper_ret_asig.left(1)) break;
                indice++;
               }
             if (indice>=ui.tipo_oper_ret_asigcomboBox->count()) indice=0;
             ui.tipo_oper_ret_asigcomboBox->setCurrentIndex(indice);

             // &dom_bancaria, &iban, &nriesgo, &cedente1, &cedente2,
             ui.domiciliacheckBox->setChecked(dom_bancaria);
             if (!iban.isEmpty())
                ui.ibanlineEdit->setText(iban);
             if (!ui.ibanlineEdit->text().isEmpty())
               {
                if (IBAN_correcto(ui.ibanlineEdit->text()))
                   ui.check_ibanlabel->show();
               }
             ui.biclineEdit->setText(bic);

             ui.ref_madatolineEdit->setText(ref_mandato);
             ui.firma_mandatodateEdit->setDate(firma_mandato);

             QPixmap foto;
             if (imagen.length()>0)
                {
                 QByteArray byteshexa;
                 byteshexa.append ( imagen.toUtf8() );
                 QByteArray bytes;
                 bytes=bytes.fromHex ( byteshexa );
                 foto.loadFromData ( bytes, "PNG");
                }
             ui.fotolabel->setPixmap(foto);
             ui.aceptarpushButton->setEnabled(true);
             if (basedatos::instancia()->tipo_proveedor_config()) {
                 QSqlQuery q = basedatos::instancia()->select_datos_tipo_proveedor_cta (codigo);
                 if (q.isActive())
                     if (q.next()) {
                         // cadena+="tipo_proveedor, "; //varchar(40),";
                         QString cad=q.value(0).toString()+"-";
                         for (int veces=0; veces<ui.tipo_proveedor_comboBox->count(); veces++) {
                             if (ui.tipo_proveedor_comboBox->itemText(veces).startsWith(cad))
                                 ui.tipo_proveedor_comboBox->setCurrentIndex(veces);
                         }
                         // cadena+="homologacion, "; // varchar(40),";
                         cad=q.value(1).toString()+"-";
                         for (int veces=0; veces<ui.homologacion_comboBox->count(); veces++) {
                             if (ui.homologacion_comboBox->itemText(veces).startsWith(cad))
                                 ui.homologacion_comboBox->setCurrentIndex(veces);
                         }
                         // cadena+="actividad, "; //varchar(40),";
                         cad=q.value(2).toString()+"-";
                         for (int veces=0; veces<ui.actividad_comboBox->count(); veces++) {
                             if (ui.actividad_comboBox->itemText(veces).startsWith(cad))
                                 ui.actividad_comboBox->setCurrentIndex(veces);
                         }
                         // cadena+="fecha_homologacion, "; // date,";
                         ui.homologacion_dateEdit->setDate(q.value(3).toDate());
                         // cadena += "proveedor_activo, "; // bool default 0,";
                         ui.activo_checkBox->setChecked(q.value(4).toBool());
                         // cadena += "acceso_lopd, "; // bool default 0,";
                         ui.acceso_lopd_checkBox->setChecked(q.value(5).toBool());
                         // cadena += "cumpli_lopd,"; //  bool default 0,";
                         ui.cumplimiento_lopd_checkBox->setChecked(q.value(6).toBool());
                         // cadena += "acceso_prl, "; //bool default 0,";
                         ui.acceso_prl_checkBox->setChecked(q.value(7).toBool());
                         // cadena += "coord_prl, "; // bool default 0,";
                         ui.coord_prl_checkBox->setChecked(q.value(8).toBool());
                         // cadena += "sol_cert_ss, "; // bool default 0,";
                         ui.cert_ss_checkBox->setChecked(q.value(9).toBool());
                         // cadena += "sol_cert_hda, "; // bool default 0,";
                         ui.cert_hda_checkBox->setChecked(q.value(10).toBool());
                         // cadena += "no_correo_pub "; // bool default 0,";
                         ui.no_correo_pub_checkBox->setChecked(q.value(11).toBool());
                     }
             }
             activaconexdatos();
            }
          }
          else
              { 
               ui.borrarpushButton->setEnabled(false);
               ui.descripcionlineEdit->setText("");
               // borrar todos los datos subcuenta y deshabilitar datossubcuenta y chequear false
               // si el grupo está habilitado
               if (ui.datosgroupBox->isEnabled())
                   {
                    resetdatosaccesorios();
                    ui.datosgroupBox->setChecked(false);
                    ui.datosgroupBox->setEnabled(false);
                   }
              }
     }
     else 
          { 
            // no encontrada
            if (!modolista)
               {
                QString buscar=codigo;
                if (buscar.contains(".") && !cod_longitud_variable()) buscar=expandepunto(codigo,anchocuentas());
                elemento=buscacadena(buscar.left(3),&encontrada);
                if (encontrada) ui.treeWidget->setCurrentItem(elemento);;
               }

            ui.borrarpushButton->setEnabled(false);
            ui.descripcionlineEdit->setText("");
            if (ui.datosgroupBox->isEnabled())
              {
               resetdatosaccesorios();
               ui.datosgroupBox->setChecked(false);
               ui.datosgroupBox->setEnabled(false);
              }
          }

}

void subcuentas::resetdatosaccesorios()
{
 desactivaconexdatos();
 ui.NombreLineEdit->setText("");
 ui.ncomerciallineEdit->setText("");
 ui.DomicilioLineEdit->setText("");
 ui.PoblacionLineEdit->setText("");
 ui.ProvinciaLineEdit->setText("");
 ui.CpLineEdit->setText("");
 ui.NifLineEdit->setText("");
 ui.NifReplegalLineEdit->setText("");
 ui.weblineEdit->setText("");
 ui.TfnoLineEdit->setText("");
 ui.FaxLineEdit->setText("");
 ui.EmailLineEdit->setText("");
 ui.ObservacionesLineEdit->setText("");
 ui.ccclineEdit->setText("");
 ui.cuotalineEdit->setText("");
 ui.ivaasocgroupBox->setChecked(false);
 ui.venciasocgroupBox->setChecked(false);
 ui.procvtocheckBox->setChecked(false);
 ui.cuentaIVAlineEdit->clear();
 ui.cuenta_tesorlineEdit->clear();
 ui.conceptodiariolineEdit->clear();
 ui.descripctaIVAlabel->clear();
 ui.descrip_cta_tesorlabel->clear();
 ui.ctaBaseIVAlineEdit->clear();
 ui.descripctaBaseIVAlabel->clear();

 ui.paislineEdit->clear();

 ui.diafijocheckBox->setChecked(false);
 ui.vfijospinBox->setEnabled(false);

 ui.alquilercheckBox->setChecked(false);
 ui.tiporetlineEdit->clear();

 ui.cuentata_ret_asiglineEdit->clear();
 ui.es_ret_asigcheckBox->setChecked(false);
 ui.tipo_oper_ret_asigcomboBox->setCurrentIndex(0);
 ui.porc_ret_asig_lineEdit->clear();
 ui.retgroupBox->setChecked(false);

 QPixmap foto;
 ui.fotolabel->setPixmap(foto);
 // delete(ui.fotolabel->pixmap());
 ui.tabWidget->setTabEnabled(2,false);
 ui.check_ciflabel->hide();

 // &dom_bancaria, &iban, &nriesgo, &cedente1, &cedente2,
 ui.domiciliacheckBox->setChecked(false);
 ui.ibanlineEdit->clear();
 ui.biclineEdit->clear();
 ui.ref_madatolineEdit->clear();
 ui.firma_mandatodateEdit->setDate(QDate(2000,1,1));
 ui.caja_ivacheckBox->setChecked(false);
 ui.forma_pago_comboBox->setCurrentIndex(0);

 ui.tipo_proveedor_comboBox->setCurrentIndex(0);
 ui.homologacion_comboBox->setCurrentIndex(0);
 ui.actividad_comboBox->setCurrentIndex(0);

 ui.homologacion_dateEdit->setDate(QDate(2000,1,1));


 ui.activo_checkBox->setChecked(false);
 ui.acceso_lopd_checkBox->setChecked(false);
 ui.cumplimiento_lopd_checkBox->setChecked(false);
 ui.acceso_prl_checkBox->setChecked(false);
 ui.coord_prl_checkBox->setChecked(false);
 ui.cert_ss_checkBox->setChecked(false);
 ui.cert_hda_checkBox->setChecked(false);
 ui.no_correo_pub_checkBox->setChecked(false);

 activaconexdatos();
}

void subcuentas::codigofinedicion()
{
  if (!cod_longitud_variable())
    ui.codigolineEdit->setText(expandepunto(ui.codigolineEdit->text(),anchocuentas()));
  if (ui.codigolineEdit->text().length()==0)
      return;
 /* if (ui.codigolineEdit->text().length()!=anchocuentas())
             {
              QMessageBox::warning( this, tr("Edición de plan contable"),
              tr("ERROR: EL CÓDIGO %1 NO TIENE EL ANCHO REQUERIDO PARA SER SUBCUENTA").arg(ui.codigolineEdit->text()) );
              ui.codigolineEdit->setFocus();
              ui.codigolineEdit->setText("");
             }
*/
 if (ui.codigolineEdit->text().length()!=anchocuentas() && !cod_longitud_variable())
     ui.datosgroupBox->setChecked(false);

 if (cod_longitud_variable() && !esauxiliar(ui.codigolineEdit->text()))
     ui.datosgroupBox->setChecked(false);

  if (ui.codigolineEdit->text().length()>3)
     {
      if (!subgrupocongrupo(ui.codigolineEdit->text()))
             {
              QMessageBox::warning( this, tr("Edición de plan contable"),
              tr("ERROR: EL CÓDIGO %1 NO ESTÁ ASIGNADO A NINGÚN GRUPO").arg(ui.codigolineEdit->text()) );
              ui.codigolineEdit->setFocus();
              ui.codigolineEdit->setText("");
              ui.datosgroupBox->setChecked(false);
             }
      if (!codigoconcuenta3digitos(ui.codigolineEdit->text()))
             {
              QMessageBox::warning( this, tr("Edición de plan contable"),
              tr("ERROR: EL CÓDIGO %1 NO ESTÁ ASIGNADO A CUENTA DE TRES DÍGITOS").arg(ui.codigolineEdit->text()) );
              ui.codigolineEdit->setFocus();
              ui.codigolineEdit->setText("");
              ui.datosgroupBox->setChecked(false);
             }
        else if (ui.codigolineEdit->text().length()!=anchocuentas() || !esauxiliar(ui.codigolineEdit->text()))
                ui.descripcionlineEdit->setFocus();
      }

}

// ***************************************************************************************
void subcuentas::filaclicked()
{

    if (ui.treeWidget->currentItem()!=0)
       {
        if (esauxiliar(ui.treeWidget->currentItem()->text(0)))
          {
           ui.codigolineEdit->setText(ui.treeWidget->currentItem()->text(0));
           ui.descripcionlineEdit->setText(ui.treeWidget->currentItem()->text(1));
           ui.aceptarpushButton->setEnabled(false);
           if (ui.datosgroupBox->isChecked()) ui.aceptarpushButton->setEnabled(true);
          }
        else
            {
            ui.datosgroupBox->setChecked(false);
            resetdatosaccesorios();
            ui.datosgroupBox->setEnabled(false);
            ui.codigolineEdit->setText("");
            ui.descripcionlineEdit->setText("");
            }
       }
/*
 QList<QVariant> datos = modeloarbol.datosfiladeindice(indice);
 if (datos.at(0).toString().length()==anchocuentas() || esauxiliar(datos.at(0).toString()))
   {
    ui.codigolineEdit->setText(datos.at(0).toString());
    ui.descripcionlineEdit->setText(datos.at(1).toString());
    ui.aceptarpushButton->setEnabled(false);
    if (ui.datosgroupBox->isChecked()) ui.aceptarpushButton->setEnabled(true);
   }
else {
    
    ui.datosgroupBox->setChecked(false);
    resetdatosaccesorios();
    ui.datosgroupBox->setEnabled(false);
    ui.codigolineEdit->setText("");
    ui.descripcionlineEdit->setText("");
   }
   */
}


void subcuentas::fila_tabla_clicked()
{

    QModelIndex indiceactual=ui.tableView->currentIndex();
    QString codigo,descripcion;
    if (indiceactual.isValid())
       {
        codigo=modeloreg->record(indiceactual.row()).value("codigo").toString();
        descripcion=modeloreg->record(indiceactual.row()).value("descripcion").toString();
        ui.codigolineEdit->setText(codigo);
        ui.descripcionlineEdit->setText(descripcion);
        ui.aceptarpushButton->setEnabled(false);
        if (ui.datosgroupBox->isChecked()) ui.aceptarpushButton->setEnabled(true);

       }
}



void subcuentas::descripcambiada()
{
 if (ui.descripcionlineEdit->text().length()==0)
     {
      ui.aceptarpushButton->setEnabled(false);
     }
   else
     {
      ui.aceptarpushButton->setEnabled(true);
      ui.datosgroupBox->setEnabled(true);
     }
}

void subcuentas::botonaceptar()
{
 QString cadena;
 if (existecodigoplan(ui.codigolineEdit->text(),&cadena))
      {
       guardacambiosplan(ui.codigolineEdit->text(),ui.descripcionlineEdit->text());
       if (existecodigoplan(ui.codigolineEdit->text(),&cadena))
         {
           if (ui.datosgroupBox->isChecked())
            {
              QString codiva=ui.tipoIVAcomboBox->currentText().section("//",0,0).trimmed();
              QString codvenci=ui.vencicomboBox->currentText().section("//",0,0).trimmed();
              QString codpais=ui.paiscomboBox->currentText().section(" ",0,0).trimmed();
              QString codclaveidfiscal=ui.claveidfiscalcomboBox->currentText().section("//",0,0).trimmed();

             guardadatosaccesorios(
              ui.codigolineEdit->text(),
	      ui.NombreLineEdit->text(),
              ui.ncomerciallineEdit->text(),
	      ui.NifLineEdit->text(),
              ui.NifReplegalLineEdit->text(),
	      ui.DomicilioLineEdit->text(),
	      ui.PoblacionLineEdit->text(),
	      ui.CpLineEdit->text(),
	      ui.ProvinciaLineEdit->text(),
              codpais,
	      ui.TfnoLineEdit->text(),
	      ui.FaxLineEdit->text(),
	      ui.EmailLineEdit->text(),
	      ui.ObservacionesLineEdit->text(),
              ui.ccclineEdit->text(),
              ui.cuotalineEdit->text(),
              ui.ivaasocgroupBox->isChecked(),
              codiva, ui.cuentaIVAlineEdit->text(),
              ui.ctaBaseIVAlineEdit->text(),
              ui.venciasocgroupBox->isChecked(),
              codvenci,
              ui.cuenta_tesorlineEdit->text(),
              ui.conceptodiariolineEdit->text(),
              ui.weblineEdit->text(),
              codclaveidfiscal,
              ui.procvtocheckBox->isChecked(),
              // pais_dat, qtipo_ret, ret_arrendamiento, tipo_operacion_ret,
              ui.paislineEdit->text(),
              ui.tabWidget->isTabEnabled(2) ? ui.tiporetlineEdit->text() :"0",
              ui.alquilercheckBox->isChecked(),
              ui.operacionretcomboBox->currentText().left(1),
              ui.diafijocheckBox->isChecked() ? ui.vfijospinBox->value() :0,
              // QString cuenta_ret_asig, bool es_ret_asig_arrend,
              // QString tipo_ret_asig, QString tipo_oper_ret_asig,
              ui.retgroupBox->isChecked() ? ui.cuentata_ret_asiglineEdit->text() :"",
              ui.retgroupBox->isChecked() ? ui.es_ret_asigcheckBox->isChecked() : false,
              ui.retgroupBox->isChecked() ? ui.porc_ret_asig_lineEdit->text(): "0",
              ui.retgroupBox->isChecked() ? ui.tipo_oper_ret_asigcomboBox->currentText().left(1) : "",
              ui.domiciliacheckBox->isChecked(),
              ui.ibanlineEdit->text(),
              "",
              "",
              "",
              ui.biclineEdit->text(),
              "",
              ui.caja_ivacheckBox->isChecked(),
              ui.ref_madatolineEdit->text(),
              ui.firma_mandatodateEdit->date(),
              ui.forma_pago_comboBox->currentText(),
              fototexto() );
             guarda_tipo_proveedor();
             }
          if (!modolista)
           {
             bool encontrada;
             QTreeWidgetItem *elemento;
             elemento=buscacadena(ui.codigolineEdit->text(), &encontrada);
             // elemento=modeloarbol.buscacadena(modeloarbol.raiz(),ui.codigolineEdit->text(),&encontrada);
             if (encontrada)
                {
                 ui.treeWidget->currentItem()->setText(1,cadena);
                 ui.treeWidget->setCurrentItem(elemento);
                }
           } else refrescar(filtrocodigo());
         }
      }
  else
      {
       if (cod_longitud_variable())
          inserta_auxiliar_enplan(ui.codigolineEdit->text(),ui.descripcionlineEdit->text());
        else
          insertaenplan(ui.codigolineEdit->text(),ui.descripcionlineEdit->text());
       if (existecodigoplan(ui.codigolineEdit->text(),&cadena))
           {
              QString codiva=ui.tipoIVAcomboBox->currentText().section("//",0,0).trimmed();
              QString codvenci=ui.vencicomboBox->currentText().section("//",0,0).trimmed();
              // QString codpais=ui.paiscomboBox->currentText().section(" ",0,0).trimmed();
              QString codclaveidfiscal=ui.claveidfiscalcomboBox->currentText().section("//",0,0).trimmed();
           if (ui.datosgroupBox->isChecked())
            guardadatosaccesorios(
              ui.codigolineEdit->text(),
	      ui.NombreLineEdit->text(),
              ui.ncomerciallineEdit->text(),
	      ui.NifLineEdit->text(),
              ui.NifReplegalLineEdit->text(),
	      ui.DomicilioLineEdit->text(),
	      ui.PoblacionLineEdit->text(),
	      ui.CpLineEdit->text(),
	      ui.ProvinciaLineEdit->text(),
              ui.paiscomboBox->currentText(),
	      ui.TfnoLineEdit->text(),
	      ui.FaxLineEdit->text(),
	      ui.EmailLineEdit->text(),
	      ui.ObservacionesLineEdit->text(),
              ui.ccclineEdit->text(),
              ui.cuotalineEdit->text(),
              ui.ivaasocgroupBox->isChecked(),
              codiva, ui.cuentaIVAlineEdit->text(),
              ui.ctaBaseIVAlineEdit->text(),
              ui.venciasocgroupBox->isChecked(),
              codvenci, ui.cuenta_tesorlineEdit->text(),
              ui.conceptodiariolineEdit->text(),
              ui.weblineEdit->text(),
              codclaveidfiscal,
              ui.procvtocheckBox->isChecked(),
              // pais_dat, qtipo_ret, ret_arrendamiento, tipo_operacion_ret,
              ui.paislineEdit->text(),
              ui.tabWidget->isTabEnabled(2) ? ui.tiporetlineEdit->text() : "0",
              ui.alquilercheckBox->isChecked(),
              ui.operacionretcomboBox->currentText().left(1),
              ui.diafijocheckBox->isChecked() ? ui.vfijospinBox->value() : 0,
              ui.retgroupBox->isChecked() ? ui.cuentata_ret_asiglineEdit->text() :"",
              ui.retgroupBox->isChecked() ? ui.es_ret_asigcheckBox->isChecked() : false,
              ui.retgroupBox->isChecked() ? ui.porc_ret_asig_lineEdit->text(): "0",
              ui.retgroupBox->isChecked() ? ui.tipo_oper_ret_asigcomboBox->currentText().left(1) : "",
              ui.domiciliacheckBox->isChecked(),
              ui.ibanlineEdit->text(),
              "",
              "",
              "",
              ui.biclineEdit->text(),
              "",
              ui.caja_ivacheckBox->isChecked(),
              ui.ref_madatolineEdit->text(),
              ui.firma_mandatodateEdit->date(),
              ui.forma_pago_comboBox->currentText(),
              fototexto() );
            guarda_tipo_proveedor();
           // ui.cuentata_ret_asiglineEdit->clear();
           // ui.es_ret_asigcheckBox->setChecked(false);
           // ui.tipo_oper_ret_asigcomboBox->setCurrentIndex(0);
           // ui.porc_ret_asig_lineEdit->clear();
              if (!modolista)
                {
                 // if (!cod_longitud_variable())
                  //insertacuentaarbol(ui.codigolineEdit->text(),cadena);
                 // else
                 // insertacuentaauxiliararbol(ui.codigolineEdit->text(),cadena);

                 if (existecodigoplan(ui.codigolineEdit->text(),&cadena))
                     {
                      //QTreeWidgetItem *elemento;
                      insertacuentaarbol(ui.codigolineEdit->text(),cadena);
                      }
                 bool encontrada;
                 QTreeWidgetItem *elemento;
                 elemento=buscacadena(ui.codigolineEdit->text(),&encontrada);
                 if (encontrada)
                     ui.treeWidget->setCurrentItem(elemento);
                } else refrescar(filtrocodigo());
           }
      }

  if (!existecodigosaldosubcuenta(ui.codigolineEdit->text()))
     insertasaldosubcuenta(ui.codigolineEdit->text());

 activacambiacod();
 ui.aceptarpushButton->setEnabled(false);
// QMessageBox::information( this, tr("Edición de plan contable"),
//                             tr("Se han efectuado los cambios en el plan contable"));


}

void subcuentas::botonborrar()
{
   // falta mensaje está seguro ?

  QMessageBox msgBox;
  msgBox.addButton(QObject::tr("Sí"), QMessageBox::ActionRole);
  QPushButton *abortButton = msgBox.addButton(QObject::tr("No"), QMessageBox::ActionRole);


  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText(tr("¿ DESEA ELIMINAR LA SUBCUENTA SELECCIONADA ?"));

  msgBox.exec();

  if (msgBox.clickedButton() == abortButton) 
      return;

   if (cuentaendiario(ui.codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Edición de subcuentas"),
       tr("No se puede elimimar la subcuenta porque existe en el diario"));
       return;
    }

   if (cuentaenpresupuesto(ui.codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Edición de subcuentas"),
       tr("No se puede elimimar la subcuenta porque existe en presupuestos"));
       return;
    }

   if (cuentaen_ajustes_conci(ui.codigolineEdit->text()))
     {
       QMessageBox::warning( this, tr("Edición de subcuentas"),
       tr("No se puede elimimar la subcuenta porque existe en ajustes de conciliación"));
       return;
     }

   if (cuentaencuenta_ext_conci(ui.codigolineEdit->text()))
     {
       QMessageBox::warning( this, tr("Edición de subcuentas"),
       tr("No se puede elimimar la subcuenta porque existe en externa de conciliación"));
       return;
     }

   eliminasubcuenta(ui.codigolineEdit->text());

   if (!modolista)
   {
       lista_codigo.clear();
       lista_puntero_item.clear();
       ui.treeWidget->clear();
       ui.codigolineEdit->setText("");
       ui.descripcionlineEdit->setText("");
       cargaarbol();
   } else refrescar(filtrocodigo());

   QMessageBox::information( this, tr("Edición de subcuentas"),
                             tr("Se ha eliminado el código seleccionado"));
   ui.codigolineEdit->setText("");
   ui.descripcionlineEdit->setText("");
   //QMessageBox::information( this, tr("Edición de subcuentas"),
   //                          tr("-----------"));
   activacambiacod();
   // QMessageBox::information( this, tr("Edición de subcuentas"),
   //                          tr("-----------"));

}
 
void subcuentas::grupodatospulsado()
{
 if (!ui.datosgroupBox->isChecked())
    {
     QMessageBox msgBox;
     msgBox.addButton(QObject::tr("Sí"), QMessageBox::ActionRole);
     QPushButton *abortButton = msgBox.addButton(QObject::tr("No"), QMessageBox::ActionRole);


     msgBox.setIcon(QMessageBox::Warning);
     msgBox.setText(tr("¿ DESEA ELIMINAR LOS DATOS ACCESORIOS ? (OPERACIÓN IRREVERSIBLE)"));

     msgBox.exec();

     if (msgBox.clickedButton() == abortButton) 
      { ui.datosgroupBox->setChecked(true); return;}
     resetdatosaccesorios();
     // queda borrar datos accesorios físicamente
     eliminadatosaccesorios(ui.codigolineEdit->text());
     desactivaconexdatos();
    }
  else
    {
     ui.aceptarpushButton->setEnabled(true);
     ui.tabWidget->setEnabled(true);
     activaconexdatos();
    }

}

void subcuentas::pasacodigo(QString qcodigo)
{
  ui.codigolineEdit->setText(qcodigo);
}

void subcuentas::desactivacambiacod()
{
 ui.codigolineEdit->setEnabled(false);
 ui.codigopushButton->setEnabled(false);
 ui.descripcionlineEdit->setEnabled(false);
 ui.treeWidget->setEnabled(false);
 ui.tableView->setEnabled(false);
 ui.aceptarpushButton->setEnabled(true);
}

void subcuentas::activacambiacod()
{
 ui.codigolineEdit->setEnabled(true);
 ui.codigopushButton->setEnabled(true);
 ui.descripcionlineEdit->setEnabled(true);
 ui.treeWidget->setEnabled(true);
 ui.tableView->setEnabled(true);
}

void subcuentas::activaconexdatos()
{
  connect(ui.NombreLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.ncomerciallineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.DomicilioLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.PoblacionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.CpLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.NifLineEdit,SIGNAL(textChanged(QString)),this,SLOT(nifcambiado()));
  connect(ui.NifReplegalLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.paiscomboBox,SIGNAL(currentIndexChanged ( int)),this,SLOT(desactivacambiacod()));
  connect(ui.claveidfiscalcomboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));
  connect(ui.TfnoLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.weblineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.FaxLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.EmailLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.ObservacionesLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.ccclineEdit,SIGNAL(textChanged(QString)),this,SLOT(ccccambiado()));
  connect(ui.NombreLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.cuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.cuentaIVAlineEdit,SIGNAL( textChanged(QString)),this,SLOT(cta_IVA_cambiada()));
  connect(ui.ctaBaseIVAlineEdit,SIGNAL(textChanged(QString)),this,SLOT(cta_base_IVA_cambiada()));

  connect(ui.cuenta_tesorlineEdit,SIGNAL( textChanged(QString)),this,SLOT(cta_tesor_cambiada()));
  connect(ui.tipoIVAcomboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));
  connect(ui.conceptodiariolineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.vencicomboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));
  connect(ui.ivaasocgroupBox,SIGNAL(toggled(bool) ),this,SLOT(desactivacambiacod()));
  connect(ui.venciasocgroupBox,SIGNAL(toggled(bool) ),this,SLOT(desactivacambiacod()));

  connect(ui.paislineEdit,SIGNAL( textChanged(QString)),this,SLOT(desactivacambiacod()));


  connect(ui.operacionretcomboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));
  connect(ui.alquilercheckBox,SIGNAL(stateChanged(int)),this,SLOT(cambia_alquilercheckBox()));
  connect(ui.tiporetlineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));

  connect(ui.retgroupBox,SIGNAL(toggled(bool)),this,SLOT(desactivacambiacod()));
  connect(ui.cuentata_ret_asiglineEdit,SIGNAL(textChanged(QString)),this, SLOT(cta_ret_asig_cambiada()));
  connect(ui.es_ret_asigcheckBox,SIGNAL(stateChanged(int)),this,SLOT(cambia_alquiler_asig_checkBox()));
  connect(ui.tipo_oper_ret_asigcomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.porc_ret_asig_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));

  connect(ui.domiciliacheckBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.ibanlineEdit,SIGNAL(textChanged(QString)),this,SLOT(ibancambiado()));
  connect(ui.biclineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.ref_madatolineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
  connect(ui.firma_mandatodateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(desactivacambiacod()));
  connect(ui.caja_ivacheckBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.forma_pago_comboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));

  connect(ui.tipo_proveedor_comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.homologacion_comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.actividad_comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(desactivacambiacod()));

  connect(ui.homologacion_dateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(desactivacambiacod()));

  connect(ui.activo_checkBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.acceso_lopd_checkBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.cumplimiento_lopd_checkBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.acceso_prl_checkBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.coord_prl_checkBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.cert_ss_checkBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.cert_hda_checkBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
  connect(ui.no_correo_pub_checkBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));

}

void subcuentas::nifcambiado()
{
  if (!ui.NifLineEdit->text().isEmpty())
    {
     if (isNifCifNie(ui.NifLineEdit->text())>0)
        ui.check_ciflabel->show();
      else ui.check_ciflabel->hide();
    }
   else ui.check_ciflabel->hide();
  desactivacambiacod();
}


void subcuentas::ccccambiado()
{
  if (!ui.ccclineEdit->text().isEmpty())
    {
     if (ccc_correcto(ui.ccclineEdit->text())) {
        ui.check_ccclabel->show();
        // comprobamos si el campo iban tiene contenido
        if (ui.ibanlineEdit->text().isEmpty()) {
            ui.ibanlineEdit->setText("ES"+digito_cotrol_IBAN("ES00"+ui.ccclineEdit->text().remove('-'))+ui.ccclineEdit->text().remove('-'));
        }
     }
      else ui.check_ccclabel->hide();
    }
   else ui.check_ccclabel->hide();
  desactivacambiacod();
}


void subcuentas::ibancambiado()
{
  if (!ui.ibanlineEdit->text().isEmpty())
    {
     if (IBAN_correcto(ui.ibanlineEdit->text()))
        ui.check_ibanlabel->show();
      else ui.check_ibanlabel->hide();
    }
   else ui.check_ibanlabel->hide();
  desactivacambiacod();
}



void subcuentas::desactivaconexdatos()
{
  ui.NombreLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.ncomerciallineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.DomicilioLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.PoblacionLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.CpLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.NifLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.NifReplegalLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.paiscomboBox->disconnect(SIGNAL(currentIndexChanged ( int)));
  ui.claveidfiscalcomboBox->disconnect(SIGNAL(currentIndexChanged ( int)));
  ui.TfnoLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.FaxLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.EmailLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.ObservacionesLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.ccclineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.NombreLineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.cuotalineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.cuentaIVAlineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.ctaBaseIVAlineEdit->disconnect(SIGNAL(textChanged(QString)));

  ui.cuenta_tesorlineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.tipoIVAcomboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));
  ui.vencicomboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));
  ui.conceptodiariolineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.weblineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.ivaasocgroupBox->disconnect(SIGNAL(toggled(bool) ));
  ui.venciasocgroupBox->disconnect(SIGNAL(toggled(bool) ));

  ui.paislineEdit->disconnect(SIGNAL(textChanged(QString)));

  ui.operacionretcomboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));
  ui.alquilercheckBox->disconnect(SIGNAL(stateChanged(int)));
  ui.tiporetlineEdit->disconnect(SIGNAL(textChanged(QString)));

  ui.retgroupBox->disconnect(SIGNAL(toggled(bool)));
  ui.cuentata_ret_asiglineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.es_ret_asigcheckBox->disconnect(SIGNAL(stateChanged(int)));
  ui.tipo_oper_ret_asigcomboBox->disconnect(SIGNAL(currentIndexChanged(int)));
  ui.porc_ret_asig_lineEdit->disconnect(SIGNAL(textChanged(QString)));

  ui.domiciliacheckBox->disconnect(SIGNAL(stateChanged(int)));
  ui.ibanlineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.biclineEdit->disconnect(SIGNAL(textChanged(QString)));
  ui.ref_madatolineEdit->disconnect(SIGNAL(textChanged( QString)));
  ui.firma_mandatodateEdit->disconnect(SIGNAL(dateChanged(QDate)));

  ui.caja_ivacheckBox->disconnect(SIGNAL(stateChanged(int)));

  ui.forma_pago_comboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));

  ui.tipo_proveedor_comboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));
  ui.homologacion_comboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));
  ui.actividad_comboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));

  ui.homologacion_dateEdit->disconnect(SIGNAL(dateChanged(QDate)));


  ui.activo_checkBox->disconnect(SIGNAL(stateChanged(int)));
  ui.acceso_lopd_checkBox->disconnect(SIGNAL(stateChanged(int)));
  ui.cumplimiento_lopd_checkBox->disconnect(SIGNAL(stateChanged(int)));
  ui.acceso_prl_checkBox->disconnect(SIGNAL(stateChanged(int)));
  ui.coord_prl_checkBox->disconnect(SIGNAL(stateChanged(int)));
  ui.cert_ss_checkBox->disconnect(SIGNAL(stateChanged(int)));
  ui.cert_hda_checkBox->disconnect(SIGNAL(stateChanged(int)));
  ui.no_correo_pub_checkBox->disconnect(SIGNAL(stateChanged(int)));

}


void subcuentas::importafoto()
{
  QString cadfich;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  if (dir->exec() == QDialog::Accepted)
    {
      cadfich=dir->ruta_actual();
    }
  delete(dir);
#else

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  dialogofich.setNameFilter(tr("Archivos de gráficos (*.png *.jpg *.jpeg *.bmp)"));
  dialogofich.setDirectory(getenv("HOME"));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE FICHERO GRÁFICO"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           cadfich=fileNames.at(0);
          }
     }

#endif

  if (cadfich.isEmpty()) return;

  QPixmap imagen(cadfich);
  // ----------------------------------------------------------------------
  graf_recorte *g = new graf_recorte(cadfich,ui.fotolabel->width(),ui.fotolabel->height());
  int resultado=g->exec();
  int altolabel=ui.fotolabel->height();
  int ancholabel=ui.fotolabel->width();
  if (resultado==QDialog::Accepted)
    {
     QPixmap ima=g->recorte();
     //ui.fotolabel->setPixmap(g->recorte());
     //int altopix=ima.height();
     //int anchopix=ima.width();
     QPixmap definitiva;
     definitiva=ima.scaledToHeight(altolabel);
     ui.fotolabel->setScaledContents(false);
     ui.fotolabel->setPixmap(definitiva);
    }
    else
     {
     // QMessageBox::information( this, tr("SUBCUENTAS"),tr("ACEPTADO"));
     // ----------------------------------------------------------------------
      int altopix=imagen.height();
      int anchopix=imagen.width();
      QPixmap definitiva;
      if (altopix/altolabel>anchopix/ancholabel)
        definitiva=imagen.scaledToHeight(altolabel);
      else
        definitiva=imagen.scaledToWidth(ancholabel);
      ui.fotolabel->setScaledContents(false);
      ui.fotolabel->setPixmap(definitiva);
     }
  delete(g);
  desactivacambiacod();

}

QString subcuentas::fototexto()
{
 if (ui.fotolabel->pixmap().isNull()) return "";
 QPixmap foto=ui.fotolabel->pixmap();
 QByteArray bytes;
 QBuffer buffer(&bytes);
 buffer.open(QIODevice::WriteOnly);
 foto.save(&buffer, "PNG"); // writes pixmap into foto in PNG format
 QByteArray cadhexa=bytes.toHex();
 QString cadena(cadhexa);

 return cadena;

}

void subcuentas::borrafoto()
{
  if (QMessageBox::question(
            this,
            tr("¿ Borrar imagen ?"),
         tr("¿ Desea borrar la imagen actual ?")) == QMessageBox::Yes )
               {
                QPixmap foto;
                ui.fotolabel->setPixmap(foto);
                desactivacambiacod();
               }
}

void subcuentas::botonctaivapulsado()
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


void subcuentas::botonctaretpulsado()
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




void subcuentas::botoncta_tesorpulsado()
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

void subcuentas::cta_IVA_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.cuentaIVAlineEdit->text(),&cadena))
    ui.descripctaIVAlabel->setText(cadena);
  else
    ui.descripctaIVAlabel->setText("");
 desactivacambiacod();
}

void subcuentas::cta_tesor_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.cuenta_tesorlineEdit->text(),&cadena))
    ui.descrip_cta_tesorlabel->setText(cadena);
  else
    ui.descrip_cta_tesorlabel->setText("");
 desactivacambiacod();
}

void subcuentas::cta_ret_asig_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.cuentata_ret_asiglineEdit->text(),&cadena))
    ui.cta_ret_label->setText(cadena);
  else
    ui.cta_ret_label->setText("");
 desactivacambiacod();
}

void subcuentas::cambia_alquilercheckBox()
{
 if (ui.alquilercheckBox->isChecked()) ui.operacionretcomboBox->setEnabled(false);
   else ui.operacionretcomboBox->setEnabled(true);
 desactivacambiacod();
}

void subcuentas::cambia_alquiler_asig_checkBox()
{
 if (ui.es_ret_asigcheckBox->isChecked()) ui.tipo_oper_ret_asigcomboBox->setEnabled(false);
   else ui.tipo_oper_ret_asigcomboBox->setEnabled(true);
 desactivacambiacod();
}


void subcuentas::cargacomboIVA()
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


void subcuentas::cargacombovenci()
{
  QStringList lista=basedatos::instancia()->listatiposdescripvenci();
  ui.vencicomboBox->addItems (lista);
}


void subcuentas::cta_IVA_finedicion()
{
    ui.cuentaIVAlineEdit->setText(expandepunto(ui.cuentaIVAlineEdit->text(),anchocuentas()));
}

void subcuentas::cta_tesor_finedicion()
{
   ui.cuenta_tesorlineEdit->setText(expandepunto(ui.cuenta_tesorlineEdit->text(),anchocuentas()));
}


void subcuentas::cta_ret_asig_finedicion()
{
    ui.cuentata_ret_asiglineEdit->setText(expandepunto(ui.cuentata_ret_asiglineEdit->text(),anchocuentas()));

}

void subcuentas::cargacomboclaveidfiscal()
{
  QStringList lista=basedatos::instancia()->listaclaveidfiscal();
  ui.claveidfiscalcomboBox->addItems (lista);
}

void subcuentas::cargacombooperacionret()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_ret();
  ui.operacionretcomboBox->addItems (lista);
  ui.tipo_oper_ret_asigcomboBox->addItems(lista);
}



void subcuentas::cargacombo_formas_pago()
{
  QStringList lista;
  lista << "- -";
  QSqlQuery q=basedatos::instancia()->carga_formas_pago();
  if (q.isActive())
      while (q.next())
        {
          lista << q.value(0).toString();
        }

  ui.forma_pago_comboBox->addItems (lista);

}

void subcuentas::cargacombopaises()
{
  QStringList lista=basedatos::instancia()->listapaises();
  ui.paiscomboBox->addItems (lista);
  QString pais=CODPAIS;
  int indice=0;
  while (indice<ui.paiscomboBox->count())
                   {
                    QString texto=ui.paiscomboBox->itemText(indice);
                    QString extracto=texto.section(" ",0,0).trimmed();
                    if (extracto==pais) break;
                    indice++;
                   }
  if (indice>ui.paiscomboBox->count()) indice--;
  ui.paiscomboBox->setCurrentIndex(indice);

}


void subcuentas::cargacombos_tipoProveedor()
{
  QStringList lista1=basedatos::instancia()->tipos_proveedor();
  QStringList lista2=basedatos::instancia()->homologaciones();
  QStringList lista3=basedatos::instancia()->actividades();

  ui.tipo_proveedor_comboBox->addItem("- -");
  ui.homologacion_comboBox->addItem("- -");
  ui.actividad_comboBox->addItem("- -");

  ui.tipo_proveedor_comboBox->addItems(lista1);
  ui.homologacion_comboBox->addItems(lista2);
  ui.actividad_comboBox->addItems(lista3);

}

void subcuentas::venciasoccambiado()
{
    if (ui.venciasocgroupBox->isChecked())
      {
        if ((conanalitica() && (escuentadegasto(ui.codigolineEdit->text()) ||
                               escuentadeingreso(ui.codigolineEdit->text()))) ||
            (conanalitica_parc() && codigo_en_analitica(ui.codigolineEdit->text())))
          {
            QMessageBox::warning( this, tr("Edición de plan contable"),
                             tr("ERROR: Las cuentas de analítica no pueden poseer vencimientos"));
            ui.venciasocgroupBox->setChecked(false);
          }
      }
}

void subcuentas::tiporet_finedicion()
{
  if (haycomadecimal()) ui.tiporetlineEdit->setText(convacoma(ui.tiporetlineEdit->text()));
}

void subcuentas::checkdiafijo()
{
   if (ui.diafijocheckBox->isChecked()) ui.vfijospinBox->setEnabled(true);
     else ui.vfijospinBox->setEnabled(false);
}

void subcuentas::botoncodigo_pulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.codigolineEdit->text());
    int rdo=labusqueda->exec();
    if (rdo==QDialog::Accepted)
        cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.codigolineEdit->setText(cadena2);
       else ui.codigolineEdit->clear();
    delete(labusqueda);

}


QString subcuentas::filtrocodigo()
{
    QString cadena;
       if (es_sqlite())
          {
           cadena="codigo like '";
           cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
           cadena+="%'";
          }
       else
         {
          cadena+="position('";
          cadena+=ui.codigolineEdit->text().left(-1).replace("'","''");
          cadena+="' in codigo)=1";
         }

       return cadena;
}


void subcuentas::refrescar(QString filtro)
{
    modeloreg->setTable("plancontable");

    modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
    modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));

    QString cadena;
    if (!cod_longitud_variable())
       {
        if ( es_sqlite() ) cadena="length(codigo)=";
        else cadena="char_length(codigo)=";
        QString cadnum;
        cadnum.setNum(anchocuentas());
        cadena+=cadnum;
       }
       else {
           if ( es_sqlite() ) { cadena = "auxiliar = 1"; }
           else { cadena = "auxiliar"; }
       }
    if (!filtro.isEmpty()) cadena+=" and ";
    cadena+=filtro;
    modeloreg->setFilter (cadena);
    modeloreg->setSort(0,Qt::AscendingOrder);
    modeloreg->select();
    ui.tableView->setModel(modeloreg);
    //ui.tabla->setSortingEnabled(TRUE);
    ui.tableView->setColumnWidth(0,100);
    ui.tableView->setColumnWidth(1,620);
    ui.tableView->hideColumn(2);
    ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

    ui.tableView->show();
}


void subcuentas::cta_base_IVA_finedicion()
{
    ui.ctaBaseIVAlineEdit->setText(expandepunto(ui.ctaBaseIVAlineEdit->text(),anchocuentas()));
}


void subcuentas::botoncta_base_ivapulsado()
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


void subcuentas::cta_base_IVA_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.ctaBaseIVAlineEdit->text(),&cadena))
    ui.descripctaBaseIVAlabel->setText(cadena);
  else
    ui.descripctaBaseIVAlabel->setText("");
 desactivacambiacod();
}

void subcuentas::boton_ue_pulsado()
{

    bool valido=false;
    QString nif_iva=ui.NifLineEdit->text();
    QString paises="AT BE BG CY CZ DE DK EE EL ES FI FR GB HR HU IE IT LT LU LV MT NL PL PT RO SE SI SK";
    if ((nif_iva.length()>2) && (paises.contains(nif_iva.left(2)))) valido=true;

    if (!valido)
    {
     QMessageBox::warning( this, tr("Verificación NIF-IVA intracomunitario"),
     tr("ERROR: NIF-IVA no válido") );
     return;
    }

    //datos.countryCode="SE";
    //datos.vatNumber="556093619601";
    QString nombre, direccion;
    bool hayerror=false;
    bool correcto=nif_ue_correcto(nif_iva, nombre, direccion, hayerror);
    if (!hayerror)
      {
        vies_msj *v = new vies_msj();
        if (correcto)
         {
          v->haz_num_correcto(nombre,direccion );
          // std::cout << "CORRECTO\n" ;
          // std::cout << respuesta.name->c_str() << std::endl;
          // std::cout << respuesta.address->c_str() << std::endl;
         }
           else v->haz_num_incorrecto();// std::cout << "INCORRECTO" << std::endl;
        v->exec();
        delete(v);
      }
    else
        QMessageBox::warning( this, tr("Verificación NIF-IVA intracomunitario"),
        tr("ERROR en conexión con página de validación") );
        // vatnum.soap_stream_fault(std::cerr);

}


void subcuentas::insertacuentaarbol_inicial(QString cadena1,QString cadena2)
{
  QString cadactual;
  cadactual=cadena1;
  bool seencuentra=false;
  QTreeWidgetItem *elemento;
  // QListViewItem *elemento;
  QStringList datos;
  datos << cadena1 << cadena2;
  for (int veces=1; veces<cadactual.length(); veces++)
      {
        QString extract;
        extract=cadactual.left(cadactual.length()-veces);
        bool encontrada=false;
        elemento=buscacadena_ini(extract,&encontrada);
        if (encontrada)
          {
            if (elemento!=0) {
            QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento,datos);
            lista_codigo << cadena1;
            lista_puntero_item << elemento4;
            // new QTreeWidgetItem(elemento,datos);
            elemento4->setExpanded(true);
            seencuentra=true;
            break;
           }
          }
       }
   if (!seencuentra)
      {
       //QMessageBox::information( this, tr("Edición de plan contable"),
       //                             tr("no se encuentra ")+ datos.at(0));
       QTreeWidgetItem *elemento = new QTreeWidgetItem(ui.treeWidget,datos);
       lista_codigo << cadena1;
       lista_puntero_item << elemento;
       elemento->setExpanded(true);
       // ui.treeWidget->setCurrentItem(elemento);
      }
}


QTreeWidgetItem *subcuentas::buscacadena(QString cadena, bool *encontrada)
{
    QModelIndex indice;
    // QTreeWidgetItem *guarda=ui.Estado1treeWidget->currentItem();
    QTreeWidgetItem *primero=0;
    indice = ui.treeWidget->currentIndex();
    if (indice.isValid())
     {
       while (ui.treeWidget->indexAbove(ui.treeWidget->currentIndex()).isValid())
           ui.treeWidget->setCurrentIndex(ui.treeWidget->indexAbove(ui.treeWidget->currentIndex()));
       primero=ui.treeWidget->currentItem();
       while (true)
         {
           QTreeWidgetItem *elemento;
           elemento=ui.treeWidget->currentItem();
           //ui.treeWidget->expandItem(elemento);
           if (cadena==elemento->text(0))
              {
               *encontrada=true;
               return ui.treeWidget->currentItem();
              }
           if (!ui.treeWidget->indexBelow(ui.treeWidget->currentIndex()).isValid()) break;
           ui.treeWidget->setCurrentIndex(ui.treeWidget->indexBelow(ui.treeWidget->currentIndex()));
         }
     }
    if (primero!=0) return primero;
      else return 0;
}

QTreeWidgetItem *subcuentas::buscacadena_ini(QString cadena, bool *encontrada)
{
    int pos=0;
    int elementos=lista_codigo.count();
    while (pos<elementos)
      {
       if (lista_codigo.at(pos)==cadena)
          {
           *encontrada=true;
           return lista_puntero_item.at(pos);
          }
       pos++;
      }
    return 0;
}


void subcuentas::primer_elemento_ini()
{
    int elementos=lista_codigo.count();
    if (elementos>0)
       {
         ui.treeWidget->setCurrentItem(lista_puntero_item.at(0));
       }
}


void subcuentas::refresca_cuentas()
{
    if (!modolista)
    {
        lista_codigo.clear();
        lista_puntero_item.clear();
        ui.treeWidget->clear();
        ui.codigolineEdit->setText("");
        ui.descripcionlineEdit->setText("");
        cargaarbol();
    } else refrescar(filtrocodigo());

}

void subcuentas::guarda_tipo_proveedor() {
  if (!basedatos::instancia()->tipo_proveedor_config()) return;
  basedatos::instancia()->guardadatos_tipo_proveedor(ui.codigolineEdit->text(),
                                  ui.tipo_proveedor_comboBox->currentText().section('-',0,0),
                                  ui.homologacion_comboBox->currentText().section('-',0,0),
                                  ui.actividad_comboBox->currentText().section('-',0,0),
                                  ui.homologacion_dateEdit->date(),
                                  ui.activo_checkBox->isChecked(),
                                  ui.acceso_lopd_checkBox->isChecked(),
                                  ui.cumplimiento_lopd_checkBox->isChecked(),
                                  ui.acceso_prl_checkBox->isChecked(),
                                  ui.coord_prl_checkBox->isChecked(),
                                  ui.cert_ss_checkBox->isChecked(),
                                  ui.cert_hda_checkBox->isChecked(),
                                  ui.no_correo_pub_checkBox->isChecked());


}
