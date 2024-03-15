#include "modelo300.h"
#include "ui_modelo300.h"
#include "basedatos.h"
#include "funciones.h"
#include <QProgressDialog>
#include "datos_accesorios.h"
#include "consmayor.h"
#include <QMessageBox>
#include <QFileDialog>
#include "buscasubcuenta.h"

modelo300::modelo300(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modelo300)
{
    ui->setupUi(this);

    QString empresa, nif, nombre, apellidos;
    basedatos::instancia()->empresa_nif_nombre_apellidos(&empresa, &nif, &nombre, &apellidos);

    ui->niflineEdit->setText(nif);
    ui->apellidos_razonlineEdit->setText(empresa);
    if (!apellidos.isEmpty()) {
        ui->apellidos_razonlineEdit->setText(apellidos);
        ui->nombrelineEdit->setText(nombre);
    }

    connect(ui->ctapushButton,SIGNAL(clicked(bool)),SLOT(buscactabase()));

}

modelo300::~modelo300()
{
    delete ui;
}

void modelo300::parametros(QString *nif, QString *razon, QString *nombre, bool *escomplementaria, bool *essustitutiva,
                           QString *declaracion_anterior, bool *redeme, QString *cta_banco, QString *tipo_operacion,
                           double *compensar)
{
  *nif=ui->niflineEdit->text();
  *razon=ui->apellidos_razonlineEdit->text();
  *nombre=ui->nombrelineEdit->text();
  *escomplementaria=ui->complementariaradioButton->isChecked() && ui->complgroupBox->isChecked();
  *essustitutiva=ui->sustitutivaradioButton->isChecked() && ui->complgroupBox->isChecked();
  *declaracion_anterior=ui->anteriorlineEdit->text();
  *redeme=ui->redemecheckBox->isChecked();
  *cta_banco=ui->CtalineEdit->text();
  *tipo_operacion=ui->tipocomboBox->currentText().left(1);
  *compensar=convapunto(ui->compensarlineEdit->text()).toDouble();
}


void modelo300::buscactabase()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->CtalineEdit->text());
    int resultado=labusqueda->exec();
    if (resultado==QDialog::Accepted)
       cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui->CtalineEdit->setText(cadena2);
    delete(labusqueda);
}


/*
bool modelo300::genfich300(QString nombre)
{
 QFile plan( adapta(nombre)  );
 if (plan.exists() && QMessageBox::question(this,
       tr("¿ Sobreescribir ?"),
       tr("'%1' ya existe."
      "¿ Desea sobreescribirlo ?")
       .arg( nombre ),
       tr("&Sí"), tr("&No"),
       QString::null, 0, 1 ) )
           return false;

   QFile doc( adapta(nombre)  );

   if ( !doc.open( QIODevice::WriteOnly ) )
      {
       QMessageBox::warning( this, tr("Fichero 303"),tr("Error: Imposible grabar fichero"));
       return false;
      }
   QTextStream docstream( &doc );
   docstream.setCodec("ISO 8859-1");
   QString contenido="";

   QMessageBox::information( this, tr("Fichero 303"),tr("303 generado en archivo: ")+nombre);

  return true;
}



void modelo300::genera300()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajobd());
  dir->activa_pide_archivo(tr("modelo.303"));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      QString nombre=dir->respuesta_arch();
      if (nombre.length()>0)
          {
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           if (nombre.right(4)!=".303") nombre=nombre+".303";
           // QMessageBox::information( this, tr("selección"),
           //                         nombre );
           QString rutadir=dir->directorio_actual();
           if (!rutadir.endsWith(QDir::separator())) rutadir.append(QDir::separator());
           nombre=rutadir+nombre;
           // QMessageBox::information( this, tr("selección"),
           //                          nombre );
           // if (!genfich347(nombre))
           //   QMessageBox::information( this, tr("Fichero 347"),tr("NO se ha generado correctamente el fichero 303"));
           if (nombre.right(4)!=".303") nombre=nombre+".303";
           if (!genfich300(nombre))
              QMessageBox::information( this, tr("Fichero 303"),tr("NO se ha generado correctamente el fichero 303"));
          }
    }
   delete(dir);
#else


  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::AnyFile);
  dialogofich.setConfirmOverwrite ( false );
  dialogofich.setAcceptMode (QFileDialog::AcceptSave );
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  QString filtrotexto=tr("Archivos 303 (*.303)");
  filtros << filtrotexto;
  filtros << tr("Archivos 303 (*.303)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("ARCHIVO 303"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      // QMessageBox::information( this, tr("EXPORTAR PLAN"),dialogofich.selectedNameFilter () );
      if (fileNames.at(0).length()>0)
          {
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           QString nombre=fileNames.at(0);
           if (nombre.right(4)!=".303") nombre=nombre+".303";
           if (!genfich300(nombre))
              QMessageBox::information( this, tr("Fichero 303"),tr("NO se ha generado correctamente el fichero 303"));
          }
       }
#endif

}
*/
