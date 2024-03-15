#include "pide_fich.h"
#include "ui_pide_fich.h"
#include "funciones.h"
#include <QFileDialog>

pide_fich::pide_fich(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pide_fich)
{
    ui->setupUi(this);
    connect(ui->fichpushButton,SIGNAL(clicked()),this,SLOT(carga_fich()));
    QString caddir=trayreport();
    caddir.append(QDir::separator());
}

pide_fich::~pide_fich()
{
    delete ui;
}

void pide_fich::pasadir(QString dir) {
 caddir=dir;
}

void pide_fich::pasa (QString titulo_ventana, QString etiqueta) {
    if (!titulo_ventana.isEmpty())
       setWindowTitle(titulo_ventana);
    if (!etiqueta.isEmpty())
       ui->label->setText(etiqueta);
}

void pide_fich::pasa_nombre_fichero(QString nombre) {
    ui->nombre_fich_lineEdit->setText(nombre);
    ui->nombre_fich_lineEdit->setCursorPosition(ui->nombre_fich_lineEdit->text().length());
}

QString pide_fich::nombre_fichero() {
    return ui->nombre_fich_lineEdit->text();
}


void pide_fich::carga_fich()
{

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros; filtros << "Ficheros xml (*.xml)" << "Todos los ficheros (*)";
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(caddir);
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
           ui->nombre_fich_lineEdit->setText(cadfich);
          }
     }

}
