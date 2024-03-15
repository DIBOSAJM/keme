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


#include "listadocs.h"
#include "basedatos.h"
#include "edita_doc.h"
#include <QMessageBox>
#include <QFileDialog>

listadocs::listadocs() : QDialog() {
    ui.setupUi(this);


        ui.codigolineEdit->setFocus();
         QSqlTableModel *modelo = new QSqlTableModel;
         modelo->setTable("tipos_doc");
         modelo->setSort(0,Qt::AscendingOrder);
         modelo->select();
         modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
         modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
         modeloreg=modelo;
         ui.tabla->setModel(modelo);
         ui.tabla->setColumnWidth(0,60);
         ui.tabla->setColumnWidth(1,350);
         ui.tabla->hideColumn(23);
         ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

         // ui.tabla->hideColumn(2);
         ui.tabla->show();

         ui.img_pushButton->hide();
         ui.sup_fondo_pushButton->hide();

         connect(ui.codigolineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
         connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado()));
         connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
         connect(ui.guardarpushButton,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
         connect(ui.cancelarpushButton,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
         connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
         connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));
         connect(ui.editarpushButton,SIGNAL(clicked()),this,SLOT(eddoc()));
         connect(ui.img_pushButton,SIGNAL(clicked(bool)),this,SLOT(imagen_fondo()));
         connect(ui.sup_fondo_pushButton,SIGNAL(clicked(bool)),this,SLOT(suprime_fondo()));
}

listadocs::~listadocs()
{
    delete ui.tabla;
    delete modeloreg;
}

void listadocs::compruebacodigo()
{
    QString caddescrip;

 if (ui.codigolineEdit->text().contains(' '))
    {
      QMessageBox::warning( this, tr("TIPOS DOCUMENTO"),
      tr("En la composición del código no pueden haber espacios"));
      ui.codigolineEdit->clear();
      return;
    }

 if (ui.codigolineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("TIPOS DOCUMENTO"),
      tr("El código no puede estar vacío"));
      ui.codigolineEdit->clear();
      return;
    }


 if (!basedatos::instancia()->existecodigotipos_doc(ui.codigolineEdit->text(),&caddescrip))
    {
       ui.cancelarpushButton->setEnabled(true);
       ui.descriplineEdit->setEnabled(true);
       ui.descriplineEdit->setFocus();
       ui.codigolineEdit->setReadOnly(true);
     }
     else // codigo correcto
            {
              disconnect(ui.descriplineEdit);
              ui.descriplineEdit->setText(caddescrip);
              ui.cancelarpushButton->setEnabled(true);
              ui.borrarpushButton->setEnabled(true);
              ui.editarpushButton->setEnabled(true);
              ui.descriplineEdit->setEnabled(true);
              ui.descriplineEdit->setFocus();
              ui.codigolineEdit->setReadOnly(true);
              connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this
                      ,SLOT(descripcioncambiada()));
            }
}


void listadocs::descripcioncambiada()
{
    if(ui.descriplineEdit->hasFocus()) ui.guardarpushButton->setEnabled(true);
}

void listadocs::codigocambiado()
{
    if (ui.codigolineEdit->text().length() == 0) {
      ui.descriplineEdit->setEnabled(false);
    }
    else {
      ui.descriplineEdit->setEnabled(true);
    }
}

void listadocs::botonguardarpulsado()
{
    QString caddescrip;

      if  (basedatos::instancia()->existecodigotipos_doc(ui.codigolineEdit->text(),&caddescrip))
        basedatos::instancia()->guardacambiostipos_doc(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
      else
       {
        basedatos::instancia()->inserttipos_doc(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
        if  (basedatos::instancia()->existecodigotipos_doc(ui.codigolineEdit->text(),&caddescrip))
          {
            edita_doc *e = new edita_doc();
            e->pasacodigo(ui.codigolineEdit->text());
            e->exec();
            delete(e);
         }
       }
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     ui.editarpushButton->setDisabled(true);
     modeloreg->setTable("tipos_doc");
     modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
     modeloreg->setSort(0,Qt::AscendingOrder);
     modeloreg->select();
}


void listadocs::botoncancelarpulsado()
{
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     ui.editarpushButton->setDisabled(true);
}

void listadocs::botoneliminarpulsado()
{
   if (QMessageBox::question(
            this,
            tr("TIPOS DOCUMENTO"),
            tr("¿ Desea borrar el elemento seleccionado ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==1 ) return;
     basedatos::instancia()->eliminatipo_doc(ui.codigolineEdit->text());
     modeloreg->select();
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     ui.editarpushButton->setDisabled(true);
}

void listadocs::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        ui.codigolineEdit->setText(
           modeloreg->record(indiceactual.row()).value("codigo").toString());
        ui.descriplineEdit->setText(
           modeloreg->record(indiceactual.row()).value("descrip").toString());
        ui.cancelarpushButton->setEnabled(true);
        ui.borrarpushButton->setEnabled(true);
        ui.editarpushButton->setEnabled(true);
        ui.descriplineEdit->setEnabled(true);
        ui.descriplineEdit->setFocus();
        ui.codigolineEdit->setReadOnly(true);
       }
}


void listadocs::eddoc()
{
  QString codigo=ui.codigolineEdit->text();
  QString qdescrip;
  // qlonglong num;


  if  (basedatos::instancia()->existecodigotipos_doc(codigo,&qdescrip))
  {
      edita_doc *e = new edita_doc();
      e->pasacodigo(codigo);
      e->exec();
      delete(e);
  }


    modeloreg->setTable("tipos_doc");
    modeloreg->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
    modeloreg->setHeaderData(1, Qt::Horizontal, tr("DESCRIP"));
    modeloreg->setSort(0,Qt::AscendingOrder);
    modeloreg->select();
    ui.codigolineEdit->clear();
    ui.codigolineEdit->setReadOnly(false);
    ui.codigolineEdit->setFocus();
    ui.descriplineEdit->clear();
    ui.guardarpushButton->setDisabled(true);
    ui.cancelarpushButton->setDisabled(true);
    ui.borrarpushButton->setDisabled(true);
    ui.editarpushButton->setDisabled(true);
}

void listadocs::imagen_fondo() {
    QModelIndex indiceactual=ui.tabla->currentIndex();
    if (!indiceactual.isValid()) return;
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
             QFile file(cadfich);
             file.open(QIODevice::ReadOnly);
             QByteArray inByteArray=file.readAll();
             QString codigo_m=modeloreg->record(indiceactual.row()).value("codigo").toString();
             basedatos::instancia()->actu_img_tipos_doc(codigo_m, inByteArray);
            }
       }


}

void listadocs::suprime_fondo() {
    QModelIndex indiceactual=ui.tabla->currentIndex();
    if (!indiceactual.isValid()) return;
    QString codigo_m=modeloreg->record(indiceactual.row()).value("codigo").toString();
    if (QMessageBox::question(
             this,
             tr("TIPOS DOCUMENTO"),
             tr("¿ Desea eliminar la imagen de fondo del documento seleccionado ?"),
             tr("&Sí"), tr("&No"),
             QString(), 0, 1 ) ==1 ) return;
    basedatos::instancia()->elimina_img_tipos_doc(codigo_m);
}
