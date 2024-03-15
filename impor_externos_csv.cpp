#include "impor_externos_csv.h"
#include "ui_impor_externos_csv.h"
#include "funciones.h"
#include "basedatos.h"
#include <QFileDialog>
#include <QMessageBox>
#include <directorio.h>

impor_externos_csv::impor_externos_csv(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::impor_externos_csv)
{
    ui->setupUi(this);

    connect(ui->busca_pushButton,SIGNAL(clicked(bool)),SLOT(busca_fich_pulsado()));
    connect(ui->aceptar_pushButton,SIGNAL(clicked(bool)),SLOT(procesa_csv()));
}

impor_externos_csv::~impor_externos_csv()
{
    delete ui;
}


void impor_externos_csv::busca_fich_pulsado()
{
    QString cadfich;
    if (ui->ruta_lineEdit->text().isEmpty()) cadfich=dirtrabajobd();
      else cadfich=ui->ruta_lineEdit->text();

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(cadfich));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      cadfich=dir->ruta_actual();
    } else return;
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
    filtros << tr("Archivos de texto con formato especial (*.*)");
    dialogofich.setNameFilters(filtros);
    dialogofich.setDirectory(cadfich);
    dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR"));
    // dialogofich.exec();
    //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
    //                                              dirtrabajo,
    //                                              tr("Ficheros de texto (*.txt)"));
    QStringList fileNames;
    if (dialogofich.exec() == QDialog::Accepted)
       {
        fileNames = dialogofich.selectedFiles();
        if (fileNames.at(0).length()>0)
            {
             // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
             cadfich=fileNames.at(0);
            }
       }
       else return;
#endif

    if (cadfich.endsWith(QDir::separator()))
        cadfich=cadfich.left(cadfich.length()-1);
    ui->ruta_lineEdit->setText(cadfich);
}


void impor_externos_csv::procesa_csv()
{
    QFile fichero(ui->ruta_lineEdit->text());

    if ( fichero.open( QIODevice::ReadOnly ) ) {
          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);
          QString linea;
          while ( !stream.atEnd() )
            {
              linea = stream.readLine(); // linea de texto excluyendo '\n'
              QString datos[15];
              enum columnas {nombre, ncomercial, nif, domicilio, poblacion,
                             cp, provincia, pais, pais_ini,iban, tfno, email, cuenta, observaciones };
              for (int veces=0; veces<12; veces++)
                  {
                   datos[veces]=linea.section('\t',veces,veces);
                  }

             /* guarda_datos_externo (QString codigo, QString cuenta, QString razon, QString ncomercial,QString cif,
                  QString nifrprlegal, QString domicilio, QString poblacion,
                  QString codigopostal, QString provincia, QString pais,
                  QString tfno, QString fax, QString email, QString observaciones,
                  QString ccc, QString cuota, bool ivaasoc, QString tipoiva, QString cuentaiva, QString cta_base_iva,
                  bool venciasoc, QString tipovenci, QString cuentatesor, QString conceptodiario,
                  QString web, QString codclaveidfiscal, bool procvto,
                  QString pais_dat, int vdiafijo,
                  QString cuenta_ret_asig, bool es_ret_asig_arrend,
                  QString tipo_ret_asig, QString tipo_oper_ret_asig,
                  bool dom_bancaria, QString iban, QString nriesgo, QString cedente1, QString cedente2,
                  QString bic, QString sufijo, bool caja_iva, QString ref_mandato, QDate firma_mandato,
                  QString forma_pago,
                  QString fototexto) */
              if (!ui->sobreescribe_checkBox->isChecked())
                 {
                  // comprobamos si existe externo. En caso afirmativo, saltamos
                  if (basedatos::instancia()->existe_externo(datos[nif]))
                      continue;
                 }
             basedatos::instancia()->guarda_datos_externo(datos[nif], datos[cuenta], datos[nombre],
                                          datos[ncomercial], datos[nif],"","",
                                          "", datos[domicilio],
                                          datos[poblacion], datos[cp],
                                          datos[provincia], datos[pais_ini], datos[tfno],
                                          "", datos[email], datos[observaciones],
                                          "","", false, "","","",false,"","","","","",
                                          false,datos[pais],0,"",false,"","",false,datos[iban],
                                          "","","","","",false,"",
                                          QDate(2009,10,31),"","");

           }
          fichero.close();
        } else return;
    QMessageBox::information(this, tr("KEME-Contabilidad"),
                         tr("El archivo se ha procesado"));
    accept();
}
