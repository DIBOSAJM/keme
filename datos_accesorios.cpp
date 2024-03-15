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

#include "datos_accesorios.h"
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include "directorio.h"
#include <QMessageBox>
#include <QFileDialog>
#include "vies_msj.h"
#include "graf_recorte.h"

#define CODPAIS "ES"

datos_accesorios::datos_accesorios() : QDialog() {
    ui.setupUi(this);
fijasololectura();
cargacombovenci();
cargacomboclaveidfiscal();
cargacombopaises();
cargacombo_formas_pago();

 ui.vfijospinBox->setEnabled(false);
connect(ui.editarpushButton,SIGNAL( clicked()),this,SLOT(botoneditar()));
connect(ui.salirpushButton,SIGNAL( clicked()),this,SLOT(botonsalir()));
connect(ui.fotopushButton,SIGNAL( clicked()),this,SLOT(importafoto()));
connect(ui.borrafotopushButton,SIGNAL( clicked()),this,SLOT(borrafoto()));
connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(guardadatosacces()));
connect(ui.cta_tesorpushButton,SIGNAL( clicked()),this,SLOT(botoncta_tesorpulsado()));
connect(ui.cuenta_tesorlineEdit,SIGNAL( editingFinished ()),this,SLOT(cta_tesor_finedicion()));
connect(ui.cuenta_tesorlineEdit,SIGNAL( textChanged(QString)),this,SLOT(cta_tesor_cambiada()));
connect(ui.NifLineEdit,SIGNAL( textChanged(QString)),this,SLOT(nifcambiado()));
connect(ui.ccclineEdit,SIGNAL( textChanged(QString)),this,SLOT(ccccambiado()));
connect(ui.IBANlineEdit,SIGNAL( textChanged(QString)),this,SLOT(ibancambiado()));

connect(ui.diafijocheckBox,SIGNAL(clicked()),this,SLOT(checkdiafijo()));

connect(ui.UE_pushButton,SIGNAL(clicked(bool)),this,SLOT(boton_ue_pulsado()));

ui.check_ciflabel->hide();
ui.check_ccclabel->hide();
ui.check_ibanlabel->hide();

}

void datos_accesorios::fijasololectura()
{
    // Fijamos todos los campos de solo lectura
    ui.NombreLineEdit->setReadOnly(true);
    ui.ncomerciallineEdit->setReadOnly(true);
    ui.DomicilioLineEdit->setReadOnly(true);
    ui.PoblacionLineEdit->setReadOnly(true);
    ui.ProvinciaLineEdit->setReadOnly(true);
    ui.CpLineEdit->setReadOnly(true);
    ui.NifLineEdit->setReadOnly(true);
    ui.NifReplegalLineEdit->setReadOnly(true);
    // ui.PaisLineEdit->setText(pais);
    ui.TfnoLineEdit->setReadOnly(true);
    ui.FaxLineEdit->setReadOnly(true);
    ui.EmailLineEdit->setReadOnly(true);
    ui.ObservacionesLineEdit->setReadOnly(true);
    ui.ccclineEdit->setReadOnly(true);
    ui.cuotalineEdit->setReadOnly(true);
    ui.weblineEdit->setReadOnly(true);
    ui.claveidfiscalcomboBox->setEnabled(false);
    ui.paiscomboBox->setEnabled(false);
    ui.fotopushButton->setEnabled(false);
    ui.borrafotopushButton->setEnabled(false);
    ui.venciasocgroupBox->setEnabled(false);
    ui.vencicomboBox->setEnabled(false);
    ui.cuenta_tesorlineEdit->setReadOnly(true);
    ui.cta_tesorpushButton->setEnabled(false);
    ui.paislineEdit->setReadOnly(true);
    ui.diafijocheckBox->setEnabled(false);
    ui.vfijospinBox->setReadOnly(true);
    ui.IBANlineEdit->setReadOnly(true);
    ui.domiciliacheckBox->setEnabled(false);
    ui.BIClineEdit->setReadOnly(true);
    ui.forma_pago_comboBox->setEnabled(false);
}

void datos_accesorios::cargacodigo(QString codigo)
{
    if (codigo.length()==anchocuentas() || esauxiliar(codigo))
        {
         ui.codigolineEdit->setText(codigo);
         ui.descripcionlineEdit->setText(descripcioncuenta(codigo));
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
         // QString pais_dat;
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
         QString tipoiva;
         QString conceptodiario;
         QString paisdat;
         QString tipo_ret;
         QString tipo_operacion_ret;
         bool ret_arrendamiento;
         int vdiafijo=0;
         QString cuenta_ret_asig; bool es_ret_asig_arrend;
         QString tipo_ret_asig; QString tipo_oper_ret_asig;
         bool dom_bancaria, caja_iva;
         QString IBAN, nriesgo, cedente1, cedente2, bic, sufijo;
         QString ref_mandato;
         QDate firma_mandato;
         QString cta_base_iva;
         QString formapago;

         if (existendatosaccesorios(codigo, &razon, &ncomercial, &cif,   &nifrprlegal, &domicilio,
                             &poblacion,&codigopostal, &provincia, &pais, &tfno,
                             &fax, &email, &observaciones, &ccc, &cuota,
                             &venciasoc, &codvenci, &tesoreria, &ivaasoc, &cuentaiva, &cta_base_iva, &tipoiva,
                             &conceptodiario, &web, &claveidfiscal, &procvto,
                             &paisdat, &tipo_ret, &tipo_operacion_ret, &ret_arrendamiento,
                             &vdiafijo,&cuenta_ret_asig, &es_ret_asig_arrend, &tipo_ret_asig,
                             &tipo_oper_ret_asig, &dom_bancaria, &IBAN, &nriesgo, &cedente1,
                             &cedente2, &bic, &sufijo, &ref_mandato, &firma_mandato, &caja_iva,
                             &formapago,
                             &imagen))

          {
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
                else ui.check_ciflabel->hide();
             }
             else ui.check_ciflabel->hide();
           ui.NifReplegalLineEdit->setText(nifrprlegal);
           ui.TfnoLineEdit->setText(tfno);
           ui.FaxLineEdit->setText(fax);
           ui.EmailLineEdit->setText(email);
           ui.ObservacionesLineEdit->setText(observaciones);
           ui.ccclineEdit->setText(ccc);
           if (!ui.ccclineEdit->text().isEmpty())
             {
              if (ccc_correcto(ui.ccclineEdit->text()))
                 ui.check_ccclabel->show();
               else ui.check_ccclabel->hide();
             }
            else ui.check_ccclabel->hide();

           ui.cuotalineEdit->setText(cuota);
           ui.weblineEdit->setText(web);
           ui.paislineEdit->setText(paisdat);
           ui.domiciliacheckBox->setChecked(dom_bancaria);
           if (!IBAN.isEmpty())
             ui.IBANlineEdit->setText(IBAN);
           if (!ui.IBANlineEdit->text().isEmpty())
             {
              if (IBAN_correcto(ui.IBANlineEdit->text()))
                 ui.check_ibanlabel->show();
               else ui.check_ibanlabel->hide();
             }
            else ui.check_ibanlabel->hide();

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

           if (venciasoc)
             {
               ui.venciasocgroupBox->setChecked(true);
               // buscamos codvenci en el combo vencicomboBox
               int indice=0;
               while (indice<ui.vencicomboBox->count())
                 {
                   if (vdiafijo>0)
                      {
                       ui.diafijocheckBox->setEnabled(true);
                       ui.diafijocheckBox->setChecked(true);
                       ui.vfijospinBox->setEnabled(true);
                       ui.vfijospinBox->setValue(vdiafijo);
                      }
                      else ui.diafijocheckBox->setEnabled(false);
                  QString texto=ui.vencicomboBox->itemText(indice);
                  QString extracto=texto.section("//",0,0).trimmed();
                  if (extracto==codvenci) break;
                  indice++;
                 }
               if (indice>ui.vencicomboBox->count()) indice--;
               ui.vencicomboBox->setCurrentIndex(indice);
               ui.cuenta_tesorlineEdit->setText(tesoreria);
             }
              else
                   {
                    ui.cuenta_tesorlineEdit->clear();
                    ui.venciasocgroupBox->setChecked(false);
                   }
           ui.procvtocheckBox->setChecked(procvto);
           ui.BIClineEdit->setText(bic);

           indice=0;
           while (indice<ui.forma_pago_comboBox->count())
                 {
                  if (ui.forma_pago_comboBox->itemText(indice)==formapago) break;
                  indice++;
                 }
           if (indice>ui.forma_pago_comboBox->count()) indice=0;
           ui.forma_pago_comboBox->setCurrentIndex(indice);

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
          }
        }

}


void datos_accesorios::cargacombovenci()
{
  QStringList lista=basedatos::instancia()->listatiposdescripvenci();
  ui.vencicomboBox->addItems (lista);
}


void datos_accesorios::cargacomboclaveidfiscal()
{
  QStringList lista=basedatos::instancia()->listaclaveidfiscal();
  ui.claveidfiscalcomboBox->addItems (lista);
}

void datos_accesorios::cargacombopaises()
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


void datos_accesorios::fijaescritura()
{
    // Fijamos todos los campos de escritura
    ui.NombreLineEdit->setReadOnly(false);
    ui.ncomerciallineEdit->setReadOnly(false);
    ui.DomicilioLineEdit->setReadOnly(false);
    ui.PoblacionLineEdit->setReadOnly(false);
    ui.ProvinciaLineEdit->setReadOnly(false);
    ui.CpLineEdit->setReadOnly(false);
    ui.NifLineEdit->setReadOnly(false);
    ui.NifReplegalLineEdit->setReadOnly(false);
    // ui.PaisLineEdit->setText(pais);
    ui.TfnoLineEdit->setReadOnly(false);
    ui.FaxLineEdit->setReadOnly(false);
    ui.EmailLineEdit->setReadOnly(false);
    ui.ObservacionesLineEdit->setReadOnly(false);
    ui.ccclineEdit->setReadOnly(false);
    ui.cuotalineEdit->setReadOnly(false);
    ui.weblineEdit->setReadOnly(false);
    ui.claveidfiscalcomboBox->setEnabled(true);
    ui.paiscomboBox->setEnabled(true);
    ui.fotopushButton->setEnabled(true);
    ui.borrafotopushButton->setEnabled(true);
    ui.venciasocgroupBox->setEnabled(true);
    ui.vencicomboBox->setEnabled(true);
    ui.cuenta_tesorlineEdit->setReadOnly(false);
    ui.cta_tesorpushButton->setEnabled(true);
    ui.paislineEdit->setReadOnly(false);
    ui.diafijocheckBox->setEnabled(true);
    ui.vfijospinBox->setReadOnly(false);
    if (ui.diafijocheckBox->isChecked())
      {
        ui.vfijospinBox->setEnabled(true);

      }
    ui.IBANlineEdit->setReadOnly(false);
    ui.domiciliacheckBox->setEnabled(true);
    ui.BIClineEdit->setReadOnly(false);
    ui.forma_pago_comboBox->setEnabled(true);
}

void datos_accesorios::botoneditar()
{
  ui.aceptarpushButton->setEnabled(true);
  ui.editarpushButton->setEnabled(false);
  fijaescritura();
}


void datos_accesorios::botonsalir()
{
  if (ui.aceptarpushButton->isEnabled())
    {
      if (QMessageBox::question(this,
          tr("Cancelar edición"),
          tr("¿ Desea cancelar la edición ?"),
          tr("&Sí"), tr("&No"),
                                  QString(), 0, 1 ) )
        return ;
    }
  accept();
}

void datos_accesorios::importafoto()
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

  //QStringList filtros;
  //filtros << tr("Archivos de gráficos (*.png *.jpg *.jpeg *.bmp)");
  //dialogofich.setFilters(filtros);
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

  if (!cadfich.isEmpty())
  {
   QPixmap imagen(cadfich);
   // ----------------------------------------------------------------------
   graf_recorte *g = new graf_recorte(cadfich,ui.fotolabel->width(),ui.fotolabel->height());
   int resultado=g->exec();
   int altolabel=ui.fotolabel->height();
   int ancholabel=ui.fotolabel->width();
   if (resultado==QDialog::Accepted)
     {
      QPixmap ima=g->recorte();
      QPixmap definitiva;
      definitiva=ima.scaledToHeight(altolabel);
      ui.fotolabel->setScaledContents(false);
      ui.fotolabel->setPixmap(definitiva);
     }
    else
     {
      // QMessageBox::information( this, tr("SUBCUENTAS"),tr("ACEPTADO"));
      // ----------------------------------------------------------------------
       altolabel=ui.fotolabel->height();
       ancholabel=ui.fotolabel->width();
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
   }

}


void datos_accesorios::borrafoto()
{
  if (QMessageBox::question(
            this,
            tr("¿ Borrar imagen ?"),
            tr("¿ Desea borrar la imagen actual ?"),
            tr("&Sí"), tr("&No"),
           QString(), 0, 1 ) ==0 )
               {
                QPixmap foto;
                ui.fotolabel->setPixmap(foto);
               }
}


void datos_accesorios::guardadatosacces()
{
       // QString codiva=ui.tipoIVAcomboBox->currentText().section("//",0,0).trimmed();
       QString codvenci=ui.vencicomboBox->currentText().section("//",0,0).trimmed();
       QString codpais=ui.paiscomboBox->currentText().section(" ",0,0).trimmed();
       QString codclaveidfiscal=ui.claveidfiscalcomboBox->currentText().section("//",0,0).trimmed();
       int diasfijo=0;
       if (ui.diafijocheckBox->isChecked()) diasfijo=ui.vfijospinBox->value();
       basedatos::instancia()->guardadatosaccesorios2(
       ui.codigolineEdit->text(),
       ui.NombreLineEdit->text(),
       ui.ncomerciallineEdit->text(),
       ui.NifLineEdit->text(),
       ui.NifReplegalLineEdit->text(),
       ui.DomicilioLineEdit->text(),
       ui.PoblacionLineEdit->text(),
       ui.CpLineEdit->text(),
       ui.ProvinciaLineEdit->text(),
       codpais, ui.paislineEdit->text(),
       ui.TfnoLineEdit->text(),
       ui.FaxLineEdit->text(),
       ui.EmailLineEdit->text(),
       ui.ObservacionesLineEdit->text(),
       ui.ccclineEdit->text(),
       ui.cuotalineEdit->text(),
       // ui.ivaasocgroupBox->isChecked(),
       // codiva, ui.cuentaIVAlineEdit->text(),
       ui.venciasocgroupBox->isChecked(),
       codvenci,
       ui.cuenta_tesorlineEdit->text(),
       // ui.conceptodiariolineEdit->text(),
       ui.weblineEdit->text(),
       codclaveidfiscal,
       ui.procvtocheckBox->isChecked(),
       diasfijo,
       ui.domiciliacheckBox->isChecked(),
       ui.IBANlineEdit->text(), ui.BIClineEdit->text(),
       ui.forma_pago_comboBox->currentText(),
       fototexto() );

      ui.aceptarpushButton->setEnabled(false);
      ui.editarpushButton->setEnabled(false);

      QMessageBox::information( this, tr("Datos accesorios"),
                        tr("Se han guardado los datos accesorios"));
      accept();

}


QString datos_accesorios::fototexto()
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

void datos_accesorios::botoncta_tesorpulsado()
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


void datos_accesorios::cta_tesor_cambiada()
{
 QString cadena;
 if (existecodigoplan(ui.cuenta_tesorlineEdit->text(),&cadena))
    ui.descrip_cta_tesorlabel->setText(cadena);
  else
    ui.descrip_cta_tesorlabel->setText("");
}


void datos_accesorios::cta_tesor_finedicion()
{
   ui.cuenta_tesorlineEdit->setText(expandepunto(ui.cuenta_tesorlineEdit->text(),anchocuentas()));
}


void datos_accesorios::checkdiafijo()
{
   if (ui.diafijocheckBox->isChecked()) ui.vfijospinBox->setEnabled(true);
     else ui.vfijospinBox->setEnabled(false);
}

void datos_accesorios::nifcambiado()
{
    if (!ui.NifLineEdit->text().isEmpty())
      {
       if (isNifCifNie(ui.NifLineEdit->text())>0)
          ui.check_ciflabel->show();
       else ui.check_ciflabel->hide();
      }
    else ui.check_ciflabel->hide();
}


void datos_accesorios::ccccambiado()
{
  if (!ui.ccclineEdit->text().isEmpty())
    {
     if (ccc_correcto(ui.ccclineEdit->text())) {
        ui.check_ccclabel->show();
        // comprobamos si el campo iban tiene contenido
        if (ui.IBANlineEdit->text().isEmpty()) {
            ui.IBANlineEdit->setText("ES"+digito_cotrol_IBAN("ES00"+ui.ccclineEdit->text().remove('-'))+ui.ccclineEdit->text().remove('-'));
        }
     }
      else ui.check_ccclabel->hide();
    }
   else ui.check_ccclabel->hide();
}


void datos_accesorios::ibancambiado()
{
  if (!ui.IBANlineEdit->text().isEmpty())
    {
     if (IBAN_correcto(ui.IBANlineEdit->text()))
        ui.check_ibanlabel->show();
      else ui.check_ibanlabel->hide();
    }
   else ui.check_ibanlabel->hide();
}

void datos_accesorios::boton_ue_pulsado()
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
        tr("ERROR en conexión con servicio de validación") );
        // vatnum.soap_stream_fault(std::cerr);

}


void datos_accesorios::cargacombo_formas_pago()
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
