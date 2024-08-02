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

#include "editconfig.h"
#include "funciones.h"
#include "basedatos.h"
#include "graf_recorte.h"
#include <QFileDialog>
#include <QMessageBox>

editconfig::editconfig() : QDialog()
 {
  ui.setupUi(this);

// esta porción de código es para asegurarnos de que la ventana
// sale por el centro del escritorio
/* QDesktopWidget *desktop = QApplication::desktop();

int screenWidth, width; 
int screenHeight, height;
int x, y;
QSize windowSize;

screenWidth = desktop->width(); // ancho de la pantalla
screenHeight = desktop->height(); // alto de la pantalla

windowSize = size(); // tamaño de nuestra ventana
width = windowSize.width(); 
height = windowSize.height();

x = (screenWidth - width) / 2;
y = (screenHeight - height) / 2;
y -= 50;

// mueve la ventana a las coordenadas deseadas
move ( x, y ); */
// -------------------------------------------------------------

  connect(ui.an_parc_radioButton,SIGNAL(toggled(bool)),SLOT(analitica_parc_cambiado()));
  connect(ui.sin_analiticaradioButton,SIGNAL(toggled(bool)),SLOT(sin_analitica_cambiado()));

  connect(ui.fotopushButton,SIGNAL( clicked()),this,SLOT(importafoto()));
  connect(ui.borrafotopushButton,SIGNAL( clicked()),this,SLOT(borrafoto()));

  connect(ui.tipo_proveedor_checkBox,SIGNAL(stateChanged(int)),SLOT(tipo_proveedor_cheq()));

  QStringList lista=basedatos::instancia()->listacodprovincias();
  ui.provinciacomboBox->addItems (lista);


  QSqlQuery query = basedatos::instancia()->selectConfiguracion();
  QString provincia_def, imagen;
  if (query.isActive())
  if (query.next())
     {
       ui.empresalineEdit->setText(query.value(0).toString());
       ui.niflineEdit->setText(query.value(1).toString());
       ui.domiciliolineEdit->setText(query.value(2).toString());
       ui.poblacionlineEdit->setText(query.value(3).toString());
       ui.cplineEdit->setText(query.value(4).toString());
       ui.provincialineEdit->setText(query.value(5).toString());
       ui.emaillineEdit->setText(query.value(6).toString());
       ui.weblineEdit->setText(query.value(7).toString());
       ui.anchocuentaslineEdit->setText(query.value(8).toString());
       ui.ivasoplineEdit->setText(query.value(9).toString());
       ui.ivareplineEdit->setText(query.value(10).toString());
       ui.retirpflineEdit->setText(query.value(11).toString());
       ui.ctaretlineEdit->setText(query.value(12).toString());
       ui.pagarlineEdit->setText(query.value(13).toString());
       ui.cobrarlineEdit->setText(query.value(14).toString());
       ui.claveivalineEdit->setText(query.value(15).toString());
       ui.clavegastoslineEdit->setText(query.value(16).toString());
       ui.claveingresoslineEdit->setText(query.value(17).toString());
       ui.pyglineEdit->setText(query.value(18).toString());
       // ui.proxasientolineEdit->setText(query.value(18).toString());
       ui.proxpaselineEdit->setText(query.value(19).toString());
       ui.proxvencilineEdit->setText(query.value(20).toString());
       // ui.numeracioncheckBox->setChecked(query.value(21).toBool());
       ui.igiccheckBox->setChecked(query.value(21).toBool());
       ui.analiticaradioButton->setChecked(query.value(22).toBool());
       ui.idlineEdit->setText(query.value(23).toString());
       QString cadnum; cadnum.setNum(query.value(24).toDouble(),'f',0);
       ui.prorratalineEdit->setText(cadnum.trimmed());
       ui.especialcheckBox->setChecked(query.value(25).toBool());
       ui.constituciondateEdit->setDate(query.value(26).toDate());
       ui.objetolineEdit->setText(query.value(27).toString());
       ui.actividadlineEdit->setText(query.value(28).toString());
       ui.an_parc_radioButton->setChecked(query.value(29).toBool());
       ui.ctas_analitic_plainTextEdit->setPlainText(query.value(30).toString());
       ui.uldoclineEdit->setText(query.value(31).toString());
       ui.usuarioscheckBox->setChecked(query.value(32).toBool());
       ui.desglosecheckBox->setChecked(query.value(33).toBool());
       provincia_def=query.value(34).toString();
       ui.aalineEdit->setText(query.value(35).toString());
       ui.aglineEdit->setText(query.value(36).toString());
       ui.amoinvcheckBox->setChecked(query.value(37).toBool());
       ui.sec_recibidascheckBox->setChecked(query.value(38).toBool());
       ui.proxdomlineEdit->setText(query.value(39).toString());
       ui.cod_ine_plaza_lineEdit->setText(query.value(40).toString());
       ui.caja_iva_checkBox->setChecked(query.value(41).toBool());
       ui.borrados_consol_checkBox->setChecked(query.value(42).toBool());
       ui.cobro_venci_lineEdit->setText(query.value(43).toString());
       ui.pago_venci_lineEdit->setText(query.value(44).toString());
       ui.externoscheckBox->setChecked(query.value(45).toBool());
       ui.tfnolineEdit->setText(query.value(46).toString());
       ui.gd_disco_radioButton->setChecked(!query.value(47).toBool());
       ui.gd_bd_radioButton->setChecked(query.value(47).toBool());
       ui.tipo_proveedor_checkBox->setChecked(query.value(48).toBool());
       ui.homol_pruebas_lineEdit->setText(query.value(49).toString());
       ui.nombre_lineEdit->setText(query.value(50).toString());
       ui.apellidos_lineEdit->setText(query.value(51).toString());
       ui.curl_checkBox->setChecked(query.value(52).toBool());
       ui.url_actu_lineEdit->setText(query.value(53).toString());
       ui.moneda_lineEdit->setText(query.value(54).toString());
       imagen=query.value(55).toString();
     }
  for (int veces=0; veces<ui.provinciacomboBox->count(); veces++)
     {
       if (ui.provinciacomboBox->itemText(veces).left(2)==provincia_def)
           ui.provinciacomboBox->setCurrentIndex(veces);
     }

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

  connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(aceptar()));
  connect(ui.igiccheckBox,SIGNAL(stateChanged(int)),SLOT(igiccambiado()));
  connect(ui.prorratalineEdit,SIGNAL(textChanged (QString)),SLOT(prorratacambiada()));
  igiccambiado();
  connect(ui.an_parc_radioButton,SIGNAL(toggled(bool)),SLOT(analitica_parc_cambiado()));
  connect(ui.proxpaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(proxpasecambiado()));
}

void editconfig::prorratacambiada()
{
 if (ui.prorratalineEdit->text().contains('.'))
   ui.prorratalineEdit->setText(ui.prorratalineEdit->text().remove('.'));

 if (ui.prorratalineEdit->text().contains(','))
   ui.prorratalineEdit->setText(ui.prorratalineEdit->text().remove(','));

}

void editconfig::aceptar()
{
    basedatos::instancia()->updateConfiguracion( ui.empresalineEdit->text() , ui.niflineEdit->text(),
                                                 ui.domiciliolineEdit->text() , ui.poblacionlineEdit->text(),
                                                 ui.cplineEdit->text() , ui.provincialineEdit->text(),
                                                 ui.emaillineEdit->text() , ui.weblineEdit->text(),
                                                 ui.ivasoplineEdit->text() , ui.ivareplineEdit->text(),
                                                 ui.retirpflineEdit->text(),
                                                 ui.ctaretlineEdit->text() , ui.pagarlineEdit->text(),
                                                 ui.cobrarlineEdit->text() , ui.claveivalineEdit->text(),
                                                 ui.clavegastoslineEdit->text() ,
                                                 ui.claveingresoslineEdit->text() , ui.pyglineEdit->text() ,
                                                 // ui.proxasientolineEdit->text() ,
                                                 ui.proxpaselineEdit->text() ,
                                                 ui.proxvencilineEdit->text() ,
                                                 // ui.numeracioncheckBox->isChecked() ,
                                                 ui.igiccheckBox->isChecked() ,
                                                 ui.analiticaradioButton->isChecked() ,
                                                 ui.idlineEdit->text() , ui.prorratalineEdit->text() ,
                                                 ui.especialcheckBox->isChecked(),
                                                 ui.constituciondateEdit->date(), ui.objetolineEdit->text(),
                                                 ui.actividadlineEdit->text(),ui.an_parc_radioButton->isChecked(),
                                                 ui.ctas_analitic_plainTextEdit->toPlainText(),
                                                 ui.usuarioscheckBox->isChecked(), ui.uldoclineEdit->text(),
                                                 ui.desglosecheckBox->isChecked(),
                                                 ui.provinciacomboBox->currentText().left(2),
                                                 ui.aalineEdit->text(),
                                                 ui.aglineEdit->text(),
                                                 ui.amoinvcheckBox->isChecked(),
                                                 ui.sec_recibidascheckBox->isChecked(),
                                                 ui.proxdomlineEdit->text(),
                                                 ui.cod_ine_plaza_lineEdit->text(),
                                                 ui.caja_iva_checkBox->isChecked(),
                                                 ui.borrados_consol_checkBox->isChecked(),
                                                 ui.cobro_venci_lineEdit->text(),
                                                 ui.pago_venci_lineEdit->text(),
                                                 ui.externoscheckBox->isChecked(),
                                                 ui.tfnolineEdit->text(),
                                                 ui.gd_bd_radioButton->isChecked(),
                                                 ui.tipo_proveedor_checkBox->isChecked(),
                                                 ui.homol_pruebas_lineEdit->text(),
                                                 ui.nombre_lineEdit->text(),
                                                 ui.apellidos_lineEdit->text(),
                                                 ui.curl_checkBox->isChecked(),
                                                 ui.url_actu_lineEdit->text(),
                                                ui.moneda_lineEdit->text());

    basedatos::instancia()->graba_config_imagenlogo(fototexto());

    accept();
}


void editconfig::igiccambiado()
{
  if (ui.igiccheckBox->isChecked())
    {
     ui.labelCuenta_iva_soportado->setText(tr("Cuenta IGIC soportado"));
     ui.labelCuenta_iva_repercutido->setText(tr("Cuenta IGIC repercutido"));
     ui.labelClave_iva_defecto->setText(tr("Clave IGIC defecto"));
    }
  else
           {
            ui.labelCuenta_iva_soportado->setText(tr("Cuenta IVA soportado"));
            ui.labelCuenta_iva_repercutido->setText(tr("Cuenta IVA repercutido"));
            ui.labelClave_iva_defecto->setText(tr("Clave IVA defecto"));
           }
}

void editconfig::analitica_parc_cambiado()
{
  if (ui.an_parc_radioButton->isChecked())
    {
      ui.ctas_analitic_plainTextEdit->setEnabled(true);
      // ui.desglosecheckBox->setEnabled(true);
    }
   else 
       {
        ui.ctas_analitic_plainTextEdit->setEnabled(false);
        ui.ctas_analitic_plainTextEdit->clear();
       }
}


void editconfig::sin_analitica_cambiado()
{
  if (ui.sin_analiticaradioButton->isChecked())
      ui.desglosecheckBox->setEnabled(false);
  else
      ui.desglosecheckBox->setEnabled(true);
}


void editconfig::importafoto()
{
QString cadfich;
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  if (dir->exec() == QDialog::Accepted)
    {
      cadfich=dir->ruta_actual();
    }
    else return;
  delete(dir);
#else

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Archivos de gráficos (*.png *.jpg *.jpeg *.bmp)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(getenv("HOME"));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE FICHERO GRÁFICO"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec()== QDialog::Accepted)
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           cadfich=fileNames.at(0);
          }
     } else return;

#endif

  if (!cadfich.isEmpty())
  {
   QPixmap imagen(cadfich);
   // ----------------------------------------------------------------------
   graf_recorte *g = new graf_recorte(cadfich,ui.logo_label->width(),ui.logo_label->height());
   int resultado=g->exec();
   int altolabel=ui.logo_label->height();
   int ancholabel=ui.logo_label->width();
   if (resultado==QDialog::Accepted)
     {
       QPixmap ima=g->recorte();
       //ui.fotolabel->setPixmap(g->recorte());
       //int altopix=ima.height();
       //int anchopix=ima.width();
       QPixmap definitiva;
       definitiva=ima.scaledToHeight(altolabel);
       ui.logo_label->setScaledContents(false);
       ui.logo_label->setPixmap(definitiva);
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

QString editconfig::fototexto()
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

void editconfig::borrafoto()
{
  if (QMessageBox::question(
            this,
            tr("¿ Borrar imagen ?"),
            tr("¿ Desea borrar la imagen actual ?")) == QMessageBox::Yes )
               {
                QPixmap foto;
                ui.logo_label->setPixmap(foto);
               }
}

void editconfig::proxpasecambiado()
{
    basedatos::instancia()->borra_tabla_apuntes_borrados();
}

void editconfig::tipo_proveedor_cheq() {
    if (ui.tipo_proveedor_checkBox->isChecked())  {
        ui.homol_pruebas_label->setEnabled(true);
        ui.homol_pruebas_lineEdit->setEnabled(true);
    }
    else {
        ui.homol_pruebas_lineEdit->clear();
        ui.homol_pruebas_label->setEnabled(false);
        ui.homol_pruebas_lineEdit->setEnabled(false);
    }
}
