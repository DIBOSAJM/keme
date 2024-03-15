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

#include "ivadatosadic.h"
#include "funciones.h"
#include "directorio.h"
#include <QFileDialog>
#include <QMessageBox>

ivadatosadic::ivadatosadic() : QDialog() {
    ui.setupUi(this);
    etiqueta = new CustomQLabel;
    etiqueta->setHidden(false);
    etiqueta->setMinimumWidth(90);
    etiqueta->setMinimumHeight(75);
    etiqueta->setText(tr("Soltar archivo\naquí"));
    etiqueta->setAlignment(Qt::AlignHCenter);
    etiqueta->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    ui.verticalLayout->addWidget(etiqueta);

    anterior=false;

connect(ui.fichdocpushButton ,SIGNAL(clicked()),SLOT(buscacopiadoc()));
connect(ui.visdocpushButton ,SIGNAL(clicked()),SLOT(visdoc()));
connect(ui.borraasdocpushButton ,SIGNAL(clicked()),SLOT(borraasignaciondoc()));

connect(ui.documentolineEdit,SIGNAL(textChanged(QString)),SLOT(doc_cambiado()));

connect(etiqueta, SIGNAL(filedroped()),SLOT(fichero_soltado()));
}
 


void ivadatosadic::pasafecha( QDate fecha )
{
  ui.fechadateEdit->setDate(fecha);
}


QString ivadatosadic::documento()
{
  return ui.documentolineEdit->text();
}

QString ivadatosadic::fichdocumento()
{
  return ui.fichdoclineEdit->text();
}

QDate ivadatosadic::fechaasiento()
{
  return ui.fechadateEdit->date();
}


bool ivadatosadic::esverificar()
{
  if (ui.radioButton->isChecked()) return true;
  return false;
}


void ivadatosadic::buscacopiadoc()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(rutacargadocs().isEmpty() ? adapta(dirtrabajo()):adapta(rutacargadocs()));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      ui.fichdoclineEdit->setText(dir->ruta_actual());
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

  QStringList filtros;
  filtros << tr("Todos los archivos (*)");
  filtros << tr("Archivos jpg (*.jpg)");
  filtros << tr("Archivos jpg (*.jpeg)");
  filtros << tr("Archivos png (*.png)");
  filtros << tr("Archivos pdf (*.pdf)");
  dialogofich.setNameFilters(filtros);
  if (rutacargadocs().isEmpty())
     dialogofich.setDirectory(adapta(dirtrabajo()));
    else
      dialogofich.setDirectory(adapta(rutacargadocs()));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE COPIA DE DOCUMENTO"));
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
          ui.fichdoclineEdit->setText(fileNames.at(0));
          }
     }
#endif
}

void ivadatosadic::visdoc()
{
   if (ui.fichdoclineEdit->text().isEmpty()) return;
   if (!ejecuta(aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())),ui.fichdoclineEdit->text()))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+ui.fichdoclineEdit->text()+tr(" con ")+
                             aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())));

}

void ivadatosadic::borraasignaciondoc()
{
    ui.fichdoclineEdit->clear();
}

void ivadatosadic::pasaconcepto(QString concepto)
{
    ui.conceptolineEdit->setText(concepto);
    guarda_concepto=concepto;
}


void ivadatosadic::doc_cambiado()
{
    ui.conceptolineEdit->setText(guarda_concepto+" - "+ui.documentolineEdit->text());
}


QString ivadatosadic::recuperaconcepto()
{
    return ui.conceptolineEdit->text();
}


void ivadatosadic::fichero_soltado() {
    ui.fichdoclineEdit->setText(etiqueta->nombre_fich());
}

void ivadatosadic::on_anterior_pushButton_clicked()
{
    anterior=true;
    accept();
}

bool ivadatosadic::navegar_anterior() {
    bool devolver=anterior;
    anterior=false;
    return devolver;
}
