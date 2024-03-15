#include "externos.h"
#include "ui_externos.h"
#include "basedatos.h"
#include "funciones.h"
#include "vies_msj.h"
#include "graf_recorte.h"
#include <QFileDialog>
#include <QMessageBox>
#include "buscasubcuenta.h"
#include "busca_externo.h"

#define CODPAIS "ES"

externos::externos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::externos)
{
    ui->setupUi(this);

    modeloreg=NULL;

    ui->codigolineEdit->setFocus();

    refrescar("");

    cargacomboIVA();
    cargacombovenci();
    cargacomboclaveidfiscal();
    cargacombopaises();
    cargacombooperacionret();
    cargacombo_formas_pago();

    ui->vfijospinBox->setEnabled(false);

    if (basedatos::instancia()->hay_criterio_caja_config())
        ui->caja_ivacheckBox->hide();

    connect(ui->codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado(QString)));
    connect(ui->codigolineEdit,SIGNAL(editingFinished()),this,SLOT(codigofinedicion()));

    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(fila_tabla_clicked()));

    connect(ui->aceptarpushButton,SIGNAL( clicked()),this,SLOT(botonaceptar()));
    connect(ui->borrarpushButton,SIGNAL( clicked()),this,SLOT(botonborrar()));


    connect(ui->fotopushButton,SIGNAL( clicked()),this,SLOT(importafoto()));

    connect(ui->borrafotopushButton,SIGNAL( clicked()),this,SLOT(borrafoto()));

    connect(ui->ctaIVApushButton,SIGNAL( clicked()),this,SLOT(botonctaivapulsado()));
    connect(ui->cta_tesorpushButton,SIGNAL( clicked()),this,SLOT(botoncta_tesorpulsado()));
    connect(ui->ctaretpushButton,SIGNAL( clicked()),this,SLOT(botonctaretpulsado()));

    connect(ui->ctaBaseIVAlineEdit,SIGNAL(editingFinished()),this,SLOT(cta_base_IVA_finedicion()));
    connect(ui->ctaBaseIVApushButton,SIGNAL(clicked(bool)),this,SLOT(botoncta_base_ivapulsado()));
    connect(ui->ctaBaseIVAlineEdit,SIGNAL(textChanged(QString)),this,SLOT(cta_base_IVA_cambiada()));

    connect(ui->cuentaIVAlineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_IVA_finedicion()));
    connect(ui->cuenta_tesorlineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_tesor_finedicion()));

    connect(ui->venciasocgroupBox, SIGNAL(clicked(bool)), this, SLOT(venciasoccambiado()));

    connect(ui->diafijocheckBox,SIGNAL(clicked()),this,SLOT(checkdiafijo()));

    connect(ui->cuentata_ret_asiglineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_ret_asig_finedicion()));

    connect(ui->codigopushButton, SIGNAL(clicked()),this, SLOT(buscar_externo()));

    connect(ui->UE_pushButton,SIGNAL(clicked(bool)),this,SLOT(boton_ue_pulsado()));

    connect(ui->refreshpushButton,SIGNAL(clicked(bool)),this,SLOT(refresca_externos()));

    connect(ui->cancelar_pushButton,SIGNAL(clicked(bool)),this,SLOT(boton_cancelar()));

    connect(ui->busca_cuenta_pushButton,SIGNAL(clicked(bool)),this,SLOT(botoncuenta_pulsado()));


    ui->check_ciflabel->hide();
    ui->check_ccclabel->hide();
    ui->check_ibanlabel->hide();

}

externos::~externos()
{
    delete ui;
}


void externos::cargacombopaises()
{
  QStringList lista=basedatos::instancia()->listapaises();
  ui->paiscomboBox->addItems (lista);
  QString pais=CODPAIS;
  int indice=0;
  while (indice<ui->paiscomboBox->count())
                   {
                    QString texto=ui->paiscomboBox->itemText(indice);
                    QString extracto=texto.section(" ",0,0).trimmed();
                    if (extracto==pais) break;
                    indice++;
                   }
  if (indice>ui->paiscomboBox->count()) indice--;
  ui->paiscomboBox->setCurrentIndex(indice);

}


void externos::cargacomboclaveidfiscal()
{
  QStringList lista=basedatos::instancia()->listaclaveidfiscal();
  ui->claveidfiscalcomboBox->addItems (lista);
}


void externos::refrescar(QString filtro)
{


    if (modeloreg==NULL)
       {
        modeloreg = new QSqlQueryModel;
        ui->tableView->setModel(modeloreg);
       }

    QString cadena="select codigo, razon from externos ";
    if (!filtro.isEmpty()) cadena+="where "+filtro;
    cadena+= " order by codigo";
    modeloreg->clear();
    //modeloreg->query().clear();

    QSqlQuery q;
    q.exec(cadena);
    modeloreg->setQuery(q);

    modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
    modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));

    //modeloreg->select();
    //modeloreg->query().exec();

    //ui->tabla->setSortingEnabled(TRUE);
    ui->tableView->setColumnWidth(0,100);
    ui->tableView->setColumnWidth(1,300);
    ui->tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

    ui->tableView->show();
}


void externos::codigocambiado(QString codigo)
{
    resetdatosaccesorios();
    ui->datosgroupBox->setChecked(false);


    bool encontrada=false;

    refrescar(filtrocodigo());
    encontrada=basedatos::instancia()->existe_externo(codigo);

    if (encontrada)
        {
         //QModelIndex indice;

              ui->aceptarpushButton->setEnabled(false);
              ui->borrarpushButton->setEnabled(true);
              // habilitar grupo de datossubcuenta
              ui->datosgroupBox->setEnabled(true);

              // ver si hay datossubcuenta, activar groupbox y cargarcampos
              QString cuenta;
              QString razon;
              QString ncomercial;
              QString cif;
              // QString nifcomunitario; eliminado
              QString nifrprlegal;
              QString nombre, apellidos;
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
              //QString tipo_ret;
              //QString tipo_operacion_ret;
              int vdiafijo=0;

              QString cuenta_ret_asig; bool es_ret_asig_arrend;
              QString tipo_ret_asig; QString tipo_oper_ret_asig;
              bool dom_bancaria=false;
              QString iban,nriesgo,cedente1,cedente2,bic,sufijo;
              QString ref_mandato; QDate firma_mandato;
              bool caja_iva=false;
              QString formapago;
              // pais_dat, qtipo_ret, ret_arrendamiento, tipo_operacion_ret
              if (basedatos::instancia()->carga_externo(codigo, &cuenta, &razon, &ncomercial, &cif,
                                                        &nombre, &apellidos, &nifrprlegal, &domicilio,
                      &poblacion,&codigopostal, &provincia, &pais, &tfno,
                                  &fax, &email, &observaciones, &ccc, &cuota,
                                  &venciasoc, &codvenci, &tesoreria, &ivaasoc, &cuentaiva, &cta_base_IVA, &tipoiva,
                                  &conceptodiario, &web, &claveidfiscal, &procvto,
                                  &paisdat, &vdiafijo, &cuenta_ret_asig, &es_ret_asig_arrend, &tipo_ret_asig,
                                  &tipo_oper_ret_asig,
                                  &dom_bancaria, &iban, &nriesgo, &cedente1, &cedente2, &bic,
                                  &sufijo,  &ref_mandato, &firma_mandato, &caja_iva, &formapago,
                                  &imagen))

               {
                desactivaconexdatos();
                // Pasamos datos a campos
                ui->cuenta_lineEdit->setText(cuenta);
                ui->NombreLineEdit->setText(razon);
                ui->ncomerciallineEdit->setText(ncomercial);
                ui->DomicilioLineEdit->setText(domicilio);
                ui->PoblacionLineEdit->setText(poblacion);
                ui->ProvinciaLineEdit->setText(provincia);
                ui->CpLineEdit->setText(codigopostal);
                ui->NifLineEdit->setText(cif);
                if (!ui->NifLineEdit->text().isEmpty())
                  {
                   if (isNifCifNie(ui->NifLineEdit->text())>0)
                      ui->check_ciflabel->show();
                  }
                ui->nombre_pf_lineEdit->setText(nombre);
                ui->apellidoslineEdit->setText(apellidos);
                ui->NifReplegalLineEdit->setText(nifrprlegal);
                // ui->PaisLineEdit->setText(pais);
                ui->TfnoLineEdit->setText(tfno);
                ui->FaxLineEdit->setText(fax);
                ui->EmailLineEdit->setText(email);
                ui->ObservacionesLineEdit->setText(observaciones);
                ui->ccclineEdit->setText(ccc);
                if (!ui->ccclineEdit->text().isEmpty())
                  {
                   if (ccc_correcto(ui->ccclineEdit->text())) {
                      ui->check_ccclabel->show();
                      // comprobamos si el campo iban tiene contenido
                      if (ui->ibanlineEdit->text().isEmpty()) {
                          ui->ibanlineEdit->setText("ES"+digito_cotrol_IBAN("ES00"+ui->ccclineEdit->text().remove('-'))+ui->ccclineEdit->text().remove('-'));
                      }
                   }
                  }

                ui->cuotalineEdit->setText(cuota);
                ui->weblineEdit->setText(web);
                ui->caja_ivacheckBox->setChecked(caja_iva);
                // buscamos código de operación
                // si vacío asignamos 1
                if (claveidfiscal.isEmpty()) claveidfiscal="1";
                int indice=0;
                while (indice<ui->claveidfiscalcomboBox->count())
                      {
                       QString texto=ui->claveidfiscalcomboBox->itemText(indice);
                       QString extracto=texto.section("//",0,0).trimmed();
                       if (extracto==claveidfiscal) break;
                       indice++;
                      }
                if (indice>ui->claveidfiscalcomboBox->count()) indice--;
                ui->claveidfiscalcomboBox->setCurrentIndex(indice);

                ui->paislineEdit->setText(paisdat);
                // buscamos código de país
                // si vacío asignamos CODPAIS
                if (pais.length()!=2) pais=CODPAIS;
                indice=0;
                while (indice<ui->paiscomboBox->count())
                      {
                       QString texto=ui->paiscomboBox->itemText(indice);
                       QString extracto=texto.section(" ",0,0).trimmed();
                       if (extracto==pais) break;
                       indice++;
                      }
                if (indice>ui->paiscomboBox->count()) indice--;
                ui->paiscomboBox->setCurrentIndex(indice);

                indice=0;
                while (indice<ui->forma_pago_comboBox->count())
                      {
                       if (ui->forma_pago_comboBox->itemText(indice)==formapago) break;
                       indice++;
                      }
                if (indice>ui->forma_pago_comboBox->count()) indice=0;
                ui->forma_pago_comboBox->setCurrentIndex(indice);

                if (venciasoc)
                  {
                    ui->venciasocgroupBox->setChecked(true);
                    // buscamos codvenci en el combo vencicomboBox
                    ui->diafijocheckBox->setEnabled(true);
                    if (vdiafijo>0)
                       {
                        // ui->diafijocheckBox->setEnabled(true);
                        ui->diafijocheckBox->setChecked(true);
                        ui->vfijospinBox->setEnabled(true);
                        ui->vfijospinBox->setValue(vdiafijo);
                       }
                       else
                          {
                           // ui->diafijocheckBox->setEnabled(false);
                           ui->vfijospinBox->setEnabled(false);
                           ui->diafijocheckBox->setChecked(false);
                          }
                    int indice=0;
                    while (indice<ui->vencicomboBox->count())
                      {
                       QString texto=ui->vencicomboBox->itemText(indice);
                       QString extracto=texto.section("//",0,0).trimmed();
                       if (extracto==codvenci) break;
                       indice++;
                      }
                    if (indice>ui->vencicomboBox->count()) indice--;
                    ui->vencicomboBox->setCurrentIndex(indice);
                    ui->cuenta_tesorlineEdit->setText(tesoreria);
                    ui->descrip_cta_tesorlabel->setText(descripcioncuenta(tesoreria));
                  }
                   else
                        {
                         ui->cuenta_tesorlineEdit->clear();
                         ui->venciasocgroupBox->setChecked(false);
                        }
                ui->procvtocheckBox->setChecked(procvto);
                if (ivaasoc)
                   {
                    ui->ivaasocgroupBox->setChecked(true);
                    // buscamos tipoiva en tipoIVAcomboBox
                    int indice=0;
                    while (indice<ui->tipoIVAcomboBox->count())
                      {
                       QString texto=ui->tipoIVAcomboBox->itemText(indice);
                       QString extracto=texto.section("//",0,0).trimmed();
                       if (extracto==tipoiva) break;
                       indice++;
                      }
                    if (indice>ui->tipoIVAcomboBox->count()) indice--;
                    ui->tipoIVAcomboBox->setCurrentIndex(indice);
                    ui->cuentaIVAlineEdit->setText(cuentaiva);
                    ui->descripctaIVAlabel->setText(descripcioncuenta(cuentaiva));
                    ui->ctaBaseIVAlineEdit->setText(cta_base_IVA);
                    ui->descripctaBaseIVAlabel->setText(descripcioncuenta(cta_base_IVA));
                   }
                   else
                      {
                       ui->ivaasocgroupBox->setChecked(false);
                       ui->cuentaIVAlineEdit->clear();
                      }
                ui->conceptodiariolineEdit->setText(conceptodiario);
                // &paisdat, &tipo_ret, &tipo_operacion_ret, &ret_arrendamiento,
                // QString cuenta_ret_asig; bool es_ret_asig_arrend;
                // QString tipo_ret_asig; QString tipo_oper_ret_asig;
                ui->retgroupBox->setChecked(!cuenta_ret_asig.isEmpty());

                ui->cuentata_ret_asiglineEdit->setText(cuenta_ret_asig);
                ui->cta_ret_label->setText(descripcioncuenta(cuenta_ret_asig));
                ui->es_ret_asigcheckBox->setChecked(es_ret_asig_arrend);
                if (es_ret_asig_arrend) ui->tipo_oper_ret_asigcomboBox->setEnabled(false);
                else {if (ui->retgroupBox->isChecked()) ui->tipo_oper_ret_asigcomboBox->setEnabled(true);}
                ui->porc_ret_asig_lineEdit->setText(haycomadecimal() ? convacoma(tipo_ret_asig):tipo_ret_asig);
                indice=0;
                while (indice<ui->tipo_oper_ret_asigcomboBox->count())
                  {
                   if (ui->tipo_oper_ret_asigcomboBox->itemText(indice).left(1)==tipo_oper_ret_asig.left(1)) break;
                   indice++;
                  }
                if (indice>=ui->tipo_oper_ret_asigcomboBox->count()) indice=0;
                ui->tipo_oper_ret_asigcomboBox->setCurrentIndex(indice);

                // &dom_bancaria, &iban, &nriesgo, &cedente1, &cedente2,
                ui->domiciliacheckBox->setChecked(dom_bancaria);
                if (!iban.isEmpty())
                   ui->ibanlineEdit->setText(iban);
                if (!ui->ibanlineEdit->text().isEmpty())
                  {
                   if (IBAN_correcto(ui->ibanlineEdit->text()))
                      ui->check_ibanlabel->show();
                  }
                ui->biclineEdit->setText(bic);

                ui->ref_madatolineEdit->setText(ref_mandato);
                ui->firma_mandatodateEdit->setDate(firma_mandato);

                QPixmap foto;
                if (imagen.length()>0)
                   {
                    QByteArray byteshexa;
                    byteshexa.append ( imagen.toUtf8() );
                    QByteArray bytes;
                    bytes=bytes.fromHex ( byteshexa );
                    foto.loadFromData ( bytes, "PNG");
                   }
                ui->fotolabel->setPixmap(foto);
                ui->aceptarpushButton->setEnabled(true);
                ui->cancelar_pushButton->setEnabled(true);
                activaconexdatos();
               }
             }
             else
                 {
                  ui->borrarpushButton->setEnabled(false);
                  ui->cancelar_pushButton->setEnabled(false);
                  ui->aceptarpushButton->setEnabled(false);
                  resetdatosaccesorios();
                  ui->datosgroupBox->setEnabled(false);
                 }
}


void externos::boton_cancelar()
{
  ui->codigolineEdit->clear();
  activacambiacod();
}

QString externos::filtrocodigo()
{
    QString cadena;
       if (es_sqlite())
          {
           cadena="codigo like '";
           cadena+=ui->codigolineEdit->text().left(-1).replace("'","''");
           cadena+="%'";
          }
       else
         {
          cadena+="position('";
          cadena+=ui->codigolineEdit->text().left(-1).replace("'","''");
          cadena+="' in codigo)=1";
         }

       return cadena;
}



void externos::activaconexdatos()
{
    connect(ui->NombreLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->ncomerciallineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->cuenta_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->DomicilioLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->PoblacionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->CpLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->NifLineEdit,SIGNAL(textChanged(QString)),this,SLOT(nifcambiado()));
    connect(ui->NifReplegalLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));

    connect(ui->nombre_pf_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->apellidoslineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));

    connect(ui->paiscomboBox,SIGNAL(currentIndexChanged ( int)),this,SLOT(desactivacambiacod()));
    connect(ui->claveidfiscalcomboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));
    connect(ui->TfnoLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->weblineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->FaxLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->EmailLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->ObservacionesLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->ccclineEdit,SIGNAL(textChanged(QString)),this,SLOT(ccccambiado()));
    connect(ui->NombreLineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->cuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->cuentaIVAlineEdit,SIGNAL( textChanged(QString)),this,SLOT(cta_IVA_cambiada()));
    connect(ui->ctaBaseIVAlineEdit,SIGNAL(textChanged(QString)),this,SLOT(cta_base_IVA_cambiada()));

    connect(ui->cuenta_tesorlineEdit,SIGNAL( textChanged(QString)),this,SLOT(cta_tesor_cambiada()));
    connect(ui->tipoIVAcomboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));
    connect(ui->conceptodiariolineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->vencicomboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));
    connect(ui->ivaasocgroupBox,SIGNAL(toggled(bool) ),this,SLOT(desactivacambiacod()));
    connect(ui->venciasocgroupBox,SIGNAL(toggled(bool) ),this,SLOT(desactivacambiacod()));

    connect(ui->paislineEdit,SIGNAL( textChanged(QString)),this,SLOT(desactivacambiacod()));

    connect(ui->retgroupBox,SIGNAL(toggled(bool)),this,SLOT(desactivacambiacod()));
    connect(ui->cuentata_ret_asiglineEdit,SIGNAL(textChanged(QString)),this, SLOT(cta_ret_asig_cambiada()));
    connect(ui->es_ret_asigcheckBox,SIGNAL(stateChanged(int)),this,SLOT(cambia_alquiler_asig_checkBox()));
    connect(ui->tipo_oper_ret_asigcomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(desactivacambiacod()));
    connect(ui->porc_ret_asig_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));

    connect(ui->domiciliacheckBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
    connect(ui->ibanlineEdit,SIGNAL(textChanged(QString)),this,SLOT(ibancambiado()));
    connect(ui->biclineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->ref_madatolineEdit,SIGNAL(textChanged(QString)),this,SLOT(desactivacambiacod()));
    connect(ui->firma_mandatodateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(desactivacambiacod()));
    connect(ui->caja_ivacheckBox,SIGNAL(stateChanged(int)),this,SLOT(desactivacambiacod()));
    connect(ui->forma_pago_comboBox,SIGNAL(currentIndexChanged ( int) ),this,SLOT(desactivacambiacod()));

    connect(ui->cuenta_lineEdit,SIGNAL(editingFinished()),this,SLOT(cuenta_finedicion()));

}


void externos::desactivaconexdatos()
{
    ui->NombreLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->ncomerciallineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->cuenta_lineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->DomicilioLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->PoblacionLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->CpLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->NifLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->NifReplegalLineEdit->disconnect(SIGNAL(textChanged(QString)));

    ui->nombre_pf_lineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->apellidoslineEdit->disconnect(SIGNAL(textChanged(QString)));

    ui->paiscomboBox->disconnect(SIGNAL(currentIndexChanged ( int)));
    ui->claveidfiscalcomboBox->disconnect(SIGNAL(currentIndexChanged ( int)));
    ui->TfnoLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->FaxLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->EmailLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->ObservacionesLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->ccclineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->NombreLineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->cuotalineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->cuentaIVAlineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->ctaBaseIVAlineEdit->disconnect(SIGNAL(textChanged(QString)));

    ui->cuenta_tesorlineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->tipoIVAcomboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));
    ui->vencicomboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));
    ui->conceptodiariolineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->weblineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->ivaasocgroupBox->disconnect(SIGNAL(toggled(bool) ));
    ui->venciasocgroupBox->disconnect(SIGNAL(toggled(bool) ));

    ui->paislineEdit->disconnect(SIGNAL(textChanged(QString)));

    ui->retgroupBox->disconnect(SIGNAL(toggled(bool)));
    ui->cuentata_ret_asiglineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->es_ret_asigcheckBox->disconnect(SIGNAL(stateChanged(int)));
    ui->tipo_oper_ret_asigcomboBox->disconnect(SIGNAL(currentIndexChanged(int)));
    ui->porc_ret_asig_lineEdit->disconnect(SIGNAL(textChanged(QString)));

    ui->domiciliacheckBox->disconnect(SIGNAL(stateChanged(int)));
    ui->ibanlineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->biclineEdit->disconnect(SIGNAL(textChanged(QString)));
    ui->ref_madatolineEdit->disconnect(SIGNAL(textChanged( QString)));
    ui->firma_mandatodateEdit->disconnect(SIGNAL(dateChanged(QDate)));

    ui->caja_ivacheckBox->disconnect(SIGNAL(stateChanged(int)));

    ui->forma_pago_comboBox->disconnect(SIGNAL(currentIndexChanged ( int) ));


}


void externos::resetdatosaccesorios()
{
    desactivaconexdatos();
    ui->NombreLineEdit->setText("");
    ui->ncomerciallineEdit->setText("");
    ui->cuenta_lineEdit->clear();
    ui->DomicilioLineEdit->setText("");
    ui->PoblacionLineEdit->setText("");
    ui->ProvinciaLineEdit->setText("");
    ui->CpLineEdit->setText("");
    ui->NifLineEdit->setText("");
    ui->NifReplegalLineEdit->setText("");
    ui->nombre_pf_lineEdit->setText("");
    ui->apellidoslineEdit->setText("");
    ui->weblineEdit->setText("");
    ui->TfnoLineEdit->setText("");
    ui->FaxLineEdit->setText("");
    ui->EmailLineEdit->setText("");
    ui->ObservacionesLineEdit->setText("");
    ui->ccclineEdit->setText("");
    ui->cuotalineEdit->setText("");
    ui->ivaasocgroupBox->setChecked(false);
    ui->venciasocgroupBox->setChecked(false);
    ui->procvtocheckBox->setChecked(false);
    ui->cuentaIVAlineEdit->clear();
    ui->cuenta_tesorlineEdit->clear();
    ui->conceptodiariolineEdit->clear();
    ui->descripctaIVAlabel->clear();
    ui->descrip_cta_tesorlabel->clear();
    ui->ctaBaseIVAlineEdit->clear();
    ui->descripctaBaseIVAlabel->clear();

    ui->paislineEdit->clear();
    QString pais=CODPAIS;
    int indice=0;
    while (indice<ui->paiscomboBox->count())
                     {
                      QString texto=ui->paiscomboBox->itemText(indice);
                      QString extracto=texto.section(" ",0,0).trimmed();
                      if (extracto==pais) break;
                      indice++;
                     }
    if (indice>ui->paiscomboBox->count()) indice--;
    ui->paiscomboBox->setCurrentIndex(indice);

    ui->diafijocheckBox->setChecked(false);
    ui->vfijospinBox->setEnabled(false);

    ui->cuentata_ret_asiglineEdit->clear();
    ui->es_ret_asigcheckBox->setChecked(false);
    ui->tipo_oper_ret_asigcomboBox->setCurrentIndex(0);
    ui->porc_ret_asig_lineEdit->clear();
    ui->retgroupBox->setChecked(false);

    QPixmap foto;
    ui->fotolabel->setPixmap(foto);
    // delete(ui->fotolabel->pixmap());
    ui->tabWidget->setTabEnabled(2,false);
    ui->check_ciflabel->hide();

    // &dom_bancaria, &iban, &nriesgo, &cedente1, &cedente2,
    ui->domiciliacheckBox->setChecked(false);
    ui->ibanlineEdit->clear();
    ui->biclineEdit->clear();
    ui->ref_madatolineEdit->clear();
    ui->firma_mandatodateEdit->setDate(QDate(2000,1,1));
    ui->caja_ivacheckBox->setChecked(false);
    ui->forma_pago_comboBox->setCurrentIndex(0);

    activaconexdatos();
}


void externos::codigofinedicion()
{

  if (!ui->codigolineEdit->text().isEmpty())
     {
      if (!ui->tableView->hasFocus()) {
         ui->NombreLineEdit->setFocus();
         ui->aceptarpushButton->setEnabled(true);
         ui->datosgroupBox->setEnabled(true);
         ui->cancelar_pushButton->setEnabled(true);
         if (ui->NifLineEdit->text().isEmpty() && ui->NombreLineEdit->text().isEmpty()) ui->NifLineEdit->setText(ui->codigolineEdit->text());
      }
     }

}


void externos::fila_tabla_clicked()
{

    QModelIndex indiceactual=ui->tableView->currentIndex();
    QString codigo,descripcion;
    if (indiceactual.isValid())
       {
        codigo=modeloreg->record(indiceactual.row()).value("codigo").toString();
        ui->codigolineEdit->setText(codigo);
        descripcion=modeloreg->record(indiceactual.row()).value("descripcion").toString();
        ui->aceptarpushButton->setEnabled(true);

       }
}



void externos::botonaceptar()
{
     QString codiva=ui->tipoIVAcomboBox->currentText().section("//",0,0).trimmed();
     QString codvenci=ui->vencicomboBox->currentText().section("//",0,0).trimmed();
     QString codpais=ui->paiscomboBox->currentText().section(" ",0,0).trimmed();
     QString codclaveidfiscal=ui->claveidfiscalcomboBox->currentText().section("//",0,0).trimmed();

     basedatos::instancia()->guarda_datos_externo(
                 ui->codigolineEdit->text(),
                 ui->cuenta_lineEdit->text(),
             ui->NombreLineEdit->text(),
                 ui->ncomerciallineEdit->text(),
             ui->NifLineEdit->text(),
                 ui->NifReplegalLineEdit->text(),
                 ui->nombre_pf_lineEdit->text(),
                 ui->apellidoslineEdit->text(),
             ui->DomicilioLineEdit->text(),
             ui->PoblacionLineEdit->text(),
             ui->CpLineEdit->text(),
             ui->ProvinciaLineEdit->text(),
                 codpais,
             ui->TfnoLineEdit->text(),
             ui->FaxLineEdit->text(),
             ui->EmailLineEdit->text(),
             ui->ObservacionesLineEdit->text(),
                 ui->ccclineEdit->text(),
                 ui->cuotalineEdit->text(),
                 ui->ivaasocgroupBox->isChecked(),
                 codiva, ui->cuentaIVAlineEdit->text(),
                 ui->ctaBaseIVAlineEdit->text(),
                 ui->venciasocgroupBox->isChecked(),
                 codvenci,
                 ui->cuenta_tesorlineEdit->text(),
                 ui->conceptodiariolineEdit->text(),
                 ui->weblineEdit->text(),
                 codclaveidfiscal,
                 ui->procvtocheckBox->isChecked(),
                 // pais_dat, qtipo_ret, ret_arrendamiento, tipo_operacion_ret,
                 ui->paislineEdit->text(),
                 ui->diafijocheckBox->isChecked() ? ui->vfijospinBox->value() :0,
                 // QString cuenta_ret_asig, bool es_ret_asig_arrend,
                 // QString tipo_ret_asig, QString tipo_oper_ret_asig,
                 ui->retgroupBox->isChecked() ? ui->cuentata_ret_asiglineEdit->text() :"",
                 ui->retgroupBox->isChecked() ? ui->es_ret_asigcheckBox->isChecked() : false,
                 ui->retgroupBox->isChecked() ? ui->porc_ret_asig_lineEdit->text(): "0",
                 ui->retgroupBox->isChecked() ? ui->tipo_oper_ret_asigcomboBox->currentText().left(1) : "",
                 ui->domiciliacheckBox->isChecked(),
                 ui->ibanlineEdit->text(),
                 "",
                 "",
                 "",
                 ui->biclineEdit->text(),
                 "",
                 ui->caja_ivacheckBox->isChecked(),
                 ui->ref_madatolineEdit->text(),
                 ui->firma_mandatodateEdit->date(),
                 ui->forma_pago_comboBox->currentText(),
                 fototexto() );

    refrescar(filtrocodigo());

    activacambiacod();
    ui->aceptarpushButton->setEnabled(false);
    ui->cancelar_pushButton->setEnabled(false);
   // QMessageBox::information( this, tr("Edición de plan contable"),
   //                             tr("Se han efectuado los cambios en el plan contable"));

}


void externos::desactivacambiacod()
{
 ui->codigolineEdit->setEnabled(false);
 ui->codigopushButton->setEnabled(false);
 ui->tableView->setEnabled(false);
 ui->aceptarpushButton->setEnabled(true);
 ui->cancelar_pushButton->setEnabled(true);
}

void externos::activacambiacod()
{
 ui->codigolineEdit->setEnabled(true);
 ui->codigopushButton->setEnabled(true);
 ui->tableView->setEnabled(true);
}


void externos::importafoto()
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
  graf_recorte *g = new graf_recorte(cadfich,ui->fotolabel->width(),ui->fotolabel->height());
  int resultado=g->exec();
  int altolabel=ui->fotolabel->height();
  int ancholabel=ui->fotolabel->width();
  if (resultado==QDialog::Accepted)
    {
     QPixmap ima=g->recorte();
     //ui->fotolabel->setPixmap(g->recorte());
     //int altopix=ima.height();
     //int anchopix=ima.width();
     QPixmap definitiva;
     definitiva=ima.scaledToHeight(altolabel);
     ui->fotolabel->setScaledContents(false);
     ui->fotolabel->setPixmap(definitiva);
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
      ui->fotolabel->setScaledContents(false);
      ui->fotolabel->setPixmap(definitiva);
     }
  delete(g);
  desactivacambiacod();

}

QString externos::fototexto()
{
 if (ui->fotolabel->pixmap().isNull()) return "";
 QPixmap foto=ui->fotolabel->pixmap();
 QByteArray bytes;
 QBuffer buffer(&bytes);
 buffer.open(QIODevice::WriteOnly);
 foto.save(&buffer, "PNG"); // writes pixmap into foto in PNG format
 QByteArray cadhexa=bytes.toHex();
 QString cadena(cadhexa);

 return cadena;

}

void externos::borrafoto()
{
  if (QMessageBox::question(
            this,
            tr("¿ Borrar imagen ?"),
            tr("¿ Desea borrar la imagen actual ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==0 )
               {
                QPixmap foto;
                ui->fotolabel->setPixmap(foto);
                desactivacambiacod();
               }
}


void externos::botonctaivapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->cuentaIVAlineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui->cuentaIVAlineEdit->setText(cadena2);
       else ui->cuentaIVAlineEdit->setText("");
    delete labusqueda;
}


void externos::botonctaretpulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->cuentata_ret_asiglineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui->cuentata_ret_asiglineEdit->setText(cadena2);
       else ui->cuentata_ret_asiglineEdit->setText("");
    delete labusqueda;
}




void externos::botoncta_tesorpulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->cuenta_tesorlineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui->cuenta_tesorlineEdit->setText(cadena2);
       else ui->cuenta_tesorlineEdit->setText("");
    delete labusqueda;
}

void externos::cta_IVA_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui->cuentaIVAlineEdit->text(),&cadena))
    ui->descripctaIVAlabel->setText(cadena);
  else
    ui->descripctaIVAlabel->setText("");
 desactivacambiacod();
}

void externos::cta_tesor_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui->cuenta_tesorlineEdit->text(),&cadena))
    ui->descrip_cta_tesorlabel->setText(cadena);
  else
    ui->descrip_cta_tesorlabel->setText("");
 desactivacambiacod();
}

void externos::cta_ret_asig_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui->cuentata_ret_asiglineEdit->text(),&cadena))
    ui->cta_ret_label->setText(cadena);
  else
    ui->cta_ret_label->setText("");
 desactivacambiacod();
}


void externos::cambia_alquiler_asig_checkBox()
{
 if (ui->es_ret_asigcheckBox->isChecked()) ui->tipo_oper_ret_asigcomboBox->setEnabled(false);
   else ui->tipo_oper_ret_asigcomboBox->setEnabled(true);
 desactivacambiacod();
}


void externos::cargacomboIVA()
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
    ui->tipoIVAcomboBox->insertItem ( 0,pasalinea) ;
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
      ui->tipoIVAcomboBox->insertItem ( -1,pasalinea) ;
         }
      }

}


void externos::cargacombovenci()
{
  QStringList lista=basedatos::instancia()->listatiposdescripvenci();
  ui->vencicomboBox->addItems (lista);
}


void externos::cta_IVA_finedicion()
{
    ui->cuentaIVAlineEdit->setText(expandepunto(ui->cuentaIVAlineEdit->text(),anchocuentas()));
}

void externos::cta_tesor_finedicion()
{
   ui->cuenta_tesorlineEdit->setText(expandepunto(ui->cuenta_tesorlineEdit->text(),anchocuentas()));
}


void externos::cta_ret_asig_finedicion()
{
    ui->cuentata_ret_asiglineEdit->setText(expandepunto(ui->cuentata_ret_asiglineEdit->text(),anchocuentas()));

}


void externos::cargacombooperacionret()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_ret();
  ui->tipo_oper_ret_asigcomboBox->addItems(lista);
}



void externos::cargacombo_formas_pago()
{
  QStringList lista;
  lista << "- -";
  QSqlQuery q=basedatos::instancia()->carga_formas_pago();
  if (q.isActive())
      while (q.next())
        {
          lista << q.value(0).toString();
        }

  ui->forma_pago_comboBox->addItems (lista);

}

void externos::venciasoccambiado()
{
    if (ui->venciasocgroupBox->isChecked())
      {
        if ((conanalitica() && (escuentadegasto(ui->codigolineEdit->text()) ||
                               escuentadeingreso(ui->codigolineEdit->text()))) ||
            (conanalitica_parc() && codigo_en_analitica(ui->codigolineEdit->text())))
          {
            QMessageBox::warning( this, tr("Edición de plan contable"),
                             tr("ERROR: Las cuentas de analítica no pueden poseer vencimientos"));
            ui->venciasocgroupBox->setChecked(false);
          }
      }
}


void externos::checkdiafijo()
{
   if (ui->diafijocheckBox->isChecked()) ui->vfijospinBox->setEnabled(true);
     else ui->vfijospinBox->setEnabled(false);
}

void externos::botoncuenta_pulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->cuenta_lineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui->cuenta_lineEdit->setText(cadena2);
       else ui->cuenta_lineEdit->clear();
    delete(labusqueda);
    cuenta_finedicion();

}

void externos::cuenta_finedicion()
{
    ui->cuenta_lineEdit->setText(expandepunto(ui->cuenta_lineEdit->text(),anchocuentas()));
    if (existesubcuenta(ui->cuenta_lineEdit->text()))
        ui->cuenta_label->setText(descripcioncuenta(ui->cuenta_lineEdit->text()));
      else ui->cuenta_label->setText("- -");
}



void externos::cta_base_IVA_finedicion()
{
    ui->ctaBaseIVAlineEdit->setText(expandepunto(ui->ctaBaseIVAlineEdit->text(),anchocuentas()));
}


void externos::botoncta_base_ivapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->ctaBaseIVAlineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui->ctaBaseIVAlineEdit->setText(cadena2);
       else ui->ctaBaseIVAlineEdit->setText("");
    delete labusqueda;
}


void externos::cta_base_IVA_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui->ctaBaseIVAlineEdit->text(),&cadena))
    ui->descripctaBaseIVAlabel->setText(cadena);
  else
    ui->descripctaBaseIVAlabel->setText("");
 desactivacambiacod();
}

void externos::boton_ue_pulsado()
{

    bool valido=false;
    QString nif_iva=ui->NifLineEdit->text();
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


void externos::refresca_externos()
{

  refrescar(filtrocodigo());

}


void externos::nifcambiado()
{
  if (!ui->NifLineEdit->text().isEmpty())
    {
     if (isNifCifNie(ui->NifLineEdit->text())>0)
        ui->check_ciflabel->show();
      else ui->check_ciflabel->hide();
    }
   else ui->check_ciflabel->hide();
  desactivacambiacod();
}


void externos::ccccambiado()
{
  if (!ui->ccclineEdit->text().isEmpty())
    {
     if (ccc_correcto(ui->ccclineEdit->text())) {
        ui->check_ccclabel->show();
        // comprobamos si el campo iban tiene contenido
        if (ui->ibanlineEdit->text().isEmpty()) {
            ui->ibanlineEdit->setText("ES"+digito_cotrol_IBAN("ES00"+ui->ccclineEdit->text().remove('-'))+ui->ccclineEdit->text().remove('-'));
        }
     }
      else ui->check_ccclabel->hide();
    }
   else ui->check_ccclabel->hide();
  desactivacambiacod();
}


void externos::ibancambiado()
{
  if (!ui->ibanlineEdit->text().isEmpty())
    {
     if (IBAN_correcto(ui->ibanlineEdit->text()))
        ui->check_ibanlabel->show();
      else ui->check_ibanlabel->hide();
    }
   else ui->check_ibanlabel->hide();
  desactivacambiacod();
}


void externos::buscar_externo()
{
    busca_externo *b = new busca_externo();
    if (b->exec()==QDialog::Accepted)
      {
        ui->codigolineEdit->setText(b->codigo_elec());
      }
      else ui->codigolineEdit->clear();
    delete(b);
}


void externos::botonborrar()
{
   // falta mensaje está seguro ?

  QMessageBox msgBox;
  msgBox.addButton(QObject::tr("Sí"), QMessageBox::ActionRole);
  QPushButton *abortButton = msgBox.addButton(QObject::tr("No"), QMessageBox::ActionRole);


  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText(tr("¿ DESEA ELIMINAR EL EXTERNO SELECCIONADO ?"));

  msgBox.exec();

  if (msgBox.clickedButton() == abortButton)
      return;

   if (basedatos::instancia()->externoendiario(ui->codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Edición de externos"),
       tr("No se puede elimimar la subcuenta porque existe en el diario"));
       return;
    }


   basedatos::instancia()->borra_exteno(ui->codigolineEdit->text());

   refrescar(filtrocodigo());

   QMessageBox::information( this, tr("Edición de externos"),
                             tr("Se ha eliminado el código seleccionado"));
   ui->codigolineEdit->clear();
   activacambiacod();

}

void externos::pasa_codigo(QString codigo)
{
    ui->codigolineEdit->setText(codigo);
}


