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

#include "edita_doc.h"
#include "basedatos.h"
#include "ajuste_imagen_logo.h"
#include <QFileDialog>
#include <QMessageBox>
#include "funciones.h"
#include "graf_recorte.h"
#include <QPixmap>

edita_doc::edita_doc() : QDialog() {
    ui.setupUi(this);

    ui.seriecomboBox->addItems(basedatos::instancia()->listacodseries());

    // cargamos datos por defecto del emisor --------------------------------
    ui.nombre_emisorlineEdit->setText(basedatos::instancia()->selectEmpresaconfiguracion());
    ui.domicilio_emisorlineEdit->setText(basedatos::instancia()->domicilio());
    ui.poblacion_emisorlineEdit->setText(basedatos::instancia()->ciudad());
    ui.cp_emisorlineEdit->setText(basedatos::instancia()->cpostal());
    ui.provincia_emisorlineEdit->setText(basedatos::instancia()->provincia());
    ui.cif_emisorlineEdit->setText(basedatos::instancia()->cif());
    ui.id_registrallineEdit->setText(basedatos::instancia()->idregistral());

    // ----------------------------------------------------------------------
    ui.donacion_groupBox->hide();

    connect(ui.aceptarpushButton, SIGNAL(clicked()), SLOT(terminar()));
    connect(ui.fotopushButton,SIGNAL( clicked()),this,SLOT(importafoto()));

    connect(ui.borrafotopushButton,SIGNAL( clicked()),this,SLOT(borrafoto()));
    connect(ui.fichpushButton,SIGNAL(clicked()),this,SLOT(carga_fich_jasp()));

}

void edita_doc::pasacodigo(QString codigo)
{
    QString descrip, serie, pie1, pie2, moneda, codigo_moneda, imagen;
    bool contabilizable, rectificativo;
    int tipo_operacion;
    QString documento, cantidad, referencia, descripref, precio, totallin, bi;
    QString tipoiva, tipore, cuota, cuotare, totalfac, notas, venci, notastex;
    QString cif_empresa, cif_cliente, numero, fecha, cliente, descuento;
    QString totallineas,suplidos;
    QString lineas_doc;
    QString nombre_emisor;
    QString domicilio_emisor;
    QString cp_emisor;
    QString poblacion_emisor;
    QString provincia_emisor;
    QString pais_emisor;
    QString cif_emisor;
    QString id_registral;
    QString fichjasper;
    QString concepto_sii;
    QString retencion;

    QString fe_idioma, fe_libro, fe_registro, fe_hoja, fe_folio, fe_seccion, fe_volumen,fe_datos_adic;
    QString clave_donacion, comunidad_autonoma;
    bool donacion_especie=false;
    bool donacion_2ejer=false;
    double porcent_deduc_autonomia=0;
    basedatos::instancia()->carga_tipo_doc(codigo,
                                   &descrip,
                                   &serie,
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
                                   &numero, &fecha, &cliente, &descuento,
                                   &totallineas,
                                   &suplidos,
                                   &lineas_doc,
                                   &nombre_emisor,
                                   &domicilio_emisor,
                                   &cp_emisor,
                                   &poblacion_emisor,
                                   &provincia_emisor,
                                   &pais_emisor,
                                   &cif_emisor,
                                   &id_registral,
                                   &fichjasper,
                                   &concepto_sii,
                                   &fe_idioma, &fe_libro, &fe_registro, &fe_hoja, &fe_folio, &fe_seccion, &fe_volumen,
                                   &fe_datos_adic,
                                   &retencion,
                                   &clave_donacion, &comunidad_autonoma, &donacion_especie, &donacion_2ejer, &porcent_deduc_autonomia,
                                   &imagen);
    QByteArray img_array = basedatos::instancia()->img_tipos_doc(codigo);
    QPixmap p; p.loadFromData(img_array);
    ui.img_fondo_label->setPixmap(p);
    ui.codigolineEdit->setText(codigo);
    ui.descriplineEdit->setText(descrip);
    ui.pie1lineEdit->setText(pie1);
    ui.pie2lineEdit->setText(pie2);
    if (contabilizable) ui.contabilizablecheckBox->setChecked(true);
    if (rectificativo) ui.rectificativocheckBox->setChecked(true);
    for (int veces=0; veces<ui.seriecomboBox->count(); veces++)
        if (ui.seriecomboBox->itemText(veces)==serie)
           {
             ui.seriecomboBox->setCurrentIndex(veces);
             break;
           }

    for (int veces=0; veces<ui.simcomboBox->count(); veces++)
        if (ui.simcomboBox->itemText(veces)==moneda)
           {
             ui.simcomboBox->setCurrentIndex(veces);
             break;
           }

    for (int veces=0; veces<ui.codcomboBox->count(); veces++)
        if (ui.codcomboBox->itemText(veces)==codigo_moneda)
           {
             ui.codcomboBox->setCurrentIndex(veces);
             break;
           }

/*    &tipo_operacion,
    &documento, &cantidad, &referencia, &descripref,
    &precio, &totallin, &bi,
    &tipoiva, &tipore, &cuota, &cuotare, &totalfac,
    &notas, &venci, */


    for (int veces=0; veces<ui.operacioncomboBox->count(); veces++)
        if (ui.operacioncomboBox->itemText(veces).left(2).toInt()==tipo_operacion)
           {
             ui.operacioncomboBox->setCurrentIndex(veces);
             break;
           }

    if (!(tipo_operacion==8)) ui.donacion_groupBox->hide();
    else {
        // actualizamos campos de la donación
        for (int veces=0; veces<ui.clave_donacion_comboBox->count(); veces++)
            if (ui.clave_donacion_comboBox->itemText(veces).left(1)==clave_donacion)
               {
                 ui.clave_donacion_comboBox->setCurrentIndex(veces);
                 break;
               }
        for (int veces=0; veces<ui.ca_comboBox->count(); veces++)
            if (ui.ca_comboBox->itemText(veces).left(2)==comunidad_autonoma)
               {
                 ui.ca_comboBox->setCurrentIndex(veces);
                 break;
               }
        ui.donacion_especie_checkBox->setChecked(donacion_especie);
        ui.recurrente_checkBox->setChecked(donacion_2ejer);
        QString cadporcent;
        cadporcent.setNum(porcent_deduc_autonomia,'f',2);
        if (haycomadecimal()) cadporcent=convacoma(cadporcent);
        ui.porcentaje_deduccion_ca_lineEdit->setText(cadporcent);
    }

    if (!documento.isEmpty())
     {
      ui.documentolineEdit->setText(documento);
      ui.cantidadlineEdit->setText(cantidad);
      ui.referencialineEdit->setText(referencia);
      ui.descripreflineEdit->setText(descripref);
      ui.preciolineEdit->setText(precio);
      ui.totallineEdit->setText(totallin);
      ui.suplidoslineEdit->setText(suplidos);
      ui.bilineEdit->setText(bi);
      ui.tipoivalineEdit->setText(tipoiva);
      ui.tiporelineEdit->setText(tipore);
      ui.cuotalineEdit->setText(cuota);
      ui.cuotarelineEdit->setText(cuotare);
      ui.totalfaclineEdit->setText(totalfac);
      ui.notaslineEdit->setText(notas);
      ui.vencilineEdit->setText(venci);
      ui.cif_empresalineEdit->setText(cif_empresa);
      ui.cif_clientelineEdit->setText(cif_cliente);
      ui.numerolineEdit->setText(numero);
      ui.fechalineEdit->setText(fecha);
      ui.clientelineEdit->setText(cliente);
      ui.descuentolineEdit->setText(descuento);
      ui.totallineaslineEdit->setText(totallineas);
      ui.retencion_lineEdit->setText(retencion);
      ui.lineasspinBox->setValue(lineas_doc.toInt());
      ui.nombre_emisorlineEdit->setText(nombre_emisor);
      ui.domicilio_emisorlineEdit->setText(domicilio_emisor);
      ui.cp_emisorlineEdit->setText(cp_emisor);
      ui.poblacion_emisorlineEdit->setText(poblacion_emisor);
      ui.provincia_emisorlineEdit->setText(provincia_emisor);
      ui.pais_emisorlineEdit->setText(pais_emisor);
      ui.cif_emisorlineEdit->setText(cif_emisor);
      ui.id_registrallineEdit->setText(id_registral);
      ui.pljasper_lineEdit->setText(fichjasper);
      ui.conceptoSII_lineEdit->setText(concepto_sii);

      //&fe_idioma, &fe_libro, &fe_registro, &fe_hoja, &fe_folio, &fe_seccion, &fe_volumen
      ui.idioma_fe_lineEdit->setText(fe_idioma);
      ui.libro_lineEdit->setText(fe_libro);
      ui.registro_lineEdit->setText(fe_registro);
      ui.hoja_lineEdit->setText(fe_hoja);
      ui.folio_lineEdit->setText(fe_folio);
      ui.seccion_lineEdit->setText(fe_seccion);
      ui.volumen_lineEdit->setText(fe_volumen);
      ui.datos_adicionales_lineEdit->setText(fe_datos_adic);
     }
    ui.notastextEdit->setText(notastex);
    QPixmap foto;
    if (imagen.length()>0)
          {
           QByteArray byteshexa;
           byteshexa.append ( imagen.toUtf8() );
           QByteArray bytes;
           bytes=bytes.fromHex ( byteshexa );
           foto.loadFromData ( bytes, "PNG");
          }
    ui.logo_label->setPixmap(foto);

    ui.pie1lineEdit->setText(pie1);
    ui.pie2lineEdit->setText(pie2);

}


void edita_doc::terminar()
{
    QString tipooperacion=ui.operacioncomboBox->currentText().left(2);
    QString lineas_doc; lineas_doc.setNum(ui.lineasspinBox->value());
    QString porcent_deduc_ca=ui.porcentaje_deduccion_ca_lineEdit->text();
    if (porcent_deduc_ca.toDouble()<0.001 && porcent_deduc_ca.toDouble() >-0.001)
        porcent_deduc_ca="0";
    basedatos::instancia()->guarda_tipo_doc(ui.codigolineEdit->text(),
                                   ui.descriplineEdit->text(),
                                   ui.seriecomboBox->currentText(),
                                   ui.pie1lineEdit->text(),
                                   ui.pie2lineEdit->text(),
                                   ui.simcomboBox->currentText(),
                                   ui.codcomboBox->currentText(),
                                   ui.contabilizablecheckBox->isChecked(),
                                   ui.rectificativocheckBox->isChecked(),
                                   tipooperacion,
                                   ui.documentolineEdit->text(),
                                   ui.cantidadlineEdit->text(),
                                   ui.referencialineEdit->text(),
                                   ui.descripreflineEdit->text(),
                                   ui.preciolineEdit->text(),
                                   ui.totallineEdit->text(),
                                   ui.bilineEdit->text(),
                                   ui.tipoivalineEdit->text(),
                                   ui.tiporelineEdit->text(),
                                   ui.cuotalineEdit->text(),
                                   ui.cuotarelineEdit->text(),
                                   ui.totalfaclineEdit->text(),
                                   ui.suplidoslineEdit->text(),
                                   ui.notaslineEdit->text(),
                                   ui.vencilineEdit->text(),
                                   ui.notastextEdit->toPlainText(),
                                   ui.cif_empresalineEdit->text(),
                                   ui.cif_clientelineEdit->text(),
                                   ui.numerolineEdit->text(),
                                   ui.fechalineEdit->text(),
                                   ui.clientelineEdit->text(),
                                   ui.descuentolineEdit->text(),
                                   ui.totallineaslineEdit->text(),
                                   lineas_doc,
                                   ui.nombre_emisorlineEdit->text(),
                                   ui.domicilio_emisorlineEdit->text(),
                                   ui.cp_emisorlineEdit->text(),
                                   ui.poblacion_emisorlineEdit->text(),
                                   ui.provincia_emisorlineEdit->text(),
                                   ui.pais_emisorlineEdit->text(),
                                   ui.cif_emisorlineEdit->text(),
                                   ui.id_registrallineEdit->text(),
                                   ui.pljasper_lineEdit->text(),
                                   ui.conceptoSII_lineEdit->text(),
                                   ui.idioma_fe_lineEdit->text(),
                                   ui.libro_lineEdit->text(),
                                   ui.registro_lineEdit->text(),
                                   ui.hoja_lineEdit->text(),
                                   ui.folio_lineEdit->text(),
                                   ui.seccion_lineEdit->text(),
                                   ui.volumen_lineEdit->text(),
                                   ui.datos_adicionales_lineEdit->text(),
                                   ui.retencion_lineEdit->text(),
                                   ui.clave_donacion_comboBox->currentText().left(1),
                                   ui.ca_comboBox->currentText().left(2),
                                   ui.donacion_especie_checkBox->isChecked(),
                                   ui.recurrente_checkBox->isChecked(),
                                   porcent_deduc_ca
                                   );
    //QString fe_idioma,
    //QString fe_libro, QString fe_registro, QString fe_hoja, QString fe_folio,
    //QString fe_seccion, QString fe_volumen

    basedatos::instancia()->grabaimagenlogo(ui.codigolineEdit->text(), fototexto());
    accept();
}



void edita_doc::importafoto()
{
  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Archivos de gráficos (*.png *.jpg *.bmp)");
  dialogofich.setNameFilters(filtros);
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
           QString cadfich=fileNames.at(0);
           QPixmap imagen(cadfich);
           graf_recorte *g = new graf_recorte(cadfich,ui.logo_label->width(),ui.logo_label->height());
           int resultado=g->exec();
           int altolabel=ui.logo_label->height();
           int ancholabel=ui.logo_label->width();
           if (resultado==QDialog::Accepted)
              {
               ui.logo_label->setPixmap(g->recorte().scaledToHeight(altolabel));
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
               ui.logo_label->setScaledContents(false);
               ui.logo_label->setPixmap(definitiva);
              }
           delete(g);
          }
     }

}

QString edita_doc::fototexto()
{
 if (ui.logo_label->pixmap().isNull()) return "";
 QPixmap foto=ui.logo_label->pixmap();
 QByteArray bytes;
 QBuffer buffer(&bytes);
 buffer.open(QIODevice::WriteOnly);
 foto.save(&buffer, "PNG"); // writes pixmap into foto in PNG format
 QByteArray cadhexa=bytes.toHex();
 QString cadena(cadhexa);

 return cadena;

}

void edita_doc::borrafoto()
{
  if (QMessageBox::question(
            this,
            tr("¿ Borrar imagen ?"),
            tr("¿ Desea borrar la imagen actual ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==0 )
               {
                QPixmap foto;
                ui.logo_label->setPixmap(foto);
               }
}

void edita_doc::carga_fich_jasp()
{
  //QString caddirjasper=trayreport();
  //caddirjasper.append(QDir::separator());

  QString qdirtrabajo=dirtrabajobd();
  qdirtrabajo.append(QDir::separator());
  qdirtrabajo.append("zz_Informes");


  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );
  QStringList filtros; filtros << "Ficheros xml (*.xml)" << "Todos los ficheros (*)";
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(qdirtrabajo);
  dialogofich.setWindowTitle(tr("SELECCIÓN DE PLANTILLA INFORME"));
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
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
           QString cadfich=fileNames.at(0);
           ui.pljasper_lineEdit->setText(cadfich);
          }
     }

}

void edita_doc::on_operacioncomboBox_currentIndexChanged(int index)
{
    if (index==7) ui.donacion_groupBox->show();
      else ui.donacion_groupBox->hide();
}
